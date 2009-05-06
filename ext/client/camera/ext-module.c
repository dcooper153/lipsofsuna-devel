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
 * \addtogroup liextcliCamera Camera
 * @{
 */

#include <client/lips-client.h>
#include "ext-module.h"

static int
private_tick (liextModule* self,
              float        secs);

/*****************************************************************************/

licliExtensionInfo liextInfo =
{
	LICLI_EXTENSION_VERSION, "Camera",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliModule* module)
{
	liextModule* self;

	self = calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->calls[0] = lieng_engine_call_insert (module->engine, LICLI_CALLBACK_TICK, 0, private_tick, self);

	liscr_script_insert_class (module->script, "Camera", liextCameraScript, self);

	return self;
}

void
liext_module_free (liextModule* self)
{
	/* FIXME: Remove the class here. */
	lieng_engine_call_remove (self->module->engine, LICLI_CALLBACK_TICK, self->calls[0]);
	free (self);
}

/*****************************************************************************/

static int
private_tick (liextModule* self,
              float        secs)
{
	lieng_camera_move (self->module->camera, secs * self->move);
	lieng_camera_tilt (self->module->camera, secs * self->tilt);
	lieng_camera_turn (self->module->camera, secs * self->turn);
	lieng_camera_zoom (self->module->camera, secs * self->zoom);

	return 1;
}

/** @} */
/** @} */
/** @} */
