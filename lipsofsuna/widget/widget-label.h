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
 * \addtogroup LIWdgLabel Label
 * @{
 */

#ifndef __WIDGET_LABEL_H__
#define __WIDGET_LABEL_H__

#include <lipsofsuna/font.h>
#include "widget.h"

#define LIWDG_LABEL(o) ((LIWdgLabel*)(o))

typedef struct _LIWdgLabel LIWdgLabel;
struct _LIWdgLabel
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	int highlight;
	float halign;
	float valign;
	char* string;
};

extern const LIWdgClass liwdg_widget_label;

LIWdgWidget*
liwdg_label_new (LIWdgManager* manager);

LIWdgWidget*
liwdg_label_new_with_text (LIWdgManager* manager,
                           const char*   text);

LIFntFont*
liwdg_label_get_font (LIWdgLabel* self);

void
liwdg_label_set_font (LIWdgLabel* self,
                      LIFntFont*  font);

float
liwdg_label_get_halign (const LIWdgLabel* self);

void
liwdg_label_set_halign (LIWdgLabel* self,
                        float       value);

int
liwdg_label_get_highlight (const LIWdgLabel* self);

void
liwdg_label_set_highlight (LIWdgLabel* self,
                           int         value);

int
liwdg_label_set_text (LIWdgLabel* self,
                      const char* text);

const char*
liwdg_label_get_text (LIWdgLabel* self);

float
liwdg_label_get_valign (const LIWdgLabel* self);

void
liwdg_label_set_valign (LIWdgLabel* self,
                        float       value);

#endif

/** @} */
/** @} */
