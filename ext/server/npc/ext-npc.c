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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvNpc Npc      
 * @{
 */

#include <network/lips-network.h>
#include "ext-npc.h"

static int
private_tick (liextNpc* self,
              float     secs);

static liengObject*
private_rescan (liextNpc* self);

/*****************************************************************************/

/**
 * \brief Creates a new non-player character logic.
 *
 * \param server Server.
 * \return Non-player character logic or NULL.
 */
liextNpc*
liext_npc_new (lisrvServer* server)
{
	liextNpc* self;

	self = calloc (1, sizeof (liextNpc));
	if (self == NULL)
		return NULL;
	self->alert = 1;
	self->radius = 30.0f;
	self->refresh = 5.0f;
	self->server = server;
	liext_npc_set_active (self, 1);

	return self;
}

/**
 * \brief Frees the Npc logic.
 *
 * \param self Npc logic.
 */
void
liext_npc_free (liextNpc* self)
{
	if (self->active)
		lieng_engine_call_remove (self->server->engine, LISRV_CALLBACK_TICK, self->calls[0]);
	free (self);
}

int
liext_npc_get_active (liextNpc* self)
{
	return self->active;
}

int
liext_npc_set_active (liextNpc* self,
                      int       value)
{
	if (self->active == value)
		return 1;
	if (value)
	{
		self->calls[0] = lieng_engine_call_insert (self->server->engine,
			LISRV_CALLBACK_TICK, 0, private_tick, self);
	}
	else
		lieng_engine_call_remove (self->server->engine, LISRV_CALLBACK_TICK, self->calls[0]);
	self->active = value;

	return 1;
}

/**
 * \brief Sets the object controlled by the logic.
 *
 * References to objects are maintained automatically.
 *
 * \param self Npc logic.
 * \param object Object.
 * \return Nonzero on success.
 */
int
liext_npc_set_object (liextNpc*    self,
                      liengObject* object)
{
	int flags;

	/* Discard old target. */
	liext_npc_set_target (self, NULL);

	/* Set new owner. */
	if (self->object != NULL)
	{
		flags = lieng_object_get_flags (self->object);
		liphy_object_set_control_mode (self->object->physics, LIPHY_CONTROL_MODE_RIGID);
		lieng_object_set_flags (self->object, flags & ~LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_unref (self->object->script, self->data);
	}
	if (object != NULL)
	{
		flags = lieng_object_get_flags (object);
		liphy_object_set_control_mode (object->physics, LIPHY_CONTROL_MODE_CHARACTER);
		lieng_object_set_flags (object, flags | LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_ref (object->script, self->data);
	}
	self->object = object;

	return 1;
}

/**
 * \brief Sets the current target of the logic.
 *
 * References to objects are maintained automatically.
 *
 * \param self Npc logic.
 * \param object Object or NULL.
 */
void
liext_npc_set_target (liextNpc*    self,
                      liengObject* object)
{
	if (self->target != NULL)
		liscr_data_unref (self->target->script, self->data);
	self->target = object;
	if (object != NULL)
		liscr_data_ref (object->script, self->data);
}

/*****************************************************************************/

static int
private_tick (liextNpc* self,
              float     secs)
{
	liengObject* object;
	limatTransform transform;
	liphyObject* physics;

	/* Get object data. */
	object = self->object;
	if (object == NULL)
		return 1;
	physics = object->physics;

	/* Check if active. */
	if (!self->alert || object->sector == NULL)
	{
		liext_npc_set_target (self, NULL);
		liphy_object_set_movement (physics, 0.0f);
		return 1;
	}

	/* Check for target loss. */
	if (self->target != NULL)
	{
		/* FIXME: Might also want to give up if too far away. */
		if (self->target->sector == NULL ||
		    self->target->script->invalid)
			liext_npc_set_target (self, NULL);
	}

	/* Scan for targets periodically. */
	self->timer += secs;
	if (self->timer >= self->refresh)
	{
		self->timer = 0.0f;
		liext_npc_set_target (self, private_rescan (self));
	}

	/* Check for target. */
	if (self->target == NULL)
	{
		liphy_object_set_movement (physics, 0.0f);
		return 1;
	}

	/* Move towards target. */
	/* FIXME: Speed not supported. */
	lieng_object_get_transform (self->target, &transform);
	lieng_object_approach (object, &transform.position, 1.0f);

	return 1;
}

static liengObject*
private_rescan (liextNpc* self)
{
	float d;
	float dist;
	lialgU32dicIter iter1;
	liengObject* tmp;
	liengObject* object;
	liengObject* target;
	liengSector* sector;
	lisecIter iter0;

	dist = self->radius;
	target = NULL;
	object = self->object;
	assert (object->sector != NULL);

	/* Search for target. */
	LI_FOREACH_SECTOR (iter0, object->sector->id, 1)
	{
		sector = lieng_engine_find_sector (object->engine, iter0.id);
		if (sector == NULL)
			continue;
		LI_FOREACH_U32DIC (iter1, sector->objects)
		{
			assert (iter1.value);
			tmp = iter1.value;
			if (LISRV_OBJECT (tmp)->client == NULL) /* FIXME: Check for alignment. */
				continue;
			d = lieng_object_get_distance (object, tmp);
			if (dist < d)
				continue;
			dist = d;
			target = tmp;
		}
	}

	return target;
}

/** @} */
/** @} */
/** @} */
