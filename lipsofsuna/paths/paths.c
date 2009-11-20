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
 * \addtogroup lipth Paths
 * @{
 * \addtogroup lipthPaths Paths
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "paths.h"

/**
 * \brief Creates a new paths object.
 *
 * \param path Package root directory or NULL for auto-detect.
 * \param name Module name.
 * \return Paths or NULL.
 */
lipthPaths*
lipth_paths_new (const char* path,
                 const char* name)
{
	lipthPaths* self;
	lisysStat stat;

	self = lisys_calloc (1, sizeof (lipthPaths));
	if (self == NULL)
		return NULL;

	/* Set module name. */
	self->module_name = listr_dup (name);
	if (self->module_name == NULL)
		goto error;

	/* Set root directory. */
	if (path != NULL)
	{
		self->root = listr_dup (path);
		if (self->root == NULL)
			goto error;
	}
	else
	{
		self->root = lipth_paths_get_root ();
		if (self->root == NULL)
			goto error;
	}

	/* Get data directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_data = listr_dup (self->root);
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
	self->global_state = listr_dup (self->global_data);
	if (self->global_state == NULL)
		goto error;
	self->module_state = lisys_path_concat (self->module_data, "save", NULL);
	if (self->module_state == NULL)
		goto error;
#else
	self->global_state = LISAVEDIR;
	if (!strcmp (name, "data"))
		self->module_data = lisys_path_concat (self->global_state, "data", NULL);
	else
		self->module_state = lisys_path_concat (self->global_state, "mods", name, NULL);
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

	/* Check for valid data directory. */
	if (!lisys_stat (self->module_data, &stat))
	{
		lisys_error_set (EIO, "missing data directory `%s'", self->module_data);
		goto error;
	}
	if (stat.type != LISYS_STAT_DIRECTORY && stat.type != LISYS_STAT_LINK)
	{
		lisys_error_set (EIO, "invalid data directory `%s': not a directory", self->module_data);
		goto error;
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
	lisys_free (self->global_exts);
	lisys_free (self->global_data);
	lisys_free (self->global_state);
#endif
	lisys_free (self->module_data);
	lisys_free (self->module_name);
	lisys_free (self->module_state);
	lisys_free (self->root);
	lisys_free (self);
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

/**
 * \brief Gets the path to an SQL database.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char*
lipth_paths_get_sql (const lipthPaths* self,
                     const char*       name)
{
	return lisys_path_concat (self->module_state, name, NULL);
}

/**
 * \brief Gets the game root directory.
 *
 * \return Path or NULL.
 */
char*
lipth_paths_get_root ()
{
#ifdef LI_RELATIVE_PATHS
	char* tmp;
	char* path;

	/* Resolve game directory. */
	tmp = lisys_relative_exedir ();
	if (tmp == NULL)
		return NULL;
	path = lisys_path_format (tmp, LISYS_PATH_STRIPLAST, NULL);
	lisys_free (tmp);

	return path;
#else
	return listr_dup (LIDATADIR);
#endif
}

/** @} */
/** @} */
