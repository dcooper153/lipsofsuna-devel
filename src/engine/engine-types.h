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

#ifndef __ENGINE_TYPES_H__
#define __ENGINE_TYPES_H__

#include <system/lips-system.h>

/* FIXME */
#define LIENG_ENABLE_GRAPHICS

#define LIENG_PHYSICS_GROUP_STATICS 0x4000
#define LIENG_PHYSICS_GROUP_TILES 0x8000

enum
{
	LIENG_DATA_CLIENT,
	LIENG_DATA_SERVER,
	LIENG_DATA_MAX
};

typedef int liengSceneType;
enum
{
	LIENG_SCENE_NORMAL,
	LIENG_SCENE_SELECTION
};

enum
{
	LIENG_FLAG_REMOTE_SECTORS = 0x01
};

enum
{
	LIENG_OBJECT_FLAG_DIRTY = 0x01,
	LIENG_OBJECT_FLAG_DYNAMIC = 0x02,
	LIENG_OBJECT_FLAG_INVISIBLE = 0x04,
	LIENG_OBJECT_FLAG_SAVE = 0x08
};

typedef uint8_t liengTile;
typedef struct _liengAnimation liengAnimation;
typedef struct _liengCamera liengCamera;
typedef struct _liengEngine liengEngine;
typedef struct _liengModel liengModel;
typedef struct _liengObject liengObject;
typedef struct _liengRender liengRender;
typedef struct _liengResources liengResources;
typedef struct _liengSample liengSample;
typedef struct _liengSector liengSector;
typedef struct _liengSelection liengSelection;

#endif
