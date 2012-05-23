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
#include "internal/render-object.h"

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

	return liren_object_get_id (self);
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

void liren_render_object_channel_animate (
	LIRenRender*    self,
	int             id,
	int             channel,
	LIMdlAnimation* anim,
	int             additive,
	int             repeat,
	int             repeat_start,
	int             keep,
	float           fade_in,
	float           fade_out,
	float           weight,
	float           weight_scale,
	float           time,
	float           time_scale,
	const char**    node_names,
	float*          node_weights,
	int             node_count)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	liren_object_channel_animate (object,
		channel, anim, additive, repeat, repeat_start, keep, fade_in,
		fade_out, weight, weight_scale, time, time_scale, node_names,
		node_weights, node_count);
}

void liren_render_object_channel_edit (
	LIRenRender*          self,
	int                   id,
	int                   channel,
	int                   frame,
	const char*           node,
	const LIMatTransform* transform,
	float                 scale)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	liren_object_channel_edit (object, channel, frame, node, transform, scale);
}

void liren_render_object_channel_fade (
	LIRenRender* self,
	int          id,
	int          channel,
	float        time)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return;

	liren_object_channel_fade (object, channel, time);
}

LIMdlPoseChannel* liren_render_object_channel_get_state (
	LIRenRender* self,
	int          id,
	int          channel)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return NULL;

	return liren_object_channel_get_state (object, channel);
}

int liren_render_object_find_node (
	LIRenRender*    self,
	int             id,
	const char*     name,
	int             world,
	LIMatTransform* result)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object == NULL)
		return 0;

	return liren_object_find_node (object, name, world, result);
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
 * \brief Returns nonzero if the object has been fully background loaded.
 * \param self Renderer.
 * \param id Object ID.
 * \return Nonzero if background loaded, zero if not.
 */
int liren_render_object_get_loaded (
	LIRenRender* self,
	int          id)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		return liren_object_get_loaded (object);
	else
		return 1;
}

/**
 * \brief Sets the model of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param model Model.
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
 * \brief Sets the particle system of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param name Particle system name.
 */
void liren_render_object_set_particle (
	LIRenRender* self,
	int          id,
	const char*  name)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_particle (object, name);
}

/**
 * \brief Enables or disables particle emission.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Nonzero to enable, zero to disable.
 */
void liren_render_object_set_particle_emitting (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_particle_emitting (object, value);
}

/**
 * \brief Sets the realized flag of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Flag value.
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
 * \brief Sets the render distance of the object.
 * \param self Object.
 * \param value Render distance.
 */
void liren_render_object_set_render_distance (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_render_distance (object, value);
}

/**
 * \brief Sets the shadow casting mode of the object.
 * \param self Renderer.
 * \param id Object ID.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_render_object_set_shadow (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenObject* object;

	object = lialg_u32dic_find (self->objects, id);
	if (object != NULL)
		liren_object_set_shadow (object, value);
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
