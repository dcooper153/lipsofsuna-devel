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

static void
private_attack (liextNpc* self);

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

	self = lisys_calloc (1, sizeof (liextNpc));
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
	{
		lieng_engine_remove_calls (self->server->engine, self->calls,
			sizeof (self->calls) / sizeof (licalHandle));
	}
	lisys_free (self);
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
		lieng_engine_insert_call (self->server->engine,
			LISRV_CALLBACK_TICK, 0, private_tick, self, self->calls + 0);
	}
	else
	{
		lieng_engine_remove_calls (self->server->engine, self->calls,
			sizeof (self->calls) / sizeof (licalHandle));
	}
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

	/* Attack if near enough. */
	if (lieng_object_get_distance (object, self->target) <= 3.0)
		private_attack (self);

	return 1;
}

static void
private_attack (liextNpc* self)
{
	liscrScript* script = self->server->script;

	/* Check for spawn function. */
	lua_getfield (script->lua, LUA_GLOBALSINDEX, "Npc");
	if (lua_type (script->lua, -1) != LUA_TTABLE)
	{
		lua_pop (script->lua, 1);
		return;
	}
	lua_getfield (script->lua, -1, "attack_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return;
	}
	lua_remove (script->lua, -2);

	/* Call the spawn function. */
	liscr_pushdata (script->lua, self->data);
	liscr_pushdata (script->lua, self->object->script);
	liscr_pushdata (script->lua, self->target->script);
	if (lua_pcall (script->lua, 3, 0, 0) != 0)
	{
		lisys_error_set (LI_ERROR_UNKNOWN, "%s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}
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
	liengRange range;
	liengRangeIter rangeiter;

	dist = self->radius;
	target = NULL;
	object = self->object;
	sector = object->sector;
	assert (object->sector != NULL);
	range = lieng_range_new (sector->x, sector->y, sector->z, 1, 0, 256);

	/* Search for target. */
	LIENG_FOREACH_RANGE (rangeiter, range)
	{
		sector = lieng_engine_find_sector (object->engine, rangeiter.index);
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
