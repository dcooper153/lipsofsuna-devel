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
 * \addtogroup licfgBans Bans
 * @{
 */

#include <string/lips-string.h>
#include <system/lips-system.h>
#include "config-bans.h"

/**
 * \brief Creates a new ban list.
 *
 * \return New ban list or NULL.
 */
licfgBans*
licfg_bans_new ()
{
	licfgBans* self;

	/* Allocate self. */
	self = calloc (1, sizeof (licfgBans));
	if (self == NULL)
		return NULL;
	self->bans = lialg_strdic_new ();
	if (self->bans == NULL)
	{
		licfg_bans_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Loads a ban list from a file.
 *
 * \param dir Directory containing bans.def.
 * \return New ban list or NULL.
 */
licfgBans*
licfg_bans_new_from_file (const char* dir)
{
	char* name;
	char* path;
	liReader* reader = NULL;
	licfgBans* self;

	/* Allocate self. */
	self = calloc (1, sizeof (licfgBans));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->bans = lialg_strdic_new ();
	if (self->bans == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Open the file. */
	path = lisys_path_concat (dir, "bans.def", NULL);
	if (path == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	reader = li_reader_new_from_file (path);
	free (path);
	if (reader == NULL)
		goto error;

	/* Read bans. */
	while (1)
	{
		li_reader_skip_chars (reader, " \t\n");
		if (li_reader_check_end (reader))
			break;
		if (!li_reader_get_text (reader, " \t\n", &name))
			goto error;
		if (!licfg_bans_insert_ban (self, name))
		{
			lisys_error_set (ENOMEM, NULL);
			free (name);
			goto error;
		}
		free (name);
	}

	li_reader_free (reader);
	return self;

error:
	if (reader != NULL)
		li_reader_free (reader);
	licfg_bans_free (self);
	return NULL;
}

/**
 * \brief Frees the ban list.
 *
 * \param self Ban list.
 */
void
licfg_bans_free (licfgBans* self)
{
	if (self->bans != NULL)
		lialg_strdic_free (self->bans);
	free (self);
}

/**
 * \brief Inserts a ban rule.
 *
 * \param self Ban list.
 * \param ip Address to ban.
 * \return Nonzero on success.
 */
int
licfg_bans_insert_ban (licfgBans*  self,
                       const char* ip)
{
	if (!lialg_strdic_insert (self->bans, ip, (void*) 1))
		return 0;
	return 1;
}

/**
 * \brief Removes a ban rule.
 *
 * \param self Ban list.
 * \param ip Address to unban.
 * \return Nonzero on success.
 */
int
licfg_bans_remove_ban (licfgBans*  self,
                       const char* ip)
{
	return lialg_strdic_remove (self->bans, ip);
}

/**
 * \brief Checks if the address is banned.
 *
 * \param self Ban list.
 * \param ip Address.
 * \return Nonzero if banned.
 */
int
licfg_bans_get_banned (licfgBans*  self,
                       const char* ip)
{
	return (lialg_strdic_find (self->bans, ip) != NULL);
}

/**
 * \brief Gets the number of ban rules in the list.
 *
 * \param self Ban list.
 * \return Number of rules.
 */
int
licfg_bans_get_count (licfgBans* self)
{
	return self->bans->size;
}

/** @} */
/** @} */
