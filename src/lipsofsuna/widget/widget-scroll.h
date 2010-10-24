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

#ifndef __WIDGET_SCROLL_H__
#define __WIDGET_SCROLL_H__

#include <lipsofsuna/font.h>
#include <lipsofsuna/system.h>
#include "widget.h"

#define LIWDG_SCROLL(o) ((LIWdgScroll*)(o))

typedef struct _LIWdgScroll LIWdgScroll;
struct _LIWdgScroll
{
	LIWdgWidget base;
	LIFntFont* font;
	LIFntLayout* text;
	float min;
	float max;
	float value;
	float reference;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_scroll, ());

LIAPICALL (LIWdgWidget*, liwdg_scroll_new, (
	LIWdgManager* manager));

LIAPICALL (void, liwdg_scroll_set_range, (
	LIWdgScroll* self,
	float        min,
	float        max));

LIAPICALL (float, liwdg_scroll_get_value, (
	LIWdgScroll* self));

LIAPICALL (void, liwdg_scroll_set_value, (
	LIWdgScroll* self,
	float        value));

LIAPICALL (float, liwdg_scroll_get_reference, (
	LIWdgScroll* self));

LIAPICALL (void, liwdg_scroll_set_reference, (
	LIWdgScroll* self,
	float        value));

#endif
