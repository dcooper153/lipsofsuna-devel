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
 * \addtogroup LIWdgView View
 * @{
 */

#ifndef __WIDGET_VIEW_H__
#define __WIDGET_VIEW_H__

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-container.h"
#include "widget-types.h"

#define LIWDG_VIEW(o) ((LIWdgView*)(o))

typedef struct _LIWdgView LIWdgView;
struct _LIWdgView
{
	LIWdgContainer base;
	LIWdgWidget* child;
	int hscroll;
	int hscrollpos;
	int vscroll;
	int vscrollpos;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_view, ());

LIAPICALL (LIWdgWidget*, liwdg_view_new, (
	LIWdgManager* manager));

LIAPICALL (LIWdgWidget*, liwdg_view_get_child, (
	LIWdgView* self));

LIAPICALL (void, liwdg_view_set_child, (
	LIWdgView*   self,
	LIWdgWidget* widget));

LIAPICALL (int, liwdg_view_get_hscroll, (
	LIWdgView* self));

LIAPICALL (void, liwdg_view_set_hscroll, (
	LIWdgView* self,
	int        value));

LIAPICALL (int, liwdg_view_get_vscroll, (
	LIWdgView* self));

LIAPICALL (void, liwdg_view_set_vscroll, (
	LIWdgView* self,
	int        value));

#endif

/** @} */
/** @} */
