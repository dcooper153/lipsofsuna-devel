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
 * \addtogroup licliClient Client
 * @{
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include "client.h"
#include "client-window.h"

licliClient*
licli_client_new (lividCalls* video,
                  const char* path,
                  const char* name)
{
	licliClient* self;
	/* FIXME: Login name and password not supported. */
	const char* login = "none";
	const char* password = "none";

	/* Allocate self. */
	self = calloc (1, sizeof (licliClient));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lisys_error_report ();
		return NULL;
	}
	self->video = *video;

	/* Initialize SDL. */
	if (self->video.SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
	{
		lisys_error_set (ENOTSUP, "initializing SDL failed");
		lisys_error_report ();
		free (self);
		return NULL;
	}

	/* Initialize sound. */
#ifndef LI_DISABLE_SOUND
	self->sound = lisnd_system_new ();
	if (self->sound == NULL)
		printf ("WARNING: cannot initialize sound\n");
#endif

	/* Create window. */
	self->video.SDL_EnableUNICODE (1);
	self->window = licli_window_new (self);
	if (self->window == NULL)
	{
		licli_client_free (self);
		lisys_error_report ();
		return NULL;
	}

	/* Load module. */
	self->module = licli_module_new (self, path, name, login, password);
	if (self->module == NULL)
	{
		licli_client_free (self);
		lisys_error_report ();
		return NULL;
	}

	return self;
}

void
licli_client_free (licliClient* self)
{
	if (self->module != NULL)
		licli_module_free (self->module);
	if (self->window != NULL)
		licli_window_free (self->window);
#ifndef LI_DISABLE_SOUND
	if (self->sound != NULL)
		lisnd_system_free (self->sound);
#endif
	self->video.SDL_Quit ();
	free (self);
}

int
licli_client_main (licliClient* self)
{
	licli_module_main (self->module);

	return 1;
}

/** @} */
/** @} */
