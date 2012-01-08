/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenLight Light
 * @{
 */

#include "render-internal.h"
#include "render.h"

#define LIGHT_RANGE_MAX 32.0f

static Ogre::String private_unique_id (
	LIRenLight* self);

static void private_update_type (
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
 * \return New light source or NULL.
 */
LIRenLight* liren_light_new (
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
	self = (LIRenLight*) lisys_calloc (1, sizeof (LIRenLight));
	if (self == NULL)
		return 0;
	self->render = render;
	self->transform = limat_transform_identity ();

	/* Initialize the private data. */
	self->data = (LIRenLightData*) lisys_calloc (1, sizeof (LIRenLightData));
	if (self->data == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x0000FFFF);
		if (lialg_u32dic_find (render->lights, self->id))
			self->id = 0;
	}

	/* Initialize the backend. */
	self->data->light = render->data->scene_manager->createLight (private_unique_id (self));
	self->data->light->setDiffuseColour (Ogre::ColourValue (diffuse[0], diffuse[1], diffuse[2], diffuse[3]));
	self->data->light->setSpecularColour (Ogre::ColourValue (specular[0], specular[1], specular[2], specular[3]));
	self->data->light->setAttenuation (LIGHT_RANGE_MAX, equation[0], equation[1], equation[2]);
	self->data->light->setCastShadows (shadows);
	self->data->light->setSpotlightOuterAngle (Ogre::Radian (cutoff));
	self->data->light->setSpotlightFalloff (exponent / 128.0f);
	self->data->light->setVisible (false);
	private_update_type (self);

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->lights, self->id, self))
	{
		liren_light_free (self);
		return 0;
	}

	return self;
}

/**
 * \brief Frees a light source.
 * \param self Light source.
 */
void liren_light_free (
	LIRenLight* self)
{
	if (self->data != NULL)
	{
		if (self->data->light != NULL)
			self->render->data->scene_manager->destroyLight (self->data->light);
		lisys_free (self->data);
	}
	lisys_free (self);
}

void liren_light_set_ambient (
	LIRenLight*  self,
	const float* value)
{
	/* FIXME: Not supported by Ogre */
}

void liren_light_set_diffuse (
	LIRenLight*  self,
	const float* value)
{
	self->data->light->setDiffuseColour (Ogre::ColourValue (value[0], value[1], value[2], value[3]));
}

/**
 * \brief Sets the light directional
 * \param self Light source.
 * \param value Nonzero for directional.
 */
void liren_light_set_directional (
	LIRenLight* self,
	int         value)
{
	self->directional = value;
	private_update_type (self);
}

/**
 * \brief Enables or disables the light.
 * \param self Light source.
 * \param value Nonzero to enable.
 */
void liren_light_set_enabled (
	LIRenLight* self,
	int         value)
{
	self->data->light->setVisible (value);
}

void liren_light_set_equation (
	LIRenLight*  self,
	const float* value)
{
	self->data->light->setAttenuation (LIGHT_RANGE_MAX, value[0], value[1], value[2]);
}

/**
 * \brief Gets the priority of the light.
 * \param self Light source.
 * \return Priority value, higher means more important.
 */
void liren_light_set_priority (
	LIRenLight* self,
	float       value)
{
	/* FIXME: Not supported by Ogre */
}

/**
 * \brief Sets the shadow casting mode of the light.
 * \param self Light source.
 * \param value Nonzero to allow shadow casting, zero to disable.
 */
void liren_light_set_shadow (
	LIRenLight* self,
	int         value)
{
	self->data->light->setCastShadows (value);
}

void liren_light_set_shadow_far (
	LIRenLight* self,
	float       value)
{
	self->data->light->setShadowFarDistance (value);
	self->data->light->setShadowFarClipDistance (value);
}

void liren_light_set_shadow_near (
	LIRenLight* self,
	float       value)
{
	self->data->light->setShadowNearClipDistance (value);
}

void liren_light_set_specular (
	LIRenLight*  self,
	const float* value)
{
	self->data->light->setSpecularColour (Ogre::ColourValue (value[0], value[1], value[2], value[3]));
}

void liren_light_set_spot_cutoff (
	LIRenLight* self,
	float       value)
{
	self->data->light->setSpotlightOuterAngle (Ogre::Radian (value));
	private_update_type (self);
}

void liren_light_set_spot_exponent (
	LIRenLight* self,
	float       value)
{
	self->data->light->setSpotlightFalloff (value / 128.0f);
}

/**
 * \brief Gets the transformation of the light.
 * \param self Light source.
 * \param value Return value for the transformation.
 */
void liren_light_get_transform (
	LIRenLight*     self,
	LIMatTransform* value)
{
	*value = self->transform;
}

/**
 * \brief Sets the transformation of the light.
 * \param self Light source.
 * \param value Transformation.
 */
void liren_light_set_transform (
	LIRenLight*           self,
	const LIMatTransform* value)
{
	LIMatVector v;

	v = limat_quaternion_transform (value->rotation, limat_vector_init (0.0f, 0.0f, -1.0f));
	self->data->light->setPosition (value->position.x, value->position.y, value->position.z);
	self->data->light->setDirection (v.x, v.y, v.z);
	self->transform = *value;
}

/*****************************************************************************/

static Ogre::String private_unique_id (
	LIRenLight* self)
{
	return Ogre::StringConverter::toString (self->id);
}

static void private_update_type (
	LIRenLight* self)
{
	Ogre::Radian angle(self->data->light->getSpotlightOuterAngle ());

	if (self->directional)
		self->data->light->setType (Ogre::Light::LT_DIRECTIONAL);
	else if (angle.valueRadians () < 0.5f * M_PI)
		self->data->light->setType (Ogre::Light::LT_SPOTLIGHT);
	else
		self->data->light->setType (Ogre::Light::LT_POINT);
}

/** @} */
/** @} */
/** @} */
