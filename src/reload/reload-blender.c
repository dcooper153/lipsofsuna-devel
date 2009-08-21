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
 * \addtogroup lirel Reload
 * @{
 * \addtogroup lirelBlender Blender
 * @{
 */

#include <unistd.h>
#include <model/lips-model.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "reload.h"

static int
private_convert (lirelReload* self,
                 const char*  name);

static void
private_filter (void* data,
                FILE* file);

/*****************************************************************************/

/**
 * \brief Converts a Blender file to a Lips of Suna model format.
 *
 * \param self Reload.
 * \param src Source file of type blend.
 * \param dst Destination file of type lmdl.
 * \return Nonzero of success.
 */
int
lirel_reload_blender (lirelReload* self,
                      const char*  src,
                      const char*  dst)
{
	/* Check if file is missing for sure. */
	if (access (src, R_OK) == -1)
		return 0;

	/* Export from Blender. */
	if (!private_convert (self, src))
		return 0;

	return 1;
}

/*****************************************************************************/

static int
private_convert (lirelReload* self,
                 const char*  name)
{
	int ret;
	char* script;

	/* Find export script. */
	script = lisys_path_concat (self->paths->root, "tool", "blender-export.py", NULL);
	if (script == NULL)
		return 0;

	/* Convert the file. */
	ret = lisys_execvl_redir_call (private_filter, NULL, "blender", "blender", "-b", name, "-P", script, NULL);
	free (script);

	return ret;
}

static void
private_filter (void* data,
                FILE* file)
{
	char buffer[1024];

	while (fgets (buffer, sizeof (buffer), file))
	{
		if (buffer[0] != '\0' && buffer[1] == ':' && buffer[2] == ' ')
		{
			if (buffer[0] == 'E' ||
			  /*buffer[0] == 'I' ||*/
				buffer[0] == 'W')
				printf ("%s", buffer);
		}
	}
}

/** @} */
/** @} */
