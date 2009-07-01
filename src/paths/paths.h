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

#ifndef __PATHS_H__
#define __PATHS_H__

typedef struct _lipthPaths lipthPaths;
struct _lipthPaths
{
	char* global_exts;
	char* global_data;
	char* global_state;
	char* module_data;
	char* module_state;
};

lipthPaths*
lipth_paths_new (const char* name);

void
lipth_paths_free (lipthPaths* self);

char*
lipth_paths_get_data (const lipthPaths* self,
                      const char*       name);

char*
lipth_paths_get_font (const lipthPaths* self,
                      const char*       name);

char*
lipth_paths_get_graphics (const lipthPaths* self,
                          const char*       name);

char*
lipth_paths_get_script (const lipthPaths* self,
                        const char*       name);

char*
lipth_paths_get_shader (const lipthPaths* self,
                        const char*       name);

char*
lipth_paths_get_sound (const lipthPaths* self,
                       const char*       name);

#endif

/** @} */
/** @} */
