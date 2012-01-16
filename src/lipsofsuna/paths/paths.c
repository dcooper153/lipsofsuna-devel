/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIPth Paths
 * @{
 * \addtogroup LIPthPaths Paths
 * @{
 */

#include "lipsofsuna/system.h"
#include "paths.h"

/**
 * \brief Creates a new paths object.
 *
 * \param path Package root directory or NULL for auto-detect.
 * \param name Module name.
 * \return Paths or NULL.
 */
LIPthPaths* lipth_paths_new (
	const char* path,
	const char* name)
{
	char* tmp;
	LIPthPaths* self;
	LISysStat stat;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIPthPaths));
	if (self == NULL)
		return NULL;

	/* Allocate the file lookup table. */
	self->files = lialg_strdic_new ();
	if (self->files == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}

	/* Set the module name. */
	self->module_name = lisys_string_dup (name);
	if (self->module_name == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}

	/* Set the root directory. */
	if (path != NULL)
	{
		self->root = lisys_string_dup (path);
		if (self->root == NULL)
		{
			lipth_paths_free (self);
			return NULL;
		}
	}
	else
	{
		self->root = lipth_paths_get_root ();
		if (self->root == NULL)
		{
			lipth_paths_free (self);
			return NULL;
		}
	}

	/* Get the data directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_data = lisys_path_concat (self->root, "data", NULL);
	if (self->global_data == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}
#else
	self->global_data = LIDATADIR;
#endif
	self->module_data = lisys_path_concat (self->global_data, name, NULL);
	if (self->module_data == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}

	/* Get the data override directory. */
	tmp = lisys_paths_get_data_home ();
	if (tmp == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}
	self->override_data = lisys_path_concat (tmp, "lipsofsuna", "data", name, NULL);
	if (self->override_data == NULL)
	{
		lisys_free (tmp);
		lipth_paths_free (self);
		return NULL;
	}

	/* Get the save directory. */
	self->global_state = lisys_path_concat (tmp, "lipsofsuna", "save", NULL);
	lisys_free (tmp);
	if (self->global_state == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}
	self->module_state = lisys_path_concat (self->global_state, name, NULL);
	if (self->module_state == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}

	/* Get the extension directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_exts = lisys_path_concat (self->global_data, "lib", "extensions", NULL);
	if (self->global_exts == NULL)
	{
		lipth_paths_free (self);
		return NULL;
	}
#else
	self->global_exts = LIEXTSDIR;
#endif

	/* Check for a valid data directory. */
	if (!lisys_filesystem_stat (self->module_data, &stat))
	{
		lisys_error_set (EIO, "missing data directory `%s'", self->module_data);
		lipth_paths_free (self);
		return NULL;
	}
	if (stat.type != LISYS_STAT_DIRECTORY && stat.type != LISYS_STAT_LINK)
	{
		lisys_error_set (EIO, "invalid data directory `%s': not a directory", self->module_data);
		lipth_paths_free (self);
		return NULL;
	}

	/* Add the base data directory. */
	lipth_paths_add_path (self, "");

	return self;
}

/**
 * \brief Frees the paths object.
 *
 * \param self Paths object.
 */
void lipth_paths_free (
	LIPthPaths* self)
{
	LIAlgList* ptr;
	LIAlgStrdicIter iter;

	/* Free path names. */
	if (self->paths != NULL)
	{
		for (ptr = self->paths ; ptr != NULL ; ptr = ptr->next)
			lisys_free (ptr->data);
		lialg_list_free (self->paths);
	}

	/* Free file names. */
	if (self->files != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->files)
			lisys_free (iter.value);
		lialg_strdic_free (self->files);
	}

#ifdef LI_RELATIVE_PATHS
	lisys_free (self->global_exts);
	lisys_free (self->global_data);
#endif
	lisys_free (self->global_state);
	lisys_free (self->module_data);
	lisys_free (self->module_name);
	lisys_free (self->module_state);
	lisys_free (self->override_data);
	lisys_free (self->root);
	lisys_free (self);
}

/**
 * \brief Adds a data directory lookup path.
 * \param self Paths.
 * \param path Module root relative path.
 */
