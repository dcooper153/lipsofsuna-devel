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

struct _liextBlockKey
{
	int sector;
	int block;
};

struct _liextListenerBlock
{
	int stamp;
};

struct _liextListener
{
	int moved;
	int radius;
	lialgMemdic* cache;
	liengObject* object;
	liextModule* module;
	limatVector center;
};

liextListener*
liext_listener_new (liextModule* module,
                    liengObject* object,
                    int          radius);

void
liext_listener_free (liextListener* self);

int
liext_listener_cache (liextListener* self,
                      int            sector,
                      int            block,
                      int            stamp);

int
liext_listener_update (liextListener* self,
                       float          secs);

int
liext_listener_get_cached (const liextListener* self,
                           int                  sector,
                           int                  block,
                           int                  stamp);

#endif

/** @} */
/** @} */
/** @} */
