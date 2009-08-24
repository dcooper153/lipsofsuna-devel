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
 * \addtogroup liwdgWindow Window
 * @{
 */

#ifndef __WIDGET_WINDOW_H__
#define __WIDGET_WINDOW_H__

#include "widget.h"
#include "widget-group.h"

#define LIWDG_WINDOW(o) ((liwdgWindow*)(o))

typedef struct _liwdgWindow liwdgWindow;
struct _liwdgWindow
{
	liwdgGroup base;
	liwdgWidget* label_title;
};

extern const liwdgClass liwdgWindowType;

liwdgWidget*
liwdg_window_new (liwdgManager* manager,
                  int           width,
                  int           height);

const char*
liwdg_window_get_title (const liwdgWindow* self);

void
liwdg_window_set_title (liwdgWindow* self,
                        const char*  title);

#endif

/** @} */
/** @} */
