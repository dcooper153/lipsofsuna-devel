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
 * \addtogroup liwdgMenu Menu
 * @{
 */

#ifndef __WIDGET_MENU_H__
#define __WIDGET_MENU_H__

#include "widget.h"
#include "widget-menugroup.h"

#define LIWDG_MENU(o) ((liwdgMenu*)(o))

typedef struct _liwdgMenu liwdgMenu;

extern const liwdgClass liwdgMenuType;

liwdgWidget*
liwdg_menu_new (liwdgManager* manager);

int
liwdg_menu_insert_group (liwdgMenu*      self,
                         liwdgMenuGroup* group);

void
liwdg_menu_remove_group (liwdgMenu*      self,
                         liwdgMenuGroup* group);

int
liwdg_menu_get_item_rect (const liwdgMenu* self,
                          const char*      name,
                          liwdgRect*       value);

int
liwdg_menu_get_vertical (const liwdgMenu* self);

void
liwdg_menu_set_vertical (liwdgMenu* self,
                         int        value);

#endif

/** @} */
/** @} */
