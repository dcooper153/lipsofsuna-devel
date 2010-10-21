/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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

#ifndef __PHYSICS_TYPES_H__
#define __PHYSICS_TYPES_H__

#include <lipsofsuna/math.h>

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyObject Object
 * @{
 */

#define LIPHY_GROUP_STATICS 0x4000
#define LIPHY_GROUP_TILES 0x8000

enum _LIPhyControlMode
{
	LIPHY_CONTROL_MODE_NONE,
	LIPHY_CONTROL_MODE_CHARACTER,
	LIPHY_CONTROL_MODE_RIGID,
	LIPHY_CONTROL_MODE_STATIC,
	LIPHY_CONTROL_MODE_VEHICLE,
	LIPHY_CONTROL_MODE_MAX
};

typedef struct _LIPhyContact LIPhyContact;
typedef enum _LIPhyControlMode LIPhyControlMode;
typedef struct _LIPhyObject LIPhyObject;
typedef void (*LIPhyCallback)(LIPhyObject* self, float secs);
typedef void (*LIPhyContactCall)(LIPhyObject* self, LIPhyContact* contact);

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyCollision Collision
 * @{
 */

typedef struct _LIPhyCollision LIPhyCollision;
struct _LIPhyCollision
{
	float fraction;
	LIMatVector normal;
	LIMatVector point;
	LIPhyObject* object;
};

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyConstraint Constraint
 * @{
 */

typedef struct _LIPhyConstraint LIPhyConstraint;

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyContact Contact
 * @{
 */

struct _LIPhyContact
{
	float impulse;
	LIMatVector point;
	LIMatVector normal;
	LIPhyObject* object;
};

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyPhysics Physics
 * @{
 */

/**
 * \brief Default movement speed for objects.
 */
#define LIPHY_DEFAULT_SPEED 3.0f

/**
 * \brief Default collision group.
 */
#define LIPHY_DEFAULT_COLLISION_GROUP 0x0001

/**
 * \brief Default collision mask.
 */
#define LIPHY_DEFAULT_COLLISION_MASK 0xFFFF

typedef void (*liphyTransformCall)(LIPhyObject* object);
typedef struct _LIPhyPhysics LIPhyPhysics;

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhySector Sector
 * @{
 */

typedef struct _LIPhySector LIPhySector;

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyShape Shape
 * @{
 */

typedef struct _LIPhyShape LIPhyShape;

/** @} */
/** @} */

#endif
