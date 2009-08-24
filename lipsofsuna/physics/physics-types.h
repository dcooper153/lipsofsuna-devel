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

#ifndef __PHYSICS_TYPES_H__
#define __PHYSICS_TYPES_H__

#include <math/lips-math.h>

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyObject Object
 * @{
 */

#define LIPHY_GROUP_STATICS 0x4000
#define LIPHY_GROUP_TILES 0x8000

enum _liphyControlMode
{
	LIPHY_CONTROL_MODE_NONE,
	LIPHY_CONTROL_MODE_CHARACTER,
	LIPHY_CONTROL_MODE_RIGID,
	LIPHY_CONTROL_MODE_STATIC,
	LIPHY_CONTROL_MODE_VEHICLE,
	LIPHY_CONTROL_MODE_MAX
};

enum _liphyShapeMode
{
	LIPHY_SHAPE_MODE_BOX,
	LIPHY_SHAPE_MODE_CAPSULE,
	LIPHY_SHAPE_MODE_CONCAVE,
	LIPHY_SHAPE_MODE_CONVEX,
	LIPHY_SHAPE_MODE_MAX
};

typedef struct _liphyContact liphyContact;
typedef enum _liphyControlMode liphyControlMode;
typedef enum _liphyShapeMode liphyShapeMode;
typedef struct _liphyObject liphyObject;
typedef void (*liphyCallback)(liphyObject* self, float secs);
typedef void (*liphyContactCall)(liphyObject* self, liphyContact* contact);

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyCollision Collision
 * @{
 */

typedef struct _liphyCollision liphyCollision;
struct _liphyCollision
{
	float fraction;
	limatVector normal;
	limatVector point;
	liphyObject* object;
};

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyContact Contact
 * @{
 */

struct _liphyContact
{
	float impulse;
	limatVector point;
	limatVector normal;
	liphyObject* object;
};

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyPhysics Physics
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

typedef void (*liphyTransformCall)(liphyObject* object);
typedef struct _liphyPhysics liphyPhysics;

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphySector Sector
 * @{
 */

typedef struct _liphySector liphySector;

/** @} */
/** @} */

/**
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyShape Shape
 * @{
 */

typedef struct _liphyShape liphyShape;

/** @} */
/** @} */

#endif
