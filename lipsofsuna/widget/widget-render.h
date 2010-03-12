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
 * \addtogroup LIWdgRender Render
 * @{
 */

#ifndef __WIDGET_RENDER_H__
#define __WIDGET_RENDER_H__

#include <lipsofsuna/render.h>
#include <lipsofsuna/system.h>
#include "widget.h"
#include "widget-group.h"

#define LIWDG_RENDER(o) ((LIWdgRender*)(o))

typedef struct _LIWdgRender LIWdgRender;
struct _LIWdgRender
{
	LIWdgGroup base;
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenDeferred* deferred;
	LIRenScene* scene;
	void (*custom_render_func)(LIWdgRender*, void*);
	void* custom_render_data;
	void (*custom_update_func)(LIWdgRender*, void*);
	void* custom_update_data;
};

LIAPICALL (const LIWdgClass*, liwdg_widget_render, ());

LIAPICALL (LIWdgWidget*, liwdg_render_new, (
	LIWdgManager* manager,
	LIRenScene*   scene));

LIAPICALL (int, liwdg_render_pick, (
	LIWdgRender*    self,
	LIRenSelection* result,
	int             x,
	int             y));

LIAPICALL (void, liwdg_render_set_modelview, (
	LIWdgRender*       self,
	const LIMatMatrix* value));

LIAPICALL (void, liwdg_render_set_projection, (
	LIWdgRender*       self,
	const LIMatMatrix* value));

#endif

/** @} */
/** @} */
