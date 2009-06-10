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
 * \addtogroup lirndLighting Lighting
 * @{
 */

#include <assert.h>
#include <stdlib.h>
#include "render-context.h"
#include "render-draw.h"
#include "render-lighting.h"
#include "render-scene.h"

static int
private_compare_lights (const void* a,
                        const void* b);

static void
private_select_light (lirndLighting*     self,
                      lirndLight*        light,
                      const limatVector* center);

/*****************************************************************************/

/**
 * \brief Creates a new light manager.
 *
 * \param render Renderer.
 * \return New light manager or NULL.
 */
lirndLighting*
lirnd_lighting_new (lirndRender* render)
{
	lirndLighting* self;
	const float texture[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	/* Allocate self. */
	self = calloc (1, sizeof (lirndLighting));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->render = render;

	/* Allocate light arrays. */
	self->lights = lialg_ptrdic_new ();
	if (self->lights == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	self->active_lights.capacity = 8;
	self->active_lights.array = calloc (self->active_lights.capacity, sizeof (lirndLight*));
	if (self->active_lights.array == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Create default depth texture. */
	if (livid_features.shader_model >= 3)
	{
		glGenTextures (1, &self->depth_texture_max);
		glBindTexture (GL_TEXTURE_2D, self->depth_texture_max);
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 2, 2,
			0, GL_DEPTH_COMPONENT, GL_FLOAT, texture);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	return self;

error:
	lirnd_lighting_free (self);
	return NULL;
}

/**
 * \brief Frees the light manager.
 *
 * \param self Light manager.
 */
void
lirnd_lighting_free (lirndLighting* self)
{
	if (self->lights != NULL)
	{
		assert (self->lights->size == 0);
		lialg_ptrdic_free (self->lights);
	}
	glDeleteTextures (1, &self->depth_texture_max);
	free (self->active_lights.array);
}

/**
 * \brief Registers a new light source.
 *
 * \param self Light manager.
 * \param light Light source.
 * \return Nonzero on success.
 */
int
lirnd_lighting_insert_light (lirndLighting* self,
                             lirndLight*    light)
{
	if (!lialg_ptrdic_insert (self->lights, light, light))
		return 0;
	light->enabled = 1;
	return 1;
}

/**
 * \brief Removes a registered light source.
 *
 * \param self Light manager.
 * \param light Light source.
 */
void
lirnd_lighting_remove_light (lirndLighting* self,
                             lirndLight*    light)
{
	lialg_ptrdic_remove (self->lights, light);
	light->enabled = 0;
}

/**
 * \brief Updates the status of all registered light sources.
 *
 * \param self Light manager.
 * \param scene Current scene.
 */
void
lirnd_lighting_update (lirndLighting* self,
                       lirndScene*    scene)
{
	lialgPtrdicIter iter;
	lirndLight* light;

	LI_FOREACH_PTRDIC (iter, self->lights)
	{
		light = iter.value;
		lirnd_light_update (light, scene);
	}
}

/**
 * \brief Sets the center of the scene and selects lights affecting it.
 *
 * \param self Light manager.
 * \param point Lighting focus point.
 */
void
lirnd_lighting_set_center (lirndLighting*     self,
                           const limatVector* point)
{
	lialgPtrdicIter iter;
	lirndLight* light;

	self->active_lights.count = 0;
	LI_FOREACH_PTRDIC (iter, self->lights)
	{
		light = iter.value;
		private_select_light (self, light, point);
	}
	qsort (self->active_lights.array, self->active_lights.count, sizeof (lirndLight*), private_compare_lights);
}

/*****************************************************************************/

static int
private_compare_lights (const void* a,
                        const void* b)
{
	const lirndLight* l0 = a;
	const lirndLight* l1 = b;

	return lirnd_light_compare (l0, l1);
}

static void
private_select_light (lirndLighting*     self,
                      lirndLight*        light,
                      const limatVector* center)
{
	int i;
	float rating;
	limatVector position;

	position = light->transform.position;
	rating = limat_vector_get_length (limat_vector_subtract (position, *center));
	rating = light->equation[0] +
		     light->equation[1] * rating +
		     light->equation[2] * rating * rating;
//	if (rating > LIRND_LIGHT_MAXIMUM_RATING)
//		return;

	/* Try to add a new light. */
	if (self->active_lights.count < self->active_lights.capacity)
	{
		i = self->active_lights.count;
		self->active_lights.array[i] = light;
		self->active_lights.array[i]->rating = rating;
		self->active_lights.count++;
		return;
	}

	/* Try to replace an existing light. */
	for (i = 0 ; i < self->active_lights.capacity ; i++)
	{
		if (rating < self->active_lights.array[i]->rating)
		{
			self->active_lights.array[i] = light;
			self->active_lights.array[i]->rating = rating;
			return;
		}
	}
}

/** @} */
/** @} */
