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
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliReload Reload
 * @{
 */

#ifndef __EXT_RELOAD_H__
#define __EXT_RELOAD_H__

#include <lipsofsuna/client.h>
#include <lipsofsuna/reload.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/thread.h>
#include <lipsofsuna/widget.h>

typedef struct _LIExtReload LIExtReload;
struct _LIExtReload
{
	int queued;
	LICalHandle calls[1];
	LICliClient* client;
	LIRelReload* reload;
	LIWdgWidget* progress;
};

LIExtReload*
liext_reload_new (LICliClient* client);

void
liext_reload_free (LIExtReload* self);

void
liext_reload_cancel (LIExtReload* self);

int
liext_reload_run (LIExtReload* self);

int
liext_reload_get_enabled (const LIExtReload* self);

int
liext_reload_set_enabled (LIExtReload* self,
                          int          value);

#endif

/** @} */
/** @} */
/** @} */
