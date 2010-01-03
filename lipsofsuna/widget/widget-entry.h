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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgEntry Entry
 * @{
 */

#ifndef __WIDGET_ENTRY_H__
#define __WIDGET_ENTRY_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/string.h>
#include "widget.h"

#define LIWDG_ENTRY(o) ((LIWdgEntry*)(o))

/**
 * \brief Text edit widget.
 *
 * This widget implements a frame in which text can be displayed and edited.
 * Both single and multiple line display are supported.
 */
typedef struct _LIWdgEntry LIWdgEntry;
struct _LIWdgEntry
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	char* string;
	unsigned int editable : 1;
	unsigned int secret : 1;
};

extern const LIWdgClass liwdg_widget_entry;

LIWdgWidget*
liwdg_entry_new (LIWdgManager* manager);

void
liwdg_entry_clear (LIWdgEntry* self);

int
liwdg_entry_get_editable (LIWdgEntry* self);

void
liwdg_entry_set_editable (LIWdgEntry* self,
                          int         editable);

LIFntFont*
liwdg_entry_get_font (LIWdgEntry* self);

void
liwdg_entry_set_font (LIWdgEntry* self,
                      LIFntFont*  font);

int
liwdg_entry_get_secret (LIWdgEntry* self);

void
liwdg_entry_set_secret (LIWdgEntry* self,
                        int         secret);

const char*
liwdg_entry_get_text (LIWdgEntry* self);

int
liwdg_entry_set_text (LIWdgEntry* self,
                      const char* text);

#endif

/** @} */
/** @} */
