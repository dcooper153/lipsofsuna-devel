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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrEvent Event
 * @{
 */

#include "lipsofsuna/script.h"
#include "lipsofsuna/system.h"
#include "script-private.h"

/* @luadoc
 * module "builtin/event"
 * ---
 * -- Receive information on events and actions.
 * -- @name Event
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Creates a new event.
 * -- @param self Event class.
 * -- @param args Arguments.
 * -- @return New event.
 * function Event.new(self, args)
 */
static void Event_new (LIScrArgs* args)
{
	LIScrData* data;

	data = liscr_event_new (args->script);
	if (data == NULL)
		return;
	liscr_args_call_setters (args, data);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

/* @luadoc
 * ---
 * -- Type of the event.
 * -- @name Event.type
 * -- @class table
 */

/*****************************************************************************/

/**
 * \brief Adds the event type members to the class.
 * \param self Class.
 * \param data Pointer to script.
 */
void liscr_script_event (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Event_new);
}

/**
 * \brief Creates a new script event.
 *
 * The created event has one initial reference.
 *
 * \param script Script.
 * \return New event or NULL.
 */
LIScrData* liscr_event_new (
	LIScrScript* script)
{
	LIScrClass* clss;
	LIScrData* data;

	clss = liscr_script_find_class (script, LISCR_SCRIPT_EVENT);
	data = liscr_data_new_alloc (script, 1, clss);
	if (data == NULL)
		return NULL;

	return data;
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
LIScrData* liscr_event_newv (
	LIScrScript* script,
	va_list      args)
{
	LIScrData* self;

	self = liscr_event_new (script);
	if (self == NULL)
		return NULL;
	liscr_event_setv (self, args);

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
LIScrData* liscr_event_newva (
	LIScrScript* script,
	             ...)
{
	va_list args;
	LIScrData* self;

	va_start (args, script);
	self = liscr_event_newv (script, args);
	va_end (args);

	return self;
}

/**
 * \brief Sets member values of the event.
 * \param self Event.
 * \param ... List of arguments.
 */
void liscr_event_set (
	LIScrData* self,
	           ...)
{
	va_list args;

	va_start (args, self);
	liscr_event_setv (self, args);
	va_end (args);
}

/**
 * \brief Sets member values of the event.
 * \param self Event.
 * \param args List of arguments.
 */
void liscr_event_setv (
	LIScrData* self,
	va_list    args)
{
	int pint;
	void* pptr;
	char* pstr;
	float pfloat;
	const char* type;
	const char* name;
	LIScrScript* script = self->script;

	liscr_pushpriv (script->lua, self);
	while (1)
	{
		/* Get name. */
		name = va_arg (args, char*);
		if (name == NULL)
			break;

		/* Duplicated from LIScrData due to the behavior of varargs
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
 * \brief Sets the type of the event.
 * \param self Event.
 * \param type Event type.
 */
void liscr_event_set_type (
	LIScrData*  self,
	const char* type)
{
	liscr_pushpriv (self->script->lua, self);
	lua_pushstring (self->script->lua, type);
	lua_setfield (self->script->lua, -2, "type");
	lua_pop (self->script->lua, 1);
}

/** @} */
/** @} */
