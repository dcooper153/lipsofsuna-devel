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
 * \addtogroup licliMain Main
 * @{
 */

#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include "client.h"

int
main (int argc, char** argv)
{
	char* path;
	licliClient* self;
	lividCalls video;

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
	self = licli_client_new (&video, path, argc > 1? argv[1] : "data");
	if (self == NULL)
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}
	if (!licli_client_main (self))
		lisys_error_report ();
	licli_client_free (self);
	lisys_free (path);

	return 0;
}

/** @} */
/** @} */
