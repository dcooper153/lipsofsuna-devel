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
 * \addtogroup liwdgWidget Widget
 * @{
 */

#ifndef __WIDGET_H__
#define __WIDGET_H__

#include <SDL.h>
#include <callback/lips-callback.h>
#include "widget-class.h"
#include "widget-event.h"
#include "widget-manager.h"
#include "widget-types.h"

#define LIWDG_WIDGET(o) ((liwdgWidget*)(o))

struct _liwdgWidget
{
	const liwdgClass* type;
	licalCallbacks* callbacks;
	liwdgManager* manager;
	liwdgWidget* parent;
	liwdgWidget* prev;
	liwdgWidget* next;
	liwdgRect allocation;
	liwdgSize hardrequest;
	liwdgSize userrequest;
	liwdgWidgetState state;
	void* userdata;
	unsigned int focusable : 1;
	unsigned int transparent : 1;
	unsigned int visible : 1;
};

extern const liwdgClass liwdgWidgetType;

liwdgWidget*
liwdg_widget_new (liwdgManager*     manager,
                  const liwdgClass* clss);

void
liwdg_widget_free (liwdgWidget* self);

int
liwdg_widget_detach (liwdgWidget* self);

int
liwdg_widget_event (liwdgWidget* self,
                    liwdgEvent*  event);

int
liwdg_widget_insert_callback (liwdgWidget* self,
                              licalType    type,
                              int          priority,
                              void*        call,
                              void*        data,
                              licalHandle* result);

void
liwdg_widget_move (liwdgWidget* self,
                   int          x,
                   int          y);

void
liwdg_widget_paint (liwdgWidget* self,
                    const char*  style,
                    liwdgRect*   rect);

int
liwdg_widget_register_callback (liwdgWidget* self,
                                licalType    type,
                                licalMarshal marshal);

void
liwdg_widget_remove_callback (liwdgWidget* self,
                              licalHandle* handle);

void
liwdg_widget_render (liwdgWidget* self);

void
liwdg_widget_translate_coords (liwdgWidget* self,
                               int          screenx,
                               int          screeny,
                               int*         widgetx,
                               int*         widgety);

int
liwdg_widget_typeis (const liwdgWidget* self,
                     const liwdgClass*  clss);

void
liwdg_widget_update (liwdgWidget* self,
                     float        secs);

void
liwdg_widget_get_allocation (liwdgWidget* self,
                             liwdgRect*   allocation);

void
liwdg_widget_set_allocation (liwdgWidget* self,
                             int          x,
                             int          y,
                             int          w,
                             int          h);

int
liwdg_widget_get_grab (const liwdgWidget* self);

void
liwdg_widget_set_grab (liwdgWidget* self,
                       int          value);

int
liwdg_widget_get_focus_mouse (liwdgWidget* self);

void
liwdg_widget_set_focus_mouse (liwdgWidget* self);

int
liwdg_widget_get_focus_keyboard (liwdgWidget* self);

void
liwdg_widget_set_focus_keyboard (liwdgWidget* self);

int
liwdg_widget_get_focusable (liwdgWidget* self);

void
liwdg_widget_set_focusable (liwdgWidget* self,
                            int          focusable);

void
liwdg_widget_get_request (liwdgWidget* self,
                          liwdgSize*   request);

void
liwdg_widget_set_request (liwdgWidget* self,
                          int          w,
                          int          h);

liwdgWidget*
liwdg_widget_get_root (liwdgWidget* self);

liwdgStyle*
liwdg_widget_get_style (liwdgWidget* self,
                        const char*  style);

void
liwdg_widget_get_style_allocation (liwdgWidget* self,
                                   const char*  style,
                                   liwdgRect*   allocation);

void
liwdg_widget_get_style_request (liwdgWidget* self,
                                const char*  style,
                                liwdgSize*   size);

void
liwdg_widget_set_style_request (liwdgWidget* self,
                                int          w,
                                int          h,
                                const char*  style);

void*
liwdg_widget_get_userdata (liwdgWidget* self);

void
liwdg_widget_set_userdata (liwdgWidget* self,
                           void*        value);

int
liwdg_widget_get_visible (liwdgWidget* self);

void
liwdg_widget_set_visible (liwdgWidget* self,
                          int          visible);

#endif

/** @} */
/** @} */
