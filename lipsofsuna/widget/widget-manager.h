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
#include <lipsofsuna/system.h>
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
		LIWdgWidget* top;
		LIWdgWidget* bottom;
	} dialogs;
	struct
	{
		int active;
		int startx;
		int starty;
	} drag;
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
		LIWdgWidget* popups;
	} widgets;
};

LIAPICALL (LIWdgManager*, liwdg_manager_new, (
	LIVidCalls*     video,
	LICalCallbacks* callbacks,
	const char*     path));

LIAPICALL (void, liwdg_manager_free, (
	LIWdgManager* self));

LIAPICALL (int, liwdg_manager_alloc_widgets, (
	LIWdgManager* self,
	              ...));

LIAPICALL (void, liwdg_manager_cycle_focus, (
	LIWdgManager* self,
	int           next));

LIAPICALL (void, liwdg_manager_cycle_window_focus, (
	LIWdgManager* self,
	int           next));

LIAPICALL (int, liwdg_manager_event, (
	LIWdgManager* self,
	LIWdgEvent*   event));

LIAPICALL (int, liwdg_manager_event_sdl, (
	LIWdgManager* manager,
	SDL_Event*    event));

LIAPICALL (LIFntFont*, liwdg_manager_find_font, (
	LIWdgManager* self,
	const char*   name));

LIAPICALL (LIWdgStyle*, liwdg_manager_find_style, (
	LIWdgManager* self,
	const char*   name,
	const char*   state));

LIAPICALL (LIWdgWidget*, liwdg_manager_find_widget_by_point, (
	LIWdgManager* self,
	int           x,
	int           y));

LIAPICALL (LIWdgWidget*, liwdg_manager_find_window_by_point, (
	LIWdgManager* self,
	int           x,
	int           y));

LIAPICALL (int, liwdg_manager_insert_popup, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (int, liwdg_manager_insert_window, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (int, liwdg_manager_remove_popup, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (int, liwdg_manager_remove_window, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_render, (
	LIWdgManager* self));

LIAPICALL (void, liwdg_manager_update, (
	LIWdgManager* self,
	float         secs));

LIAPICALL (LIWdgWidget*, liwdg_manager_get_focus, (
	LIWdgManager* self));

LIAPICALL (void, liwdg_manager_set_focus, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_get_projection, (
	LIWdgManager* self,
	LIMatMatrix*  matrix));

LIAPICALL (LIWdgWidget*, liwdg_manager_get_root, (
	LIWdgManager* self));

LIAPICALL (void, liwdg_manager_set_root, (
	LIWdgManager* self,
	LIWdgWidget*  widget));

LIAPICALL (void, liwdg_manager_get_size, (
	LIWdgManager* self,
	int*          x,
	int*          y));

LIAPICALL (void, liwdg_manager_set_size, (
	LIWdgManager* self,
	int           width,
	int           height));

#endif

/** @} */
/** @} */
