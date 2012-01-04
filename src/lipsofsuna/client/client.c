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
	int           sync);

static void private_server_main (
	LISysThread* thread,
	void*        data);

static void private_server_shutdown (
	LICliClient* self);

/*****************************************************************************/

LICliClient* licli_client_new (
	LIMaiProgram* program,
	int           width,
	int           height,
	int           fullscreen,
	int           vsync)
{
	LICliClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliClient));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Initialize graphics. */
	if (!private_init (self, program, width, height, fullscreen, vsync))
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

	/* Free the server thread. */
	private_server_shutdown (self);

	/* Free the graphics engine. */
	if (self->render != NULL)
		liren_render_free (self->render);

	lisys_free (self);
}

/**
 * \brief Starts an embedded server.
 *
 * \param self Client.
 * \param args Arguments to pass to the server.
 * \return Nonzero on success.
 */
int licli_client_host (
	LICliClient* self,
	const char*  args)
{
	/* Kill old thread. */
	private_server_shutdown (self);

	/* Create new server. */
	self->server = licli_server_new (self->program->paths->root, self->program->paths->module_name, args);
	if (self->server == NULL)
		return 0;

	/* Create server thread. */
	self->server_thread = lisys_thread_new (private_server_main, self);
	if (self->server_thread == NULL)
	{
		licli_server_free (self->server);
		self->server = NULL;
	}

	return 1;
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
	int           sync)
{
	LIRenVideomode mode;

	/* Initialize the renderer. */
	mode.width = width;
	mode.height = height;
	mode.fullscreen = fullscreen;
	mode.sync = sync;
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

static void private_server_main (
	LISysThread* thread,
	void*        data)
{
	LICliClient* self = data;

	if (!licli_server_main (self->server))
		lisys_error_report ();
	licli_server_free (self->server);
	self->server = NULL;
}

static void private_server_shutdown (
	LICliClient* self)
{
	/* Terminate the server if it's running. If the server closed on its own,
	   for example due to an error its scripts, the server program has already
	   been freed by the server thread but the thread still exists. */
	if (self->server != NULL)
		limai_program_shutdown (self->server->program);

	/* Free the server thread. The server program is guaranteed to be freed
	   by the server thread so all we need to do is to wait for the thread to
	   exit. This doesn't take long since we asked the server to quit already. */
	if (self->server_thread != NULL)
	{
		lisys_thread_free (self->server_thread);
		self->server_thread = NULL;
		lisys_assert (self->server == NULL);
	}
}

/** @} */
/** @} */
