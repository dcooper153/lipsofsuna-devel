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
 * \addtogroup licli Client
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
private_init_camera (LICliClient* self);

static int
private_init_paths (LICliClient* self,
                    const char*  path,
                    const char*  name);

static int
private_init_render (LICliClient* self);

static int
private_init_script (LICliClient* self);

static int
private_init_widgets (LICliClient* self);

static void
private_server_main (LIThrThread* thread,
                     void*        data);

static int private_select (
	LICliClient*    self,
	LIRenSelection* selection);

static int private_update (
	LICliClient* self,
	float        secs);

/*****************************************************************************/

LICliClient* licli_client_new (
	LIVidCalls* video,
	const char* path,
	const char* name)
{
	LICliClient* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliClient));
	if (self == NULL)
		return NULL;
	self->video = *video;

	/* Allocate root directory. */
	self->root = listr_dup (path);
	if (self->root == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Initialize SDL. */
	if (self->video.SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_free (self->root);
		lisys_free (self);
		return NULL;
	}

	/* Create window. */
	self->video.SDL_EnableUNICODE (1);
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
	self->video.SDL_Quit ();
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

	/* Free camera. */
	if (self->camera != NULL)
	{
		lialg_camera_free (self->camera);
		self->camera = NULL;
	}

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

	if (self->widgets != NULL)
	{
		liwdg_manager_free (self->widgets);
		self->widgets = NULL;
	}
	if (self->server_thread != NULL)
	{
		limai_program_shutdown (self->server->program);
		lithr_thread_free (self->server_thread);
		self->server_thread = NULL;
	}
	lisys_assert (self->server == NULL);
	if (self->paths != NULL)
	{
		lipth_paths_free (self->paths);
		self->paths = NULL;
	}
	lisys_free (self->camera_node);
	lisys_free (self->path);
	lisys_free (self->name);
	self->camera_node = NULL;
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
	if (self->server_thread != NULL)
	{
		lisys_assert (self->server != NULL);
		limai_program_shutdown (self->server->program);
		lithr_thread_free (self->server_thread);
		lisys_assert (self->server == NULL);
	}

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
 * \param client Client.
 * \param path Package root directory.
 * \param name Module name.
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
	lieng_engine_set_local_range (self->engine, LINET_RANGE_CLIENT_START, LINET_RANGE_CLIENT_END);
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
	    !private_init_widgets (self) ||
	    !private_init_camera (self) ||
	    !private_init_script (self) ||
	    !licli_render_init (self) ||
	    !licli_client_init_callbacks_misc (self) ||
	    !licli_client_init_callbacks_widget (self))
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
		self->video.SDL_ShowCursor (SDL_DISABLE);
		self->video.SDL_EventState (SDL_MOUSEMOTION, SDL_IGNORE);
		self->video.SDL_WarpMouse (cx, cy);
		self->video.SDL_EventState (SDL_MOUSEMOTION, SDL_ENABLE);
		self->video.SDL_WM_GrabInput (SDL_GRAB_ON);
	}
	else
	{
		self->video.SDL_ShowCursor (SDL_ENABLE);
		self->video.SDL_WM_GrabInput (SDL_GRAB_OFF);
	}
}

/*****************************************************************************/

static int
private_init_camera (LICliClient* self)
{
	GLint viewport[4];

	self->camera_node = strdup ("#camera");
	if (self->camera_node == NULL)
		return 0;
	self->camera = lialg_camera_new ();
	if (self->camera == NULL)
		return 0;
	glGetIntegerv (GL_VIEWPORT, viewport);
	lialg_camera_set_driver (self->camera, LIALG_CAMERA_THIRDPERSON);
	lialg_camera_set_viewport (self->camera, viewport[0], viewport[1], viewport[2], viewport[3]);

	return 1;
}

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
	    !liscr_script_create_class (self->script, "Data", liscr_script_data, self->script) ||
	    !liscr_script_create_class (self->script, "Event", liscr_script_event, self->script) ||
	    !liscr_script_create_class (self->script, "Client", licli_script_client, self) ||
	    !liscr_script_create_class (self->script, "Group", licli_script_group, self) ||
	    !liscr_script_create_class (self->script, "Light", licli_script_light, self) ||
	    !liscr_script_create_class (self->script, "Object", liscr_script_object, self->program) ||
	    !liscr_script_create_class (self->script, "Packet", liscr_script_packet, self->script) ||
	    !liscr_script_create_class (self->script, "Path", liscr_script_path, self->script) ||
	    !liscr_script_create_class (self->script, "Program", liscr_script_program, self->program) ||
	    !liscr_script_create_class (self->script, "Quaternion", liscr_script_quaternion, self->script) ||
	    !liscr_script_create_class (self->script, "Scene", licli_script_scene, self) ||
	    !liscr_script_create_class (self->script, "Vector", liscr_script_vector, self->script) ||
	    !liscr_script_create_class (self->script, "Widget", licli_script_widget, self))
		return 0;

	return 1;
}

static int
private_init_widgets (LICliClient* self)
{
	self->widgets = liwdg_manager_new (&self->video, self->callbacks, self->path);
	if (self->widgets == NULL)
		return 0;
	liwdg_manager_set_size (self->widgets, self->window->mode.width, self->window->mode.height);

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
	int w;
	int h;
	SDL_Event event;

	/* Invoke input callbacks. */
	while (self->video.SDL_PollEvent (&event))
		lical_callbacks_call (self->callbacks, self->engine, "event", lical_marshal_DATA_PTR, &event);

	/* Render widgets. */
	licli_window_get_size (self->window, &w, &h);
	liwdg_manager_set_size (self->widgets, w, h);
	lialg_camera_set_viewport (self->camera, 0, 0, w, h);
	liwdg_manager_render (self->widgets);
	self->video.SDL_GL_SwapBuffers ();

	return 1;
}

/** @} */
/** @} */
