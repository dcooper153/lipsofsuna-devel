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
 * \addtogroup licliscr Script
 * @{
 */

#include <lipsofsuna/network.h>
#include <lipsofsuna/system.h>
#include <lipsofsuna/client.h>

/* @luadoc
 * module "Core.Client.Client"
 * --- Access and manipulate the state of the client.
 * -- @name Client
 * -- @class table
 */

/* @luadoc
 * --- Launches a server.
 * --
 * -- @param clss Client class.
 * -- @param args Arguments.<ul>
 * --   <li>1,args: String to pass to the server.</li></ul>
 * -- @return True on success.
 * function Client.host(clss, args)
 */
static void Client_host (LIScrArgs* args)
{
	const char* str = NULL;
	LICliClient* client;

	if (!liscr_args_gets_string (args, "args", &str))
	    liscr_args_geti_string (args, 0, &str);

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	if (!licli_client_host (client, str))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Takes a screenshot.
 * -- @param clss Client class.
 * -- @return Screenshot filename.
 * function Client.screenshot(clss)
 */
static void Client_screenshot (LIScrArgs* args)
{
	int i;
	int width;
	int height;
	int pitch;
	char* home;
	char* file;
	char* path;
	uint32_t rmask;
	uint32_t gmask;
	uint32_t bmask;
	uint32_t amask;
	uint8_t* pixels;
	SDL_Surface* surface;
	LICliClient* client;

	/* Get window size. */
	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	width = client->window->mode.width;
	height = client->window->mode.height;
	pitch = 4 * width;

	/* Capture pixel data. */
	/* The one extra row we allocate is used for flipping. */
	pixels = calloc ((height + 1) * pitch, sizeof (uint8_t));
	if (pixels == NULL)
		return;
	glReadPixels (0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	/* Flip the image vertically. */
	/* We use the extra row as temporary storage. */
	for (i = 0 ; i < height / 2 ; i++)
	{
		memcpy (pixels + pitch * height, pixels + pitch * i, pitch);
		memcpy (pixels + pitch * i, pixels + pitch * (height - i - 1), pitch);
		memcpy (pixels + pitch * (height - i - 1), pixels + pitch * height, pitch);
	}

	/* Create a temporary SDL surface. */
	if (lisys_endian_big ())
	{
		rmask = 0xFF000000;
		gmask = 0x00FF0000;
		bmask = 0x0000FF00;
		amask = 0x000000FF;
	}
	else
	{
		rmask = 0x000000FF;
		gmask = 0x0000FF00;
		bmask = 0x00FF0000;
		amask = 0xFF000000;
	}
	surface = SDL_CreateRGBSurfaceFrom(pixels, width, height,
		32, pitch, rmask, gmask, bmask, amask);
	if (surface == NULL)
	{
		lisys_free (pixels);
		return;
	}

	/* Construct file path. */
	home = lisys_system_get_path_home ();
	file = listr_format ("screenshot-%d.bmp", (int) time (NULL));
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

/* @luadoc
 * --- Copies the rendered scene to the screen.
 * -- @param clss Client class.
 * function Client.swap_buffers(clss)
 */
static void Client_swap_buffers (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	client->video.SDL_GL_SwapBuffers ();
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/* @luadoc
 * --- Gets the current cursor position.
 * -- @name Client.cursor_pos
 * -- @class table
 */
static void Client_getter_cursor_pos (LIScrArgs* args)
{
	int x;
	int y;
	LICliClient* client;
	LIMatVector tmp;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	client->video.SDL_GetMouseState (&x, &y);
	tmp = limat_vector_init (x, client->window->mode.height - y - 1, 0.0f);
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Short term average frames per second.
 * --
 * -- @name Client.tick
 * -- @class table
 */
static void Client_getter_fps (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_float (args, client->program->fps);
}

/* @luadoc
 * --- Movement mode flag.
 * -- @name Client.moving
 * -- @class table
 */
static void Client_getter_moving (LIScrArgs* args)
{
	LICliClient* client;

	client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
	liscr_args_seti_bool (args, licli_client_get_moving (client));
}
static void Client_setter_moving (LIScrArgs* args)
{
	int value;
	LICliClient* client;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		licli_client_set_moving (client, value);
	}
}

/* @luadoc
 * --- Main window title.
 * -- @name Client.title
 * -- @class table
 */
static void Client_setter_title (LIScrArgs* args)
{
	const char* value;
	LICliClient* client;

	if (liscr_args_geti_string (args, 0, &value))
	{
		client = liscr_class_get_userdata (args->clss, LICLI_SCRIPT_CLIENT);
		client->video.SDL_WM_SetCaption (value, value);
	}
}

/*****************************************************************************/

void
licli_script_client (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LICLI_SCRIPT_CLIENT, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "host", Client_host);
	liscr_class_insert_cfunc (self, "screenshot", Client_screenshot);
	liscr_class_insert_cfunc (self, "swap_buffers", Client_swap_buffers);
	liscr_class_insert_cvar (self, "cursor_pos", Client_getter_cursor_pos, NULL);
	liscr_class_insert_cvar (self, "fps", Client_getter_fps, NULL);
	liscr_class_insert_cvar (self, "moving", Client_getter_moving, Client_setter_moving);
	liscr_class_insert_cvar (self, "title", NULL, Client_setter_title);
}

/** @} */
/** @} */

