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
 * \addtogroup liwdgButton Button
 * @{
 */

#ifndef __WIDGET_BUTTON_H__
#define __WIDGET_BUTTON_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_BUTTON(o) ((liwdgButton*)(o))

/**
 * \brief Button widget.
 */
typedef struct _liwdgButton liwdgButton;
struct _liwdgButton
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	char* string;
};

extern const liwdgWidgetClass liwdgButtonType;

liwdgWidget*
liwdg_button_new (liwdgManager* manager);

lifntFont*
liwdg_button_get_font (liwdgButton*  self);

void
liwdg_button_set_font (liwdgButton* self,
                       lifntFont*   font);

int
liwdg_button_set_text (liwdgButton* self,
                       const char*  text);

const char*
liwdg_button_get_text (liwdgButton* self);

#endif

/** @} */
/** @} */
