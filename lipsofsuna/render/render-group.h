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
 * \addtogroup lirndObject Object
 * @{
 */

#ifndef __RENDER_GROUP_H__
#define __RENDER_GROUP_H__

#include <math/lips-math.h>
#include <model/lips-model.h>
#include "render.h"
#include "render-object.h"
#include "render-types.h"

typedef struct _lirndGroupObject lirndGroupObject;
struct _lirndGroupObject
{
	limatAabb aabb;
	limatTransform transform;
	lirndModel* model;
	lirndGroupObject* next;
};

struct _lirndGroup
{
	int realized;
	limatAabb aabb;
	lirndScene* scene;
	lirndGroupObject* objects;
	struct
	{
		int count;
		lirndLight** array;
	} lights;
};

lirndGroup*
lirnd_group_new (lirndScene* scene);

void
lirnd_group_free (lirndGroup* self);

void
lirnd_group_clear (lirndGroup* self);

int
lirnd_group_insert_model (lirndGroup*     self,
                          lirndModel*     model,
                          limatTransform* transform);

void
lirnd_group_reload_model (lirndGroup* self,
                          lirndModel* model_old,
                          lirndModel* model_new);

void
lirnd_group_update (lirndGroup* self,
                    float       secs);

void
lirnd_group_get_bounds (const lirndGroup* self,
                        limatAabb*        result);

int
lirnd_group_get_realized (const lirndGroup* self);

int
lirnd_group_set_realized (lirndGroup* self,
                          int         value);

#endif

/** @} */
/** @} */

