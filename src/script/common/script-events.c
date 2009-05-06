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
 * \addtogroup licom Common
 * @{
 * \addtogroup licomEvents Events
 * @{
 */

#include <script/lips-script.h>
#include "lips-common-script.h"

#define MAGICPTR ((void*) 1)

typedef struct _licomEvents licomEvents;
struct _licomEvents
{
};

/*****************************************************************************/

/* @luadoc
 * module "Core.Common.Events"
 * ---
 * -- Register event handlers.
 * -- @name Events
 * -- @class table
 */

static int
Events___gc (lua_State* lua)
{
	licomEvents* events;
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_EVENTS);
	events = self->data;

	free (events);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Registers an event handler.
 * --
 * -- @param self Events object.
 * -- @param type Event number.
 * -- @param handler Handler function.
 * function Events.insert(self, type, handler)
 */
static int
Events_insert (lua_State* lua)
{
	int type;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICOM_SCRIPT_EVENTS);
	type = luaL_checkinteger (lua, 2);
	luaL_checktype (lua, 3, LUA_TFUNCTION);

	/* Get handler table. */
	lua_pushlightuserdata (lua, MAGICPTR + type);
	lua_gettable (lua, 1);
	if (lua_isnil (lua, -1))
	{
		lua_pop (lua, 1);
		luaL_argerror (lua, 2, "invalid event type");
	}

	/* Store function. */
	lua_pushvalue (lua, 3);
	lua_pushvalue (lua, 3);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/* @luadoc
 * ---
 * -- Unregisters an event handler.
 * --
 * -- @param self Events object.
 * -- @param type Event number.
 * -- @param handler Handler function.
 * function Events.remove(self, type, handler)
 */
static int
Events_remove (lua_State* lua)
{
	int type;
	liscrData* data;

	data = liscr_checkdata (lua, 1, LICOM_SCRIPT_EVENTS);
	type = luaL_checkinteger (lua, 2);
	luaL_checktype (lua, 3, LUA_TFUNCTION);

	/* Get handler table. */
	lua_pushlightuserdata (lua, MAGICPTR + type);
	lua_gettable (lua, 1);
	if (lua_isnil (lua, -1))
	{
		lua_pop (lua, 1);
		luaL_argerror (lua, 2, "invalid event type");
	}

	/* Remove function. */
	lua_pushvalue (lua, 3);
	lua_pushnil (lua);
	lua_settable (lua, -3);
	lua_pop (lua, 1);

	return 0;
}

/*****************************************************************************/

/**
 * \brief Adds the Events members to the class.
 *
 * \param self Class.
 * \param data Pointer to script.
 */
void
licomEventsScript (liscrClass* self,
                   void*       data)
{
	liscr_class_set_convert (self, (void*) abort);
	liscr_class_insert_func (self, "__gc", Events___gc);
	liscr_class_insert_func (self, "insert", Events_insert);
	liscr_class_insert_func (self, "remove", Events_remove);
}

/**
 * \brief Creates a new script event dispatcher.
 *
 * The created object has one initial reference.
 *
 * \param script Script.
 * \return New event or NULL.
 */
liscrData*
licom_events_new (liscrScript* script)
{
	liscrData* self;
	licomEvents* events;

	/* Allocate self. */
	events = calloc (1, sizeof (licomEvents));
	if (events == NULL)
		return NULL;

	/* Allocate script data. */
	self = liscr_data_new (script, events, LICOM_SCRIPT_EVENTS);
	if (self == NULL)
	{
		free (events);
		return NULL;
	}

	return self;
}

void
licom_events_event (liscrData* self,
                    int        type,
                    va_list    args)
{
	va_list copy;
	liscrData* event = NULL;
	liscrScript* script = liscr_data_get_script (self);

	/* Get handler table. */
	liscr_pushdata (script->lua, self);
	lua_pushlightuserdata (script->lua, MAGICPTR + type);
	lua_gettable (script->lua, -2);
	if (lua_isnil (script->lua, -1))
	{
		printf ("MISSING EVENT TYPE %d\n", type);
		lua_pop (script->lua, 2);
		assert (0);
		return;
	}
	assert (lua_istable (script->lua, -1));

	/* Loop through all handlers. */
	lua_pushnil (script->lua);
	while (lua_next (script->lua, -2) != 0)
	{
		/* Create event. */
		if (event == NULL)
		{
			va_copy (copy, args);
			event = licom_event_newv (script, copy);
			va_end (copy);
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

	lua_pop (script->lua, 2);
	if (event != NULL)
		liscr_data_unref (event, NULL);
}

void
licom_events_insert_type (liscrData* self,
                          int        type)
{
	liscrScript* script = liscr_data_get_script (self);

	/* Create handler table. */
	liscr_pushdata (script->lua, self);
	lua_pushlightuserdata (script->lua, MAGICPTR + type);
	lua_newtable (script->lua);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);
}

/** @} */
/** @} */
