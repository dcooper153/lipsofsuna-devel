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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup LIEngModel Model
 * @{
 */

#ifndef __ENGINE_MODEL_H__
#define __ENGINE_MODEL_H__

#include <lipsofsuna/model.h>
#include <lipsofsuna/physics.h>
#include "engine.h"
#include "engine-types.h"

struct _LIEngModel
{
	char* name;
	char* path;
	LIMatAabb bounds;
	LIEngEngine* engine;
	LIMdlModel* model;
	LIPhyShape* physics;
};

LIEngModel*
lieng_model_new (LIEngEngine* engine,
                 const char*  dir,
                 const char*  name);

void
lieng_model_free (LIEngModel* self);

int
lieng_model_load (LIEngModel* self);

void
lieng_model_unload (LIEngModel* self);

void
lieng_model_get_bounds (const LIEngModel* self,
                        LIMatAabb*        result);

void
lieng_model_get_bounds_transform (const LIEngModel*     self,
                                  const LIMatTransform* transform,
                                  LIMatAabb*            result);

#endif

/** @} */
/** @} */
