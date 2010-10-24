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

#include <lipsofsuna/script.h>
#include <lipsofsuna/system.h>

typedef struct _LIScrEvent LIScrEvent;
struct _LIScrEvent
{
	LIScrData* data;
	char* type;
};

/*****************************************************************************/

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
	liscr_data_unref (data, NULL);
}

/* @luadoc
 * ---
 * -- Type number of the event.
 * -- @name Event.type
 * -- @class table
 */
static void Event_getter_type (LIScrArgs* args)
{
	LIScrEvent* self = args->self;

	lisys_assert (self->type != NULL);
	liscr_args_seti_string (args, self->type);
}
static void Event_setter_type (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liscr_event_set_type (args->data, value);
}

/*****************************************************************************/

/**
 * \brief Adds the event type members to the class.
 *
 * \param self Class.
 * \param data Pointer to script.
 */
void
liscr_script_event (LIScrClass* self,
                  void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Event_new);
	liscr_class_insert_mvar (self, "type", Event_getter_type, Event_setter_type);
}

/**
 * \brief Creates a new script event.
 *
 * The created event has one initial reference.
 *
 * \param script Script.
 * \return New event or NULL.
 */
LIScrData*
liscr_event_new (LIScrScript* script)
{
	LIScrClass* clss;
	LIScrData* data;
	LIScrEvent* self;

	clss = liscr_script_find_class (script, LISCR_SCRIPT_EVENT);
	data = liscr_data_new_alloc (script, sizeof (LIScrEvent), clss);
	if (data == NULL)
		return NULL;
	self = data->data;
	self->data = data;
	self->type = listr_dup ("event");
	if (self->type == NULL)
	{
		liscr_data_unref (data, NULL);
		return NULL;
	}

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
LIScrData*
liscr_event_newv (LIScrScript* script,
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
LIScrData*
liscr_event_newva (LIScrScript* script,
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
 *
 * \param self Event.
 * \param ... List of arguments.
 */
void
liscr_event_set (LIScrData* self,
                            ...)
{
	va_list args;

	va_start (args, self);
	liscr_event_setv (self, args);
	va_end (args);
}

/**
 * \brief Sets member values of the event.
 *
 * \param self Event.
 * \param args List of arguments.
 */
void
liscr_event_setv (LIScrData* self,
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
 * \brief Gets the type of the event.
 *
 * No validity check beyound the value actually being a number is performed.
 * You need to check that the type isn't out of bounds yourself.
 *
 * \param self Event.
 * \return Event type.
 */
const char*
liscr_event_get_type (const LIScrData* self)
{
	LIScrEvent* event = self->data;

	return event->type;
}

/**
 * \brief Sets the type of the event.
 *
 * \param self Event.
 * \param type Event type.
 */
void
liscr_event_set_type (LIScrData*  self,
                      const char* type)
{
	char* tmp;
	LIScrEvent* event = self->data;

	tmp = listr_dup (type);
	if (tmp != NULL)
	{
		lisys_free (event->type);
		event->type = tmp;
	}
}

/** @} */
/** @} */