int lipth_paths_add_path (
	LIPthPaths* self,
	const char* path)
{
	int ret = 1;
	char* path1;

	/* Add the module directory. */
	path1 = lisys_path_concat (self->module_data, path, NULL);
	if (path1 != NULL)
	{
		if (!lipth_paths_add_path_abs (self, path1))
			ret = 0;
		lisys_free (path1);
	}
	else
		ret = 0;

	/* Add the override directory. */
	path1 = lisys_path_concat (self->override_data, path, NULL);
	if (path1 != NULL)
	{
		lipth_paths_add_path_abs (self, path1);
		lisys_free (path1);
	}
	else
		ret = 0;

	return ret;
}

/**
 * \brief Adds a data directory lookup path.
 * \param self Paths.
 * \param path Absolute path.
 */
int lipth_paths_add_path_abs (
	LIPthPaths* self,
	const char* path)
{
	int i;
	int c;
	char* p;
	const char* name;
	LISysDir* dir;
	LIAlgList* ptr;
	LIAlgStrdicNode* node;

	/* Add the path to the list. */
	/* The path isn't added if it already exists. Its files are scanned
	   and can override previously registered files, however. */
	for (ptr = self->paths ; ptr != NULL ; ptr = ptr->next)
	{
		if (!strcmp (ptr->data, path))
			break;
	}
	if (ptr == NULL)
	{
		p = lisys_string_dup (path);
		if (p == NULL)
			return 0;
		lialg_list_prepend (&self->paths, p);
	}

	/* Scan the files in the directory. */
	dir = lisys_dir_open (path);
	if (dir == NULL)
		return 0;
	lisys_dir_set_filter (dir, lisys_dir_filter_files, NULL);
	if (!lisys_dir_scan (dir))
	{
		lisys_dir_free (dir);
		return 0;
	}

	/* Add the files to the lookup table. */
	/* If there are duplicate files, the new ones replace the old ones.
	   This allows mods to override files based on the registration order. */
	c = lisys_dir_get_count (dir);
	for (i = 0 ; i < c ; i++)
	{
		/* Construct the path name. */
		name = lisys_dir_get_name (dir, i);
		p = lisys_dir_get_path (dir, i);
		if (p == NULL)
			continue;

		/* Override or add. */
		node = lialg_strdic_find_node (self->files, name);
		if (node != NULL)
		{
			lisys_free (node->value);
			node->value = p;
		}
		else
			lialg_strdic_insert (self->files, name, p);
	}
	lisys_dir_free (dir);

	return 1;
}

/**
 * \brief Finds a file by name.
 * \param self Paths.
 * \param name File name.
 * \return path Absolute path or NULL.
 */
const char* lipth_paths_find_file (
	LIPthPaths* self,
	const char* name)
{
	return lialg_strdic_find (self->files, name);
}

/**
 * \brief Gets the path to a generic data file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_data (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, name, NULL);
}

/**
 * \brief Gets the path to a script file.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Full path or NULL.
 */
char* lipth_paths_get_script (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Try the override path. */
	path = lisys_path_concat (self->override_data, "scripts", name, NULL);
	if (path == NULL)
		return NULL;
	if (lisys_filesystem_access (path, LISYS_ACCESS_READ))
		return path;
	lisys_free (path);

	/* Try the real path. */
	return lisys_path_concat (self->module_data, "scripts", name, NULL);
}

/**
 * \brief Gets the path to an SQL database.
 *
 * Calling this function will create the save directory if it doesn't exist
 * yet. If the creation fails or the function runs out of memory, NULL is
 * returned and the error message is set.
 *
 * \param self Paths object.
 * \param name File name.
 * \return Newly allocated absolute path or NULL.
 */
char* lipth_paths_get_sql (
	const LIPthPaths* self,
	const char*       name)
{
	char* path;

	/* Format the path. */
	path = lisys_path_concat (self->module_state, name, NULL);
	if (path == NULL)
		return NULL;

	/* Check if the save directory exists. */
	if (lisys_filesystem_access (self->module_state, LISYS_ACCESS_EXISTS))
	{
		if (!lisys_filesystem_access (self->module_state, LISYS_ACCESS_WRITE))
		{
			lisys_error_set (EINVAL, "save path `%s' is not writable", path);
			lisys_free (path);
			return NULL;
		}
		return path;
	}

	/* Create the save directory. */
	if (!lisys_filesystem_makepath (self->module_state))
	{
		lisys_free (path);
		return NULL;
	}

	return path;
}

/**
 * \brief Gets the game root directory.
 * \return Path or NULL.
 */
char* lipth_paths_get_root ()
{
#ifdef LI_RELATIVE_PATHS
	return lisys_relative_exedir ();
#else
	return lisys_string_dup (LIDATADIR);
#endif
}

/** @} */
/** @} */
