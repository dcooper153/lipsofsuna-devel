/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
 *
 * Lips of Suna is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Lips of Suna is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Lips of Suna. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxBuilder Builder
 * @{
 */

#include "voxel.h"
#include "voxel-builder.h"
#include "voxel-manager.h"
#include "voxel-material.h"
#include "voxel-private.h"
#include "voxel-sector.h"

#define PRIVATE_MISSING_MATERIAL 0xFFFFFFFF

static const int voxel_face_normals[6][3] =
{
	{ -1,  0,  0 },
	{  1,  0,  0 },
	{  0, -1,  0 },
	{  0,  1,  0 },
	{  0,  0, -1 },
	{  0,  0,  1 }
};

static int
private_build_full (livoxBuilder* self,
                    livoxBlock*   block,
                    int           bx,
                    int           by,
                    int           bz);

static int
private_build_tiles (livoxBuilder* self,
                     livoxBlock*   block,
                     int           bx,
                     int           by,
                     int           bz);

static void
private_calculate_texcoords (livoxBuilder* self,
                             int           material,
                             limdlVertex*  vertices);

#ifndef LIVOX_DISABLE_GRAPHICS
static int
private_insert_material (livoxBuilder* self,
                         int           id);
#endif

static int
private_insert_vertices (livoxBuilder* self,
                         int           material,
                         limatVector*  blockoff,
                         limatVector*  tileoff,
                         limdlVertex*  vertices,
                         int           count);

/*****************************************************************************/

livoxBuilder*
livox_builder_new (livoxSector* sector)
{
	livoxBuilder* self;

	/* Allocate self. */
	self = calloc (1, sizeof (livoxBuilder));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->sector = sector;

	/* Allocate rendering specific. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (sector->manager->render != NULL)
	{
		self->helpers.materials = lialg_u32dic_new ();
		if (self->helpers.materials == NULL)
		{
			livox_builder_free (self);
			return NULL;
		}
		self->helpers.model = limdl_model_new ();
		if (self->helpers.model == NULL)
		{
			livox_builder_free (self);
			return NULL;
		}
		self->helpers.normals = lialg_memdic_new ();
		if (self->helpers.normals == NULL)
		{
			limdl_model_free (self->helpers.model);
			livox_builder_free (self);
			return NULL;
		}
	}
#endif

	return self;
}

void
livox_builder_free (livoxBuilder* self)
{
	lialgMemdicIter iter;

	if (self->helpers.normals != NULL)
	{
		LI_FOREACH_MEMDIC (iter, self->helpers.normals)
			free (iter.value);
		lialg_memdic_free (self->helpers.normals);
	}
	if (self->helpers.materials != NULL)
		lialg_u32dic_free (self->helpers.materials);
	if (self->helpers.model != NULL)
		limdl_model_free (self->helpers.model);
	if (self->helpers.shape != NULL)
		liphy_shape_free (self->helpers.shape);
	free (self->vertices.array);
	free (self);
}

int
livox_builder_build (livoxBuilder* self,
                     int           bx,
                     int           by,
                     int           bz)
{
#ifndef LIVOX_DISABLE_GRAPHICS
	int i;
	livoxBuilderNormal* lookup;
	limatVector coord;
	limatVector normal;
#endif
	livoxBlock* block;

	/* Triangulate the block. */
	block = self->sector->blocks + LIVOX_BLOCK_INDEX (bx, by, bz);
	switch (block->type)
	{
		case LIVOX_BLOCK_TYPE_FULL:
			if (!private_build_full (self, block, bx, by, bz))
				return 0;
			break;
		case LIVOX_BLOCK_TYPE_TILES:
			if (!private_build_tiles (self, block, bx, by, bz))
				return 0;
			break;
		default:
			return 0;
	}

	/* Apply smooth normals and calculate bounding box. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->helpers.model != NULL && self->vertices.count)
	{
		for (i = 0 ; i < self->helpers.model->vertex.count ; i++)
		{
			coord = self->helpers.model->vertex.vertices[i].coord;
			lookup = lialg_memdic_find (self->helpers.normals, &coord, sizeof (limatVector));
			assert (lookup != NULL);
			if (lookup != NULL)
			{
				normal = limat_vector_multiply (lookup->normals, 1.0f / lookup->count);
				self->helpers.model->vertex.vertices[i].normal = normal;
			}
		}
		limdl_model_calculate_bounds (self->helpers.model);
	}
#endif

	return 1;
}

limdlModel*
livox_builder_get_model (livoxBuilder* self)
{
	limdlModel* model;

	assert (self->helpers.model != NULL);
	model = self->helpers.model;
	self->helpers.model = NULL;

	return model;
}

liphyShape*
livox_builder_get_shape (livoxBuilder* self)
{
	liphyShape* shape;

	shape = self->helpers.shape;
	self->helpers.shape = NULL;

	return shape;
}

/*****************************************************************************/

static int
private_build_full (livoxBuilder* self,
                    livoxBlock*   block,
                    int           bx,
                    int           by,
                    int           bz)
{
	int c;
	int i;
	int tx;
	int ty;
	int tz;
	int mat;
	livoxVoxel tile;
	livoxVoxel mask[6];
	limatAabb aabb;
	limatVector blockoff;
	limatVector tileoff;
	limdlVertex vertices[64];

	if (!block->full.terrain)
		return 1;
	tile = block->full.terrain;

	/* Get block offset. */
	blockoff = limat_vector_init (bx, by, bz);
	blockoff = limat_vector_multiply (blockoff, LIVOX_BLOCK_WIDTH);
	blockoff = limat_vector_add (blockoff, self->sector->origin);

	/* Generate vertices. */
	for (tz = 0 ; tz < LIVOX_TILES_PER_LINE ; tz++)
	for (ty = 0 ; ty < LIVOX_TILES_PER_LINE ; ty++)
	for (tx = 0 ; tx < LIVOX_TILES_PER_LINE ; tx++)
	{
		for (i = 0 ; i < 6 ; i++)
		{
			mask[i] = livox_sector_get_voxel (self->sector,
				LIVOX_TILES_PER_LINE * bx + tx + voxel_face_normals[i][0],
				LIVOX_TILES_PER_LINE * by + ty + voxel_face_normals[i][1],
				LIVOX_TILES_PER_LINE * bz + tz + voxel_face_normals[i][2]);
		}
		c = livox_voxel_triangulate (tile, mask, vertices);
		if (c)
		{
			mat = livox_voxel_get_type (tile) - 1;
			tileoff = limat_vector_init (tx, ty, tz);
			tileoff = limat_vector_multiply (tileoff, LIVOX_TILE_WIDTH);
			if (!private_insert_vertices (self, mat, &blockoff, &tileoff, vertices, c))
				return 0;
		}
	}
	if (!self->vertices.count)
		return 1;

	/* Create collision shape. */
	aabb.min = limat_vector_init (0.0f, 0.0f, 0.0f);
	aabb.max = limat_vector_init (LIVOX_BLOCK_WIDTH, LIVOX_BLOCK_WIDTH, LIVOX_BLOCK_WIDTH);
	aabb.min = limat_vector_add (aabb.min, blockoff);
	aabb.max = limat_vector_add (aabb.max, blockoff);
	self->helpers.shape = liphy_shape_new_aabb (self->sector->manager->physics, &aabb);
	if (self->helpers.shape == NULL)
		return 0;

	return 1;
}

static int
private_build_tiles (livoxBuilder* self,
                     livoxBlock*   block,
                     int           bx,
                     int           by,
                     int           bz)
{
	int c;
	int i;
	int tx;
	int ty;
	int tz;
	int mat;
	livoxVoxel tile;
	livoxVoxel mask[6];
	limatVector blockoff;
	limatVector tileoff;
	limdlVertex vertices[64];

	/* Get block offset. */
	blockoff = limat_vector_init (bx, by, bz);
	blockoff = limat_vector_multiply (blockoff, LIVOX_BLOCK_WIDTH);
	blockoff = limat_vector_add (blockoff, self->sector->origin);

	/* Generate vertices. */
	for (tz = 0 ; tz < LIVOX_TILES_PER_LINE ; tz++)
	for (ty = 0 ; ty < LIVOX_TILES_PER_LINE ; ty++)
	for (tx = 0 ; tx < LIVOX_TILES_PER_LINE ; tx++)
	{
		tile = block->tiles->tiles[LIVOX_TILE_INDEX (tx, ty, tz)];
		if (!tile)
			continue;
		for (i = 0 ; i < 6 ; i++)
		{
			mask[i] = livox_sector_get_voxel (self->sector,
				LIVOX_TILES_PER_LINE * bx + tx + voxel_face_normals[i][0],
				LIVOX_TILES_PER_LINE * by + ty + voxel_face_normals[i][1],
				LIVOX_TILES_PER_LINE * bz + tz + voxel_face_normals[i][2]);
		}
		c = livox_voxel_triangulate (tile, mask, vertices);
		if (c)
		{
			mat = livox_voxel_get_type (tile) - 1;
			tileoff = limat_vector_init (tx, ty, tz);
			tileoff = limat_vector_multiply (tileoff, LIVOX_TILE_WIDTH);
			if (!private_insert_vertices (self, mat, &blockoff, &tileoff, vertices, c))
				return 0;
		}
	}
	if (!self->vertices.count)
		return 1;

	/* Create collision shape. */
	self->helpers.shape = liphy_shape_new_concave (self->sector->manager->physics,
		self->vertices.array, self->vertices.count);
	if (self->helpers.shape == NULL)
		return 0;

	return 1;
}

static void
private_calculate_texcoords (livoxBuilder* self,
                             int           material,
                             limdlVertex*  vertices)
{
	int j;
	float dx = vertices->normal.x;
	float dy = vertices->normal.y;
	float dz = vertices->normal.z;
	float scale;
	limatVector u;
	limatVector v;
	livoxMaterial* voxmat;

	/* Calculate plane vectors. */
	if (LI_ABS (dx) < LI_ABS (dy) && LI_ABS (dx) < LI_ABS (dz))
		u = limat_vector_init (1.0f, 0.0f, 0.0f);
	else if (LI_ABS (dy) < LI_ABS (dz))
		u = limat_vector_init (1.0f, 1.0f, 0.0f);
	else
		u = limat_vector_init (0.0f, 0.0f, 1.0f);
	v = limat_vector_cross (u, vertices->normal);

	/* Get scale factor. */
	voxmat = lialg_u32dic_find (self->sector->manager->materials, material);
	if (voxmat != NULL)
		scale = voxmat->scale;
	else
		scale = 0.05f;

	/* Calculate texture coordinates. */
	for (j = 0 ; j < 3 ; j++)
	{
		vertices[j].texcoord[0] = scale * limat_vector_dot (vertices[j].coord, u);
		vertices[j].texcoord[1] = scale * limat_vector_dot (vertices[j].coord, v);
	}
}

#ifndef LIVOX_DISABLE_GRAPHICS
static int
private_insert_material (livoxBuilder* self,
                         int           id)
{
	void* ret;
	limdlMaterial tmp;
	limdlMaterial* mdlmat;
	livoxMaterial* voxmat;

	assert (self->helpers.model != NULL);

	/* Check for existing. */
	ret = lialg_u32dic_find (self->helpers.materials, id);
	if (ret != NULL)
		return (int)(intptr_t)(ret - 1);

	/* Find material info. */
	voxmat = livox_manager_find_material (self->sector->manager, id);
	if (voxmat == NULL)
	{
		id = PRIVATE_MISSING_MATERIAL;
		ret = lialg_u32dic_find (self->helpers.materials, id);
		if (ret != NULL)
			return (int)(intptr_t)(ret - 1);
		memset (&tmp, 0, sizeof (limdlMaterial));
		tmp.diffuse[0] = 1.0f;
		tmp.diffuse[1] = 1.0f;
		tmp.diffuse[2] = 1.0f;
		tmp.diffuse[3] = 1.0f;
		tmp.shader = "default";
		mdlmat = &tmp;
	}
	else
		mdlmat = &voxmat->model;

	/* Create new material. */
	if (!limdl_model_insert_material (self->helpers.model, mdlmat))
		return -1;
	if (!lialg_u32dic_insert (self->helpers.materials, id, NULL + self->helpers.model->materials.count))
		return -1;

	return self->helpers.model->materials.count - 1;
}
#endif

static int
private_insert_vertices (livoxBuilder* self,
                         int           material,
                         limatVector*  blockoff,
                         limatVector*  tileoff,
                         limdlVertex*  vertices,
                         int           count)
{
	int i;
	int j;
	int mat;
	livoxBuilderNormal* lookup;
	limatVector coord;
	limatVector normal;
	limatVector* tmp;

	/* Scale and translate vertices. */
	for (i = 0 ; i < count ; i++)
	{
		vertices[i].coord = limat_vector_multiply (vertices[i].coord, LIVOX_TILE_WIDTH);
		vertices[i].coord = limat_vector_add (vertices[i].coord, *tileoff);
		vertices[i].coord = limat_vector_add (vertices[i].coord, *blockoff);
	}

	/* Insert collision vertices. */
	tmp = realloc (self->vertices.array, (self->vertices.count + count) * sizeof (limatVector));
	if (tmp == NULL)
		return 0;
	for (i = 0 ; i < count ; i++)
		tmp[self->vertices.count + i] = vertices[i].coord;
	self->vertices.array = tmp;
	self->vertices.count += count;

	/* Insert model vertices. */
#ifndef LIVOX_DISABLE_GRAPHICS
	if (self->helpers.model != NULL)
	{
		for (i = 0 ; i < count ; i += 3)
		{
			/* Insert model triangle. */
			mat = private_insert_material (self, material);
			if (mat == -1)
				return 0;
			private_calculate_texcoords (self, material, vertices + i);
			limdl_model_insert_triangle (self->helpers.model, mat, vertices + i, NULL);

			/* Create normal lookup. */
			normal = vertices[i].normal;
			for (j = 0 ; j < 3 ; j++)
			{
				coord = vertices[i + j].coord;
				lookup = lialg_memdic_find (self->helpers.normals, &coord, sizeof (limatVector));
				if (lookup == NULL)
				{
					lookup = malloc (sizeof (livoxBuilderNormal));
					if (lookup == NULL)
						continue;
					lookup->vertex = coord;
					lookup->normals = normal;
					lookup->count = 1;
					if (!lialg_memdic_insert (self->helpers.normals, &coord, sizeof (limatVector), lookup))
						free (lookup);
				}
				else
				{
					lookup->normals = limat_vector_add (lookup->normals, normal);
					lookup->count++;
				}
			}
		}
	}
#endif

	return 1;
}

/** @} */
/** @} */

