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
 * \addtogroup LIObj Object
 * @{
 * \addtogroup LIObjObject Object
 * @{
 */

#include "object.h"
#include "object-manager.h"
#include "object-sector.h"

LIMaiExtensionInfo liext_object_info =
{
	LIMAI_EXTENSION_VERSION, "Object",
	liobj_manager_new,
	liobj_manager_free,
	liobj_manager_get_memstat
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

	/* Register the component. */
	if (!limai_program_insert_component (program, "object", self))
	{
		liobj_manager_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_OBJECT, self);
	liext_script_object (program->script);

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
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liobj_manager_get_memstat (
	LIObjManager* self,
	LIMaiMemstat* stat)
{
/* TODO: memstat */
}

/** @} */
/** @} */
/** @} */
