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

#include <stdio.h>
#include <stdlib.h>
#include "reload.h"
#undef main

static void
private_reload_image (lirelReload* self,
                      const char*  name)
{
	printf ("Imported image `%s'\n", name);
}

static void
private_reload_model (lirelReload* self,
                      const char*  name)
{
	printf ("Imported model `%s'\n", name);
}

int
main (int argc, char** argv)
{
	char* path;
	lipthPaths* paths;
	lirelReload* self;

	/* Resolve game directory. */
	paths = lipth_paths_new (NULL, argc > 1? argv[1] : "data");
	if (paths == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Execute program. */
	self = lirel_reload_new (paths);
	if (self == NULL)
	{
		lisys_error_report ();
		lipth_paths_free (paths);
		return 1;
	}
	lirel_reload_set_image_callback (self, private_reload_image, self);
	lirel_reload_set_model_callback (self, private_reload_model, self);
	lirel_reload_set_enabled (self, 1);
	if (!lirel_reload_main (self))
		lisys_error_report ();
	lirel_reload_free (self);
	lipth_paths_free (paths);

	return 0;
}

/** @} */
/** @} */
