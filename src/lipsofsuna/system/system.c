/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup LISys System
 * @{
 * \addtogroup LISysSystem System
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "system.h"
#include "system-error.h"
#include "system-path.h"

void lisys_assert_fail (
	const char* asrt,
	const char* file,
	int         line,
	const char* func)
{
	fprintf (stderr, "%s:%d: %s: Assertion `%s' failed.\n", file, line, func, asrt);
	abort ();
}

/**
 * \brief Gets the current time.
 *
 * \param t Return location for the time or NULL.
 * \return Time.
 */
time_t lisys_time (
	time_t* t)
{
	return time (t);
}

/**
 * \brief Gets the home directory.
 *
 * If no home directory is specified, the current
 * working directory is returned instead.
 *
 * \return New string or NULL if out of memory.
 */
char* lisys_system_get_path_home ()
{
	char* home;

	home = getenv ("HOME");
	if (home == NULL)
		return strdup (".");
	return strdup (home);
}

/**
 * \brief Gets the home data directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_system_get_path_data_home ()
{
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_DATA_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_system_get_path_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".local/share", NULL);
	free (tmp);

	return ret;
}

/**
 * \brief Gets the home config directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_system_get_path_config_home ()
{
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_CONFIG_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_system_get_path_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".config", NULL);
	free (tmp);

	return ret;
}

/**
 * \brief Gets the home cache directory.
 *
 * Follows the XDG Base Directory Specification:
 * http://www.freedesktop.org/Standards/basedir-spec
 *
 * \return New string or NULL.
 */
char* lisys_system_get_path_cache_home ()
{
	char* ret;
	char* tmp;
	const char* dir;

	dir = getenv ("XDG_CACHE_HOME");
	if (dir != NULL && dir[0] != '\0')
		return strdup (dir);
	tmp = lisys_system_get_path_home ();
	if (tmp == NULL)
		return NULL;
	ret = lisys_path_concat (tmp, ".cache", NULL);
	free (tmp);

	return ret;
}

/** @} */
/** @} */
