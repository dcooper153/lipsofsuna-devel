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
 * \addtogroup liwdgView View
 * @{
 */

#ifndef __WIDGET_VIEW_H__
#define __WIDGET_VIEW_H__

#include "widget.h"
#include "widget-container.h"
#include "widget-types.h"

#define LIWDG_VIEW(o) ((liwdgView*)(o))

typedef struct _liwdgView liwdgView;
struct _liwdgView
{
	liwdgContainer base;
	liwdgWidget* child;
	int hscroll;
	int hscrollpos;
	int vscroll;
	int vscrollpos;
};

extern const liwdgClass liwdgViewType;

liwdgWidget*
liwdg_view_new (liwdgManager* manager);

liwdgWidget*
liwdg_view_get_child (liwdgView* self);

void
liwdg_view_set_child (liwdgView*   self,
                      liwdgWidget* widget);

void
liwdg_view_set_hscroll (liwdgView* self,
                        int        value);

void
liwdg_view_set_vscroll (liwdgView* self,
                        int        value);

#endif

/** @} */
/** @} */
