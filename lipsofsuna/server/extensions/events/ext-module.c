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
 * \addtogroup liextsrvEvents Events
 * @{
 */

#include "ext-module.h"

static int
private_animation (liextModule*   self,
                   liengObject*   object,
                   lisrvAniminfo* info);

static int
private_control (liextModule*     self,
                 liengObject*     object,
                 limatQuaternion* rotation,
                 int              flags);

static int
private_login (liextModule* self,
               liengObject* object,
               const char*  name,
               const char*  pass);

static int
private_logout (liextModule* self,
                liengObject* object);

static int
private_motion (liextModule* self,
                liengObject* object);

static int
private_packet (liextModule* self,
                lisrvClient* client,
                liarcReader* packet);

static int
private_sample (liextModule* self,
                liengObject* object,
                liengSample* sample,
                int          flags);

static int
private_tick (liextModule* self,
              float        secs);

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          visible);

/*****************************************************************************/

lisrvExtensionInfo liextInfo =
{
	LISRV_EXTENSION_VERSION, "Events",
	liext_module_new,
	liext_module_free
};

liextModule*
liext_module_new (lisrvServer* server)
{
	int i;
	liextModule* self;
	liscrScript* script = server->script;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (liextModule));
	if (self == NULL)
		return NULL;
	self->server = server;
	liscr_script_create_class (script, "Event", licomEventScript, self);
	liscr_script_create_class (script, "Events", liextEventsScript, self);

	/* Register callbacks. */
	if (!lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_ANIMATION, 0,
	     	private_animation, self, self->calls + 0) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_CONTROL, 0,
	     	private_control, self, self->calls + 1) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_LOGIN, 0,
	     	private_login, self, self->calls + 2) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_LOGOUT, 0,
	     	private_logout, self, self->calls + 3) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_MOTION, 0,
	     	private_motion, self, self->calls + 4) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_CLIENT_PACKET, 0,
	     	private_packet, self, self->calls + 5) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_SAMPLE, 0,
	     	private_sample, self, self->calls + 6) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_TICK, 0,
	     	private_tick, self, self->calls + 7) ||
	    !lieng_engine_insert_call (server->engine, LISRV_CALLBACK_OBJECT_VISIBILITY, 0,
	     	private_visibility, self, self->calls + 8))
	{
		liext_module_free (self);
		return NULL;
	}

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
	/* Remove callbacks. */
	lieng_engine_remove_calls (self->server->engine, self->calls,
		sizeof (self->calls) / sizeof (licalHandle));

	/* FIXME: Remove the class here. */
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
	liscrScript* script = self->server->script;

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
private_animation (liextModule*   self,
                   liengObject*   object,
                   lisrvAniminfo* info)
{
#warning animation events disabled.
#if 0
	liext_module_event (self, LIEXT_EVENT_OBJECT_ANIMATION,
		"object", LICOM_SCRIPT_OBJECT, object->script,
		"animation", LISCR_TYPE_STRING, animation->name, NULL);
#endif
	return 1;
}

static int
private_control (liextModule*     self,
                 liengObject*     object,
                 limatQuaternion* rotation,
                 int              flags)
{
	liscrData* data0;

	data0 = liscr_quaternion_new (self->server->script, rotation);
	liext_module_event (self, LIEXT_EVENT_CONTROL,
		"object", LICOM_SCRIPT_OBJECT, object->script,
		"rotation", LICOM_SCRIPT_QUATERNION, data0,
		"controls", LISCR_TYPE_INT, flags, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_login (liextModule* self,
               liengObject* object,
               const char*  name,
               const char*  pass)
{
	liext_module_event (self, LIEXT_EVENT_LOGIN,
		"object", LICOM_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_logout (liextModule* self,
                liengObject* object)
{
	liext_module_event (self, LIEXT_EVENT_LOGOUT,
		"object", LICOM_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_motion (liextModule* self,
                liengObject* object)
{
	liext_module_event (self, LIEXT_EVENT_SIMULATE,
		"object", LICOM_SCRIPT_OBJECT, object, NULL);
	return 1;
}

static int
private_packet (liextModule* self,
                lisrvClient* client,
                liarcReader* packet)
{
	int type;
	liscrData* data0;

	type = ((uint8_t*) packet->buffer)[0];
	data0 = liscr_packet_new_readable (self->server->script, packet);
	liext_module_event (self, LIEXT_EVENT_PACKET,
		"object", LICOM_SCRIPT_OBJECT, client->object->script,
		"message", LISCR_TYPE_INT, type,
		"packet", LICOM_SCRIPT_PACKET, data0, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_sample (liextModule* self,
                liengObject* object,
                liengSample* sample,
                int          flags)
{
	liext_module_event (self, LIEXT_EVENT_EFFECT,
		"object", LICOM_SCRIPT_OBJECT, object->script,
		"effect", LISCR_TYPE_STRING, sample->name,
		"flags", LISCR_TYPE_INT, flags, NULL);
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

static int
private_visibility (liextModule* self,
                    liengObject* object,
                    int          visible)
{
	liext_module_event (self, LIEXT_EVENT_VISIBILITY,
		"object", LICOM_SCRIPT_OBJECT, object->script,
		"visible", LISCR_TYPE_BOOLEAN, visible, NULL);
	return 1;
}

/** @} */
/** @} */
/** @} */
