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
 * \addtogroup licomEvent Event
 * @{
 */

#include <assert.h>
#include <stdlib.h>
#include <script/lips-script.h>
#include "lips-common-script.h"

typedef struct _licomEvent licomEvent;
struct _licomEvent
{
	liscrData* data;
	int type;
};

/*****************************************************************************/

/* @luadoc
 * module "Core.Common.Event"
 * ---
 * -- Receive information on events and actions.
 * -- @name Event
 * -- @class table
 */

static int
Event___gc (lua_State* lua)
{
	licomEvent* event;
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_EVENT);
	event = self->data;

	free (event);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Creates a new event.
 * -- @param self Event class.
 * -- @param table Optional table of arguments.
 * -- @return New event.
 * function Event.new(self, table)
 */
static int
Event_new (lua_State* lua)
{
	liscrData* event;
	liscrScript* script = liscr_script (lua);

	/* Allocate self. */
	event = licom_event_new (script);
	if (event == NULL)
	{
		lua_pushnil (lua);
		return 1;
	}

	/* Copy attributes. */
	if (!lua_isnoneornil (lua, 2))
		liscr_copyargs (lua, event, 2);

	liscr_pushdata (lua, event);
	liscr_data_unref (event, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Type number of the event.
 * -- @name Event.type
 * -- @class table
 */
static int
Event_getter_type (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_EVENT);

	lua_pushnumber (lua, ((licomEvent*) self)->type);
	return 1;
}
static int
Event_setter_type (lua_State* lua)
{
	int value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_EVENT);
	value = luaL_checknumber (lua, 3);

	((licomEvent*) self)->type = value;
	return 0;
}

/*****************************************************************************/

/**
 * \brief Adds the event type members to the class.
 *
 * \param self Class.
 * \param data Pointer to script.
 */
void
licomEventScript (liscrClass* self,
                  void*       data)
{
	liscr_class_insert_func (self, "__gc", Event___gc);
	liscr_class_insert_func (self, "new", Event_new);
	liscr_class_insert_getter (self, "type", Event_getter_type);
	liscr_class_insert_setter (self, "type", Event_setter_type);
}

/**
 * \brief Creates a new script event.
 *
 * The created event has one initial reference.
 *
 * \param script Script.
 * \return New event or NULL.
 */
liscrData*
licom_event_new (liscrScript* script)
{
	liscrData* self;
	licomEvent* event;

	event = calloc (1, sizeof (licomEvent));
	if (event == NULL)
		return NULL;
	self = liscr_data_new (script, event, LICOM_SCRIPT_EVENT);
	if (self == NULL)
	{
		free (event);
		return NULL;
	}
	event->data = self;

	return self;
}

/**
 * \brief Creates a new script event.
 *
 * The created event has one initial reference.
 *
 * \param script Script.
 * \param args List of fields to set.
 * \return New event or NULL.
 */
liscrData*
licom_event_newv (liscrScript* script,
                  va_list      args)
{
	liscrData* self;

	self = licom_event_new (script);
	if (self == NULL)
		return NULL;
	licom_event_setv (self, args);

	return self;
}

/**
 * \brief Creates a new script event.
 *
 * The created event has one initial reference.
 *
 * \param script Script.
 * \param ... List of fields to set.
 * \return New script event or NULL.
 */
liscrData*
licom_event_newva (liscrScript* script,
                                ...)
{
	va_list args;
	liscrData* self;

	va_start (args, script);
	self = licom_event_newv (script, args);
	va_end (args);

	return self;
}

/**
 * \brief Sets member values of the event.
 *
 * \param self Event.
 * \param ... List of arguments.
 */
void
licom_event_set (liscrData* self,
                            ...)
{
	va_list args;

	va_start (args, self);
	licom_event_setv (self, args);
	va_end (args);
}

/**
 * \brief Sets member values of the event.
 *
 * \param self Event.
 * \param args List of arguments.
 */
void
licom_event_setv (liscrData* self,
                  va_list    args)
{
	int pint;
	void* pptr;
	char* pstr;
	float pfloat;
	const char* type;
	const char* name;
	liscrScript* script = self->script;

	liscr_pushpriv (script->lua, self);
	while (1)
	{
		/* Get name. */
		name = va_arg (args, char*);
		if (name == NULL)
			break;

		/* Duplicated from liscrData due to the behavior of varargs
		   being undefined when passed to a function and then reused. */
		type = va_arg (args, char*);
		if (type == LISCR_TYPE_BOOLEAN)
		{
			pint = va_arg (args, int);
			lua_pushboolean (script->lua, pint);
		}
		else if (type == LISCR_TYPE_FLOAT)
		{
			pfloat = va_arg (args, double);
			lua_pushnumber (script->lua, pfloat);
		}
		else if (type == LISCR_TYPE_INT)
		{
			pint = va_arg (args, int);
			lua_pushnumber (script->lua, pint);
		}
		else if (type == LISCR_TYPE_STRING)
		{
			pstr = va_arg (args, char*);
			lua_pushstring (script->lua, pstr);
		}
		else
		{
			pptr = va_arg (args, void*);
			if (pptr == NULL)
				break;
			liscr_pushdata (script->lua, pptr);
		}

		/* Set field. */
		lua_setfield (script->lua, -2, name);
	}
	lua_pop (script->lua, 1);
}

/**
 * \brief Gets the type of the event.
 *
 * No validity check beyound the value actually being a number is performed.
 * You need to check that the type isn't out of bounds yourself.
 *
 * \param self Event.
 * \return Event type.
 */
int
licom_event_get_type (const liscrData* self)
{
	licomEvent* event = self->data;

	return event->type;
}

/**
 * \brief Sets the type of the event.
 *
 * \param self Event.
 * \param type Event type.
 */
void
licom_event_set_type (liscrData* self,
                      int        type)
{
	licomEvent* event = self->data;

	event->type = type;
}

/** @} */
/** @} */
