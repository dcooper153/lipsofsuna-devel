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
 * \addtogroup liextsrv Server
 * @{
 * \addtogroup liextsrvEvents Events
 * @{
 */

#include "ext-module.h"

static int
private_object_model (liextModule* self,
                      liengObject* object,
                      liengModel*  model);

static int
private_object_new (liextModule* self,
                    liengObject* object);

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader);

static int
private_select (liextModule*    self,
                lirndSelection* selection);

static int
private_tick (liextModule* self,
              float        secs);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Events",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (licliClient* client)
{
	int i;
	liextModule* self;
	liscrScript* script = client->script;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->client = client;

	/* Register callbacks. */
	if (!lical_callbacks_insert (client->callbacks, client->engine, "object-model", 2, private_object_model, self, self->calls + 0) ||
	    !lical_callbacks_insert (client->callbacks, client->engine, "object-new", 2, private_object_new, self, self->calls + 1) ||
	    !lical_callbacks_insert (client->callbacks, client->engine, "packet", 2, private_packet, self, self->calls + 2) ||
	    !lical_callbacks_insert (client->callbacks, client->engine, "select", 2, private_select, self, self->calls + 3) ||
	    !lical_callbacks_insert (client->callbacks, client->engine, "tick", 2, private_tick, self, self->calls + 4))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (script, "Event", licomEventScript, self);
	liscr_script_create_class (script, "Events", liextEventsScript, self);

	/* Register events. */
	lua_newtable (script->lua);
	lua_pushlightuserdata (script->lua, self);
	lua_pushvalue (script->lua, -2);
	lua_settable (script->lua, LUA_REGISTRYINDEX);
	for (i = 0 ; i < LIEXT_EVENT_MAX ; i++)
	{
		lua_pushnumber (script->lua, i);
		lua_newtable (script->lua);
		lua_settable (script->lua, -3);
	}
	lua_pop (script->lua, 1);

	return self;
}

void
liext_module_free (liextModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (licalHandle));
	lisys_free (self);
}

/**
 * \brief Emits an event.
 *
 * \param self Module.
 * \param type Event type.
 * \param ... Event arguments.
 */
void
liext_module_event (liextModule* self,
                    int          type,
                                 ...)
{
	va_list args;
	liscrData* event = NULL;
	liscrScript* script = self->client->script;

	/* Get event table. */
	lua_pushlightuserdata (script->lua, self);
	lua_gettable (script->lua, LUA_REGISTRYINDEX);
	if (!lua_istable (script->lua, -1))
	{
		lua_pop (script->lua, 1);
		assert (0);
		return;
	}

	/* Get handler table. */
	lua_pushnumber (script->lua, type);
	lua_gettable (script->lua, -2);
	lua_remove (script->lua, -2);
	if (!lua_istable (script->lua, -1))
	{
		lua_pop (script->lua, 1);
		assert (0);
		return;
	}

	/* Loop through all handlers. */
	lua_pushnil (script->lua);
	while (lua_next (script->lua, -2) != 0)
	{
		/* Create event. */
		if (event == NULL)
		{
			va_start (args, type);
			event = licom_event_newv (script, args);
			va_end (args);
			if (event == NULL)
				break;
			licom_event_set_type (event, type);
		}

		/* Call handler. */
		lua_pushvalue (script->lua, -1);
		liscr_pushdata (script->lua, event);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "%s", lua_tostring (script->lua, -1));
			lisys_error_report ();
			lua_pop (script->lua, 1);
		}

		/* Next handler. */
		lua_pop (script->lua, 1);
	}

	lua_pop (script->lua, 1);
	if (event != NULL)
		liscr_data_unref (event, NULL);
	va_end (args);
}

/*****************************************************************************/

static int
private_object_model (liextModule* self,
                      liengObject* object,
                      liengModel*  model)
{
	if (object->script != NULL && model != NULL)
	{
		liext_module_event (self, LIEXT_EVENT_OBJECT_MODEL,
			"object", LISCR_SCRIPT_OBJECT, object->script,
			"model", LISCR_TYPE_STRING, model->name, NULL);
	}

	return 1;
}

static int
private_object_new (liextModule* self,
                    liengObject* object)
{
	if (object->script != NULL)
	{
		/* Emit object creation event. */
		liext_module_event (self, LIEXT_EVENT_OBJECT_NEW,
			"object", LISCR_SCRIPT_OBJECT, object->script, NULL);

		/* Emit assign event if player object. */
		if (self->client->network != NULL &&
		    self->client->network->id == object->id)
			liext_module_event (self, LIEXT_EVENT_ASSIGN, NULL);
	}

	return 1;
}

static int
private_packet (liextModule* self,
                int          type,
                liarcReader* reader)
{
	liscrData* data0;

	reader->pos = 1;
	data0 = liscr_packet_new_readable (self->client->script, reader);
	liext_module_event (self, LIEXT_EVENT_PACKET,
		"message", LISCR_TYPE_INT, type,
		"packet", LISCR_SCRIPT_PACKET, data0, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_select (liextModule*    self,
                lirndSelection* selection)
{
	liengObject* object;

	if (selection != NULL)
	{
		object = lieng_engine_find_object (self->client->engine, selection->object);
		if (object != NULL && object->script != NULL)
		{
			liext_module_event (self, LIEXT_EVENT_SELECT,
				"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
			return 1;
		}
	}

	return 1;
}

static int
private_tick (liextModule* self,
              float        secs)
{
	liext_module_event (self, LIEXT_EVENT_TICK,
		"secs", LISCR_TYPE_FLOAT, secs, NULL);

	return 1;
}

/** @} */
/** @} */
/** @} */
