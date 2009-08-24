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
 * \addtogroup lisys System
 * @{
 * \addtogroup lisysSystem System
 * @{
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef _WIN32
#define LISYS_EXTENSION_DLL "dll"
#define LISYS_EXTENSION_EXE ".exe"
#else
#define LISYS_EXTENSION_DLL "so"
#define LISYS_EXTENSION_EXE ""
#endif

char* lisys_system_get_path_home ();
char* lisys_system_get_path_data_home ();
char* lisys_system_get_path_config_home ();
char* lisys_system_get_path_cache_home ();

#endif

/** @} */
/** @} */
