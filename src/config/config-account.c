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
 * \addtogroup licfg Config 
 * @{
 * \addtogroup licfgAccount Account
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string/lips-string.h>
#include "config-account.h"

/**
 * \brief Parses an account file.
 *
 * \param path Full path to the file.
 * \return New account configuration or NULL.
 */
licfgAccount*
licfg_account_new (const char* path)
{
	int ok;
	char* key;
	char* value;
	liReader* reader = NULL;
	licfgAccount* self;

	/* Allocate self. */
	self = calloc (1, sizeof (licfgAccount));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the file. */
	reader = li_reader_new_from_file (path);
	if (reader == NULL)
		goto error;
	
	/* Read configuration. */
	while (1)
	{
		/* Skip empty lines. */
		li_reader_skip_chars (reader, " \t\n");
		if (li_reader_check_end (reader))
			break;

		/* Extract a key value pair. */
		if (!li_reader_get_key_value_pair (reader, &key, &value))
			goto error;
		ok = 1;

		/* Handle variables. */
		if (!strcmp (key, "admin"))
		{
			if (!strcmp (value, "1"))
				self->admin = 1;
			else
				self->admin = 0;
		}
		else if (!strcmp (key, "password"))
		{
			if (self->password == NULL)
			{
				self->password = value;
				value = NULL;
			}
			else
				ok = 0;
		}

		/* Catch errors. */
		free (key);
		free (value);
		if (!ok)
			goto error;
	}

	/* Sanity checks. */
	if (self->password == NULL)
	{
		lisys_error_set (EINVAL, "account has no password");
		goto error;
	}

	li_reader_free (reader);
	return self;

error:
	licfg_account_free (self);
	if (reader != NULL)
		li_reader_free (reader);
	return NULL;
}

/**
 * \brief Frees the account configuration.
 *
 * \param self Account configuration.
 */
void
licfg_account_free (licfgAccount* self)
{
	free (self->password);
	free (self);
}

/** @} */
/** @} */

