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
 * \addtogroup liengModel Model
 * @{
 */

#ifndef __ENGINE_MODEL_H__
#define __ENGINE_MODEL_H__

#include <model/lips-model.h>
#include <physics/lips-physics.h>
#include "engine.h"
#include "engine-types.h"

#ifndef LIENG_DISABLE_GRAPHICS
#include <render/lips-render.h>
#endif

struct _liengModel
{
	int id;
	int invalid;
	char* name;
	char* path;
	limatAabb bounds;
	liengEngine* engine;
	limdlModel* model;
	liphyShape* physics;
#ifndef LIENG_DISABLE_GRAPHICS
	lirndModel* render;
#endif
};

liengModel*
lieng_model_new (liengEngine* engine,
                 int          id,
                 const char*  dir,
                 const char*  name);

void
lieng_model_free (liengModel* self);

int
lieng_model_load (liengModel* self);

void
lieng_model_unload (liengModel* self);

void
lieng_model_get_bounds (const liengModel* self,
                        limatAabb*        result);

void
lieng_model_get_bounds_transform (const liengModel*     self,
                                  const limatTransform* transform,
                                  limatAabb*            result);

#endif

/** @} */
/** @} */
