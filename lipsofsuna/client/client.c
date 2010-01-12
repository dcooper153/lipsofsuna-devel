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
#include <assert.h>
#include <lipsofsuna/network.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "client.h"
#include "client-callbacks.h"
#include "client-script.h"
#include "client-window.h"

#define FPS_TICKS 32

static void
private_free_module (LICliClient* self);

static int
private_load_module (LICliClient* self,
                     const char*  path,
                     const char*  name,
                     const char*  login,
                     const char*  password);

static int
private_init_bindings (LICliClient* self);

static int
private_init_camera (LICliClient* self);

static int
private_init_engine (LICliClient* self);

static int
private_init_extensions (LICliClient* self);

static int
private_init_paths (LICliClient* self,
                    const char*  path,
                    const char*  name);

static int
private_init_script (LICliClient* self);

static int
private_init_widgets (LICliClient* self);

static void
private_server_main (LIThrThread* thread,
                     void*        data);

/*****************************************************************************/


LICliClient*
licli_client_new (LIVidCalls* video,
                  const char* path,
                  const char* name)
{
	LICliClient* self;
	/* FIXME: Login name and password not supported. */
	const char* login = "none";
	const char* password = "none";

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

	/* Load module. */
	if (!private_load_module (self, path, name, login, password))
	{
		licli_client_free (self);
		return NULL;
	}

	return self;
}

void
licli_client_free (LICliClient* self)
{
	private_free_module (self);
	if (self->window != NULL)
		licli_window_free (self->window);
	self->video.SDL_Quit ();
	lisys_free (self->root);
	lisys_free (self);
}

/**
 * \brief Connects to a server.
 *
 * \param self Client.
 * \param name Login name or NULL.
 * \param pass Login password or NULL.
 * \return Nonzero on success.
 */
int
licli_client_connect (LICliClient* self,
                      const char*  name,
                      const char*  pass)
{
	if (name == NULL) name = self->login;
	if (pass == NULL) pass = self->password;
	self->network = licli_network_new (self, name, pass);
	if (self->network == NULL)
		return 0;
	return 1;
}

/**
 * \brief Finds an extension by name.
 *
 * \param self Client.
 * \param name Extension name.
 * \return Extension or NULL.
 */
LICliExtension*
licli_client_find_extension (LICliClient* self,
                             const char*  name)
{
	return lialg_strdic_find (self->extensions, name);
}

/**
 * \brief Finds and object by object number.
 *
 * If the game isn't networked, NULL is returned.
 *
 * \param self Client.
 * \param id Object number.
 * \return Object owned by the module or NULL.
 */
LIEngObject*
licli_client_find_object (LICliClient* self,
                          uint32_t     id)
{
	return lieng_engine_find_object (self->engine, id);
}

/**
 * \brief Starts an embedded server.
 *
 * \param self Client.
 * \return Nonzero on success.
 */
