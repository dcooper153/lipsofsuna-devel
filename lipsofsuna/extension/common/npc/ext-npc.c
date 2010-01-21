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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtNpc Npc      
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-npc.h"

static int
private_tick (LIExtNpc* self,
              float     secs);

static void
private_attack (LIExtNpc* self);

static int
private_rescan (LIExtNpc* self);

/*****************************************************************************/

/**
 * \brief Creates a new non-player character logic.
 *
 * \param module Module.
 * \return Non-player character logic or NULL.
 */
LIExtNpc*
liext_npc_new (LIExtModule* module)
{
	LIExtNpc* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtNpc));
	if (self == NULL)
		return NULL;
	self->alert = 1;
	self->radius = 30.0f;
	self->refresh = 5.0f;
	self->module = module;
	liext_npc_set_active (self, 1);

	return self;
}

/**
 * \brief Frees the Npc logic.
 *
 * \param self Npc logic.
 */
void
liext_npc_free (LIExtNpc* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lialg_ptrdic_remove (self->module->dictionary, self->owner);
	lisys_free (self);
}

int
liext_npc_get_active (LIExtNpc* self)
{
	return self->active;
}

int
liext_npc_set_active (LIExtNpc* self,
                      int       value)
{
	if (self->active == value)
		return 1;
	if (value)
		lical_callbacks_insert (self->module->program->callbacks, self->module->program->engine, "tick", 0, private_tick, self, self->calls + 0);
	else
		lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	self->active = value;

	return 1;
}

/**
 * \brief Sets the object controlled by the logic.
 *
 * References to objects are maintained automatically.
 *
 * \param self Npc logic.
 * \param value Object.
 * \return Nonzero on success.
 */
