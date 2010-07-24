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
 * \addtogroup liser Server
 * @{
 * \addtogroup liserMain Main
 * @{
 */

#include "server.h"
#undef main

int
main (int argc, char** argv)
{
	LIPthPaths* paths;
	LISerServer* self;
	char* launch_name;
	char* launch_args;

	/* Choose the first mod. */
	launch_name = listr_dup ((argc > 1)? argv[1] : "data");
	launch_args = listr_dup ((argc > 2)? argv[2] : "");
	if (launch_name == NULL || launch_args == NULL)
	{
		lisys_error_report ();
		lisys_free (launch_name);
		lisys_free (launch_args);
		return 1;
	}

	/* Execute mods until one exits without starting a new one. */
	while (1)
	{
		/* Resolve game directory. */
		paths = lipth_paths_new (NULL, launch_name);
		if (paths == NULL)
		{
			lisys_error_report ();
			return 1;
		}

		/* Load and execute the mod. */
		self = liser_server_new (paths, launch_args);
		if (self == NULL)
		{
			lisys_error_report ();
			lipth_paths_free (paths);
			return 1;
		}
		if (!liser_server_main (self))
			lisys_error_report ();

		/* Check if there's another mod to launch. */
		launch_name = self->program->launch_name;
		launch_args = self->program->launch_args;
		self->program->launch_name = NULL;
		self->program->launch_args = NULL;

		/* Close the old mod. */
		liser_server_free (self);
		lipth_paths_free (paths);
	}

	return 0;
}

/** @} */
/** @} */
