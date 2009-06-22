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
 * \addtogroup licli Client
 * @{
 * \addtogroup licliPaths Paths
 * @{
 */

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "client-paths.h"

licliPaths*
licli_paths_new ()
{
	licliPaths* self;

	self = calloc (1, sizeof (licliPaths));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* FIXME: Saves everything under config. */
#ifdef LI_RELATIVE_PATHS
	self->local_data = lisys_path_concat (lisys_system_get_path_config_home (), PACKAGE, NULL);
	if (self->local_data == NULL)
		goto error;
	self->global_data = lisys_relative_exedir (NULL);
	if (self->global_data == NULL)
		goto error;
#else
	self->local_data = lisys_path_concat (lisys_system_get_path_config_home (), PACKAGE, NULL);
	if (self->local_data == NULL)
		goto error;
	self->global_data = LIDATADIR;
#endif

	/* Get extension directory. */
#ifdef LI_RELATIVE_PATHS
	self->global_exts = lisys_path_concat (self->global_data, "lib", "extensions", NULL);;
	if (self->global_exts == NULL)
		goto error;
#else
	self->global_exts = LIEXTSDIR;
#endif

	/* Create the local directory. */
	if (mkdir (self->local_data, S_IRWXU))
	{
		if (errno != EEXIST)
			printf ("WARNING: Cannot create directory `%s'.", self->local_data);
	}

	return self;

error:
	licli_paths_free (self);
	return NULL;
}

void
licli_paths_free (licliPaths* self)
{
	free (self->local_data);
#ifdef LI_RELATIVE_PATHS
	free (self->global_data);
	free (self->global_exts);
#endif
	free (self);
}

/** @} */
/** @} */
