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
 * \addtogroup ligen Generator
 * @{
 * \addtogroup ligenMain Main
 * @{
 */

#include <time.h>
#include "generator.h"
#undef main

int
main (int argc, char** argv)
{
	lialgSectors* sectors;
	licalCallbacks* callbacks;
	ligenGenerator* self;
	lipthPaths* paths;

	/* Resolve game directory. */
	paths = lipth_paths_new (NULL, argc > 1? argv[1] : "data");
	if (paths == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Allocate callbacks. */
	callbacks = lical_callbacks_new ();
	if (callbacks == NULL)
	{
		lisys_error_report ();
		lipth_paths_free (paths);
		return 1;
	}

	/* Allocate sectors. */
#warning Hardcoded sector size
	sectors = lialg_sectors_new (256, 64.0f);
	if (sectors == NULL)
	{
		lisys_error_report ();
		lical_callbacks_free (callbacks);
		lipth_paths_free (paths);
		return 1;
	}

	/* Execute program. */
	srand (time (NULL));
	self = ligen_generator_new (paths, callbacks, sectors);
	if (self == NULL)
	{
		lisys_error_report ();
		lialg_sectors_free (sectors);
		lical_callbacks_free (callbacks);
		lipth_paths_free (paths);
		return 1;
	}
	ligen_generator_load_materials (self);
	if (!ligen_generator_main (self))
		lisys_error_report ();
	ligen_generator_free (self);
	lialg_sectors_free (sectors);
	lical_callbacks_free (callbacks);
	lipth_paths_free (paths);

	return 0;
}

/** @} */
/** @} */
