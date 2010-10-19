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
 * \addtogroup licliMain Main
 * @{
 */

#include <SDL.h>
#include <SDL_main.h>
#include "client.h"

int
main (int argc, char** argv)
{
	char* path;
	char* launch_name;
	char* launch_args;
	LICliClient* self;
	LIVidCalls video;

	/* Resolve game directory. */
	path = lipth_paths_get_root ();
	if (path == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Start the program. */
	if (!livid_calls_init (&video))
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}
	self = licli_client_new (&video, path, argc > 1? argv[1] : "default");
	if (self == NULL)
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}

	/* Load the first mod. */
	if (argc > 1)
	{
		/* FIXME: Should concatenate the arguments. */
		if (!licli_client_load_module (self, argv[1], (argc > 2)? argv[2] : NULL))
			lisys_error_report ();
	}
	else
	{
		if (!licli_client_load_module (self, "default", NULL))
			lisys_error_report ();
	}

	/* Execute mods until one exits without starting a new one. */
	while (self->program != NULL)
	{
		/* Execute the module until the script exits. */
		if (!licli_client_main (self))
		{
			lisys_error_report ();
			break;
		}

		/* Check if the module started another one. */
		launch_name = self->program->launch_name;
		launch_args = self->program->launch_args;
		if (launch_name == NULL)
			break;
		self->program->launch_name = NULL;
		self->program->launch_args = NULL;

		/* Unload the old module and load a new one. */
		licli_client_free_module (self);
		if (!licli_client_load_module (self, launch_name, launch_args))
			lisys_error_report ();
		lisys_free (launch_name);
		lisys_free (launch_args);
	}

	/* Free all resources. */
	licli_client_free (self);
	lisys_free (path);

	return 0;
}

/** @} */
/** @} */