int
licli_client_host (LICliClient* self)
{
	/* Kill old thread. */
	if (self->server_thread != NULL)
	{
		assert (self->server != NULL);
		self->server->quit = 1;
		lithr_thread_free (self->server_thread);
		assert (self->server == NULL);
	}

	/* Create new server. */
	self->server = liser_server_new (self->paths);
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
 * \brief Loads an extension.
 *
 * \param self Client.
 * \param name Extensions name.
 * \return Nonzero on success.
 */
int
licli_client_load_extension (LICliClient* self,
                             const char*  name)
{
	char* path;
	LISysModule* module;
	LICliExtension* extension;
	LICliExtensionInfo* info;

	/* Check if already loaded. */
	module = lialg_strdic_find (self->extensions, name);
	if (module != NULL)
		return 1;

	/* Construct full path. */
	path = lisys_path_format (
		self->paths->global_exts, LISYS_PATH_SEPARATOR,
		"lib", name, "-cli.", LISYS_EXTENSION_DLL, NULL);
	if (path == NULL)
		return 0;

	/* Open module file. */
	module = lisys_module_new (path, 0);
	lisys_free (path);
	if (module == NULL)
		goto error;

	/* Find module info. */
	info = lisys_module_symbol (module, "liextInfo");
	if (info == NULL)
	{
		lisys_error_set (EINVAL, "no module info");
		lisys_module_free (module);
		goto error;
	}
	if (info->version != LICLI_EXTENSION_VERSION)
	{
		lisys_error_set (EINVAL, "invalid module version");
		lisys_module_free (module);
		goto error;
	}
	if (info->name == NULL || info->init == NULL || info->free == NULL)
	{
		lisys_error_set (EINVAL, "invalid module format");
		lisys_module_free (module);
		goto error;
	}

	/* Insert to extension list. */
	extension = lisys_calloc (1, sizeof (LICliExtension));
	if (extension == NULL)
	{
		lisys_module_free (module);
		goto error;
	}
	extension->info = info;
	extension->module = module;
	if (!lialg_strdic_insert (self->extensions, name, extension))
	{
		lisys_module_free (module);
		lisys_free (extension);
		goto error;
	}

	/* Call module initializer. */
	extension->object = ((void* (*)(LICliClient*)) info->init)(self);
	if (extension->object == NULL)
	{
		lialg_strdic_remove (self->extensions, name);
		lisys_module_free (module);
		lisys_free (extension);
		goto error;
	}

	return 1;

error:
	lisys_error_append ("cannot initialize module `%s'", name);
	lisys_error_report ();
	return 0;
}

int
licli_client_main (LICliClient* self)
{
	int i;
	int ticki = 0;
	int active = 1;
	float secs;
	float ticks[FPS_TICKS];
	struct timeval curr_tick;
	struct timeval prev_tick;

	memset (ticks, 0, sizeof (ticks));
	gettimeofday (&prev_tick, NULL);

	while (1)
	{
		/* Timing. */
		gettimeofday (&curr_tick, NULL);
		secs = curr_tick.tv_sec - prev_tick.tv_sec +
			  (curr_tick.tv_usec - prev_tick.tv_usec) * 0.000001;
		prev_tick = curr_tick;

		/* Frames per second. */
		ticks[ticki++] = secs;
		if (ticki == FPS_TICKS)
			ticki = 0;
		self->tick = 0.0f;
		for (i = 0 ; i < FPS_TICKS ; i++)
			self->tick += ticks[i];
		self->fps = FPS_TICKS / self->tick;
		self->tick = self->tick / FPS_TICKS;

		/* Update tick. */
		licli_client_update (self, secs);
		if (self->quit)
			break;
		licli_client_render (self);
		if (!active)
			self->video.SDL_Delay (100);

		/* TODO: Do we want to keep running even when not connected? */
		if (self->network == NULL || !licli_network_get_connected (self->network))
			self->quit = 1;
	}

	return 1;
}

/**
 * \brief Renders everything.
 *
 * Called once per frame to render everything.
 *
 * \param self Client.
 */
void
licli_client_render (LICliClient* self)
{
	int w;
	int h;

	licli_window_get_size (self->window, &w, &h);
	liwdg_manager_set_size (self->widgets, w, h);
	lialg_camera_set_viewport (self->camera, 0, 0, w, h);
	liwdg_manager_render (self->widgets);
	self->video.SDL_GL_SwapBuffers ();
}

/**
 * \brief Sends a network package to the server.
 *
 * If the game isn't networked, nothing is done.
 *
 * \param self Client.
 * \param writer Network package.
 * \param flags Grapple send flags.
 */
void
licli_client_send (LICliClient* self,
                   LIArcWriter* writer,
                   int          flags)
{
	if (self->network != NULL)
	{
		grapple_client_send (self->network->socket, GRAPPLE_SERVER, flags,
			liarc_writer_get_buffer (writer),
			liarc_writer_get_length (writer));
	}
}

/**
 * \brief Updates the module state.
 *
 * \param self Client.
 * \param secs Length of the tick in seconds.
 * \return Nonzero on success, zero if the module must be terminated.
 */
int
licli_client_update (LICliClient* self,
                     float        secs)
{
	SDL_Event event;

	/* Invoke input callbacks. */
	while (self->video.SDL_PollEvent (&event))
		lical_callbacks_call (self->callbacks, self->engine, "event", lical_marshal_DATA_PTR, &event);

	/* Invoke tick callbacks. */
	lical_callbacks_call (self->callbacks, self->engine, "tick", lical_marshal_DATA_FLT, secs);

	return 1;
}

/**
 * \brief Returns nonzero if movement mode is active.
 *
 * \param self Client.
 * \return Boolean.
 */
int
licli_client_get_moving (LICliClient* self)
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
void
licli_client_set_moving (LICliClient* self,
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
		self->video.SDL_WarpMouse (cx, cy);
		self->video.SDL_WM_GrabInput (SDL_GRAB_OFF);
	}
	else
	{
		self->video.SDL_ShowCursor (SDL_ENABLE);
		self->video.SDL_WM_GrabInput (SDL_GRAB_OFF);
	}
}

