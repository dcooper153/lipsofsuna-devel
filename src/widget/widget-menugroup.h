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
 * \addtogroup liwdgMenuGroup MenuGroup
 * @{
 */

#ifndef __WIDGET_MENUGROUP_H__
#define __WIDGET_MENUGROUP_H__

#include "widget-types.h"

typedef struct _liwdgMenuGroup liwdgMenuGroup;

liwdgMenuGroup*
liwdg_menu_group_new (const char* markup);

void
liwdg_menu_group_free (liwdgMenuGroup* self);

void
liwdg_menu_group_set_callback (liwdgMenuGroup* self,
                               liwdgHandler    call,
                               void*           data);

void*
liwdg_menu_group_get_userdata (liwdgMenuGroup* self);

void
liwdg_menu_group_set_userdata (liwdgMenuGroup* self,
                               void*           value);

#endif

/** @} */
/** @} */
