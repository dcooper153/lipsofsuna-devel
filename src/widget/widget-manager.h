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
 * \addtogroup liwdgManager Manager
 * @{
 */

#ifndef __WIDGET_MANAGER_H__
#define __WIDGET_MANAGER_H__

#include <algorithm/lips-algorithm.h>
#include <font/lips-font.h>
#include <image/lips-image.h>
#include "widget.h"
#include "widget-style.h"
#include "widget-types.h"

struct _liwdgManager
{
	int width;
	int height;
	liwdgStyles* styles;
	limatMatrix projection;
	lividCalls video;
	struct
	{
		int active;
		int startx;
		int starty;
	} drag;
	struct
	{
		liwdgWidget* keyboard;
		liwdgWidget* mouse;
	} focus;
	struct
	{
		liwdgWidget* iter;
		liwdgWidget* root;
		liwdgWidget* grab;
		liwdgWidget* active;
		liwdgWidget* dialogs;
		liwdgWidget* popups;
	} widgets;
};

liwdgManager*
liwdg_manager_new (lividCalls* video,
                   const char* path);

void
liwdg_manager_free (liwdgManager* self);

int
liwdg_manager_event (liwdgManager* self,
                     liwdgEvent*   event);

int
liwdg_manager_event_sdl (liwdgManager* manager,
                         SDL_Event*    event);

lifntFont*
liwdg_manager_find_font (liwdgManager* self,
                         const char*   name);

liwdgStyle*
liwdg_manager_find_style (liwdgManager* self,
                          const char*   name);

liwdgWidget*
liwdg_manager_find_window_by_point (liwdgManager* self,
                                    int           x,
                                    int           y);

void
liwdg_manager_fix_focus (liwdgManager* self);

void
liwdg_manager_focus_next (liwdgManager* self);

void
liwdg_manager_focus_prev (liwdgManager* self);

int
liwdg_manager_insert_popup (liwdgManager* self,
                            liwdgWidget*  widget);

int
liwdg_manager_insert_window (liwdgManager* self,
                             liwdgWidget*  widget);

int
liwdg_manager_remove_popup (liwdgManager* self,
                            liwdgWidget*  widget);

int
liwdg_manager_remove_window (liwdgManager* self,
                             liwdgWidget*  widget);

void
liwdg_manager_render (liwdgManager* self);

void
liwdg_manager_update (liwdgManager* self,
                      float         secs);

liwdgWidget*
liwdg_manager_get_focus_keyboard (liwdgManager* self);

void
liwdg_manager_set_focus_keyboard (liwdgManager* self,
                                  liwdgWidget*  widget);

liwdgWidget*
liwdg_manager_get_focus_mouse (liwdgManager* self);

void
liwdg_manager_set_focus_mouse (liwdgManager* self,
                               liwdgWidget*  widget);

void
liwdg_manager_get_projection (liwdgManager* self,
                              limatMatrix*  matrix);

liwdgWidget*
liwdg_manager_get_root (liwdgManager* self);

void
liwdg_manager_set_root (liwdgManager* self,
                        liwdgWidget*  widget);

void
liwdg_manager_get_size (liwdgManager* self,
                        int*          x,
                        int*          y);

void
liwdg_manager_set_size (liwdgManager* self,
                        int           width,
                        int           height);

#endif

/** @} */
/** @} */
