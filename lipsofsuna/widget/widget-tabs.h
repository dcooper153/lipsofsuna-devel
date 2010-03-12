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
 * \addtogroup LIWdgTabs Tabs
 * @{
 */

#ifndef __WIDGET_TABS_H__
#define __WIDGET_TABS_H__

#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-container.h"
#include "widget-types.h"

#define LIWDG_TABS(o) ((LIWdgTabs*)(o))

typedef struct _LIWdgTab LIWdgTab;
struct _LIWdgTab
{
	char* text;
	LIFntFont* font;
	LIFntLayout* layout;
	LIWdgRect rect;
	LIWdgWidget* widget;
};

typedef struct _LIWdgTabs LIWdgTabs;
struct _LIWdgTabs
{
	LIWdgContainer base;
	int active;
	struct
	{
		int count;
		LIWdgTab* array;
	} tabs;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_tabs, ());

LIAPICALL (LIWdgWidget*, liwdg_tabs_new, (
	LIWdgManager* manager));

LIAPICALL (int, liwdg_tabs_append_tab, (
	LIWdgTabs*   self,
	const char*  title,
	LIWdgWidget* widget));

#endif

/** @} */
/** @} */
