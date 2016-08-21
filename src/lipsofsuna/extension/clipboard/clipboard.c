/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 */
#include "clipboard-module.h"

static char* clipboard_text = NULL;

static void Clipboard_init (LIScrArgs* args)
{
}

static void Clipboard_set (LIScrArgs* args)
{
	const char* text;
	if (!liscr_args_geti_string (args, 0, &text)) {
		return;
	}
	/*Copy string into the clipboard for later use*/
	if(clipboard_text != NULL) {
		/*Release the old string.*/
		lisys_free(clipboard_text);
	}
	clipboard_text = lisys_string_dup(text);
}

static void Clipboard_get (LIScrArgs* args)
{
	/*Set return text with the contents of the clipboard.*/
	liscr_args_seti_string(args, clipboard_text);
}

void liext_script_clipboard (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_CLIPBOARD, "clipboard_init", Clipboard_init);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_CLIPBOARD, "clipboard_get", Clipboard_get);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_CLIPBOARD, "clipboard_set", Clipboard_set);
}

/** @} */
