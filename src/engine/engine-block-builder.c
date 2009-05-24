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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengBlockBuilder BlockBuilder
 * @{
 */

#include "engine-block-builder.h"
#include "engine-voxel.h"

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
private_build_full (liengBlockBuilder* self,
                    liengBlock*        block,
                    int                bx,
                    int                by,
                    int                bz);

static int
private_build_tiles (liengBlockBuilder* self,
                     liengBlock*        block,
                     int                bx,
                     int                by,
                     int                bz);

static int
private_insert_materials (liengBlockBuilder* self);

/*****************************************************************************/

liengBlockBuilder*
lieng_block_builder_new (liengSector* sector)
{
	liengBlockBuilder* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liengBlockBuilder));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->sector = sector;

	/* Allocate rendering specific. */
	if (sector->engine->render != NULL)
	{
		self->helpers.model = limdl_model_new ();
		if (self->helpers.model == NULL)
		{
			free (self);
			return NULL;
		}
		self->helpers.normals = lialg_memdic_new ();
		if (self->helpers.normals == NULL)
		{
			limdl_model_free (self->helpers.model);
			free (self);
			return NULL;
		}
	}

	return self;
}

void
lieng_block_builder_free (liengBlockBuilder* self)
{
	lialgMemdicIter iter;

	if (self->helpers.normals != NULL)
	{
		LI_FOREACH_MEMDIC (iter, self->helpers.normals)
			free (iter.value);
		lialg_memdic_free (self->helpers.normals);
	}
	if (self->helpers.model != NULL)
		limdl_model_free (self->helpers.model);
	if (self->helpers.shape != NULL)
		liphy_shape_free (self->helpers.shape);
	free (self->vertices.array);
	free (self);
}

int
lieng_block_builder_build (liengBlockBuilder* self,
                           int                bx,
                           int                by,
                           int                bz)
{
	liengBlock* block;

	block = self->sector->blocks + LIENG_BLOCK_INDEX (bx, by, bz);
	switch (block->type)
	{
		case LIENG_BLOCK_TYPE_FULL:
			return private_build_full (self, block, bx, by, bz);
		case LIENG_BLOCK_TYPE_TILES:
			return private_build_tiles (self, block, bx, by, bz);
	}

	return 0;
}

limdlModel*
lieng_block_builder_get_model (liengBlockBuilder* self)
{
	limdlModel* model;

	assert (self->helpers.model != NULL);
	model = self->helpers.model;
	self->helpers.model = NULL;

	return model;
}

liphyShape*
lieng_block_builder_get_shape (liengBlockBuilder* self)
{
	liphyShape* shape;

	shape = self->helpers.shape;
	self->helpers.shape = NULL;

	return shape;
}

/*****************************************************************************/

static int
private_build_full (liengBlockBuilder* self,
                    liengBlock*        block,
                    int                bx,
                    int                by,
                    int                bz)
{
	limatAabb aabb;
	limatVector blockoff;

	if (!block->full.terrain)
		return 1;

	if (!private_insert_materials (self))
		return 0;

	/* Get block offset. */
	blockoff = limat_vector_init (bx, by, bz);
	blockoff = limat_vector_multiply (blockoff, LIENG_BLOCK_WIDTH);
	blockoff = limat_vector_add (blockoff, self->sector->origin);

	/* TODO: Build model. */
#ifndef LIENG_DISABLE_GRAPHICS
#endif

	/* Create collision shape. */
	aabb.min = limat_vector_init (0.0f, 0.0f, 0.0f);
	aabb.max = limat_vector_init (LIENG_BLOCK_WIDTH, LIENG_BLOCK_WIDTH, LIENG_BLOCK_WIDTH);
	aabb.min = limat_vector_add (aabb.min, blockoff);
	aabb.max = limat_vector_add (aabb.max, blockoff);
	self->helpers.shape = liphy_shape_new_aabb (self->sector->engine->physics, &aabb);
	if (self->helpers.shape == NULL)
		return 0;

	return 1;
}

