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

#include <stdio.h>
#include <stdlib.h>
#include "render-model.h"

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model passed retains the ownership and must
 * ensure that it's not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \return New model or NULL.
 */
lirndModel*
lirnd_model_new (lirndRender* render,
                 limdlModel*  model)
{
	lirndModel* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lirndModel));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->model = model;
	self->aabb = model->bounds;

	return self;
}

/**
 * \brief Frees a model.
 *
 * \param self Model.
 */
void
lirnd_model_free (lirndModel* self)
{
	free (self);
}

void
lirnd_model_get_bounds (lirndModel* self,
                        limatAabb*  aabb)
{
	*aabb = self->aabb;
}

/**
 * \brief Checks if the model is static.
 *
 * \param self Model.
 * \return Nonzero if the model is static.
 */
int
lirnd_model_get_static (lirndModel* self)
{
	return !self->model->animation.count;
}

/** @} */
/** @} */
