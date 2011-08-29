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
 * \addtogroup LICliScript Script
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/client.h>

static void Client_host (LIScrArgs* args)
{
	const char* str = "--server";
	LICliClient* client;

	if (!liscr_args_gets_string (args, "args", &str))
	    liscr_args_geti_string (args, 0, &str);

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	if (!licli_client_host (client, str))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

static void Client_screenshot (LIScrArgs* args)
{
	char* home;
	char* file;
	char* path;
	void* pixels;
	LICliClient* client;
	SDL_Surface* surface;

	/* Capture the screen. */
	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	surface = liren_render_screenshot (client->render);
	if (surface == NULL)
		return;
	pixels = surface->pixels;

	/* Construct file path. */
	home = lisys_paths_get_home ();
	file = lisys_string_format ("screenshot-%d.bmp", (int) time (NULL));
	if (home == NULL || file == NULL)
	{
		SDL_FreeSurface (surface);
		lisys_free (pixels);
		lisys_free (home);
		lisys_free (file);
		return;
	}
	path = lisys_path_concat (home, file, NULL);
	lisys_free (home);
	if (path == NULL)
	{
		SDL_FreeSurface (surface);
		lisys_free (pixels);
		lisys_free (file);
		return;
	}

	/* Save the surface to a file. */
	SDL_SaveBMP (surface, path);
	SDL_FreeSurface (surface);
	lisys_free (pixels);

	/* Return the file name. */
	liscr_args_seti_string (args, file);
	lisys_free (path);
	lisys_free (file);
}

static void Client_set_video_mode (LIScrArgs* args)
{
	int width = 1024;
	int height = 768;
	int fullscreen = 0;
	int vsync = 0;
	LICliClient* client;

	if (!liscr_args_gets_int (args, "width", &width))
		liscr_args_geti_int (args, 0, &width);
	if (!liscr_args_gets_int (args, "height", &height))
		liscr_args_geti_int (args, 1, &height);
	if (!liscr_args_gets_bool (args, "fullscreen", &fullscreen))
		liscr_args_geti_bool (args, 2, &fullscreen);
	if (!liscr_args_gets_bool (args, "vsync", &vsync))
		liscr_args_geti_bool (args, 3, &vsync);
	width = LIMAT_MAX (320, width);
	height = LIMAT_MAX (240, height);

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	if (!licli_client_set_videomode (client, width, height, fullscreen, vsync))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

static void Client_get_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIMatVector tmp;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, y, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

static void Client_get_fps (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_float (args, client->program->fps);
}

static void Client_get_moving (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_bool (args, licli_client_get_moving (client));
}
static void Client_set_moving (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
		licli_client_set_moving (client, value);
	}
}

static void Client_set_title (LIScrArgs* args)
{
	const char* value;
	LICliClient* client;

	if (liscr_args_geti_string (args, 0, &value))
	{
		client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
		SDL_WM_SetCaption (value, value);
	}
}

static void Client_get_video_mode (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	liscr_args_seti_int (args, client->mode.width);
	liscr_args_seti_int (args, client->mode.height);
	liscr_args_seti_bool (args, client->mode.fullscreen);
	liscr_args_seti_bool (args, client->mode.sync);
}

static void Client_get_video_modes (LIScrArgs* args)
{
	int i;
	SDL_Rect** modes;
	LICliClient* client;

	client = liscr_script_get_userdata (args->script, LICLI_SCRIPT_CLIENT);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	modes = SDL_ListModes (NULL, SDL_OPENGL | SDL_FULLSCREEN);
	if (modes != NULL && modes != (SDL_Rect**) -1)
	{
		for (i = 0 ; modes[i] ; i++)
		{
			lua_newtable (args->lua);
			lua_pushnumber (args->lua, 1);
			lua_pushnumber (args->lua, modes[i]->w);
			lua_settable (args->lua, -3);
			lua_pushnumber (args->lua, 2);
			lua_pushnumber (args->lua, modes[i]->h);
			lua_settable (args->lua, -3);
			liscr_args_seti_stack (args);
		}
	}
}

/*****************************************************************************/

void licli_script_client (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_host", Client_host);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_screenshot", Client_screenshot);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_video_mode", Client_set_video_mode);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_cursor_pos", Client_get_cursor_pos);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_fps", Client_get_fps);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_moving", Client_get_moving);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_moving", Client_set_moving);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_set_title", Client_set_title);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_video_mode", Client_get_video_mode);
	liscr_script_insert_cfunc (self, LICLI_SCRIPT_CLIENT, "client_get_video_modes", Client_get_video_modes);
}

/** @} */
/** @} */

