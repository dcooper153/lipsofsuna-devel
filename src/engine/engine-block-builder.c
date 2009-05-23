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

static const int voxel_triangle_masks[6][2][4] =
{
	  /* Processed. */                    /* Neighbor. */
	{ { 0x1500, 0x5400, 0x5100, 0x4500 }, { 0x2A00, 0xA800, 0xA200, 0x8A00 } },
	{ { 0x2A00, 0xA800, 0xA200, 0x8A00 }, { 0x1500, 0x5400, 0x5100, 0x4500 } },
	{ { 0x1300, 0x3200, 0x2300, 0x3100 }, { 0x4C00, 0xC800, 0x8C00, 0xC400 } },
	{ { 0x4C00, 0xC800, 0x8C00, 0xC400 }, { 0x1300, 0x3200, 0x2300, 0x3100 } },
	{ { 0x0700, 0x0E00, 0x0B00, 0x0D00 }, { 0x7000, 0xE000, 0xB000, 0xD000 } },
	{ { 0x7000, 0xE000, 0xB000, 0xD000 }, { 0x0700, 0x0E00, 0x0B00, 0x0D00 } }
};

static const limatVector voxel_triangle_vertices[24][3] =
{
	/* Negative X */
	{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 0.0f } },
	/* Positive X */
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
	/* Negative Y */
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f } },
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } },
	/* Positive Y */
	{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
	{ { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
	{ { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f } },
	{ { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
	/* Negative Z */
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
	{ { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f } },
	{ { 0.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } },
	/* Positive Z */
	{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
	{ { 1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } },
	{ { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f, 1.0f } },
	{ { 0.0f, 0.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f } }
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
	int n;
	int tx;
	int ty;
	int tz;
	int mask0;
	int mask1;
	liengBlockBuilderNormal* lookup;
	liengTile tile;
	limatVector* tmp;
	limatVector coord;
	limatVector normal;
	limatVector blockoff;
	limatVector tileoff;
	limatVector vertices[64];
	limdlVertex mdlverts[3];

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

		/* Triangulate smooth surface. */
		c = lieng_voxel_triangulate (tile, vertices);

		/* Triangulate visible sharp faces. */
		mask0 = tile;
		for (i = 0 ; i < 6 ; i++)
		{
			mask1 = lieng_sector_get_voxel (self->sector,
				LIENG_TILES_PER_LINE * bx + tx + voxel_face_normals[i][0],
				LIENG_TILES_PER_LINE * by + ty + voxel_face_normals[i][1],
				LIENG_TILES_PER_LINE * bz + tz + voxel_face_normals[i][2]);
			for (j = n = 0 ; j < 4 && n < 2 ; j++)
			{
				if ((mask0 & voxel_triangle_masks[i][0][j]) == voxel_triangle_masks[i][0][j] &&
				    (mask1 & voxel_triangle_masks[i][1][j]) != voxel_triangle_masks[i][1][j])
				{
					vertices[c++] = voxel_triangle_vertices[4 * i + j][0];
					vertices[c++] = voxel_triangle_vertices[4 * i + j][1];
					vertices[c++] = voxel_triangle_vertices[4 * i + j][2];
					n++;
				}
			}
		}

		/* Check for empty. */
		if (!c)
			continue;

		/* Scale and translate vertices. */
		tileoff = limat_vector_init (tx, ty, tz);
		tileoff = limat_vector_multiply (tileoff, LIENG_TILE_WIDTH);
		for (i = 0 ; i < c ; i++)
		{
			vertices[i] = limat_vector_multiply (vertices[i], LIENG_TILE_WIDTH);
			vertices[i] = limat_vector_add (vertices[i], tileoff);
			vertices[i] = limat_vector_add (vertices[i], blockoff);
		}

		/* Insert collision vertices. */
		tmp = realloc (self->vertices.array, (self->vertices.count + c) * sizeof (limatVector));
		if (tmp == NULL)
			return 0;
		memcpy (tmp + self->vertices.count, vertices, c * sizeof (limatVector));
		self->vertices.array = tmp;
		self->vertices.count += c;

		/* Insert model vertices. */
#ifndef LIENG_DISABLE_GRAPHICS
		if (self->helpers.model != NULL)
		{
			for (i = 0 ; i < c ; i += 3)
			{
				/* Calculate face normal. */
				normal = limat_vector_normalize (limat_vector_cross (
					limat_vector_subtract (vertices[i + 0], vertices[i + 1]),
					limat_vector_subtract (vertices[i + 1], vertices[i + 2])));

				/* Insert model triangle. */
				/* FIXME: Bad material. */
				memset (mdlverts, 0, 3 * sizeof (limdlVertex));
				for (j = 0 ; j < 3 ; j++)
				{
					mdlverts[j].coord = vertices[i + j];
					mdlverts[j].normal = normal;
				}
				limdl_model_insert_triangle (self->helpers.model, 0, mdlverts, NULL);

				/* Create normal lookup. */
				for (j = 0 ; j < 3 ; j++)
				{
					coord = mdlverts[j].coord;
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

	if (self->helpers.model == NULL)
		return 1;

	/* FIXME */
	memset (&material, 0, sizeof (limdlMaterial));
	material.flags = 0;
	material.shininess = 64.0f;
	material.diffuse[0] = 1.0f;
	material.diffuse[1] = 1.0f;
	material.diffuse[2] = 1.0f;
	material.diffuse[3] = 1.0f;
	material.specular[0] = 1.0f;
	material.specular[1] = 1.0f;
	material.specular[2] = 1.0f;
	material.specular[3] = 1.0f;
	material.shader = "lava";
	if (!limdl_model_insert_material (self->helpers.model, &material))
		return 0;

	return 1;
#else
	return 1;
#endif
}

/** @} */
/** @} */

