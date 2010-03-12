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
 * \addtogroup LIWdgWindow Window
 * @{
 */

#ifndef __WIDGET_WINDOW_H__
#define __WIDGET_WINDOW_H__

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-group.h"

#define LIWDG_WINDOW(o) ((LIWdgWindow*)(o))

typedef struct _LIWdgWindow LIWdgWindow;
struct _LIWdgWindow
{
	LIWdgGroup base;
	LIFntFont* font;
	LIFntLayout* text;
	char* title;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_window, ());

LIAPICALL (LIWdgWidget*, liwdg_window_new, (
	LIWdgManager* manager,
	int           width,
	int           height));

LIAPICALL (const char*, liwdg_window_get_title, (
	const LIWdgWindow* self));

LIAPICALL (int, liwdg_window_set_title, (
	LIWdgWindow* self,
	const char*  title));

#endif

/** @} */
/** @} */
