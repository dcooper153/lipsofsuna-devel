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

enum
{
	LIENG_DATA_CLIENT,
	LIENG_DATA_SERVER,
	LIENG_DATA_MAX
};

enum
{
	LIENG_FLAG_REMOTE_SECTORS = 0x01
};

enum
{
	LIENG_CALLBACK_FREE, /* (data, engine) */
	LIENG_CALLBACK_MODEL_NEW, /* (data, model) */
	LIENG_CALLBACK_MODEL_FREE, /* (data, model) */
	LIENG_CALLBACK_OBJECT_NEW, /* (data, object) */
	LIENG_CALLBACK_OBJECT_FREE, /* (data, object) */
	LIENG_CALLBACK_OBJECT_MODEL, /* (data, object, model) */
	LIENG_CALLBACK_OBJECT_TRANSFORM, /* (data, object, transform) */
	LIENG_CALLBACK_OBJECT_VISIBILITY, /* (data, object, boolean) */
	LIENG_CALLBACK_SECTOR_LOAD, /* (data, sector) */
	LIENG_CALLBACK_SECTOR_UNLOAD, /* (data, sector) */
	LIENG_CALLBACK_LAST
};

enum _liengMatrix
{
	LIENG_MATRIX_LOCAL,
	LIENG_MATRIX_WORLD,
};

enum
{
	LIENG_OBJECT_FLAG_DIRTY = 0x0001,
	LIENG_OBJECT_FLAG_DYNAMIC = 0x0002,
	LIENG_OBJECT_FLAG_INVISIBLE = 0x0004,
	LIENG_OBJECT_FLAG_SAVE = 0x0008,
	LIENG_OBJECT_FLAG_RELOAD = 0x8000
};

typedef struct _liengAnimation liengAnimation;
typedef struct _liengConstraint liengConstraint;
typedef struct _liengEngine liengEngine;
typedef enum _liengMatrix liengMatrix;
typedef struct _liengModel liengModel;
typedef struct _liengObject liengObject;
typedef struct _liengRender liengRender;
typedef struct _liengResources liengResources;
typedef struct _liengSample liengSample;
typedef struct _liengSector liengSector;
typedef struct _liengSelection liengSelection;

#endif
