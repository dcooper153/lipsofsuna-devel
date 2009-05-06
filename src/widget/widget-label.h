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
 * \addtogroup liwdgLabel Label
 * @{
 */

#ifndef __WIDGET_LABEL_H__
#define __WIDGET_LABEL_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_LABEL(o) ((liwdgLabel*)(o))

typedef struct _liwdgLabel liwdgLabel;
struct _liwdgLabel
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	char* string;
};

extern const liwdgWidgetClass liwdgLabelType;

liwdgWidget*
liwdg_label_new (liwdgManager* manager);

lifntFont*
liwdg_label_get_font (liwdgLabel* self);

void
liwdg_label_set_font (liwdgLabel* self,
                      lifntFont*  font);

int
liwdg_label_set_text (liwdgLabel* self,
                      const char* text);

const char*
liwdg_label_get_text (liwdgLabel* self);

#endif

/** @} */
/** @} */
