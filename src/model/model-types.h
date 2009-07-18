/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup limdl Model
 * @{
 * \addtogroup limdlBone Bone
 * @{
 */

#ifndef __MODEL_TYPES_H__
#define __MODEL_TYPES_H__

typedef struct _limdlBone limdlBone;

/**
 * @}
 * \addtogroup limdlLight Light
 * @{
 */

enum
{
	LIMDL_LIGHT_FLAG_SHADOW = 0x01
};

typedef struct _limdlLight limdlLight;

/**
 * @}
 * \addtogroup limdlModel Model
 * @{
 */

typedef struct _limdlModel limdlModel;

/**
 * @}
 * \addtogroup limdlNode Node
 * @{
 */

enum _limdlNodeType
{
	LIMDL_NODE_BONE,
	LIMDL_NODE_EMPTY,
	LIMDL_NODE_LIGHT,
};

typedef enum _limdlNodeType limdlNodeType;
typedef struct _limdlNode limdlNode;

/**
 * @}
 * \addtogroup limdlHair Hair
 * @{
 */

typedef struct _limdlHair limdlHair;
typedef struct _limdlHairNode limdlHairNode;

#endif

/** @} */
/** @} */
