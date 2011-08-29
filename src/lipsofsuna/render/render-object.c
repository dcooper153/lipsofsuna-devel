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
#include "internal/render.h"
#include "render32/render-private.h"

/**
 * \brief Creates a new render object and adds it to the scene.
 * \param render Renderer.
 * \param id Unique identifier.
 * \return Object ID or zero on failure.
 */
int liren_render_object_new (
	LIRenRender* render,
	int          id)
{
	LIRenObject* self;

	self = liren_object_new (render, id);
	if (self == NULL)
		return 0;

	return self->id;
}

/**
 * \brief Frees the render object.
 * \param self Renderer.
 * \param id Object ID.
 */
void liren_render_object_free (
	LIRenRender* self,
	int          id)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_free (object);
}

/**
 * \brief Deforms the object.
 * \param self Renderer.
 * \param id Object ID.
 */
void liren_render_object_deform (
	LIRenRender* self,
	int          id)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_deform (object);
}

/**
 * \brief Sets the particle animation state of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param start Animation offset in seconds.
 * \param loop Nonzero to enable looping.
 */
void liren_render_object_particle_animation (
	LIRenRender* self,
	int          id,
	float        start,
	int          loop)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_particle_animation (object, start, loop);
}

/**
 * \brief Sets the effect layer of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param shader Shader name or NULL to disable the effect.
 * \param params Effect parameters or NULL.
 * \return Nonzero on success.
 */
void liren_render_object_set_effect (
	LIRenRender* self,
	int          id,
	const char*  shader,
	const float* params)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_effect (object, shader, params);
}

/**
 * \brief Sets the model of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param model Model.
 * \return Nonzero on success.
 */
void liren_render_object_set_model (
	LIRenRender* self,
	int          id,
	int          model)
{
	LIRenObject* object;
	LIRenModel* model_;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
	{
		model_ = lialg_u32dic_find (self->models, model);
		liren_object_set_model (object, model_);
	}
}

/**
 * \brief Sets the pose of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param pose Pose.
 * \return Nonzero on success.
 */
void liren_render_object_set_pose (
	LIRenRender* self,
	int          id,
	LIMdlPose*   pose)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_pose (object, pose);
}

/**
 * \brief Sets the realized flag of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Flag value.
 * \return Nonzero if succeeded.
 */
void liren_render_object_set_realized (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_realized (object, value);
}

/**
 * \brief Sets the transformation of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Transformation.
 */
void liren_render_object_set_transform (
	LIRenRender*          self,
	int                   id,
	const LIMatTransform* value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_transform (object, value);
}

/** @} */
/** @} */
