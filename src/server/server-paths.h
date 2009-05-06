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
 * \addtogroup lisrv Server
 * @{
 * \addtogroup lisrvPaths Paths
 * @{
 */

#ifndef __SERVER_PATHS_H__
#define __SERVER_PATHS_H__

typedef struct _lisrvPaths lisrvPaths;
struct _lisrvPaths
{
	char* global_exts;
	char* global_data;
	char* global_state;
	char* server_data;
	char* server_state;
};

lisrvPaths*
lisrv_paths_new (const char* name);

void
lisrv_paths_free (lisrvPaths* self);

#endif

/** @} */
/** @} */
