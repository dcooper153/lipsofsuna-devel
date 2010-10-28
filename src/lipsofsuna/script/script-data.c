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
 * \addtogroup LIScrData Data
 * @{
 */

#include "script.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"
#include "script-util.h"

/**
 * \brief Allocates a script userdata object.
 *
 * The created userdata has one initial reference to avoid garbage collection
 * when it's still being initialized. If you intend to give the control of the
 * variable to the script, remember to call #liscr_data_unref after pushing the
 * variable to the stack as return value.
 *
 * \param script Script.
 * \param data Wrapped data.
 * \param clss Class.
 * \param free Free function called by garbage collector.
 * \return New script userdata or NULL.
 */
LIScrData* liscr_data_new (
	LIScrScript* script,
	void*        data,
	LIScrClass*  clss,
	LIScrGCFunc  free)
{
	LIScrData* object;

	/* Allocate object. */
	object = lua_newuserdata (script->lua, sizeof (LIScrData));
	if (object == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	memset (object, 0, sizeof (LIScrData));
	object->signature = 'D';
	object->clss = clss;
	object->script = script;
	object->data = data;
	object->free = free;
	liscr_pushclasspriv (script->lua, clss);
	lua_setmetatable (script->lua, -2);

	/* Add to lookup table. */
	lua_pushlightuserdata (script->lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (script->lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (script->lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (script->lua, object);
	lua_pushvalue (script->lua, -3);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);

	/* Create private table. */
	lua_newtable (script->lua);
	lua_setfenv (script->lua, -2);

	/* Reference the userdata. */
	liscr_data_ref (object);
	lua_pop (script->lua, 1);

	return object;
}

/**
 * \brief Allocates a script userdata object.
 *
 * Like #liscr_data_new but allocates a new wrapped data.
 *
 * \param script Script.
 * \param size Wrapped data size.
 * \param clss Class.
 * \return New script userdata or NULL.
 */
LIScrData* liscr_data_new_alloc (
	LIScrScript* script,
	size_t       size,
	LIScrClass*  clss)
{
	void* data;
	LIScrData* self;

	data = lisys_calloc (1, size);
	if (data == NULL)
		return NULL;
	self = liscr_data_new (script, data, clss, lisys_free);
	if (self == NULL)
	{
		lisys_free (data);
		return NULL;
	}

	return self;
}

/**
 * \brief Called in the garbage collection routines.
 *
 * All the values referenced by the userdata are garbage
 * collected automatically since they are stored to the
 * private table, which is always collected at the same
 * time with the userdata. What is left for us to do is
 * removing some values from the lookup tables.
 *
 * \param object Script userdata.
 */
void liscr_data_free (
	LIScrData* object)
{
	LIScrScript* script = object->script;

	/* Call free function. */
	if (object->free != NULL)
		object->free (object->data, object);

	/* Remove from lookup table. */
	lua_pushlightuserdata (script->lua, LISCR_SCRIPT_LOOKUP_DATA);
	lua_gettable (script->lua, LUA_REGISTRYINDEX);
	lisys_assert (lua_type (script->lua, -1) == LUA_TTABLE);
	lua_pushlightuserdata (script->lua, object);
	lua_pushnil (script->lua);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);
}

/**
 * \brief References the userdata.
 *
 * The function increments the global reference count of the userdata.
 * This has the effect of preventing the garbage collection of the userdata
 * as long as any global references remain. After the global references have
 * been cleared, the normal garbage collection rules apply.
 * 
 * \param self Script userdata.
 */
void liscr_data_ref (
	LIScrData* self)
{
	LIScrScript* script = self->script;

	if (!self->refcount++)
	{
		/* Add to reference table. */
		lua_pushlightuserdata (script->lua, LISCR_SCRIPT_REFS);
		lua_gettable (script->lua, LUA_REGISTRYINDEX);
		lisys_assert (lua_type (script->lua, -1) == LUA_TTABLE);
		lua_pushlightuserdata (script->lua, self);
		liscr_pushdata (script->lua, self);
		lua_settable (script->lua, -3);
		lua_pop (script->lua, 1);
	}
}

/**
 * \brief Unreferences the userdata.
 * \param self Script userdata.
 */
void liscr_data_unref (
	LIScrData* self)
{
	LIScrScript* script = self->script;

	lisys_assert (self->refcount > 0);
	if (!--self->refcount)
	{
		/* Remove from reference table. */
		lua_pushlightuserdata (script->lua, LISCR_SCRIPT_REFS);
		lua_gettable (script->lua, LUA_REGISTRYINDEX);
		lisys_assert (lua_type (script->lua, -1) == LUA_TTABLE);
		lua_pushlightuserdata (script->lua, self);
		lua_pushnil (script->lua);
		lua_settable (script->lua, -3);
		lua_pop (script->lua, 1);
	}
}

/**
 * \brief Gets the class of the userdata.
 * \param self Script userdata.
 * \return Script class.
 */
LIScrClass* liscr_data_get_class (
	LIScrData* self)
{
	return self->clss;
}

/**
 * \brief Sets the class of the userdata to a compatible class.
 * \param self Script userdata.
 * \param clss Script class.
 * \return Nonzero if set successfully.
 */
int liscr_data_set_class (
	LIScrData*  self,
	LIScrClass* clss)
{
	LIScrClass* ptr;

	/* Check for compatibility. */
	for (ptr = clss ; ptr != NULL ; ptr = ptr->base)
	{
		if (ptr == self->clss)
			break;
	}
	if (ptr == NULL)
		return 0;

	/* Set the class. */
	self->clss = clss;

	return 1;
}

/**
 * \brief Gets the C data stored to the userdata.
 * \param self Script userdata.
 * \return Script.
 */
void* liscr_data_get_data (
	LIScrData* self)
{
	return self->data;
}

/**
 * \brief Gets the script that owns this userdata.
 * \param self Script userdata.
 * \return Script.
 */
LIScrScript* liscr_data_get_script (
	LIScrData* self)
{
	return self->script;
}

/** @} */
/** @} */
