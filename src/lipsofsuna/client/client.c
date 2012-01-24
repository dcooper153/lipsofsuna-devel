/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LICli Client
 * @{
 * \addtogroup LICliClient Client
 * @{
 */

#include "lipsofsuna/system.h"
#include "client.h"
#include "client-script.h"

static int private_init (
	LICliClient*  self,
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           sync,
	int           multisamples);

/*****************************************************************************/

LICliClient* licli_client_new (
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           vsync,
	int           multisamples)
{
	LICliClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliClient));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize graphics. */
	if (!private_init (self, program, width, height, fullscreen, vsync, multisamples))
	{
		licli_client_free (self);
		return NULL;
	}

	return self;
}

void licli_client_free (
	LICliClient* self)
{
	/* Invoke callbacks. */
	lical_callbacks_call (self->program->callbacks, "client-free", lical_marshal_DATA);

	/* Remove the client component. */
	if (self->program != NULL)
	{
		limai_program_remove_component (self->program, "client");
		limai_program_remove_component (self->program, "render");
	}

	/* Free the graphics engine. */
	if (self->render != NULL)
		liren_render_free (self->render);

	lisys_free (self);
}

int licli_client_set_videomode (
	LICliClient* self,
	int          width,
	int          height,
	int          fullscreen,
	int          sync)
{
	LIRenVideomode mode;

	mode.width = width;
	mode.height = height;
	mode.fullscreen = fullscreen;
	mode.sync = sync;
	if (!liren_render_set_videomode (self->render, &mode))
		return 0;
	self->mode = mode;

	return 1;
}

/*****************************************************************************/

static int private_init (
	LICliClient*  self,
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           sync,
	int           multisamples)
{
	LIRenVideomode mode;

	/* Initialize the renderer. */
	mode.width = width;
	mode.height = height;
	mode.fullscreen = fullscreen;
	mode.sync = sync;
	mode.multisamples = multisamples;
	self->render = liren_render_new (self->program->paths, &mode);
	if (self->render == NULL)
		return 0;
	self->mode = mode;

	/* Register component. */
	if (!limai_program_insert_component (self->program, "client", self))
		return 0;
	if (!limai_program_insert_component (self->program, "render", self->render))
		return 0;

	/* Register classes. */
	liscr_script_set_userdata (program->script, LICLI_SCRIPT_CLIENT, self);
	licli_script_client (program->script);

	return 1;
}

/** @} */
/** @} */
