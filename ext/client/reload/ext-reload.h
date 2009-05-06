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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliReload Reload
 * @{
 */

#ifndef __EXT_RELOAD_H__
#define __EXT_RELOAD_H__

#include <client/lips-client.h>
#include <system/lips-system.h>
#include <thread/lips-thread.h>
#include <widget/lips-widget.h>

typedef struct _liextReload liextReload;
struct _liextReload
{
	int queued;
	licalHandle calls[1];
	licliModule* module;
	lisysNotify* notify;
	lithrAsyncCall* worker;
	liwdgWidget* progress;
};

liextReload*
liext_reload_new (licliModule* module);

void
liext_reload_free (liextReload* self);

void
liext_reload_cancel (liextReload* self);

int
liext_reload_run (liextReload* self);

int
liext_reload_get_enabled (const liextReload* self);

int
liext_reload_set_enabled (liextReload* self,
                          int          value);

int
liext_reload_blender (liextReload* self,
                      const char*  src,
                      const char*  dst);

int
liext_reload_image (const char* src,
                    const char* dst);

int
liext_reload_gimp (const char* src,
                   const char* dst);

#endif

/** @} */
/** @} */
/** @} */
