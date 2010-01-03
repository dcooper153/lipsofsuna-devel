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

/**
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvVoxel Voxel
 * @{
 */

#ifndef __EXT_LISTENER_H__
#define __EXT_LISTENER_H__

#include "ext-module.h"

struct _LIExtBlockKey
{
	int sector;
	int block;
};

struct _LIExtListenerBlock
{
	int stamp;
};

struct _LIExtListener
{
	int moved;
	int radius;
	LIAlgMemdic* cache;
	LIEngObject* object;
	LIExtModule* module;
	LIMatVector center;
};

LIExtListener*
liext_listener_new (LIExtModule* module,
                    LIEngObject* object,
                    int          radius);

void
liext_listener_free (LIExtListener* self);

int
liext_listener_cache (LIExtListener* self,
                      int            sector,
                      int            block,
                      int            stamp);

int
liext_listener_update (LIExtListener* self,
                       float          secs);

int
liext_listener_get_cached (const LIExtListener* self,
                           int                  sector,
                           int                  block,
                           int                  stamp);

#endif

/** @} */
/** @} */
/** @} */
