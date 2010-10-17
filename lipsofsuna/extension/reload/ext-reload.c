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

#include <sys/stat.h>
#include "ext-module.h"
#include "ext-reload.h"

static int private_callback_tick (
	LIExtReload* self,
	float        secs);

static void private_reload_image (
	LIExtReload* self,
	const char*  name);

static void private_reload_model (
	LIExtReload* self,
	const char*  name);

/*****************************************************************************/

LIExtReload* liext_reload_new (
	LIMaiProgram* program)
{
	LIExtReload* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtReload));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");

	/* Allocate reloader. */
	self->reload = lirel_reload_new (program->paths);
	if (self->reload == NULL)
	{
		liext_reload_free (self);
		return NULL;
	}
	lirel_reload_set_image_callback (self->reload, private_reload_image, self);
	lirel_reload_set_model_callback (self->reload, private_reload_model, self);

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_callback_tick, self, self->calls + 0))
	{
		liext_reload_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Reload", liext_script_reload, self);

	return self;
}

void liext_reload_free (
	LIExtReload* self)
{
	if (self->reload != NULL)
		lirel_reload_free (self->reload);
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/**
 * \brief Updates the status of the reloader.
 *
 * \param self Reload.
 */
void liext_reload_update (
	LIExtReload* self)
{
	/* Update reloader state. */
	lirel_reload_update (self->reload);
}

int liext_reload_get_enabled (
	const LIExtReload* self)
{
	return lirel_reload_get_enabled (self->reload);
}

int liext_reload_set_enabled (
	LIExtReload* self,
	int          value)
{
	return lirel_reload_set_enabled (self->reload, value);
}

/*****************************************************************************/

/**
 * \brief Monitors data files for changes if enabled.
 *
 * \param self Reload.
 */
static int private_callback_tick (
	LIExtReload* self,
	float        secs)
{
	liext_reload_update (self);

	return 1;
}

static void private_reload_image (
	LIExtReload* self,
	const char*  name)
{
	liren_render_load_image (self->client->render, name);
	limai_program_event (self->program, "reload-image",
		"file", LISCR_TYPE_STRING, name, NULL);
}

static void private_reload_model (
	LIExtReload* self,
	const char*  name)
{
	limai_program_event (self->program, "reload-model",
		"file", LISCR_TYPE_STRING, name, NULL);
}

/** @} */
/** @} */
/** @} */
