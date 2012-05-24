/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTime Time
 * @{
 */

#include "module.h"

static void Time_get_date (LIScrArgs* args)
{
	char* str;
	time_t time;
	LISysTimeinfo info;

	/* Get the time. */
	lisys_time (&time);
	lisys_time_get_components (time, &info);

	/* Format the string. */
	str = lisys_string_format ("%4d-%02d-%02d",
		info.tm_year + 1900, info.tm_mon + 1, info.tm_mday);
	if (str == NULL)
		return;
	liscr_args_seti_string (args, str);
	lisys_free (str);
}

static void Time_get_date_time (LIScrArgs* args)
{
	char* str;
	time_t time;
	LISysTimeinfo info;

	/* Get the time. */
	lisys_time (&time);
	lisys_time_get_components (time, &info);

	/* Format the string. */
	str = lisys_string_format ("%4d-%02d-%02dT%02d:%02d:%02dZ",
		info.tm_year + 1900, info.tm_mon + 1, info.tm_mday,
		info.tm_hour, info.tm_min, info.tm_sec);
	if (str == NULL)
		return;
	liscr_args_seti_string (args, str);
	lisys_free (str);
}

static void Time_get_time (LIScrArgs* args)
{
	char* str;
	time_t time;
	LISysTimeinfo info;

	/* Get the time. */
	lisys_time (&time);
	lisys_time_get_components (time, &info);

	/* Format the string. */
	str = lisys_string_format ("%02d:%02d:%02d",
		info.tm_hour, info.tm_min, info.tm_sec);
	if (str == NULL)
		return;
	liscr_args_seti_string (args, str);
	lisys_free (str);
}

/*****************************************************************************/

void liext_script_time (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TIME, "time_get_date", Time_get_date);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TIME, "time_get_date_time", Time_get_date_time);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_TIME, "time_get_time", Time_get_time);
}

/** @} */
/** @} */
