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
 * \addtogroup LIWdgWidget Widget
 * @{
 */

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <SDL.h>
#include <lipsofsuna/system.h>
#include "widget-class.h"
#include "widget-event.h"
#include "widget-manager.h"
#include "widget-types.h"

#define LIWDG_WIDGET(o) ((LIWdgWidget*)(o))

struct _LIWdgWidget
{
	const LIWdgClass* type;
	LIWdgManager* manager;
	LIWdgWidget* parent;
	LIWdgWidget* prev;
	LIWdgWidget* next;
	LIWdgRect allocation;
	LIWdgSize hardrequest;
	LIWdgSize userrequest;
	LIWdgStyle* style;
	void* userdata;
	char* state_name;
	char* style_name;
	unsigned int behind : 1;
	unsigned int floating : 1;
	unsigned int focusable : 1;
	unsigned int fullscreen : 1;
	unsigned int temporary : 1;
	unsigned int visible : 1;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_widget, ());

LIAPICALL (LIWdgWidget*, liwdg_widget_new, (
	LIWdgManager*     manager,
	const LIWdgClass* clss));

LIAPICALL (void, liwdg_widget_free, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_connect, (
	LIWdgWidget* self,
	const char*  type,
	void*        func,
	void*        data,
	LICalHandle* handle));

LIAPICALL (int, liwdg_widget_detach, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_draw, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_event, (
	LIWdgWidget* self,
	LIWdgEvent*  event));

LIAPICALL (int, liwdg_widget_insert_callback, (
	LIWdgWidget* self,
	const char*  type,
	void*        func,
	void*        data));

LIAPICALL (int, liwdg_widget_insert_callback_full, (
	LIWdgWidget* self,
	const char*  type,
	int          priority,
	void*        func,
	void*        data,
	LICalHandle* handle));

LIAPICALL (void, liwdg_widget_move, (
	LIWdgWidget* self,
	int          x,
	int          y));

LIAPICALL (void, liwdg_widget_paint, (
	LIWdgWidget* self,
	LIWdgRect*   rect));

LIAPICALL (void, liwdg_widget_paint_custom, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_translate_coords, (
	LIWdgWidget* self,
	int          screenx,
	int          screeny,
	int*         widgetx,
	int*         widgety));

LIAPICALL (int, liwdg_widget_typeis, (
	const LIWdgWidget* self,
	const LIWdgClass*  clss));

LIAPICALL (void, liwdg_widget_update, (
	LIWdgWidget* self,
	float        secs));

LIAPICALL (void, liwdg_widget_get_allocation, (
	LIWdgWidget* self,
	LIWdgRect*   allocation));

LIAPICALL (void, liwdg_widget_set_allocation, (
	LIWdgWidget* self,
	int          x,
	int          y,
	int          w,
	int          h));

LIAPICALL (int, liwdg_widget_get_behind, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_behind, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (void, liwdg_widget_get_content, (
	LIWdgWidget* self,
	LIWdgRect*   allocation));

LIAPICALL (int, liwdg_widget_get_floating, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_floating, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_focusable, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_focusable, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_focused, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_focused, (
	LIWdgWidget* self));

LIAPICALL (LIFntFont*, liwdg_widget_get_font, (
	const LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_get_fullscreen, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_fullscreen, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (int, liwdg_widget_get_grab, (
	const LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_grab, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (void, liwdg_widget_get_request, (
	LIWdgWidget* self,
	LIWdgSize*   request));

LIAPICALL (void, liwdg_widget_set_request, (
	LIWdgWidget* self,
	int          w,
	int          h));

LIAPICALL (void, liwdg_widget_set_request_internal, (
	LIWdgWidget* self,
	int          w,
	int          h));

LIAPICALL (LIWdgWidget*, liwdg_widget_get_root, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_state, (
	LIWdgWidget* self,
	const char*  state));

LIAPICALL (LIWdgStyle*, liwdg_widget_get_style, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_style, (
	LIWdgWidget* self,
	const char*  style));

LIAPICALL (int, liwdg_widget_get_temporary, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_temporary, (
	LIWdgWidget* self,
	int          value));

LIAPICALL (void*, liwdg_widget_get_userdata, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_userdata, (
	LIWdgWidget* self,
	void*        value));

LIAPICALL (int, liwdg_widget_get_visible, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_set_visible, (
	LIWdgWidget* self,
	int          visible));

#endif

/** @} */
/** @} */