/**
 * \brief Gets the current player object.
 *
 * \param self Client.
 * \return Object or NULL.
 */
LIEngObject*
licli_client_get_player (LICliClient* self)
{
	if (self->network == NULL)
		return NULL;
	return lieng_engine_find_object (self->engine, self->network->id);
}

/*****************************************************************************/

/**
 * \brief Frees the module.
 *
 * \param self Client.
 */
static void
private_free_module (LICliClient* self)
{
	LIAlgStrdicIter iter;
	LICliExtension* extension;

	/* Free camera. */
	if (self->camera != NULL)
	{
		lialg_camera_free (self->camera);
		self->camera = NULL;
	}

	/* Free script. */
	if (self->script != NULL)
	{
		liscr_script_free (self->script);
		self->script = NULL;
	}

	/* Free extensions. */
	if (self->extensions != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->extensions)
		{
			extension = iter.value;
			((void (*)(void*)) extension->info->free) (extension->object);
			lisys_module_free (extension->module);
			lisys_free (extension);
		}
		lialg_strdic_free (self->extensions);
		self->extensions = NULL;
	}

	/* Free network. */
	if (self->network != NULL)
	{
		licli_network_free (self->network);
		self->network = NULL;
	}

	/* Free engine. */
	if (self->engine != NULL)
	{
		lieng_engine_free (self->engine);
		self->engine = NULL;
	}

	if (self->bindings != NULL)
	{
		libnd_manager_free (self->bindings);
		self->bindings = NULL;
	}
	if (self->widgets != NULL)
	{
		liwdg_manager_free (self->widgets);
		self->widgets = NULL;
	}
	if (self->server_thread != NULL)
	{
		self->server->quit = 1;
		lithr_thread_free (self->server_thread);
		self->server_thread = NULL;
	}
	assert (self->server == NULL);
	if (self->sectors != NULL)
	{
		lialg_sectors_free (self->sectors);
		self->sectors = NULL;
	}
	if (self->callbacks != NULL)
	{
		lical_callbacks_free (self->callbacks);
		self->callbacks = NULL;
	}
	if (self->paths != NULL)
	{
		lipth_paths_free (self->paths);
		self->paths = NULL;
	}
	lisys_free (self->camera_node);
	lisys_free (self->login);
	lisys_free (self->password);
	lisys_free (self->path);
	lisys_free (self->name);
	self->camera_node = NULL;
	self->login = NULL;
	self->password = NULL;
	self->path = NULL;
	self->name = NULL;
}

/**
 * \brief Loads a module.
 *
 * \param client Client.
 * \param path Package root directory.
 * \param name Module name.
 * \param login Login name.
 * \param password Login password.
 * \return Nonzero on success.
 */
