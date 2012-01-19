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

typedef struct _LIPthPaths LIPthPaths;
struct _LIPthPaths
{
	char* root;
	char* global_exts;
	char* global_data;
	char* global_state;
	char* module_data;
	char* module_name;
	char* module_state;
	char* override_data;
	LIAlgList* paths;
	LIAlgStrdic* files;
};

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
	const char* name));

LIAPICALL (const char*, lipth_paths_find_file, (
	LIPthPaths* self,
	const char* name));

LIAPICALL (char*, lipth_paths_get_data, (
	const LIPthPaths* self,
	const char*       name));

LIAPICALL (char*, lipth_paths_get_script, (
	const LIPthPaths* self,
	const char*       name));

LIAPICALL (char*, lipth_paths_get_sql, (
	const LIPthPaths* self,
	const char*       name));

LIAPICALL (char*, lipth_paths_get_root, ());

#endif
