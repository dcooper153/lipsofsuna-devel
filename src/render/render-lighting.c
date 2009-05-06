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

lirndLighting*
lirnd_lighting_new (lirndRender* render)
{
	const float ambient[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float diffuse[4] = { 0.5f, 0.5f, 0.5f, 1.0f };
	const float texture[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const limatVector direction = { 0.3f, 0.5f, 0.7f };
	lirndLighting* self;

	/* Allocase self. */
	self = calloc (1, sizeof (lirndLighting));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->render = render;

	/* Allocate lights. */
	self->lights.capacity = 8;
	self->lights.active = calloc (self->lights.capacity, sizeof (lirndLight*));
	if (self->lights.active == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}
	self->lights.dict = lialg_ptrdic_new ();
	if (self->lights.dict == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		goto error;
	}

	/* Create the sun.  */
	self->sun.light = lirnd_light_new_directional (self->render, diffuse);
	if (self->sun.light == NULL)
		goto error;
	lirnd_lighting_set_sun_direction (self, &direction);
	memcpy (self->sun.ambient, ambient, 4 * sizeof (float));
	lirnd_light_set_ambient (self->sun.light, self->sun.ambient);

	/* Create default depth texture. */
	if (livid_features.shader_model >= 3)
	{
		glGenTextures (1, &self->lights.depth_texture_max);
		glBindTexture (GL_TEXTURE_2D, self->lights.depth_texture_max);
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

void
lirnd_lighting_free (lirndLighting* self)
{
	if (self->sun.light != NULL)
		lirnd_light_free (self->sun.light);
	if (self->lights.dict != NULL)
	{
		assert (self->lights.dict->size == 0);
		lialg_ptrdic_free (self->lights.dict);
	}
	glDeleteTextures (1, &self->lights.depth_texture_max);
	free (self->lights.active);
}

int
lirnd_lighting_create_light (lirndLighting*   self,
                             const limdlNode* node)
{
	lirndLight* light;
	limatTransform transform;

	/* Create light. */
	light = lirnd_light_new_from_model (self->render, node);
	if (light == NULL)
		return 0;
	if (!lialg_ptrdic_insert (self->lights.dict, light, light))
	{
		lirnd_light_free (light);
		return 0;
	}

	/* Update transformation. */
	limdl_node_get_pose_transform (node, &transform);
	lirnd_light_set_transform (light, &transform);

	return 1;
}

int
lirnd_lighting_insert_light (lirndLighting* self,
                             lirndLight*    light)
{
	if (!lialg_ptrdic_insert (self->lights.dict, light, light))
		return 0;
	return 1;
}

void
lirnd_lighting_remove_light (lirndLighting* self,
                             lirndLight*    light)
{
	lialg_ptrdic_remove (self->lights.dict, light);
}

/**
 * \brief Updates the status of all registered light sources.
 *
 * \param self Light manager.
 * \param scene Scene surrounding the light source.
 */
void
lirnd_lighting_update (lirndLighting* self,
                       lirndScene*    scene)
{
	lialgPtrdicIter iter;
	lirndLight* light;

	lirnd_light_update (self->sun.light, scene);
	LI_FOREACH_PTRDIC (iter, self->lights.dict)
	{
		light = iter.value;
		lirnd_light_update (light, scene);
	}
}

void
lirnd_lighting_set_ambient (lirndLighting* self,
                            const float*   value)
{
	memcpy (self->sun.ambient, value, 4 * sizeof (float));
	lirnd_light_set_ambient (self->sun.light, self->sun.ambient);
}

void
lirnd_lighting_set_center (lirndLighting*     self,
                           const limatVector* point)
{
	lialgPtrdicIter iter;
	lirndLight* light;

	/* Select sun. */
	self->lights.active[0] = self->sun.light;
	self->lights.active[0]->rating = -1.0f;
	self->lights.count = 1;

	/* Select other lights. */
	LI_FOREACH_PTRDIC (iter, self->lights.dict)
	{
		light = iter.value;
		private_select_light (self, light, point);
	}

	/* Sort lights. */
	qsort (self->lights.active, self->lights.count, sizeof (lirndLight*), private_compare_lights);
}

void
lirnd_lighting_set_sun_color (lirndLighting* self,
                              const float*   value)
{
	memcpy (self->sun.light->diffuse, value, 4 * sizeof (float));
}

void
lirnd_lighting_set_sun_direction (lirndLighting*     self,
                                  const limatVector* value)
{
	self->sun.direction = limat_vector_normalize (*value);
	lirnd_light_set_direction (self->sun.light, &self->sun.direction);
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
	if (self->lights.count < self->lights.capacity)
	{
		i = self->lights.count;
		self->lights.active[i] = light;
		self->lights.active[i]->rating = rating;
		self->lights.count++;
		return;
	}

	/* Try to replace an existing light. */
	for (i = 0 ; i < self->lights.capacity ; i++)
	{
		if (rating < self->lights.active[i]->rating)
		{
			self->lights.active[i] = light;
			self->lights.active[i]->rating = rating;
			return;
		}
	}
}

/** @} */
/** @} */