static int
private_load_module (LICliClient* self,
                     const char*  path,
                     const char*  name,
                     const char*  login,
                     const char*  password)
{
	/* Allocate self. */
	self->name = listr_dup (name);
	self->login = listr_dup (login);
	self->password = listr_dup (password);
	if (self->name == NULL ||
	    self->login == NULL ||
	    self->password == NULL)
	{
		private_free_module (self);
		return 0;
	}

	/* Initialize subsystems. */
	if (!private_init_paths (self, path, name) ||
	    !private_init_bindings (self) ||
	    !private_init_engine (self) ||
	    !private_init_widgets (self) ||
	    !private_init_camera (self) ||
	    !private_init_extensions (self) ||
	    !private_init_script (self))
	{
		private_free_module (self);
		return 0;
	}

	return 1;
}

static int
private_init_bindings (LICliClient* self)
{
	self->bindings = libnd_manager_new ();
	if (self->bindings == NULL)
		return 0;
	return 1;
}	

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
private_init_engine (LICliClient* self)
{
	int flags;

	/* Initialize callbacks. */
	self->callbacks = lical_callbacks_new ();
	if (self->callbacks == NULL)
		return 0;

	/* Initialize sectors. */
#warning Hardcoded sector size
	self->sectors = lialg_sectors_new (256, 64.0f);
	if (self->sectors == NULL)
		return 0;

	/* Initialize engine. */
	self->engine = lieng_engine_new (self->callbacks, self->sectors, self->paths->module_data);
	if (self->engine == NULL)
		return 0;
	flags = lieng_engine_get_flags (self->engine);
	lieng_engine_set_flags (self->engine, flags | LIENG_FLAG_REMOTE_SECTORS);
	lieng_engine_set_local_range (self->engine, LINET_RANGE_CLIENT_START, LINET_RANGE_CLIENT_END);
	lieng_engine_set_userdata (self->engine, self);

	/* Connect callbacks. */
	if (!licli_client_init_callbacks_binding (self) ||
	    !licli_client_init_callbacks_misc (self) ||
	    !licli_client_init_callbacks_widget (self) ||
	    !licli_render_init (self))
		return 0;

	/* Initialize graphics. */
	self->render = liren_render_new (self->paths->module_data);
	if (self->render == NULL)
		return 0;
	self->scene = liren_scene_new (self->render);
	if (self->scene == NULL)
		return 0;

	return 1;
}

static int
private_init_extensions (LICliClient* self)
{
	self->extensions = lialg_strdic_new ();
	if (self->extensions == NULL)
		return 0;
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
private_init_script (LICliClient* self)
{
	int ret;
	char* path;

	/* Allocate script. */
	self->script = liscr_script_new ();
	if (self->script == NULL)
		return 0;
	liscr_script_set_userdata (self->script, self);

	/* Register classes. */
	if (!liscr_script_create_class (self->script, "Action", licli_script_action, self) ||
	    !liscr_script_create_class (self->script, "Binding", licli_script_binding, self) ||
	    !liscr_script_create_class (self->script, "Client", licli_script_client, self) ||
	    !liscr_script_create_class (self->script, "Extension", licli_script_extension, self) ||
	    !liscr_script_create_class (self->script, "Group", licli_script_group, self) ||
	    !liscr_script_create_class (self->script, "Light", licli_script_light, self) ||
	    !liscr_script_create_class (self->script, "Object", licli_script_object, self) ||
	    !liscr_script_create_class (self->script, "Packet", liscr_script_packet, self->script) ||
	    !liscr_script_create_class (self->script, "Path", liscr_script_path, self->script) ||
	    !liscr_script_create_class (self->script, "Player", licli_script_player, self) ||
	    !liscr_script_create_class (self->script, "Quaternion", liscr_script_quaternion, self->script) ||
	    !liscr_script_create_class (self->script, "Scene", licli_script_scene, self) ||
	    !liscr_script_create_class (self->script, "Vector", liscr_script_vector, self->script) ||
	    !liscr_script_create_class (self->script, "Widget", licli_script_widget, self) ||
	    !liscr_script_create_class (self->script, "Window", licli_script_window, self))
		return 0;

	/* Load script. */
	path = lisys_path_concat (self->path, "scripts", "client", "main.lua", NULL);
	if (path == NULL)
		return 0;
	ret = liscr_script_load (self->script, path);
	lisys_free (path);
	if (!ret)
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

/** @} */
/** @} */
