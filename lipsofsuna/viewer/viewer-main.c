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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup livieMain Main
 * @{
 */

#include <SDL.h>
#include <SDL_main.h>
#include "viewer.h"

int
main (int argc, char** argv)
{
	LIPthPaths* paths;
	LIVidCalls video;
	LIVieViewer* self;

	/* Resolve game directory. */
	paths = lipth_paths_new (NULL, argc > 2? argv[2] : "data");
	if (paths == NULL)
	{
		lisys_error_report ();
		return 1;
	}

	/* Execute program. */
	if (argc < 2)
	{
		lisys_error_set (EINVAL, "no model name provided");
		lisys_error_report ();
		lipth_paths_free (paths);
		return 1;
	}
	if (!livid_calls_init (&video))
	{
		lisys_error_report ();
		lipth_paths_free (paths);
		return 1;
	}
	self = livie_viewer_new (&video, paths, argv[1]);
	if (self == NULL)
	{
		lisys_error_report ();
		lipth_paths_free (paths);
		return 1;
	}
	if (!livie_viewer_main (self))
		lisys_error_report ();
	livie_viewer_free (self);
	lipth_paths_free (paths);

	return 0;
}

/** @} */
/** @} */
