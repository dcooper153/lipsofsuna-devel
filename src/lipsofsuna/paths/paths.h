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

#ifndef __PATHS_H__
#define __PATHS_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/algorithm.h"

/**
 * \addtogroup LIPth Paths
 * @{
 * \addtogroup LIPthPaths Paths
 * @{
 */

/**
 * \brief Stores a list of paths, and a cache of filenames found in those directories.
 *
 * When a LIPthPaths is constructed with \ref lipth_paths_new , it takes a root directory and a module name. The root directory acts is the base of the default path to search for files.
 * - \ref _LIPthPaths::module_data is the base path for searching for data files.
 * - \ref _LIPthPaths::module_data_save is the base path used for loading and saving game files.
 * - \ref _LIPthPaths::module_config is the base path used for loading and saving configuration.
 * - \ref _LIPthPaths::paths contains a linked list of paths that have already been added.
 *
 * By default `<module_data>` and `<module_data_save>` are added to \ref _LIPthPaths::paths , addtional paths are added by calling \ref lipth_paths_add_path, which will add `<module_data>/<sub_path>` and `<module_data_save>/<sub_path>`. When a path is added it will scan the path for files and each filename found into the dictionary \ref _LIPthPaths::files. If a file of the same name already exists, the most recently found one replaces any previously found. If an existing path is added again, \ref _LIPthPaths::paths is left unchanged but the directories are scanned, with new/existing files added/updated to \ref _LIPthPaths::files accordingly.
 *
 * Configuration files and save files are handled by a special mechanism, of calling \ref lipth_paths_create_file . Despite the name, this function doesn't actually create a file, but instead creates a full path to the desired file into either configuration directory or save directory based upon the config option. The path is `<module_config>/<name>` if config is non-zero or `<moduel_data_save>/<name>` if config is zero.
 *
 * The following paths don't refer to members of LIPthPaths or function parameter, but special system as described below:
 * - `<system_config_home>` refers to the path of the directory used for storing configuration, as obtained by \ref lisys_paths_get_config_home  (e.g. `/home/username/.config`).
 * - `<system_global_data/lipsofsuna>` refers to the path where the data directory with `lipsofsuna` was found in user's home data paths, as found by \ref lisys_paths_get_data_home .
 * - `<system_data_home>` refers to the path of the directory used for storing data in the users home directory, as obtained by \ref lisys_paths_get_data_home (e.g. `/home/username/.local/share`).
 */
typedef struct _LIPthPaths LIPthPaths;
struct _LIPthPaths
{
	char* root; /**< The root directory, which is used in global paths. This is defined when this object is instantiated.*/
	char* global_exts; /**< This is the base path used for searching for .dll/.so library extnesions `<global_data>/lib/extensions` or `<system_global_data>/lipsofsuna/lib/extensions` or NULL.*/
	char* global_data; /**< This contains the 'global' data directory `<root>/data`, `<system_global_data/lipsofsuna>`, or LIDATADIR . */
	char* module_name; /**< The module name. This is defined when this object is instantiated.*/
	char* module_config; /**< The path to the config directory for this module `<system_config_home>/lipsofsuna/<module_name>` . */
	char* module_data; /**< The path to the data directory of this module `<global_data>/<module_name>`*/
	char* module_data_save; /**< The path to the save data directory for this module `<system_data_home>/lipsofsuna/<module_name>`.*/
	LIAlgList* paths; /**< This is a linked list of paths that have been searched.*/
	LIAlgStrdic* files; /**< This dictionary is a cache of files previously found, and the absolute path to them.*/
};

/** @} */
/** @} */

LIAPICALL (LIPthPaths*, lipth_paths_new, (
	const char* path,
	const char* name));

LIAPICALL (void, lipth_paths_free, (
	LIPthPaths* self));

LIAPICALL (int, lipth_paths_add_path, (
	LIPthPaths* self,
	const char* path));

LIAPICALL (int, lipth_paths_add_path_abs, (
	LIPthPaths* self,
	const char* path));

LIAPICALL (const char*, lipth_paths_create_file, (
	LIPthPaths* self,
	const char* name,
	int         config));

LIAPICALL (const char*, lipth_paths_find_file, (
	LIPthPaths* self,
	const char* name));

LIAPICALL (char*, lipth_paths_find_path, (
	const LIPthPaths* self,
	const char*       path,
	int               config));

LIAPICALL (char*, lipth_paths_get_root, ());

#endif
