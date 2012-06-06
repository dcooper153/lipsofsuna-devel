/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "module.h"

LIExtHeightmap* liext_heightmap_new (
	LIExtHeightmapModule* module,
	LIImgImage*           image,
	const LIMatVector*    position,
	int                   size,
	float                 spacing,
	float                 scaling)
{
	int x;
	int z;
	float h;
	LIExtHeightmap* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtHeightmap));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->position = *position;
	self->size = size;
	self->spacing = spacing;
	self->min = 0.0f;
	self->max = scaling;
	self->visible = 1;

	/* Allocate heights. */
	if (size)
	{
		self->heights = lisys_calloc (size * size, sizeof (float));
		if (self->heights == NULL)
		{
			liext_heightmap_free (self);
			return NULL;
		}
	}

	/* Add to the dictionary. */
	self->id = lialg_u32dic_insert_auto (module->heightmaps, &module->program->random, self);
	if (!self->id)
	{
		liext_heightmap_free (self);
		return NULL;
	}

	/* Copy heights from the image. */
	if (image != NULL && image->width && image->height)
	{
		for (z = 0 ; z < size && z < image->height ; z++)
		{
			for (x = 0 ; x < size && x < image->width ; x++)
			{
				h = scaling / 255.0f * ((uint8_t*) image->pixels)[4 * (x + z * image->width)];
				self->heights[x + z * size] = h;
			}
		}
	}

	/* Call hooks. */
	if (module->physics_hooks != NULL)
		module->physics_hooks->init (module->physics_hooks->data, self);
	if (module->render_hooks != NULL)
		module->render_hooks->init (module->render_hooks->data, self);

	return self;
}

void liext_heightmap_free (
	LIExtHeightmap* self)
{
	/* Call hooks. */
	if (self->module->physics_hooks != NULL)
		self->module->physics_hooks->free (self->module->physics_hooks->data, self);
	if (self->module->render_hooks != NULL)
		self->module->render_hooks->free (self->module->render_hooks->data, self);

	/* Register from the dictionary. */
	lialg_u32dic_remove (self->module->heightmaps, self->id);

	/* Free self. */
	lisys_free (self->heights);
	lisys_free (self);
}

int liext_heightmap_get_height (
	LIExtHeightmap* self,
	float           x,
	float           z,
	int             clamp,
	float*          result)
{
	float local_x;
	float local_z;
	int vx;
	int vz;
	float bx;
	float bz;
	int i;
	int j;
	float h;
	LIMatPlane plane;
	LIMatVector v[2][2];
	LIMatVector src;
	LIMatVector dst;
	LIMatVector res;

	/* Translate and scale the point to the heightmap space. */
	/* The space between two vertices is one unit and the first vertex is at the origin. */
	local_x = (x - self->position.x) / self->spacing + 0.5f * (self->size - 1);
	local_z = (z - self->position.z) / self->spacing + 0.5f * (self->size - 1);

	/* Clamp the point to terrain bounds if clamping is enabled.
	   Otherwise, return 0 to indicate a miss. */
	if (clamp)
	{
		local_x = LIMAT_MAX (0.0f, local_x);
		local_z = LIMAT_MAX (0.0f, local_z);
		local_x = LIMAT_MIN (self->size - 1.001f, local_x);
		local_z = LIMAT_MIN (self->size - 1.001f, local_z);
	}
	else if (local_x < 0.0f || local_x >= self->size - 1 ||
	         local_z < 0.0f || local_z >= self->size - 1)
		return 0;

	/* Calculate the position relative to the four nearby vertices. */
	/* At this point, it's guaranteed that there's at least one more
	   vertex available along both axes. Hence, we can interpolate
	   without worrying about going out of bounds. */
	vx = (int) local_x;
	vz = (int) local_z;
	bx = local_x - vx;
	bz = local_z - vz;

	/* Calculate the positions of the four vertices. */
	/* Since only Y needs to be in the world space, we can set X and Z
	   to the [0,1] range and use the sampling position within the quad
	   when calculating the plane intersection. */
	for (j = 0 ; j < 2 ; j++)
	{
		for (i = 0 ; i < 2 ; i++)
		{
			h = self->position.y + self->heights[(vx + i) + (vz + j) * self->size];
			v[i][j] = limat_vector_init (i, h, j);
		}
	}

	/* The way how the quad is triangulated depends on the row index
	   due to the triangulation scheme used by Ogre and Bullet. We use
	   the algorithm from Bullet source code. */
	if (!(vz % 2))
	{
		/* 3---2
		   | / | Even rows.
		   0---1 */
		if (bz > bx)
			limat_plane_init_from_points (&plane, &(v[0][0]), &(v[1][1]), &(v[0][1]));
		else
			limat_plane_init_from_points (&plane, &(v[0][0]), &(v[1][0]), &(v[1][1]));
	}
	else
	{
		/* 3---2
		   | \ | Odd rows.
		   0---1 */
		if (1.0 - bz > bx)
			limat_plane_init_from_points (&plane, &(v[0][0]), &(v[1][0]), &(v[0][1]));
		else
			limat_plane_init_from_points (&plane, &(v[1][0]), &(v[1][1]), &(v[0][1]));
	}

	/* Solve the height with a plane intersection. */
	src = limat_vector_init (bx, 0.0, bz);
	dst = limat_vector_init (bx, 1.0, bz);
	res.y = 0;
	limat_plane_intersects_line (&plane, &src, &dst, &res);
	*result = res.y;

	return 1;
}

/** @} */
/** @} */
