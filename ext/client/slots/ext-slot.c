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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#include "ext-slot.h"

/**
 * \brief Creates a new equipment slot.
 *
 * \param object Parent object.
 * \param node0 Node name in parent model.
 * \param node1 Node name in equipment model.
 * \param model Equipment model.
 * \return New slot or NULL.
 */
liextSlot*
liext_slot_new (liengObject* object,
                const char*  node0,
                const char*  node1,
                int          model)
{
	liengModel* mdl;
	liextSlot* self;

	/* Allocate self. */
	self = calloc (1, sizeof (liextSlot));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->module = LICLI_OBJECT (object)->module;

	/* Allocate model. */
	mdl = lieng_engine_find_model_by_code (object->engine, model);
	if (mdl == NULL)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot find model `%d'", model);
		goto error;
	}
	self->object = licli_object_new (self->module, 0, LINET_OBJECT_FLAG_DYNAMIC);
	if (self->object == NULL)
	{
		lisys_error_append ("cannot create object");
		goto error;
	}
	lieng_object_set_model (self->object, mdl);
	liphy_object_set_collision_group (self->object->physics, LICLI_PHYSICS_GROUP_OBJECTS);
	lieng_object_set_realized (self->object, 1);

	/* Allocate constraint. */
	self->constraint = lirnd_constraint_new (object->render, node0, self->object->render, node1);
	if (self->constraint == NULL)
	{
		lisys_error_append ("cannot create object");
		goto error;
	}
	lirnd_render_insert_constraint (self->object->engine->render, self->constraint);

	return self;

error:
	liext_slot_free (self);
	return NULL;
}

/**
 * \brief Frees the slot.
 *
 * \param self Slot.
 */
void
liext_slot_free (liextSlot* self)
{
	liengObject* tmp;

	/* Avoid feedback loops that may lead to double removal of our object in
	 * its own free callback by clearing the object pointer. */
	tmp = self->object;
	self->object = NULL;

	/* Free constraint. */
	if (self->constraint != NULL)
	{
		lirnd_render_remove_constraint (self->module->engine->render, self->constraint);
		lirnd_constraint_free (self->constraint);
	}

	/* Free object. */
	if (tmp != NULL)
		lieng_object_set_realized (tmp, 0);

	free (self);
}

/** @} */
/** @} */
/** @} */
