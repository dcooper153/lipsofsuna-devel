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
 * \addtogroup LIObjObject Object
 * @{
 */

#include "object.h"
#include "object-manager.h"
#include "object-sector.h"

static void private_model_free (
	LIObjManager* self,
	LIMdlModel*   model);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_info =
{
	LIMAI_EXTENSION_VERSION, "Object",
	liobj_manager_new,
	liobj_manager_free
};

LIObjManager* liobj_manager_new (
	LIMaiProgram* program)
{
	LIObjManager* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIObjManager));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liobj_manager_free (self);
		return NULL;
	}

	/* Create sector data. */
	if (!lialg_sectors_insert_content (program->sectors, LIALG_SECTORS_CONTENT_ENGINE, self,
	     (LIAlgSectorFreeFunc) liobj_sector_free,
	     (LIAlgSectorLoadFunc) liobj_sector_new))
	{
		liobj_manager_free (self);
		return NULL;
	}

	/* Register the component. */
	if (!limai_program_insert_component (program, "object", self))
	{
		liobj_manager_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_OBJECT, self);
	liext_script_object (program->script);

	/* Register callback handlers. */
	lical_callbacks_insert (program->callbacks, "model-free", 0, private_model_free, self, self->calls + 0);

	return self;
}

void liobj_manager_free (
	LIObjManager* self)
{
	LIAlgU32dicIter iter;
	LIObjObject* object;

	/* Free objects. */
	if (self->objects != NULL)
	{
		LIALG_U32DIC_FOREACH (iter, self->objects)
		{
			object = iter.value;
			liobj_object_set_realized (object, 0);
		}
		while (self->objects->list)
		{
			object = self->objects->list->value;
			liobj_object_free (object);
		}
		lialg_u32dic_free (self->objects);
	}

	/* Free sector data. */
	lialg_sectors_remove_content (self->program->sectors, LIALG_SECTORS_CONTENT_ENGINE);

	/* Unregister the component. */
	limai_program_remove_component (self->program, "object");

	lisys_free (self);
}

/**
 * \brief Find an object by ID.
 * \param self Object manager.
 * \param id Object ID.
 * \return Object, or NULL.
 */
LIObjObject* liobj_manager_find_object (
	LIObjManager* self,
	uint32_t      id)
{
	return lialg_u32dic_find (self->objects, id);
}

/**
 * \brief Notifies scripts of an object being moved.
 * \param self Object manager.
 * \param object Object.
 */
void liobj_manager_notify_object_motion (
	LIObjManager* self,
	LIObjObject*  object)
{
	float diff_pos;
	float diff_rot;
	const float POSITION_THRESHOLD = 0.02;
	const float ROTATION_THRESHOLD = 0.02;

	if (object->script != NULL)
	{
		/* Don't emit events for ridiculously small changes. */
		diff_pos = LIMAT_ABS (object->transform.position.x - object->transform_event.position.x) +
		           LIMAT_ABS (object->transform.position.y - object->transform_event.position.y) +
		           LIMAT_ABS (object->transform.position.z - object->transform_event.position.z);
		diff_rot = LIMAT_ABS (object->transform.rotation.x - object->transform_event.rotation.x) +
		           LIMAT_ABS (object->transform.rotation.y - object->transform_event.rotation.y) +
		           LIMAT_ABS (object->transform.rotation.z - object->transform_event.rotation.z) +
		           LIMAT_ABS (object->transform.rotation.w - object->transform_event.rotation.w);
		if (diff_pos < POSITION_THRESHOLD && diff_rot < ROTATION_THRESHOLD)
			return;
		object->transform_event = object->transform;

		/* Emit an object-motion event. */
		limai_program_event (self->program, "object-motion", "id", LIMAI_FIELD_INT, liobj_object_get_external_id (object), NULL);
	}
}

/*****************************************************************************/

static void private_model_free (
	LIObjManager* self,
	LIMdlModel*   model)
{
	LIAlgU32dicIter iter;
	LIObjObject* object;

	/* Remove from objects. */
	/* Models can become subject to garbage collection even when used by an
	   object if scripts don't reference them. To handle the removal
	   gracefully, we remove the deleted model from objects. */
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		object = iter.value;
		if (object->model == model)
			object->model = NULL;
	}
}

/** @} */
/** @} */
