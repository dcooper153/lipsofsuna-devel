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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtObjectRender ObjectRender
 * @{
 */

#include "ext-module.h"

LIExtRenderObject* liext_render_object_new (
	LIExtModule* module)
{
	int object;
	LIExtRenderObject* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtRenderObject));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->render = module->render;
	self->transform = limat_transform_identity ();

	/* Choose the object number. */
	self->id = 0;
	while (!self->id)
	{
		self->id = lialg_random_range (&module->program->random, 0, 0x7FFFFFFF);
		if (lialg_u32dic_find (module->objects, self->id) != NULL)
			self->id = 0;
	}

	/* Insert ourselves to the object list. */
	if (!lialg_u32dic_insert (module->objects, self->id, self))
	{
		liext_render_object_free (self);
		return NULL;
	}

	/* Allocate the render object. */
	object = liren_render_object_new (module->render, self->id);
	if (!object)
	{
		liext_render_object_free (self);
		return NULL;
	}
	lisys_assert (object == self->id);

	return self;
}

void liext_render_object_free (
	LIExtRenderObject* self)
{
	lialg_u32dic_remove (self->module->objects, self->id);
	liren_render_object_free (self->render, self->id);
	lisys_free (self);
}

/** @} */
/** @} */
