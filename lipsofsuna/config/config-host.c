/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup licfg Config 
 * @{
 * \addtogroup licfgHost Host 
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "config-host.h"

/**
 * \brief Parses the host.def file, given the data directory root.
 *
 * \param dir The data directory root.
 * \return A new host configuration or NULL.
 */
licfgHost*
licfg_host_new (const char* dir)
{
	char* path;
	char* key;
	char* value;
	liReader* reader;
	licfgHost* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (licfgHost));
	if (self == NULL)
		return NULL;
                                
	/* Open the definition file. */
	path = lisys_path_concat (dir, "config", "host.def", NULL);
	if (path == NULL)
	{
		licfg_host_free (self);
		return NULL;
	}
	reader = li_reader_new_from_file (path);
	lisys_free (path);
	if (reader == NULL)
	{
		licfg_host_free (self);
		return NULL;
	}

	/* Read the host name. */
	if (!li_reader_get_key_value_pair (reader, &key, &value))
	{
		li_reader_free (reader);
		licfg_host_free (self);
		return NULL;
	}
	if (strcmp (key, "host"))
	{
		lisys_error_set (EINVAL, "excepted host field");
		li_reader_free (reader);
		licfg_host_free (self);
		lisys_free (key);
		lisys_free (value);
		return NULL;
	}
	self->host = value;
	lisys_free (key);

	/* Read the port. */
	if (!li_reader_get_key_value_pair (reader, &key, &value))
	{
		li_reader_free (reader);
		licfg_host_free (self);
		return NULL;
	}
	if (strcmp (key, "port"))
	{
		lisys_error_set (EINVAL, "expected port field");
		li_reader_free (reader);
		licfg_host_free (self);
		lisys_free (key);
		lisys_free (value);
		return NULL;
	}
	self->port = atoi (value);
	lisys_free (key);
	lisys_free (value);

	/* Read the protocol. */
	if (!li_reader_get_key_value_pair (reader, &key, &value))
	{
		li_reader_free (reader);
		licfg_host_free (self);
		return NULL;
	}
	if (strcmp (key, "protocol"))
	{
		lisys_error_set (EINVAL, "expected protocol field");
		li_reader_free (reader);
		licfg_host_free (self);
		lisys_free (key);
		lisys_free (value);
		return NULL;
	}
	if (!strcmp (value, "udp"))
		self->udp = 1;
	else if (!strcmp (value, "tcp"))
		self->udp = 0;
	else
	{
		lisys_error_set (EINVAL, "unknown protocol");
		li_reader_free (reader);
		licfg_host_free (self);
		lisys_free (key);
		lisys_free (value);
		return NULL;
	}
	lisys_free (key);
	lisys_free (value);

	li_reader_free (reader);
	return self;
}

/**
 * \brief Frees the host configuration parser.
 *
 * \param self A host configuration parser.
 */
void
licfg_host_free (licfgHost* self)
{
	lisys_free (self->host);
	lisys_free (self);
}

/** @} */
/** @} */

