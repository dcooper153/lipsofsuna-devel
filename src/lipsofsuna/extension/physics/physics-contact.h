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

#ifndef __PHYSICS_CONTACT_H__
#define __PHYSICS_CONTACT_H__

#include "lipsofsuna/math.h"
#include "lipsofsuna/system.h"

typedef struct _LIPhyContact LIPhyContact;
struct _LIPhyContact
{
	int object_id;
	int object1_id;
	int terrain_id;
	int terrain_tile[3];
	int voxels_id;
	float fraction;
	float impulse;
	LIMatVector point;
	LIMatVector normal;
};

static inline void liphy_contact_init (
	LIPhyContact* self)
{
	memset (self, 0, sizeof (LIPhyContact));
	self->fraction = 1.0f;
	self->normal = limat_vector_init (0.0f, 1.0f, 0.0f);
}

static inline void liphy_contact_init_from_voxel (
	LIPhyContact*      self,
	float              tile_width,
	const LIMatVector* ray_src,
	const LIMatVector* ray_dst,
	const LIMatVector* point,
	const LIMatVector* tile)
{
	LIMatVector center;

	memset (self, 0, sizeof (LIPhyContact));
	self->fraction =
		limat_vector_get_length (limat_vector_subtract (*point, *ray_src)) /
		(limat_vector_get_length (limat_vector_subtract (*ray_dst, *ray_src)) + 0.00001f);
	center = limat_vector_add (*tile, limat_vector_init (0.5, 0.5f, 0.5f));
	self->normal = limat_vector_subtract (*point, center);
	self->normal = limat_vector_normalize (self->normal);
	self->point = limat_vector_multiply (*point, tile_width);
	self->voxels_id = 1;
	self->terrain_tile[0] = (int) tile->x;
	self->terrain_tile[1] = (int) tile->y;
	self->terrain_tile[2] = (int) tile->z;
}

#endif
