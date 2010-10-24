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
 * \addtogroup LICli Client
 * @{
 * \addtogroup LICliClient Client
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "client.h"
#include "client-callbacks.h"
#include "client-script.h"
#include "client-window.h"

static int
private_init_paths (LICliClient* self,
                    const char*  path,
                    const char*  name);

static int
private_init_render (LICliClient* self);

static int
private_init_script (LICliClient* self);

static void
private_server_main (LIThrThread* thread,
                     void*        data);

static void private_server_shutdown (
	LICliClient* self);

static int private_select (
	LICliClient*    self,
	LIRenSelection* selection);

static int private_update (
	LICliClient* self,
	float        secs);

/*****************************************************************************/

LICliClient* licli_client_new (
	const char* path,
	const char* name)
{
	LICliClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliClient));
	if (self == NULL)
		return NULL;

	/* Allocate root directory. */
	self->root = listr_dup (path);
	if (self->root == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize SDL. */
	if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_free (self->root);
		lisys_free (self);
		return NULL;
	}

	/* Create window. */
	SDL_EnableUNICODE (1);
	self->window = licli_window_new (self);
	if (self->window == NULL)
	{
		licli_client_free (self);
		return NULL;
	}

	return self;
}

void licli_client_free (
	LICliClient* self)
{
	licli_client_free_module (self);
	if (self->window != NULL)
		licli_window_free (self->window);
	SDL_Quit ();
	lisys_free (self->root);
	lisys_free (self);
}

/**
 * \brief Frees the currently loaded module.
 *
 * \param self Client.
 */
void licli_client_free_module (
	LICliClient* self)
{
	/* Invoke callbacks. */
	if (self->callbacks != NULL)
		lical_callbacks_call (self->callbacks, self, "client-free", lical_marshal_DATA);

	/* Free program. */
	if (self->program != NULL)
	{
		limai_program_remove_component (self->program, "client");
		limai_program_free (self->program);
		self->sectors = NULL;
		self->callbacks = NULL;
		self->engine = NULL;
		self->program = NULL;
		self->script = NULL;
	}

	/* Free the server. */
	private_server_shutdown (self);

	if (self->paths != NULL)
	{
		lipth_paths_free (self->paths);
		self->paths = NULL;
	}
	lisys_free (self->path);
	lisys_free (self->name);
	self->path = NULL;
	self->name = NULL;
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
	self->server = liser_server_new (self->paths, args);
	if (self->server == NULL)
		return 0;

	/* Create server thread. */
	self->server_thread = lithr_thread_new (private_server_main, self);
	if (self->server_thread == NULL)
	{
		liser_server_free (self->server);
		self->server = NULL;
	}

	return 1;
}

/**
 * \brief Loads a module.
 *
 * \param self Client.
 * \param name Module name.
 * \param args Module arguments.
 */
int licli_client_load_module (
	LICliClient* self,
	const char*  name,
	const char*  args)
{
	/* Initialize paths. */
	if (!private_init_paths (self, self->root, name))
		return 0;

	/* Create program. */
	self->program = limai_program_new (self->paths, args);
	if (self->program == NULL)
	{
		lipth_paths_free (self->paths);
		self->paths = NULL;
		return 0;
	}
	self->sectors = self->program->sectors;
	self->callbacks = self->program->callbacks;
	self->engine = self->program->engine;
	self->paths = self->program->paths;
	self->script = self->program->script;
	lical_callbacks_insert (self->callbacks, self->engine, "tick", -1000, private_update, self, NULL);
	lical_callbacks_insert (self->callbacks, self->engine, "select", 32768, private_select, self, NULL);

	/* Store credentials. */
	self->name = listr_dup (name);
	if (self->name == NULL)
	{
		licli_client_free_module (self);
		return 0;
	}

	/* Initialize client component. */
	if (!limai_program_insert_component (self->program, "client", self))
	{
		licli_client_free_module (self);
		return 0;
	}
	if (!private_init_render (self) ||
	    !private_init_script (self) ||
	    !licli_client_init_callbacks_misc (self))
	{
		licli_client_free_module (self);
		return 0;
	}

	return 1;
}

