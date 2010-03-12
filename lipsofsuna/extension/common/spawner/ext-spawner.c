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
 * \addtogroup LIExtSpawner Spawner    
 * @{
 */

#include "ext-spawner.h"

#define LIEXT_SPAWNER_VERSION 0xFF

static int
private_tick (LIExtSpawner* self,
              float         secs);

static int
private_spawn (LIExtSpawner* self,
               int           slot);

/*****************************************************************************/

/**
 * \brief Creates a new spawner logic.
 *
 * \param module Module.
 * \return Spawner logic or NULL.
 */
LIExtSpawner*
liext_spawner_new (LIExtModule* module)
{
	LIExtSpawner* self;

	self = lisys_calloc (1, sizeof (LIExtSpawner));
	if (self == NULL)
		return NULL;
	self->delay = 600.0f;
	self->limit = 1;
	self->module = module;
	liext_spawner_set_active (self, 1);

	return self;
}

void
liext_spawner_free (LIExtSpawner* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

int
liext_spawner_get_active (LIExtSpawner* self)
{
	return self->active;
}

int
liext_spawner_set_active (LIExtSpawner* self,
                          int           value)
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
 * \brief Sets the spawn limit of the logic.
 *
 * \param self Spawner logic.
 * \param value Spawn limit.
 */
int
liext_spawner_set_limit (LIExtSpawner* self,
                         int           value)
{
	int i;
	int count = 0;
	LIScrData* spawn;

	/* Remove all dead objects. */
	for (i = 0 ; i < self->spawns.count ; i++)
	{
		spawn = self->spawns.array[i];
		self->spawns.array[i] = NULL;
		if (spawn != NULL)
		{
			if (!lieng_object_get_realized (spawn->data))
				liscr_data_unref (spawn, self->script);
			else
				self->spawns.array[count++] = spawn;
		}
	}

	/* Disown extraneous objects. */
	for (i = value ; i < count ; i++)
	{
		spawn = self->spawns.array[i];
		liscr_data_unref (spawn, self->script);
		self->spawns.array[i] = NULL;
	}

	/* Resize the array. */
	if (!lialg_array_resize (&self->spawns, value))
		return 0;
	if (value > count)
		lialg_array_zero (&self->spawns, count, value - count);

	return 1;
}

int
liext_spawner_set_owner (LIExtSpawner* self,
                         LIEngObject*  value)
{
	LIExtSpawner* old;

	if (self->owner == value)
		return 1;

	/* Clear old spawner. */
	if (value != NULL)
	{
		old = lialg_ptrdic_find (self->module->dictionary, value);
		if (old != NULL)
		{
			liphy_object_set_control_mode (old->owner->physics, LIPHY_CONTROL_MODE_RIGID);
			liscr_data_unref (old->owner->script, old->script);
			lialg_ptrdic_remove (old->module->dictionary, value);
			old->owner = NULL;
		}
	}

	/* Set new owner. */
	if (self->owner != NULL)
	{
		liphy_object_set_control_mode (self->owner->physics, LIPHY_CONTROL_MODE_RIGID);
		liscr_data_unref (self->owner->script, self->script);
		lialg_ptrdic_remove (self->module->dictionary, self->owner);
	}
	if (value != NULL)
	{
		liphy_object_set_control_mode (value->physics, LIPHY_CONTROL_MODE_STATIC);
		liscr_data_ref (value->script, self->script);
		lialg_ptrdic_insert (self->module->dictionary, value, self);
	}
	self->owner = value;

	return 1;
}

/*****************************************************************************/

static int
private_tick (LIExtSpawner* self,
              float         secs)
{
	int i;
	LIScrData* spawn;

	/* Wait for next spawn check. */
	self->timer += secs;
	if (self->timer < self->delay)
		return 1;
	self->timer = 0.0f;

	/* Remove all dead objects. */
	for (i = 0 ; i < self->spawns.count ; i++)
	{
		spawn = self->spawns.array[i];
		if (spawn != NULL && !lieng_object_get_realized (spawn->data))
		{
			liscr_data_unref (spawn, self->script);
			self->spawns.array[i] = NULL;
		}
	}

	/* Don't spawn if not realized. */
	if (self->owner == NULL)
		return 1;
	if (!lieng_object_get_realized (self->owner))
		return 1;

	/* Find an empty slot. */
	for (i = 0 ; i < self->spawns.count ; i++)
	{
		spawn = self->spawns.array[i];
		if (spawn == NULL)
			break;
	}
	if (i == self->spawns.count)
		return 1;

	/* Populate the slot. */
	if (!private_spawn (self, i))
		lisys_error_report ();

	return 1;
}

static int
private_spawn (LIExtSpawner* self,
               int           slot)
{
	LIScrData* object;
	LIScrScript* script = self->module->program->script;

	/* Check for spawn function. */
	liscr_pushdata (script->lua, self->script);
	lua_getfield (script->lua, -1, "spawn_cb");
	if (lua_type (script->lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (script->lua, 2);
		return 1;
	}
	lua_remove (script->lua, -2);

	/* Call the spawn function. */
	liscr_pushdata (script->lua, self->owner->script);
	if (lua_pcall (script->lua, 1, 1, 0) != 0)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Spawner.spawn_cb: %s", lua_tostring (script->lua, -1));
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
		lisys_error_set (LISYS_ERROR_UNKNOWN, "Spawner.spawn_cb: did not return an object");
		lua_pop (script->lua, 1);
		return 0;
	}

	/* Populate the slot. */
	liscr_data_ref (object, self->script);
	self->spawns.array[slot] = object;
	lua_pop (script->lua, 1);

	return 1;
}

/** @} */
/** @} */
