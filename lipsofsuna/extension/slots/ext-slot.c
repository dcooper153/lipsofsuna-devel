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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliSlots Slots      
 * @{
 */

#include "ext-module.h"
#include "ext-slot.h"

/**
 * \brief Creates a new equipment slot.
 *
 * \param client Client.
 * \param object Parent object.
 * \param node0 Node name in parent model.
 * \param node1 Node name in equipment model.
 * \param model Equipment model.
 * \return New slot or NULL.
 */
LIExtSlot*
liext_slot_new (LICliClient* client,
                LIEngObject* object,
                const char*  node0,
                const char*  node1,
                LIEngModel*  model)
{
	LIExtSlot* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtSlot));
	if (self == NULL)
		return NULL;
	self->client = client;

	/* Allocate object. */
	self->object = lieng_object_new (self->client->engine, NULL, LIPHY_CONTROL_MODE_STATIC, 0);
	if (self->object == NULL)
	{
		lisys_error_append ("cannot create object");
		goto error;
	}
	lieng_object_set_smoothing (self->object, 0.0f, 0.0f);
	lieng_object_set_model (self->object, model);

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
liext_slot_free (LIExtSlot* self)
{
	LIExtSlot tmp;

	/* Avoid feedback loops that may lead to double removal of our object in
	 * its own free callback by clearing the object pointer. */
	tmp = *self;
	self->object = NULL;
	self->constraint = NULL;

	/* Free constraint. */
	if (tmp.constraint != NULL)
	{
		lieng_engine_remove_constraint (self->client->engine, tmp.constraint);
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
