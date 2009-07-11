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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliWindow Window
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "client.h"
#include "client-window.h"

static int
private_init_input (licliWindow* self);

static int
private_init_video (licliWindow* self);

static int
private_resize (licliWindow* self,
                int          width,
                int          height,
                int          fsaa);

/****************************************************************************/

licliWindow*
licli_window_new (licliClient* client)
{
	licliWindow* self;

	/* Allocate self. */
	self = calloc (1, sizeof (licliWindow));
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
licli_window_free (licliWindow* self)
{
	if (self->joystick != NULL)
		self->client->video.SDL_JoystickClose (self->joystick);
	if (self->screen != NULL)
		self->client->video.SDL_FreeSurface (self->screen);
	if (self->client->video.TTF_WasInit ())
		self->client->video.TTF_Quit ();
	free (self);
}

void
licli_window_set_fsaa (licliWindow* self,
                       int          samples)
{
	private_resize (self, self->mode.width, self->mode.height, samples);
}

void
licli_window_get_size (const licliWindow* self,
                       int*               width,
                       int*               height)
{
	if (width != NULL)
		*width = self->mode.width;
	if (height != NULL)
		*height = self->mode.height;
}

int
licli_window_set_size (licliWindow* self,
                       int          width,
                       int          height)
{
	if (!private_resize (self, width, height, self->mode.fsaa))
		return 0;
	return 1;
}

/****************************************************************************/

static int
private_init_input (licliWindow* self)
{
	self->joystick = self->client->video.SDL_JoystickOpen (0);
	self->client->video.SDL_EnableKeyRepeat (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	return 1;
}

static int
private_init_video (licliWindow* self)
{
	/* Create the window. */
	if (private_resize (self, 1024, 768, livid_features_get_max_samples ()) +
	    private_resize (self, 1024, 768, 0) == 0)
		return 0;
	livid_features_init ();
	if (self->client->video.TTF_Init () == -1)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot initialize SDL TTF");
		return 0;
	}

	/* FIXME: Shouldn't be here? */
	/* FIXME: Should be user configurable. */
	if (1/*settings.graphics_quality == high*/)
	{
		glHint (GL_FOG_HINT, GL_NICEST);
		glHint (GL_LINE_SMOOTH_HINT, GL_NICEST);
		glHint (GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glHint (GL_POINT_SMOOTH_HINT, GL_NICEST);
		glHint (GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	}

	return 1;
}

static int
private_resize (licliWindow* self,
                int          width,
                int          height,
                int          fsaa)
{
	int depth;
	GLenum error;

	/* Recreate surface. */
	for ( ; fsaa >= 0 ; fsaa--)
	{
		for (depth = 32 ; depth ; depth -= 8)
		{
			self->client->video.SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, depth);
			self->client->video.SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
			self->client->video.SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
			self->client->video.SDL_GL_SetAttribute (SDL_GL_MULTISAMPLEBUFFERS, fsaa? 1 : 0);
			self->client->video.SDL_GL_SetAttribute (SDL_GL_MULTISAMPLESAMPLES, fsaa);
			self->screen = self->client->video.SDL_SetVideoMode (width, height, 0, SDL_OPENGL | SDL_RESIZABLE);
			if (self->screen != NULL)
				break;
		}
		if (self->screen != NULL)
			break;
	}
	if (self->screen == NULL)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

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
