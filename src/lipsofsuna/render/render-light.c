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
 * \addtogroup LIRenLight Light
 * @{
 */

#include "lipsofsuna/network.h"
#include "lipsofsuna/system.h"
#include "render-light.h"
#include "internal/render-internal.h"
#include "render21/render.h"
#include "render32/render.h"

static void private_free (
	LIRenLight* self);

/*****************************************************************************/

/**
 * \brief Creates a new light source.
 * \param render Renderer.
 * \param ambient Ambient color, array of 4 floats.
 * \param diffuse Diffuse color, array of 4 floats.
 * \param specular Specular color, array of 4 floats.
 * \param equation Attenuation equation, array of 3 floats.
 * \param cutoff Spot cutoff in radians.
 * \param exponent Spot expoent.
 * \param shadows Nonzero if the lamp casts shadows.
 * \return Light ID or zero on failure.
 */
int liren_render_light_new (
	LIRenRender* render,
	const float* ambient,
	const float* diffuse,
	const float* specular,
	const float* equation,
	float        cutoff,
	float        exponent,
	int          shadows)
{
	LIRenLight* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLight));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x0000FFFF);
		if (lialg_u32dic_find (render->lights, self->id))
			self->id = 0;
	}

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->lights, self->id, self))
	{
		private_free (self);
		return 0;
	}

	/* Initialize the backend. */
	if (render->v32 != NULL)
	{
		self->v32 = liren_light32_new (render->v32, ambient, diffuse,
			specular, equation, cutoff, exponent, shadows);
		if (self->v32 == NULL)
		{
			private_free (self);
			return 0;
		}
	}
	else
	{
		self->v21 = liren_light21_new (render->v21, ambient, diffuse,
			specular, equation, cutoff, exponent, shadows);
		if (self->v21 == NULL)
		{
			private_free (self);
			return 0;
		}
	}

	return self->id;
}

/**
 * \brief Frees a light source.
 * \param self Light source.
 */
void liren_render_light_free (
	LIRenRender* self,
	int          id)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	private_free (light);
}

void liren_render_light_set_ambient (
	LIRenRender* self,
	int          id,
	const float* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_ambient (light->v32, value);
	else
		liren_light21_set_ambient (light->v21, value);
}

void liren_render_light_set_diffuse (
	LIRenRender* self,
	int          id,
	const float* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_diffuse (light->v32, value);
	else
		liren_light21_set_diffuse (light->v21, value);
}

/**
 * \brief Enables or disables the light.
 * \param self Light source.
 * \param value Nonzero to enable.
 */
void liren_render_light_set_enabled (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
	{
		if (value == liren_light32_get_enabled (light->v32))
			return;
		if (value)
			liren_render32_insert_light (light->render->v32, light->v32);
		else
			liren_render32_remove_light (light->render->v32, light->v32);
	}
	else
	{
		if (value == liren_light21_get_enabled (light->v21))
			return;
		if (value)
			liren_render21_insert_light (light->render->v21, light->v21);
		else
			liren_render21_remove_light (light->render->v21, light->v21);
	}
}

void liren_render_light_set_equation (
	LIRenRender* self,
	int          id,
	const float* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_equation (light->v32, value);
	else
		liren_light21_set_equation (light->v21, value);
}

/**
 * \brief Sets the priority of the light.
 * \param self Light source.
 * \param value Priority value, higher means more important.
 */
void liren_render_light_set_priority (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_priority (light->v32, value);
	else
		liren_light21_set_priority (light->v21, value);
}

/**
 * \brief Sets the shadow casting mode of the light.
 * \param self Light source.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_render_light_set_shadow (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_shadow (light->v32, value);
	else
		liren_light21_set_shadow (light->v21, value);
}

void liren_render_light_set_shadow_far (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_shadow_far (light->v32, value);
	else
		liren_light21_set_shadow_far (light->v21, value);
}

void liren_render_light_set_shadow_near (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_shadow_near (light->v32, value);
	else
		liren_light21_set_shadow_near (light->v21, value);
}

void liren_render_light_set_specular (
	LIRenRender* self,
	int          id,
	const float* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_specular (light->v32, value);
	else
		liren_light21_set_specular (light->v21, value);
}

void liren_render_light_set_spot_cutoff (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_spot_cutoff (light->v32, value);
	else
		liren_light21_set_spot_cutoff (light->v21, value);
}

void liren_render_light_set_spot_exponent (
	LIRenRender* self,
	int          id,
	float        value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_spot_exponent (light->v32, value);
	else
		liren_light21_set_spot_exponent (light->v21, value);
}

/**
 * \brief Gets the transformation of the light.
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void liren_render_light_get_transform (
	LIRenRender*    self,
	int             id,
	LIMatTransform* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_get_transform (light->v32, value);
	else
		liren_light21_get_transform (light->v21, value);
}

/**
 * \brief Sets the transformation of the light.
 * \param self Light source.
 * \param value Transformation.
 */
void liren_render_light_set_transform (
	LIRenRender*          self,
	int                   id,
	const LIMatTransform* value)
{
	LIRenLight* light;

	light = lialg_u32dic_find (self->lights, id);
	if (light == NULL)
		return;

	if (light->v32 != NULL)
		liren_light32_set_transform (light->v32, value);
	else
		liren_light21_set_transform (light->v21, value);
}

/*****************************************************************************/

static void private_free (
	LIRenLight* self)
{
	if (self->v32 != NULL)
	{
		if (liren_light32_get_enabled (self->v32))
			liren_render32_remove_light (self->render->v32, self->v32);
		liren_light32_free (self->v32);
	}
	if (self->v21 != NULL)
	{
		if (liren_light21_get_enabled (self->v21))
			liren_render21_remove_light (self->render->v21, self->v21);
		liren_light21_free (self->v21);
	}
	lialg_u32dic_remove (self->render->lights, self->id);
	lisys_free (self);
}

/** @} */
/** @} */
