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

#endif
