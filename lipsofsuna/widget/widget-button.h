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
 * \addtogroup LIWdgButton Button
 * @{
 */

#ifndef __WIDGET_BUTTON_H__
#define __WIDGET_BUTTON_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/system.h>
#include "widget.h"

#define LIWDG_BUTTON(o) ((LIWdgButton*)(o))

/**
 * \brief Button widget.
 */
typedef struct _LIWdgButton LIWdgButton;
struct _LIWdgButton
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	char* string;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_button, ());

LIAPICALL (LIWdgWidget*, liwdg_button_new, (
	LIWdgManager* manager));

LIAPICALL (LIFntFont*, liwdg_button_get_font, (
	LIWdgButton*  self));

LIAPICALL (void, liwdg_button_set_font, (
	LIWdgButton* self,
	LIFntFont*   font));

LIAPICALL (int, liwdg_button_set_text, (
	LIWdgButton* self,
	const char*  text));

LIAPICALL (const char*, liwdg_button_get_text, (
	LIWdgButton* self));

#endif

/** @} */
/** @} */
