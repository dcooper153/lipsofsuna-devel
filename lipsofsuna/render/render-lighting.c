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

#include <system/lips-system.h>
#include "render-context.h"
#include "render-draw.h"
#include "render-lighting.h"

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

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndLighting));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate light arrays. */
	self->lights = lialg_ptrdic_new ();
	if (self->lights == NULL)
		goto error;
	self->active_lights.array = lisys_calloc (8, sizeof (lirndLight*));
	if (self->active_lights.array == NULL)
		goto error;

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
	lisys_free (self->active_lights.array);
	lisys_free (self);
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
 */
void
lirnd_lighting_update (lirndLighting* self)
{
	lialgPtrdicIter iter;
	lirndLight* light;

	LI_FOREACH_PTRDIC (iter, self->lights)
	{
		light = iter.value;
		lirnd_light_update (light);
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
	int best_index;
	float best_rating;
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
	if (self->active_lights.count < 8)
	{
		i = self->active_lights.count;
		self->active_lights.array[i] = light;
		self->active_lights.array[i]->rating = rating;
		self->active_lights.count++;
		return;
	}

	/* Try to replace an existing light. */
	best_index = -1;
	best_rating = rating;
	for (i = 0 ; i < 8 ; i++)
	{
		if (self->active_lights.array[i]->rating < best_rating)
		{
			best_rating = self->active_lights.array[i]->rating;
			best_index = i;
		}
	}
	if (best_index != -1)
	{
		self->active_lights.array[best_index] = light;
		self->active_lights.array[best_index]->rating = rating;
	}
}

/** @} */
/** @} */
