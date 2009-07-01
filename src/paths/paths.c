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
 * \addtogroup lipth Server
 * @{
 * \addtogroup lipthPaths Paths
 * @{
 */

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system/lips-system.h>
#include "paths.h"

/**
 * \brief Creates a new paths object.
 *
 * \param name Module name.
 * \return Paths or NULL.
 */
lipthPaths*
lipth_paths_new (const char* name)
{
	char* tmp;
	lipthPaths* self;

	self = calloc (1, sizeof (lipthPaths));
	if (self == NULL)
		return NULL;

	/* Get data directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_data = lisys_relative_exedir (NULL);
	if (self->global_data == NULL)
		goto error;
	if (!strcmp (name, "data"))
		self->module_data = lisys_path_concat (self->global_data, "data", NULL);
	else
		self->module_data = lisys_path_concat (self->global_data, "mods", name, NULL);
	if (self->module_data == NULL)
		goto error;
#else
	self->global_data = LIDATADIR;
	if (!strcmp (name, "data"))
		self->module_data = lisys_path_concat (self->global_data, "data", NULL);
	else
		self->module_data = lisys_path_concat (self->global_data, "mods", name, NULL);
	if (self->module_data == NULL)
		goto error;
#endif

	/* Get save directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_state = strdup (self->global_data);
	if (self->global_state == NULL)
		goto error;
	self->module_state = lisys_path_concat (self->module_data, "save", NULL);
	if (self->module_state == NULL)
		goto error;
#else
	self->global_state = LISAVEDIR;
	self->module_state = lisys_path_concat (self->global_state, name, NULL);
	if (self->module_state == NULL)
		goto error;
#endif

	/* Get extension directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_exts = lisys_path_concat (self->global_data, "lib", "extensions", NULL);;
	if (self->global_exts == NULL)
		goto error;
#else
	self->global_exts = LIEXTSDIR;
#endif

	/* Create the save directory. */
	tmp = lisys_path_concat (self->module_state, "accounts", NULL);
	if (tmp != NULL)
	{
		mkdir (self->module_state, S_IRWXU);
		mkdir (tmp, S_IRWXU);
		free (tmp);
	}

	return self;

error:
	lipth_paths_free (self);
	return NULL;
}

/**
 * \brief Frees the paths object.
 *
 * \param self Paths object.
 */
void
lipth_paths_free (lipthPaths* self)
{
#ifdef LI_RELATIVE_PATHS
	free (self->global_exts);
	free (self->global_data);
	free (self->global_state);
#endif
	free (self->module_data);
	free (self->module_state);
	free (self);
}

/**
 * \brief Gets the path to a generic data file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_data (const lipthPaths* self,
                      const char*       name)
{
	return lisys_path_concat (self->module_data, name, NULL);
}

/**
 * \brief Gets the path to a font file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_font (const lipthPaths* self,
                      const char*       name)
{
	return lisys_path_concat (self->module_data, "fonts", name, NULL);
}

/**
 * \brief Gets the path to a graphics file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_graphics (const lipthPaths* self,
                          const char*       name)
{
	return lisys_path_concat (self->module_data, "graphics", name, NULL);
}

/**
 * \brief Gets the path to a script file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_script (const lipthPaths* self,
                        const char*       name)
{
	return lisys_path_concat (self->module_data, "scripts", name, NULL);
}

/**
 * \brief Gets the path to a shader file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_shader (const lipthPaths* self,
                        const char*       name)
{
	return lisys_path_concat (self->module_data, "shaders", name, NULL);
}

/**
 * \brief Gets the path to a sound file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_sound (const lipthPaths* self,
                       const char*       name)
{
	return lisys_path_concat (self->module_data, "sounds", name, NULL);
}

/** @} */
/** @} */
