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
 * \addtogroup LIWdgMenu Menu
 * @{
 */

#ifndef __WIDGET_MENU_H__
#define __WIDGET_MENU_H__

#include "widget.h"

#define LIWDG_MENU(o) ((LIWdgMenu*)(o))

typedef struct _LIWdgMenu LIWdgMenu;
typedef struct _LIWdgMenuItem LIWdgMenuItem;

extern const LIWdgClass liwdg_widget_menu;

LIWdgWidget*
liwdg_menu_new (LIWdgManager* manager);

int
liwdg_menu_insert_item (LIWdgMenu*   self,
                        const char*  label,
                        const char*  icon,
                        liwdgHandler call,
                        void*        data);

int
liwdg_menu_get_autohide (const LIWdgMenu* self);

void
liwdg_menu_set_autohide (LIWdgMenu* self,
                         int        value);

LIWdgMenuItem*
liwdg_menu_get_item (const LIWdgMenu* self,
                     int              index);

int
liwdg_menu_get_item_count (const LIWdgMenu* self);

int
liwdg_menu_get_item_rect (const LIWdgMenu* self,
                          const char*      name,
                          LIWdgRect*       value);

int
liwdg_menu_get_vertical (const LIWdgMenu* self);

void
liwdg_menu_set_vertical (LIWdgMenu* self,
                         int        value);

#endif

/** @} */
/** @} */
