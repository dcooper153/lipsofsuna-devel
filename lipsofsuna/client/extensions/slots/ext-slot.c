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
 * \param module Module.
 * \param object Parent object.
 * \param node0 Node name in parent model.
 * \param node1 Node name in equipment model.
 * \param model Equipment model.
 * \return New slot or NULL.
 */
liextSlot*
liext_slot_new (licliModule* module,
                liengObject* object,
                const char*  node0,
                const char*  node1,
                int          model)
{
	liengModel* mdl;
	liextSlot* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextSlot));
	if (self == NULL)
		return NULL;
	self->module = module;

	/* Allocate object. */
	mdl = lieng_engine_find_model_by_code (object->engine, model);
	if (mdl == NULL)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "cannot find model `%d'", model);
		goto error;
	}
	self->object = lieng_object_new (self->module->engine, NULL, LIPHY_CONTROL_MODE_STATIC, 0);
	lieng_object_set_smoothing (self->object, 0.0f, 0.0f);
	if (self->object == NULL)
	{
		lisys_error_append ("cannot create object");
		goto error;
	}
	lieng_object_set_model (self->object, mdl);
	liphy_object_set_collision_group (self->object->physics, LICLI_PHYSICS_GROUP_OBJECTS);

	/* Allocate constraint. */
	self->constraint = lieng_constraint_new (object, node0, self->object, node1);
	if (self->constraint == NULL)
	{
		lisys_error_append ("cannot create constraint");
		goto error;
	}
	lieng_engine_insert_constraint (self->object->engine, self->constraint);

	/* Snap to anchor. */
	lieng_constraint_update (self->constraint, 1.0f);
	lieng_object_set_realized (self->object, 1);

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
	liextSlot tmp;

	/* Avoid feedback loops that may lead to double removal of our object in
	 * its own free callback by clearing the object pointer. */
	tmp = *self;
	self->object = NULL;
	self->constraint = NULL;

	/* Free constraint. */
	if (tmp.constraint != NULL)
	{
		lieng_engine_remove_constraint (self->module->engine, tmp.constraint);
		lieng_constraint_free (tmp.constraint);
	}

	/* Free object. */
	if (tmp.object != NULL)
		lieng_object_set_realized (tmp.object, 0);

	lisys_free (self);
}

/** @} */
/** @} */
/** @} */
