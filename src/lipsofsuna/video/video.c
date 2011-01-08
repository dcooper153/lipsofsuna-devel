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
 * \addtogroup LIVid Video
 * @{
 * \addtogroup LIVidVideo Video
 * @{
 */

#include <string.h>
#include "video.h"

/**
 * \brief Initializes the global video card information.
 * \return Nonzero on success.
 */
int livid_video_init ()
{
	GLenum error;

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

	/* Get capabilities. */
	if (!GLEW_VERSION_3_2)
	{
		lisys_error_set (EINVAL, "OpenGL 3.2 isn't supported by your graphics card");
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
