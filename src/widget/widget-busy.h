/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup liwdgBusy Busy
 * @{
 */

#ifndef __WIDGET_BUSY_H__
#define __WIDGET_BUSY_H__

#include "widget-window.h"

#define LIWDG_BUSY(o) ((liwdgBusy*)(o))

typedef struct _liwdgBusy liwdgBusy;
struct _liwdgBusy
{
	liwdgWindow base;
	liwdgWidget* progress;
	liwdgWidget* button;
	liwdgHandler update_func;
	void* update_data;
};

extern const liwdgClass liwdgBusyType;

liwdgWidget*
liwdg_busy_new (liwdgManager* manager);

void
liwdg_busy_set_cancel (liwdgBusy*   self,
                       liwdgHandler handler,
                       void*        data);

void
liwdg_busy_set_progress (liwdgBusy* self,
                         float      value);

void
liwdg_busy_set_text (liwdgBusy*  self,
                     const char* value);

void
liwdg_busy_set_update (liwdgBusy*   self,
                       liwdgHandler handler,
                       void*        data);

#endif

/** @} */
/** @} */

