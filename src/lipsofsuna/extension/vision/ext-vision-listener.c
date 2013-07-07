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

#include "lipsofsuna/extension.h"
#include "lipsofsuna/extension/object/object.h"
#include "ext-vision-listener.h"

static float private_cone_factor (
	LIExtVisionListener* self,
	const LIMatVector*   pos);

/*****************************************************************************/

LIExtVisionListener* liext_vision_listener_new (
	LIExtModule* module)
{
	LIExtVisionListener* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtVisionListener));
	if (self == NULL)
		return NULL;
	self->cone_angle = 0.0f;
	self->cone_cosine = 0.0f;
	self->cone_factor = 0.5f;
	self->keep_threshold = 5.0f;
	self->scan_radius = 32.0f;
	self->direction = limat_vector_init (0.0f, 0.0f, -1.0f);
	self->module = module;

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new (self->objects);
	if (self->objects == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	/* Allocate the terrain dictionary. */
	self->terrain = lialg_u32dic_new (self->objects);
	if (self->terrain == NULL)
	{
		lialg_u32dic_free (self->objects);
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liext_vision_listener_free (
	LIExtVisionListener* self)
{
	lialg_u32dic_free (self->objects);
	lialg_u32dic_free (self->terrain);
	lisys_free (self);
}

void liext_vision_listener_clear (
	LIExtVisionListener* self)
{
	lialg_u32dic_clear (self->objects);
	lialg_u32dic_clear (self->terrain);
}

/**
 * \brief Updates the vision and emits global vision events.
 *
 * The events can be distinguished from the events of other vision
 * listeners by the external ID in the "vision" field.
 *
 * \param self Vision listener.
 */
void liext_vision_listener_update (
	LIExtVisionListener* self)
{
	float dist;
	float mult;
	float radius_add;
	float radius_del;
	float radius_add_obj;
	float radius_del_obj;
	LIAlgU32dicIter iter;
	LIObjManager* objects;
	LIObjObject* object;
	LIMatVector diff;

	/* Get the object manager. */
	objects = limai_program_find_component (self->module->program, "object");
	if (objects == NULL)
		return;

	/* Calculate the second powers of the vision radii. */
	radius_add = self->scan_radius;
	radius_del = self->scan_radius + self->keep_threshold;

	/* Add and remove vision objects. */
	LIALG_U32DIC_FOREACH (iter, objects->objects)
	{
		object = iter.value;

		/* Make sure the object is realized. */
		if (!liobj_object_get_realized (object))
		{
			if (lialg_u32dic_find (self->objects, object->id) != NULL)
			{
				lialg_u32dic_remove (self->objects, object->id);
				limai_program_event (self->module->program, "vision-object-hidden", "vision", LIMAI_FIELD_INT, self->external_id, "object", LIMAI_FIELD_INT, liobj_object_get_external_id (object), NULL);
			}
			continue;
		}

		/* Calculate the distance to the object. */
		diff = limat_vector_subtract (object->transform.position, self->position);
		dist = limat_vector_get_length (diff);
		mult = private_cone_factor (self, &diff);
		radius_add_obj = mult * radius_add;
		radius_del_obj = mult * radius_del;

		/* Check if the object just entered the vision sphere. */
		if (lialg_u32dic_find (self->objects, object->id) == NULL)
		{
			if (dist <= radius_add_obj)
			{
				lialg_u32dic_insert (self->objects, object->id, NULL + 1);
				limai_program_event (self->module->program, "vision-object-shown", "vision", LIMAI_FIELD_INT, self->external_id, "object", LIMAI_FIELD_INT, liobj_object_get_external_id (object), NULL);
			}
		}

		/* Check if the object has just left the vision sphere. */
		else
		{
			if (dist > radius_del_obj)
			{
				lialg_u32dic_remove (self->objects, object->id);
				limai_program_event (self->module->program, "vision-object-hidden", "vision", LIMAI_FIELD_INT, self->external_id, "object", LIMAI_FIELD_INT, liobj_object_get_external_id (object), NULL);
			}
		}
	}

	/* TODO: Silently remove garbage collected objects. */
}

/*****************************************************************************/

static float private_cone_factor (
	LIExtVisionListener* self,
	const LIMatVector*   diff)
{
	float dot;
	float mult;

	dot = limat_vector_dot (self->direction, limat_vector_normalize (*diff));
	if (dot >= self->cone_cosine)
		return 1.0f;
	mult = (dot + 1.0f) / (self->cone_cosine + 1.0f);
	mult = (1.0f - mult) * self->cone_factor + mult;

	return mult;
}
