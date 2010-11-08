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
 * \addtogroup LICliMain Main
 * @{
 */

#include "SDL.h"
#include "SDL_main.h"
#include "main-program.h"

int main (int argc, char** argv)
{
	char* path;
	char* launch_name;
	char* launch_args;
	LIMaiProgram* program;

	/* Resolve game directory. */
	path = lipth_paths_get_root ();
	if (path == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Start the program. */
	/* FIXME: Should concatenate the arguments. */
	program = limai_program_new (path, argc > 1? argv[1] : "default", (argc > 2)? argv[2] : NULL);
	if (program == NULL)
	{
		lisys_error_report ();
		lisys_free (path);
		return 1;
	}

	/* Execute mods until one exits without starting a new one. */
	while (program != NULL)
	{
		/* Execute the module until the script exits. */
		if (!limai_program_execute_script (program, "main.lua"))
		{
			lisys_error_report ();
			break;
		}

		/* Check if the module started another one. */
		launch_name = program->launch_name;
		launch_args = program->launch_args;
		if (launch_name == NULL)
			break;
		program->launch_name = NULL;
		program->launch_args = NULL;
		limai_program_free (program);

		/* Unload the old module and load a new one. */
		program = limai_program_new (path, launch_name, launch_args);
		lisys_free (launch_name);
		lisys_free (launch_args);
		if (program == NULL)
		{
			lisys_error_report ();
			break;
		}
	}

	/* Free all resources. */
	limai_program_free (program);
	lisys_free (path);
	SDL_Quit ();

	return 0;
}

/** @} */
/** @} */
