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
 * \addtogroup LIWdgBusy Busy
 * @{
 */

#ifndef __WIDGET_BUSY_H__
#define __WIDGET_BUSY_H__

#include <lipsofsuna/system.h>
#include "widget-window.h"

#define LIWDG_BUSY(o) ((LIWdgBusy*)(o))

typedef struct _LIWdgBusy LIWdgBusy;
struct _LIWdgBusy
{
	LIWdgWindow base;
	LIWdgWidget* progress;
	LIWdgWidget* button;
	liwdgHandler update_func;
	void* update_data;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_busy, ());

LIAPICALL (LIWdgWidget*, liwdg_busy_new, (
	LIWdgManager* manager));

LIAPICALL (void, liwdg_busy_set_cancel, (
	LIWdgBusy*   self,
	liwdgHandler handler,
	void*        data));

LIAPICALL (void, liwdg_busy_set_progress, (
	LIWdgBusy* self,
	float      value));

LIAPICALL (void, liwdg_busy_set_text, (
	LIWdgBusy*  self,
	const char* value));

LIAPICALL (void, liwdg_busy_set_update, (
	LIWdgBusy*   self,
	liwdgHandler handler,
	void*        data));

#endif

/** @} */
/** @} */

