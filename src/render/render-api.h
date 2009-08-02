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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndApi Api
 * @{
 */

#ifndef __RENDER_API_H__
#define __RENDER_API_H__

#include "render.h"
#include "render-model.h"
#include "render-object.h"
#include "render-scene.h"

typedef struct _lirndApi lirndApi;
struct _lirndApi
{
	int (*lirnd_buffer_init)(lirndBuffer*   self,
	                         lirndMaterial* material,
	                         lirndFormat*   format,
	                         void*          data,
	                         int            count);
	void (*lirnd_buffer_free)(lirndBuffer* self);
	lirndMaterial* (*lirnd_material_new)();
	lirndMaterial* (*lirnd_material_new_from_model)(lirndRender*, const limdlMaterial*);
	void (*lirnd_material_free)(lirndMaterial* self);
	lirndModel* (*lirnd_model_new)(lirndRender*, limdlModel*);
	void (*lirnd_model_free)(lirndModel*);
	lirndObject* (*lirnd_object_new)(lirndScene*, int);
	lirndObject* (*lirnd_object_new_from_data)(lirndScene*      scene,
	                                           int              id,
	                                           const limatAabb* aabb,
	                                           lirndBuffer*     buffers,
	                                           int              buffercount,
	                                           lirndMaterial**  materials,
	                                           int              materialcount,
	                                           lirndLight**     lights,
	                                           int              lightcount);
	void (*lirnd_object_free)(lirndObject*);
	void (*lirnd_object_deform)(lirndObject*, limdlPose*);
	void (*lirnd_object_update)(lirndObject*, float);
	int (*lirnd_object_set_model)(lirndObject*, lirndModel*, limdlPose*);
	int (*lirnd_object_set_realized)(lirndObject*, int);
	void (*lirnd_object_set_transform)(lirndObject*, const limatTransform*);
	lirndRender* (*lirnd_render_new)(const char*);
	void (*lirnd_render_free)(lirndRender*);
	void (*lirnd_render_update)(lirndRender*, float);
	lirndImage* (*lirnd_render_find_image)(lirndRender*, const char*);
	int (*lirnd_render_load_image)(lirndRender*, const char*);
	lirndScene* (*lirnd_scene_new)(lirndRender*);
	void (*lirnd_scene_free)(lirndScene*);
	void (*lirnd_scene_update)(lirndScene*, float);
};

extern lirndApi lirnd_render_api;

#endif

/** @} */
/** @} */