static int
private_build_tiles (liengBlockBuilder* self,
                     liengBlock*        block,
                     int                bx,
                     int                by,
                     int                bz)
{
	int c;
	int i;
	int j;
	int tx;
	int ty;
	int tz;
	liengBlockBuilderNormal* lookup;
	liengTile tile;
	liengTile mask[6];
	limatVector* tmp;
	limatVector coord;
	limatVector normal;
	limatVector blockoff;
	limatVector tileoff;
	limdlVertex vertices[64];

	if (!private_insert_materials (self))
		return 0;

	/* Get block offset. */
	blockoff = limat_vector_init (bx, by, bz);
	blockoff = limat_vector_multiply (blockoff, LIENG_BLOCK_WIDTH);
	blockoff = limat_vector_add (blockoff, self->sector->origin);

	/* Generate vertices. */
	for (tz = 0 ; tz < LIENG_TILES_PER_LINE ; tz++)
	for (ty = 0 ; ty < LIENG_TILES_PER_LINE ; ty++)
	for (tx = 0 ; tx < LIENG_TILES_PER_LINE ; tx++)
	{
		tile = block->tiles->tiles[LIENG_TILE_INDEX (tx, ty, tz)];
		if (!tile)
			continue;

		/* Triangulate the voxel. */
		for (i = 0 ; i < 6 ; i++)
		{
			mask[i] = lieng_sector_get_voxel (self->sector,
				LIENG_TILES_PER_LINE * bx + tx + voxel_face_normals[i][0],
				LIENG_TILES_PER_LINE * by + ty + voxel_face_normals[i][1],
				LIENG_TILES_PER_LINE * bz + tz + voxel_face_normals[i][2]);
		}
		c = lieng_voxel_triangulate (tile, mask, vertices);
		if (!c)
			continue;

		/* Scale and translate vertices. */
		tileoff = limat_vector_init (tx, ty, tz);
		tileoff = limat_vector_multiply (tileoff, LIENG_TILE_WIDTH);
		for (i = 0 ; i < c ; i++)
		{
			vertices[i].coord = limat_vector_multiply (vertices[i].coord, LIENG_TILE_WIDTH);
			vertices[i].coord = limat_vector_add (vertices[i].coord, tileoff);
			vertices[i].coord = limat_vector_add (vertices[i].coord, blockoff);
		}

		/* Insert collision vertices. */
		tmp = realloc (self->vertices.array, (self->vertices.count + c) * sizeof (limatVector));
		if (tmp == NULL)
			return 0;
		for (i = 0 ; i < c ; i++)
			tmp[self->vertices.count + i] = vertices[i].coord;
		self->vertices.array = tmp;
		self->vertices.count += c;

		/* Insert model vertices. */
#ifndef LIENG_DISABLE_GRAPHICS
		if (self->helpers.model != NULL)
		{
			for (i = 0 ; i < c ; i += 3)
			{
				/* Project texture coordinates. */
#warning FIXME: Hardcoded terrain texture coordinate generation.
#define TEXTURE_SCALE 0.05f
				float dx = vertices[i].normal.x;
				float dy = vertices[i].normal.y;
				float dz = vertices[i].normal.z;
				limatVector u;
				if (LI_ABS (dx) < LI_ABS (dy) && LI_ABS (dx) < LI_ABS (dz))
					u = limat_vector_init (1.0f, 0.0f, 0.0f);
				else if (LI_ABS (dy) < LI_ABS (dz))
					u = limat_vector_init (1.0f, 1.0f, 0.0f);
				else
					u = limat_vector_init (0.0f, 0.0f, 1.0f);
				limatVector v = limat_vector_cross (u, vertices[i].normal);
				for (j = 0 ; j < 3 ; j++)
				{
					float tu = limat_vector_dot (vertices[i + j].coord, u);
					float tv = limat_vector_dot (vertices[i + j].coord, v);
					vertices[i + j].texcoord[0] = tu * TEXTURE_SCALE;
					vertices[i + j].texcoord[1] = tv * TEXTURE_SCALE;
				}

				/* Insert model triangle. */
				/* FIXME: Bad material. */
				limdl_model_insert_triangle (self->helpers.model, 0, vertices + i, NULL);

				/* Create normal lookup. */
				normal = vertices[i].normal;
				for (j = 0 ; j < 3 ; j++)
				{
					coord = vertices[i + j].coord;
					lookup = lialg_memdic_find (self->helpers.normals, &coord, sizeof (limatVector));
					if (lookup == NULL)
					{
						lookup = malloc (sizeof (liengBlockBuilderNormal));
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
	}
	if (!self->vertices.count)
		return 1;

	/* Create collision shape. */
	self->helpers.shape = liphy_shape_new_concave (self->sector->engine->physics,
		self->vertices.array, self->vertices.count);
	if (self->helpers.shape == NULL)
		return 0;

	/* Apply smooth normals. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->helpers.model != NULL)
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
	}
#endif

	/* Calculate bounding box. */
#ifndef LIENG_DISABLE_GRAPHICS
	if (self->helpers.model != NULL)
		limdl_model_calculate_bounds (self->helpers.model);
#endif

	return 1;
}

static int
private_insert_materials (liengBlockBuilder* self)
{
#ifndef LIENG_DISABLE_GRAPHICS
	limdlMaterial material;
	limdlTexture texture;

	if (self->helpers.model == NULL)
		return 1;

	/* FIXME */
#warning FIXME: Hardcoded terrain material.
	memset (&material, 0, sizeof (limdlMaterial));
	material.flags = 0;
	material.shininess = 1.0f;
	material.diffuse[0] = 1.0f;
	material.diffuse[1] = 1.0f;
	material.diffuse[2] = 1.0f;
	material.diffuse[3] = 1.0f;
	material.specular[0] = 0.0f;
	material.specular[1] = 0.0f;
	material.specular[2] = 0.0f;
	material.specular[3] = 0.0f;
	material.shader = "default";
	material.textures.count = 1;
	material.textures.textures = &texture;
	material.textures.textures[0].type = LIMDL_TEXTURE_TYPE_IMAGE;
	material.textures.textures[0].flags = LIMDL_TEXTURE_FLAG_REPEAT | LIMDL_TEXTURE_FLAG_MIPMAP;
	material.textures.textures[0].width = 256;
	material.textures.textures[0].height = 256;
	material.textures.textures[0].string = "stone-002";
	if (!limdl_model_insert_material (self->helpers.model, &material))
		return 0;

	return 1;
#else
	return 1;
#endif
}

/** @} */
/** @} */