int
liext_npc_set_owner (LIExtNpc*    self,
                     LIEngObject* value)
{
	int flags;
	LIExtNpc* old;

	/* Discard old target. */
	liext_npc_set_target (self, NULL);

	/* Clear old logic. */
	if (value != NULL)
	{
		old = lialg_ptrdic_find (self->module->dictionary, value);
		if (old != NULL)
		{
			liext_npc_set_target (old, NULL);
			flags = lieng_object_get_flags (old->owner);
			liphy_object_set_control_mode (old->owner->physics, LIPHY_CONTROL_MODE_RIGID);
			lieng_object_set_flags (old->owner, flags & ~LIENG_OBJECT_FLAG_DYNAMIC);
			liscr_data_unref (old->owner->script, self->script);
			liscr_data_unref (self->script, old->owner->script);
			old->owner = NULL;
		}
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		flags = lieng_object_get_flags (self->owner);
		liphy_object_set_control_mode (self->owner->physics, LIPHY_CONTROL_MODE_RIGID);
		lieng_object_set_flags (self->owner, flags & ~LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_unref (self->owner->script, self->script);
		liscr_data_unref (self->script, self->owner->script);
		lialg_ptrdic_remove (self->module->dictionary, self->owner);
	}
	if (value != NULL)
	{
		flags = lieng_object_get_flags (value);
		liphy_object_set_control_mode (value->physics, LIPHY_CONTROL_MODE_CHARACTER);
		lieng_object_set_flags (value, flags | LIENG_OBJECT_FLAG_DYNAMIC);
		liscr_data_ref (value->script, self->script);
		liscr_data_ref (self->script, value->script);
		lialg_ptrdic_insert (self->module->dictionary, value, self);
	}
	self->owner = value;

	return 1;
}

/**
 * \brief Sets the path for the NPC to traverse.
 *
 * \param self Npc.
 * \param path Script path data or NULL.
 */
void
liext_npc_set_path (LIExtNpc*  self,
                    LIScrData* path)
{
	if (self->path == path)
		return;
	if (self->path != NULL)
		liscr_data_unref (self->path, self->script);
	if (path != NULL)
		liscr_data_ref (path, self->script);
	self->path = path;
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
liext_npc_set_target (LIExtNpc*    self,
                      LIEngObject* object)
{
	if (self->target != NULL)
		liscr_data_unref (self->target->script, self->script);
	self->target = object;
	if (object != NULL)
		liscr_data_ref (object->script, self->script);
}

/*****************************************************************************/

static int
private_tick (LIExtNpc* self,
              float     secs)
{
	LIAiPath* path;
	LIEngObject* object;
	LIMatTransform transform;
	LIMatVector diff;
	LIMatVector impulse;
	LIMatVector vector;
	LIScrData* tmp;
	LIPhyObject* physics;

	/* Get object data. */
	object = self->owner;
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
		if (self->target->sector == NULL || !lieng_object_get_realized (self->target))
			liext_npc_set_target (self, NULL);
	}

	/* Scan for targets periodically. */
	self->timer += secs;
	if (self->timer >= self->refresh)
	{
		/* Get new target. */
		self->timer = 0.0f;
		if (!private_rescan (self))
			lisys_error_report ();

		/* Solve path. */
		if (self->owner != NULL && self->target != NULL)
		{
			lieng_object_get_transform (self->target, &transform);
			transform.position.y += 0.5f;
			path = liext_module_solve_path (self->module, self->owner, &transform.position);
			if (path != NULL)
			{
				tmp = liscr_data_new (self->script->script, path, LISCR_SCRIPT_PATH, liai_path_free);
				if (tmp != NULL)
				{
					liext_npc_set_path (self, tmp);
					liscr_data_unref (tmp, NULL);
				}
				else
					liai_path_free (path);
			}
		}
	}

	/* Traverse path if set. */
	/* TODO: Give up if can't reach. */
	if (self->path != NULL)
	{
		path = self->path->data;
		if (path->position < path->points.count)
		{
			/* Get distance to waypoint. */
			lieng_object_get_transform (object, &transform);
			liai_path_get_point (path, path->position, &vector);
			diff = limat_vector_subtract (transform.position, vector);
			diff = limat_vector_multiply (diff, 1.0f / LIAI_WAYPOINT_WIDTH);

			/* Check if reached it. */
#warning These path traversal tolerance values are sketchy
			if (-0.9f < diff.x && diff.x < 0.9f &&
			    -2.5f < diff.y && diff.y < 0.9f &&
			    -0.9f < diff.z && diff.z < 0.9f)
			{
				/* Next waypoint. */
				path->position++;
				if (path->position == path->points.count)
					return 1;

				/* Get distance to waypoint. */
				lieng_object_get_transform (object, &transform);
				liai_path_get_point (path, path->position, &vector);
				diff = limat_vector_subtract (transform.position, vector);

				/* Jump if too high for walking. */
				/* FIXME: Check for walkable slopes first. */
				/* FIXME: This fails most of the time because of the wall being too close. */
				if (diff.y > 0.5f)
				{
					impulse = limat_vector_init (0.0f, 100.0f, 0.0f);
					lieng_object_jump (object, &impulse);
				}
			}

			/* Move towards waypoint. */
			lieng_object_approach (object, &vector, 1.0f);
			return 1;
		}
		else
		{
			/* TODO: Emit path traversed event. */
			liext_npc_set_path (self, NULL);
		}
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
private_attack (LIExtNpc* self)
{
	LIScrScript* script = self->module->program->script;

	/* Check for attack function. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "attack_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return;
	}
	lua_remove (script->lua, -2);

	/* Call the attack function. */
	liscr_pushdata (script->lua, self->script);
	liscr_pushdata (script->lua, self->owner->script);
	liscr_pushdata (script->lua, self->target->script);
	if (lua_pcall (script->lua, 3, 0, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Npc.attack_cb: %s", lua_tostring (script->lua, -1));
		lisys_error_report ();
		lua_pop (script->lua, 1);
	}
}

static int
private_rescan (LIExtNpc* self)
{
	LIScrData* object;
	LIScrScript* script = self->module->program->script;

	/* Check for scan function. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "scan_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}
	lua_remove (script->lua, -2);

	/* Call the attack function. */
	liscr_pushdata (script->lua, self->script);
	liscr_pushdata (script->lua, self->owner->script);
	if (lua_pcall (script->lua, 2, 1, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Npc.scan_cb: %s", lua_tostring (script->lua, -1));
		lua_pop (script->lua, 1);
		return 0;
	}

	/* Check if got a return value. */
	if (lua_isnil (script->lua, -1))
	{
		lua_pop (script->lua, 1);
		return 1;
	}

	/* Check if return value is valid. */
	object = liscr_isdata (script->lua, -1, LISCR_SCRIPT_OBJECT);
	if (object == NULL)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Npc.scan_cb: did not return an object");
		lua_pop (script->lua, 1);
		return 0;
	}

	/* Populate the slot. */
	liext_npc_set_target (self, object->data);
	lua_pop (script->lua, 1);

	return 1;
}

/** @} */
/** @} */
