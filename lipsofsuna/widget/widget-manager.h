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
 * \addtogroup LIWdgManager Manager
 * @{
 */

#ifndef __WIDGET_MANAGER_H__
#define __WIDGET_MANAGER_H__

#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/callback.h>
#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include "widget.h"
#include "widget-style.h"
#include "widget-types.h"

struct _LIWdgManager
{
	int width;
	int height;
	LIWdgStyles* styles;
	LICalCallbacks* callbacks;
	LIMatMatrix projection;
	LIVidCalls video;
	struct
	{
		int active;
		int startx;
		int starty;
	} drag;
	struct
	{
		LIWdgWidget* keyboard;
		LIWdgWidget* mouse;
	} focus;
	struct
	{
		int x;
		int y;
	} pointer;
	struct
	{
		LIWdgWidget* iter;
		LIWdgWidget* root;
		LIWdgWidget* grab;
		LIWdgWidget* active;
		LIWdgWidget* dialogs;
		LIWdgWidget* popups;
	} widgets;
};

LIWdgManager*
liwdg_manager_new (LIVidCalls*     video,
                   LICalCallbacks* callbacks,
                   const char*     path);

void
liwdg_manager_free (LIWdgManager* self);

int
liwdg_manager_alloc_widgets (LIWdgManager* self,
                                           ...);

void
liwdg_manager_cycle_focus (LIWdgManager* self,
                           int           next);

void
liwdg_manager_cycle_window_focus (LIWdgManager* self,
                                  int           next);

int
liwdg_manager_event (LIWdgManager* self,
                     liwdgEvent*   event);

int
liwdg_manager_event_sdl (LIWdgManager* manager,
                         SDL_Event*    event);

LIFntFont*
liwdg_manager_find_font (LIWdgManager* self,
                         const char*   name);

LIWdgStyle*
liwdg_manager_find_style (LIWdgManager* self,
                          const char*   name);

LIWdgWidget*
liwdg_manager_find_window_by_point (LIWdgManager* self,
                                    int           x,
                                    int           y);

void
liwdg_manager_fix_focus (LIWdgManager* self);

int
liwdg_manager_insert_popup (LIWdgManager* self,
                            LIWdgWidget*  widget);

int
liwdg_manager_insert_window (LIWdgManager* self,
                             LIWdgWidget*  widget);

int
liwdg_manager_remove_popup (LIWdgManager* self,
                            LIWdgWidget*  widget);

int
liwdg_manager_remove_window (LIWdgManager* self,
                             LIWdgWidget*  widget);

void
liwdg_manager_render (LIWdgManager* self);

void
liwdg_manager_update (LIWdgManager* self,
                      float         secs);

LIWdgWidget*
liwdg_manager_get_focus_keyboard (LIWdgManager* self);

void
liwdg_manager_set_focus_keyboard (LIWdgManager* self,
                                  LIWdgWidget*  widget);

LIWdgWidget*
liwdg_manager_get_focus_mouse (LIWdgManager* self);

void
liwdg_manager_set_focus_mouse (LIWdgManager* self,
                               LIWdgWidget*  widget);

void
liwdg_manager_get_projection (LIWdgManager* self,
                              LIMatMatrix*  matrix);

LIWdgWidget*
liwdg_manager_get_root (LIWdgManager* self);

void
liwdg_manager_set_root (LIWdgManager* self,
                        LIWdgWidget*  widget);

void
liwdg_manager_get_size (LIWdgManager* self,
                        int*          x,
                        int*          y);

void
liwdg_manager_set_size (LIWdgManager* self,
                        int           width,
                        int           height);

#endif

/** @} */
/** @} */
