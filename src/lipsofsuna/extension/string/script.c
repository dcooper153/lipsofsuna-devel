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
 * \addtogroup LIExtString String
 * @{
 */

#include "module.h"

static void String_utf8_to_wchar (LIScrArgs* args)
{
	int i;
	const char* str;
	wchar_t* wstr;

	if (liscr_args_geti_string (args, 0, &str))
	{
		wstr = lisys_utf8_to_wchar (str);
		if (wstr == NULL)
			return;
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		for (i = 0 ; wstr[i] != L'\0' ; i++)
			liscr_args_seti_int (args, wstr[i]);
		lisys_free (wstr);
	}
}

static void String_wchar_to_utf8 (LIScrArgs* args)
{
	int i;
	int val;
	char* str;
	LIArcWriter* writer;

	writer = liarc_writer_new ();
	if (writer == NULL)
		return;
	for (i = 0 ; liscr_args_geti_int (args, i, &val) ; i++)
	{
		str = lisys_wchar_to_utf8 (val);
		if (str != NULL)
		{
			liarc_writer_append_string (writer, str);
			lisys_free (str);
		}
	}
	if (liarc_writer_append_nul (writer))
		liscr_args_seti_string (args, liarc_writer_get_buffer (writer));
	liarc_writer_free (writer);
}

/*****************************************************************************/

void liext_script_string (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_STRING, "string_utf8_to_wchar", String_utf8_to_wchar);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_STRING, "string_wchar_to_utf8", String_wchar_to_utf8);
}

/** @} */
/** @} */
