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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup liwdgEntry Entry
 * @{
 */

#ifndef __WIDGET_ENTRY_H__
#define __WIDGET_ENTRY_H__

#include <font/lips-font.h>
#include <string/lips-string.h>
#include "widget.h"

#define LIWDG_ENTRY(o) ((liwdgEntry*)(o))

/**
 * \brief Text edit widget.
 *
 * This widget implements a frame in which text can be displayed and edited.
 * Both single and multiple line display are supported.
 */
typedef struct _liwdgEntry liwdgEntry;
struct _liwdgEntry
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	char* string;
	unsigned int editable : 1;
	unsigned int secret : 1;
};

extern const liwdgWidgetClass liwdgEntryType;

liwdgWidget*
liwdg_entry_new (liwdgManager* manager);

void
liwdg_entry_clear (liwdgEntry* self);

int
liwdg_entry_get_editable (liwdgEntry* self);

void
liwdg_entry_set_editable (liwdgEntry* self,
                          int         editable);

lifntFont*
liwdg_entry_get_font (liwdgEntry* self);

void
liwdg_entry_set_font (liwdgEntry* self,
                      lifntFont*  font);

int
liwdg_entry_get_secret (liwdgEntry* self);

void
liwdg_entry_set_secret (liwdgEntry* self,
                        int         secret);

const char*
liwdg_entry_get_text (liwdgEntry* self);

int
liwdg_entry_set_text (liwdgEntry* self,
                      const char* text);

#endif

/** @} */
/** @} */
