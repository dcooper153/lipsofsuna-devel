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
 * \addtogroup liwdgRender Render
 * @{
 */

#ifndef __WIDGET_RENDER_H__
#define __WIDGET_RENDER_H__

#include <render/lips-render.h>
#include "widget.h"
#include "widget-group.h"

#define LIWDG_RENDER(o) ((liwdgRender*)(o))

typedef struct _liwdgRender liwdgRender;
struct _liwdgRender
{
	liwdgGroup base;
	limatMatrix modelview;
	limatMatrix projection;
	lirndScene* scene;
	void (*custom_render_func)(liwdgRender*, void*);
	void* custom_render_data;
	void (*custom_update_func)(liwdgRender*, void*);
	void* custom_update_data;
};

extern const liwdgClass liwdgRenderType;

liwdgWidget*
liwdg_render_new (liwdgManager* manager,
                  lirndScene*   scene);

void
liwdg_render_set_modelview (liwdgRender*       self,
                            const limatMatrix* value);

void
liwdg_render_set_projection (liwdgRender*       self,
                             const limatMatrix* value);

#endif

/** @} */
/** @} */
