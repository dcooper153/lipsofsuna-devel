/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenLighting Lighting
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-context.h"
#include "render-draw.h"
#include "render-lighting.h"

/**
 * \brief Creates a new light manager.
 *
 * \param render Renderer.
 * \return New light manager or NULL.
 */
LIRenLighting*
liren_lighting_new (LIRenRender* render)
{
	LIRenLighting* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenLighting));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Allocate light arrays. */
	self->lights = lialg_ptrdic_new ();
	if (self->lights == NULL)
		goto error;

	return self;

error:
	liren_lighting_free (self);
	return NULL;
}

/**
 * \brief Frees the light manager.
 *
 * \param self Light manager.
 */
void
liren_lighting_free (LIRenLighting* self)
{
	if (self->lights != NULL)
	{
		assert (self->lights->size == 0);
		lialg_ptrdic_free (self->lights);
	}
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
liren_lighting_insert_light (LIRenLighting* self,
                             LIRenLight*    light)
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
liren_lighting_remove_light (LIRenLighting* self,
                             LIRenLight*    light)
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
liren_lighting_update (LIRenLighting* self)
{
	LIAlgPtrdicIter iter;
	LIRenLight* light;

	LIALG_PTRDIC_FOREACH (iter, self->lights)
	{
		light = iter.value;
		liren_light_update (light);
	}
}

/** @} */
/** @} */