int licli_client_main (
	LICliClient* self)
{
	return limai_program_execute_script (self->program, "client/main.lua");
}

/**
 * \brief Returns nonzero if movement mode is active.
 *
 * \param self Client.
 * \return Boolean.
 */
int licli_client_get_moving (
	LICliClient* self)
{
	return self->moving;
}

/**
 * \brief Enables or disables movement mode.
 *
 * When the movement mode is enabled, all mouse events are passed directly to
 * the scripts. Otherwise, the events are first passed to the user interface.
 *
 * \param self Client.
 * \param value Nonzero for movement mode, zero for user interface mode
 */
void licli_client_set_moving (
	LICliClient* self,
	int          value)
{
	int cx;
	int cy;

	self->moving = value;
	if (value)
	{
		cx = self->window->mode.width / 2;
		cy = self->window->mode.height / 2;
		SDL_ShowCursor (SDL_DISABLE);
		SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		SDL_WarpMouse (cx, cy);
		SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
#ifdef ENABLE_GRABS
		SDL_WM_GrabInput (SDL_GRAB_ON);
#endif
	}
	else
	{
		SDL_ShowCursor (SDL_ENABLE);
#ifdef ENABLE_GRABS
		SDL_WM_GrabInput (SDL_GRAB_OFF);
#endif
	}
}

/*****************************************************************************/

static int
private_init_paths (LICliClient* self,
                    const char*  path,
                    const char*  name)
{
	self->paths = lipth_paths_new (path, name);
	if (self->paths == NULL)
		return 0;
	self->path = listr_dup (self->paths->module_data);
	if (self->path == NULL)
		return 0;

	return 1;
}

static int
private_init_render (LICliClient* self)
{
	self->render = liren_render_new (self->paths->module_data);
	if (self->render == NULL)
		return 0;
	self->scene = liren_scene_new (self->render);
	if (self->scene == NULL)
		return 0;

	return 1;
}

static int
private_init_script (LICliClient* self)
{
	if (!liscr_script_create_class (self->script, "Class", liscr_script_class, self->script) ||
	    !liscr_script_create_class (self->script, "Event", liscr_script_event, self->script) ||
	    !liscr_script_create_class (self->script, "Client", licli_script_client, self) ||
	    !liscr_script_create_class (self->script, "Light", licli_script_light, self) ||
	    !liscr_script_create_class (self->script, "Model", liscr_script_model, self->program) ||
	    !liscr_script_create_class (self->script, "Object", liscr_script_object, self->program) ||
	    !liscr_script_create_class (self->script, "Packet", liscr_script_packet, self->script) ||
	    !liscr_script_create_class (self->script, "Path", liscr_script_path, self->script) ||
	    !liscr_script_create_class (self->script, "Program", liscr_script_program, self->program) ||
	    !liscr_script_create_class (self->script, "Quaternion", liscr_script_quaternion, self->script) ||
	    !liscr_script_create_class (self->script, "Vector", liscr_script_vector, self->script))
		return 0;

	return 1;
}

static void
private_server_main (LIThrThread* thread,
                     void*        data)
{
	LICliClient* self = data;

	if (!liser_server_main (self->server))
		lisys_error_report ();
	liser_server_free (self->server);
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
		lithr_thread_free (self->server_thread);
		self->server_thread = NULL;
		lisys_assert (self->server == NULL);
	}
}

static int private_select (
	LICliClient*    self,
	LIRenSelection* selection)
{
	LIEngObject* object;

	if (selection != NULL)
	{
		object = lieng_engine_find_object (self->engine, selection->object);
		if (object != NULL && object->script != NULL)
		{
			limai_program_event (self->program, "select",
				"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
			return 1;
		}
	}

	return 1;
}

static int private_update (
	LICliClient* self,
	float        secs)
{
	SDL_Event event;

	/* Invoke input callbacks. */
	while (SDL_PollEvent (&event))
		lical_callbacks_call (self->callbacks, self->engine, "event", lical_marshal_DATA_PTR, &event);

	return 1;
}

/** @} */
/** @} */
