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
 * \addtogroup LICliWindow Window
 * @{
 */

#include <lipsofsuna/system.h>
#include "client.h"
#include "client-window.h"

static int
private_init_input (LICliWindow* self);

static int
private_init_video (LICliWindow* self);

static int
private_resize (LICliWindow* self,
                int          width,
                int          height,
                int          fsaa);

/****************************************************************************/

LICliWindow*
licli_window_new (LICliClient* client)
{
	LICliWindow* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LICliWindow));
	if (self == NULL)
		return NULL;
	self->client = client;

	/* Initialize subsystems. */
	if (!private_init_input (self) ||
	    !private_init_video (self))
	{
		licli_window_free (self);
		return NULL;
	}

	return self;
}

void
licli_window_free (LICliWindow* self)
{
	if (self->joystick != NULL)
		SDL_JoystickClose (self->joystick);
	if (self->screen != NULL)
		SDL_FreeSurface (self->screen);
	if (TTF_WasInit ())
		TTF_Quit ();
	lisys_free (self);
}

void
licli_window_set_fsaa (LICliWindow* self,
                       int          samples)
{
	private_resize (self, self->mode.width, self->mode.height, samples);
}

void
licli_window_get_size (const LICliWindow* self,
                       int*               width,
                       int*               height)
{
	if (width != NULL)
		*width = self->mode.width;
	if (height != NULL)
		*height = self->mode.height;
}

int
licli_window_set_size (LICliWindow* self,
                       int          width,
                       int          height)
{
	if (!private_resize (self, width, height, self->mode.fsaa))
		return 0;
	return 1;
}

/****************************************************************************/

static int
private_init_input (LICliWindow* self)
{
	self->joystick = SDL_JoystickOpen (0);
	SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	return 1;
}

static int
private_init_video (LICliWindow* self)
{
	/* Create the window. */
	if (!private_resize (self, 1024, 768, 0))
		return 0;
	if (TTF_Init () == -1)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot initialize SDL_ttf");
		return 0;
	}

	return 1;
}

static int
private_resize (LICliWindow* self,
                int          width,
                int          height,
                int          fsaa)
{
	int depth;

	/* Recreate surface. */
	for ( ; fsaa >= 0 ; fsaa--)
	{
		for (depth = 32 ; depth ; depth -= 8)
		{
			SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, depth);
			SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
			SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, fsaa? 1 : 0);
			SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, fsaa);
			self->screen = SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
			if (self->screen != NULL)
				break;
		}
		if (self->screen != NULL)
			break;
	}
	if (self->screen == NULL)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize libraries. */
	if (!livid_video_init ())
		return 0;

	/* Store mode. */
	self->mode.width = width;
	self->mode.height = height;
	self->mode.fsaa = fsaa;
	if (fsaa)
		glEnable (GL_MULTISAMPLE_ARB);
	else
		glDisable (GL_MULTISAMPLE_ARB);

	return 1;
}

/** @} */
/** @} */
