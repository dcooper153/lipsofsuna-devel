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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengRender Render
 * @{
 */

#ifndef __ENGINE_RENDER_H__
#define __ENGINE_RENDER_H__
#ifndef LIENG_DISABLE_GRAPHICS

#include <image/lips-image.h>
#include <render/lips-render.h>
#include "engine-types.h"

struct _liengRender
{
	lirndRender* render;
	lirndModel* (*lirnd_model_new)(lirndRender*, limdlModel*);
	void (*lirnd_model_free)(lirndModel*);
	lirndObject* (*lirnd_object_new)(lirndRender*, int);
	void (*lirnd_object_free)(lirndObject*);
	void (*lirnd_object_deform)(lirndObject*, limdlPose*);
	void (*lirnd_object_replace_image)(lirndObject*, lirndImage*, lirndImage*);
	void (*lirnd_object_update)(lirndObject*, lirndScene*, float);
	int (*lirnd_object_set_model)(lirndObject*, lirndModel*, limdlPose*);
	int (*lirnd_object_set_realized)(lirndObject*, int);
	void (*lirnd_object_set_transform)(lirndObject*, const limatTransform*);
	lirndRender* (*lirnd_render_new)(const char*);
	void (*lirnd_render_free)(lirndRender*);
	void (*lirnd_render_update)(lirndRender*, float);
	lirndImage* (*lirnd_render_find_image)(lirndRender*, const char*);
	int (*lirnd_render_load_image)(lirndRender*, const char*);
};

liengRender*
lieng_render_new (const char* dir);

void
lieng_render_free (liengRender* self);

#endif
#endif

/** @} */
/** @} */
