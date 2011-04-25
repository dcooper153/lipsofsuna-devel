/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenObject Object
 * @{
 */

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-object.h"
#include "render-private.h"
#include "render32/render-private.h"

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param scene Scene.
 * \param id Unique identifier.
 * \return New object or NULL.
 */
LIRenObject* liren_object_new (
	LIRenScene* scene,
	int         id)
{
	LIRenObject* self;

	self = lisys_calloc (1, sizeof (LIRenObject));
	if (self == NULL)
		return NULL;
	self->scene = scene;

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&scene->render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (scene->objects, id))
			id = 0;
	}
	self->id = id;

	/* Initialize backend. */
	self->v32 = liren_object32_new (scene->v32, id);
	if (self->v32 == NULL)
	{
		liren_object_free (self);
		return NULL;
	}

	/* Add to dictionary. */
	if (!lialg_u32dic_insert (scene->objects, id, self))
	{
		liren_object_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the render object.
 * \param self Object.
 */
void liren_object_free (
	LIRenObject* self)
{
	lialg_u32dic_remove (self->scene->objects, self->id);
	if (self->v32 != NULL)
		liren_object32_free (self->v32);
	lisys_free (self);
}

/**
 * \brief Deforms the object.
 * \param self Object.
 */
void liren_object_deform (
	LIRenObject* self)
{
	liren_object32_deform (self->v32);
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Object.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_object_particle_animation (
	LIRenObject* self,
	float        start,
	int          loop)
{
	liren_object32_particle_animation (self->v32, start, loop);
}

/**
 * \brief Gets the model of the object.
 * \param self Object.
 * \return Model or NULL.
 */
LIRenModel* liren_object_get_model (
	LIRenObject* self)
{
	return self->model;
}

/**
 * \brief Sets the model of the object.
 * \param self Object.
 * \param model Model.
 * \return Nonzero on success.
 */
int liren_object_set_model (
	LIRenObject* self,
	LIRenModel*  model)
{
	if (!liren_object32_set_model (self->v32, (model != NULL)? model->v32 : NULL))
		return 0;
	self->model = model;

	return 1;
}

/**
 * \brief Sets the pose of the object.
 * \param self Object.
 * \param pose Pose.
 * \return Nonzero on success.
 */
int liren_object_set_pose (
	LIRenObject* self,
	LIMdlPose*   pose)
{
	return liren_object32_set_pose (self->v32, pose);
}

/**
 * \brief Sets the realized flag of the object.
 * \param self Object.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
int liren_object_set_realized (
	LIRenObject* self,
	int          value)
{
	return liren_object32_set_realized (self->v32, value);
}

/**
 * \brief Sets the transformation of the object.
 * \param self Object.
 * \param value Transformation.
 */
void liren_object_set_transform (
	LIRenObject*          self,
	const LIMatTransform* value)
{
	liren_object32_set_transform (self->v32, value);
}

/** @} */
/** @} */
