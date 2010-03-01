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
 * \addtogroup LIExtEvents Events
 * @{
 */

#include "ext-module.h"

static int
private_animation (LIExtModule*   self,
                   LIEngObject*   object,
                   LISerAniminfo* info);

static int
private_control (LIExtModule*     self,
                 LIEngObject*     object,
                 LIMatQuaternion* rotation,
                 int              flags);

static int
private_login (LIExtModule* self,
               LIEngObject* object,
               const char*  name,
               const char*  pass);

static int
private_logout (LIExtModule* self,
                LIEngObject* object);

static int
private_motion (LIExtModule* self,
                LIEngObject* object);

static int
private_packet (LIExtModule* self,
                LIEngObject* object,
                LIArcReader* packet);

static int
private_sample (LIExtModule* self,
                LIEngObject* object,
                LIEngSample* sample,
                int          flags);

static int
private_tick (LIExtModule* self,
              float        secs);

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          visible);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Events",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	int i;
	LIExtModule* self;
	LIScrScript* script = program->script;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	liscr_script_create_class (script, "Event", liscr_script_event, self);
	liscr_script_create_class (script, "Events", liext_script_events, self);

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "object-animation", 0, private_animation, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-control", 0, private_control, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-login", 0, private_login, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-logout", 0, private_logout, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-motion", 0, private_motion, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "client-packet", 5, private_packet, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-effect", 0, private_sample, self, self->calls + 6) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", 0, private_visibility, self, self->calls + 8))
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
liext_module_free (LIExtModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
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
liext_module_event (LIExtModule* self,
                    int          type,
                                 ...)
{
	va_list args;
	LIScrData* event = NULL;
	LIScrScript* script = self->program->script;

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
			event = liscr_event_newv (script, args);
			va_end (args);
			if (event == NULL)
				break;
			liscr_event_set_type (event, type);
		}

		/* Call handler. */
		lua_pushvalue (script->lua, -1);
		liscr_pushdata (script->lua, event);
		if (lua_pcall (script->lua, 1, 0, 0) != 0)
		{
			lisys_error_set (EINVAL, "Event handler %d: %s", type, lua_tostring (script->lua, -1));
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
private_animation (LIExtModule*   self,
                   LIEngObject*   object,
                   LISerAniminfo* info)
{
#warning animation events disabled.
#if 0
	liext_module_event (self, LIEXT_EVENT_OBJECT_ANIMATION,
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"animation", LISCR_TYPE_STRING, animation->name, NULL);
#endif
	return 1;
}

static int
private_control (LIExtModule*     self,
                 LIEngObject*     object,
                 LIMatQuaternion* rotation,
                 int              flags)
{
	LIScrData* data0;

	data0 = liscr_quaternion_new (self->program->script, rotation);
	liext_module_event (self, LIEXT_EVENT_CONTROL,
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"rotation", LISCR_SCRIPT_QUATERNION, data0,
		"controls", LISCR_TYPE_INT, flags, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_login (LIExtModule* self,
               LIEngObject* object,
               const char*  name,
               const char*  pass)
{
	liext_module_event (self, LIEXT_EVENT_LOGIN,
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_logout (LIExtModule* self,
                LIEngObject* object)
{
	liext_module_event (self, LIEXT_EVENT_LOGOUT,
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_motion (LIExtModule* self,
                LIEngObject* object)
{
	liext_module_event (self, LIEXT_EVENT_SIMULATE,
		"object", LISCR_SCRIPT_OBJECT, object->script, NULL);
	return 1;
}

static int
private_packet (LIExtModule* self,
                LIEngObject* object,
                LIArcReader* packet)
{
	int type;
	LIScrData* data0;

	type = ((uint8_t*) packet->buffer)[0];
	data0 = liscr_packet_new_readable (self->program->script, packet);
	liext_module_event (self, LIEXT_EVENT_PACKET,
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"message", LISCR_TYPE_INT, type,
		"packet", LISCR_SCRIPT_PACKET, data0, NULL);
	if (data0 != NULL)
		liscr_data_unref (data0, NULL);

	return 1;
}

static int
private_sample (LIExtModule* self,
                LIEngObject* object,
                LIEngSample* sample,
                int          flags)
{
	liext_module_event (self, LIEXT_EVENT_EFFECT,
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"effect", LISCR_TYPE_STRING, sample->name,
		"flags", LISCR_TYPE_INT, flags, NULL);
	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	liext_module_event (self, LIEXT_EVENT_TICK,
		"secs", LISCR_TYPE_FLOAT, secs, NULL);
	return 1;
}

static int
private_visibility (LIExtModule* self,
                    LIEngObject* object,
                    int          visible)
{
	liext_module_event (self, LIEXT_EVENT_VISIBILITY,
		"object", LISCR_SCRIPT_OBJECT, object->script,
		"visible", LISCR_TYPE_BOOLEAN, visible, NULL);
	return 1;
}

/** @} */
/** @} */
