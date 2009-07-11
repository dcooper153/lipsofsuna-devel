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
 * \addtogroup livie Viewer
 * @{
 * \addtogroup livieMain Main
 * @{
 */

#include <SDL/SDL.h>
#include <SDL/SDL_main.h>
#include "viewer.h"

int
main (int argc, char** argv)
{
	lividCalls video;
	livieViewer* self;

	if (argc != 4)
		return 1;
	if (!livid_calls_init (&video))
	{
		lisys_error_report ();
		return 1;
	}
	self = livie_viewer_new (&video, argv[1], argv[2], argv[3]);
	if (self == NULL)
	{
		lisys_error_report ();
		return 1;
	}
	if (!livie_viewer_main (self))
		lisys_error_report ();
	livie_viewer_free (self);

	return 0;
}

/** @} */
/** @} */
