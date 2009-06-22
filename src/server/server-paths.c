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

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system/lips-system.h>
#include "server-paths.h"

lisrvPaths*
lisrv_paths_new (const char* name)
{
	char* tmp;
	lisrvPaths* self;

	self = calloc (1, sizeof (lisrvPaths));
	if (self == NULL)
		return NULL;

	/* Get data directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_data = lisys_relative_exedir (NULL);
	if (self->global_data == NULL)
		goto error;
	if (!strcmp (name, "data"))
		self->server_data = lisys_path_concat (self->global_data, "data", NULL);
	else
		self->server_data = lisys_path_concat (self->global_data, "mods", name, NULL);
	if (self->server_data == NULL)
		goto error;
#else
	self->global_data = LIDATADIR;
	if (!strcmp (name, "data"))
		self->server_data = lisys_path_concat (self->global_data, "data", NULL);
	else
		self->server_data = lisys_path_concat (self->global_data, "mods", name, NULL);
	if (self->server_data == NULL)
		goto error;
#endif

	/* Get save directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_state = strdup (self->global_data);
	if (self->global_state == NULL)
		goto error;
	self->server_state = lisys_path_concat (self->server_data, "save", NULL);
	if (self->server_state == NULL)
		goto error;
#else
	self->global_state = LISAVEDIR;
	self->server_state = lisys_path_concat (self->global_state, name, NULL);
	if (self->server_state == NULL)
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
	tmp = lisys_path_concat (self->server_state, "accounts", NULL);
	if (tmp != NULL)
	{
		mkdir (self->server_state, S_IRWXU);
		mkdir (tmp, S_IRWXU);
		free (tmp);
	}

	return self;

error:
	lisrv_paths_free (self);
	return NULL;
}

void
lisrv_paths_free (lisrvPaths* self)
{
#ifdef LI_RELATIVE_PATHS
	free (self->global_exts);
	free (self->global_data);
	free (self->global_state);
#endif
	free (self->server_data);
	free (self->server_state);
	free (self);
}

/** @} */
/** @} */
