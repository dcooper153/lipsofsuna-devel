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
 * \addtogroup liwdgTabs Tabs
 * @{
 */

#ifndef __WIDGET_TABS_H__
#define __WIDGET_TABS_H__

#include "widget.h"
#include "widget-container.h"
#include "widget-types.h"

#define LIWDG_TABS(o) ((liwdgTabs*)(o))

typedef struct _liwdgTab liwdgTab;
struct _liwdgTab
{
	char* text;
	lifntFont* font;
	lifntLayout* layout;
	liwdgRect rect;
	liwdgWidget* widget;
};

typedef struct _liwdgTabs liwdgTabs;
struct _liwdgTabs
{
	liwdgContainer base;
	int active;
	struct
	{
		int count;
		liwdgTab* array;
	} tabs;
};

extern const liwdgClass liwdgTabsType;

liwdgWidget*
liwdg_tabs_new (liwdgManager* manager);

int
liwdg_tabs_append_tab (liwdgTabs*   self,
                       const char*  title,
                       liwdgWidget* widget);

#endif

/** @} */
/** @} */
