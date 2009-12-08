/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lirndModel Model
 * @{
 */

#ifndef __RENDER_MODEL_H__
#define __RENDER_MODEL_H__

#include <image/lips-image.h>
#include <model/lips-model.h>
#include "render.h"
#include "render-buffer.h"
#include "render-material.h"
#include "render-types.h"

struct _lirndModel
{
	int id;
	int refs;
	limatAabb aabb;
	limdlModel* model;
	lirndBuffer* vertices;
	lirndRender* render;
	struct
	{
		int count;
		lirndBuffer* array;
	} buffers;
	struct
	{
		int count;
		lirndMaterial** array;
	} materials;
};

lirndModel*
lirnd_model_new (lirndRender* render,
                 limdlModel*  model);

lirndModel*
lirnd_model_new_instance (lirndModel* model);

void
lirnd_model_free (lirndModel*  self);

void
lirnd_model_replace_image (lirndModel* self,
                           lirndImage* image);

void
lirnd_model_get_bounds (lirndModel* self,
                        limatAabb*  aabb);

int
lirnd_model_get_static (lirndModel* self);

#endif

/** @} */
/** @} */
