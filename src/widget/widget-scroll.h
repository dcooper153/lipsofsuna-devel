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
 * \addtogroup liwdgScroll Scroll
 * @{
 */

#ifndef __WIDGET_SCROLL_H__
#define __WIDGET_SCROLL_H__

#include <font/lips-font.h>
#include "widget.h"

#define LIWDG_SCROLL(o) ((liwdgScroll*)(o))

typedef struct _liwdgScroll liwdgScroll;
struct _liwdgScroll
{
	liwdgWidget base;
	lifntFont* font;
	lifntLayout* text;
	float min;
	float max;
	float value;
};

extern const liwdgClass liwdgScrollType;

liwdgWidget*
liwdg_scroll_new (liwdgManager* manager);

lifntFont*
liwdg_scroll_get_font (liwdgScroll* self);

void
liwdg_scroll_set_font (liwdgScroll* self,
                       lifntFont*   font);

void
liwdg_scroll_set_range (liwdgScroll* self,
                        float        min,
                        float        max);

float
liwdg_scroll_get_value (liwdgScroll* self);

void
liwdg_scroll_set_value (liwdgScroll* self,
                        float        value);

#endif

/** @} */
/** @} */
