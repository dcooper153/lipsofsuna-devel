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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrData Data
 * @{
 */

#include <assert.h>
#include "script.h"
#include "script-class.h"
#include "script-data.h"
#include "script-private.h"

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
 * \param meta Name of the metatable.
 * \return New script userdata or NULL.
 */
liscrData*
liscr_data_new (liscrScript* script,
                void*        data,
                const char*  meta)
{
	liscrClass* clss;
	liscrData* object;

	/* Find class. */
	clss = liscr_script_find_class (script, meta);
	if (clss == NULL)
	{
		lisys_error_set (EINVAL, "invalid class `%s'", meta);
		assert (0);
		return NULL;
	}

	/* Allocate object. */
	object = lua_newuserdata (script->lua, sizeof (liscrData));
	if (object == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	memset (object, 0, sizeof (liscrData));
	object->clss = clss;
	object->script = script;
	object->data = data;
	luaL_getmetatable (script->lua, meta);
	lua_setmetatable (script->lua, -2);

	/* Add to lookup table. */
	lua_rawgeti (script->lua, LUA_REGISTRYINDEX, script->userdata.lookup);
	lua_pushlightuserdata (script->lua, object);
	lua_pushvalue (script->lua, -3);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);

	/* Create private table. */
	lua_newtable (script->lua);
	lua_setfenv (script->lua, -2);

	/* Reference the userdata. */
	liscr_data_ref (object, NULL);
	lua_pop (script->lua, 1);

	/* Statistics. */
	script->userdata.count++;

	return object;
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
void
liscr_data_free (liscrData* object)
{
	liscrScript* script = object->script;

	/* Remove from lookup table. */
	lua_rawgeti (script->lua, LUA_REGISTRYINDEX, script->userdata.lookup);
	lua_pushlightuserdata (script->lua, object);
	lua_pushnil (script->lua);
	lua_settable (script->lua, -3);
	lua_pop (script->lua, 1);

	/* Statistics. */
	script->userdata.count--;
}

/**
 * \brief References the userdata.
 *
 * If the referencer is not NULL, the reference is stored to the reference
 * table of the referencer, effectively providing the same effect as if the
 * userdata was stored into its data table. In practice, this means that both
 * objects need to be subject to collection before either is collected.
 *
 * If the referencer is NULL, the global reference count of the userdata is
 * incremented. This has the effect of preventing the garbage collection of
 * the userdata as long as any global references remain. After the global
 * references have been cleared, the normal garbage collection rules still
 * apply.
 * 
 * \param object Script userdata.
 * \param referencer Script userdata or NULL.
 */
void
liscr_data_ref (liscrData* object,
                liscrData* referencer)
{
	int count;
	liscrScript* script = object->script;

	if (referencer == NULL)
	{
		if (!object->refcount++)
		{
			/* Set protection reference. */
			lua_pushlightuserdata (script->lua, object);
			liscr_pushdata (script->lua, object);
			lua_settable (script->lua, LUA_REGISTRYINDEX);
		}
	}
	else
	{
		/* Get reference count. */
		liscr_pushpriv (script->lua, referencer);
		lua_pushlightuserdata (script->lua, object + 1);
		lua_gettable (script->lua, -2);
		assert (lua_isnumber (script->lua, -1) || lua_isnil (script->lua, -1));
		count = lua_tointeger (script->lua, -1);
		lua_pop (script->lua, 1);

		/* Increment reference count. */
		lua_pushlightuserdata (script->lua, object + 1);
		lua_pushnumber (script->lua, count + 1);
		lua_settable (script->lua, -3);

		/* Set field in referencer. */
		if (!count)
		{
			lua_pushlightuserdata (script->lua, object);
			liscr_pushdata (script->lua, object);
			lua_settable (script->lua, -3);
		}
		lua_pop (script->lua, 1);
	}
}

/**
 * \brief Unreferences the userdata.
 *
 * \param object Script userdata.
 * \param referencer Script userdata or NULL.
 */
void
liscr_data_unref (liscrData* object,
                  liscrData* referencer)
{
	int count;
	liscrScript* script = object->script;

	if (referencer == NULL)
	{
		assert (object->refcount > 0);
		if (!--object->refcount)
		{
			/* Clear protection reference. */
			lua_pushlightuserdata (script->lua, object);
			lua_pushnil (script->lua);
			lua_settable (script->lua, LUA_REGISTRYINDEX);
		}
	}
	else
	{
		/* Get reference count. */
		liscr_pushpriv (script->lua, referencer);
		lua_pushlightuserdata (script->lua, object + 1);
		lua_gettable (script->lua, -2);
		assert (lua_isnumber (script->lua, -1));
		count = lua_tointeger (script->lua, -1);
		lua_pop (script->lua, 1);

		/* Decrement reference count. */
		if (--count)
		{
			lua_pushlightuserdata (script->lua, object + 1);
			lua_pushnumber (script->lua, count);
			lua_settable (script->lua, -3);
		}

		/* Clear field in referencer. */
		if (!count)
		{
			lua_pushlightuserdata (script->lua, object + 1);
			lua_pushnil (script->lua);
			lua_settable (script->lua, -3);
			lua_pushlightuserdata (script->lua, object);
			lua_pushnil (script->lua);
			lua_settable (script->lua, -3);
		}
		lua_pop (script->lua, 1);
	}
}

liscrClass*
liscr_data_get_class (liscrData* self)
{
	return self->clss;
}

liscrScript*
liscr_data_get_script (liscrData* self)
{
	return self->script;
}

/*****************************************************************************/
/* Lua specific. */

liscrClass*
liscr_isanyclass (lua_State*  lua,
                  int         arg)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrScript* script = liscr_script (lua);

	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		lua_pushvalue (lua, arg);
		lua_getfield (lua, LUA_REGISTRYINDEX, clss->meta);
		ret = lua_rawequal (lua, -1, -2);
		lua_pop (lua, 2);
		if (ret)
			return clss;
	}

	return NULL;
}

liscrData*
liscr_isanydata (lua_State* lua,
                 int        arg)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = lua_touserdata (lua, arg);
	if (object == NULL)
		return NULL;
	if (!lua_getmetatable (lua, arg))
		return NULL;
	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		lua_getfield (lua, LUA_REGISTRYINDEX, clss->meta);
		ret = lua_rawequal (lua, -1, -2);
		lua_pop (lua, 1);
		if (!ret)
		{
			lua_pop (lua, 1);
			return object;
		}
	}
	lua_pop (lua, 1);

	return NULL;
}

liscrClass*
liscr_isclass (lua_State*  lua,
               int         arg,
               const char* meta)
{
	int ret;
	lialgStrdicIter iter;
	liscrClass* clss;
	liscrScript* script = liscr_script (lua);

	lua_pushvalue (lua, arg);
	lua_getfield (lua, LUA_REGISTRYINDEX, meta);
	ret = lua_rawequal (lua, -1, -2);
	lua_pop (lua, 2);
	if (!ret)
		return NULL;
	LI_FOREACH_STRDIC (iter, script->classes)
	{
		clss = iter.value;
		if (!strcmp (clss->meta, meta))
			return clss;
	}

	return NULL;
}

/**
 * \brief Gets a userdata from stack.
 *
 * If the type check fails, NULL is returned.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Userdata owned by Lua or NULL.
 */
liscrData*
liscr_isdata (lua_State*  lua,
              int         arg,
              const char* meta)
{
	liscrData* data;

	data = liscr_isanydata (lua, arg);
	if (data == NULL)
		return NULL;
	if (liscr_class_get_interface (data->clss, meta))
		return data;

	return data;
}

liscrData*
liscr_checkanydata (lua_State* lua,
                    int        arg)
{
	char msg[256];
	liscrData* object;

	object = liscr_isanydata (lua, arg);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected wrapped userdata");
		luaL_argcheck (lua, 0, arg, msg);
	}
	if (object->invalid)
	{
		snprintf (msg, 255, "invalid %s", object->clss->meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return object;
}

liscrClass*
liscr_checkclass (lua_State*  lua,
                  int         arg,
                  const char* meta)
{
	char msg[256];
	liscrClass* clss;

	clss = liscr_isclass (lua, arg, meta);
	if (clss == NULL)
	{
		snprintf (msg, 255, "expected class %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return clss;
}

void*
liscr_checkclassdata (lua_State*  lua,
                      int         arg,
                      const char* meta)
{
	void* data;
	liscrClass* clss;

	clss = liscr_checkclass (lua, arg, meta);
	data = liscr_class_get_userdata (clss, meta);
	assert (data != NULL);

	return data;
}

/**
 * \brief Gets a userdata from stack.
 *
 * If the type check fails, a Lua error is raised.
 *
 * Consumes: 0.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param arg Stack index.
 * \param meta Class type.
 * \return Userdata owned by Lua.
 */
liscrData*
liscr_checkdata (lua_State*  lua,
                 int         arg,
                 const char* meta)
{
	char msg[256];
	liscrData* object;

	object = liscr_isdata (lua, arg, meta);
	if (object == NULL)
	{
		snprintf (msg, 255, "expected interface %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}
	if (object->invalid)
	{
		snprintf (msg, 255, "invalid interface %s", meta);
		luaL_argcheck (lua, 0, arg, msg);
	}

	return object;
}

/**
 * \brief Copies fields from a table to a userdata.
 *
 * \param lua Lua state.
 * \param data Destination userdata.
 * \param arg Index of the source table.
 * \return Nonzero on success.
 */
int
liscr_copyargs (lua_State* lua,
                liscrData* data,
                int        arg)
{
	if (lua_type (lua, arg) != LUA_TTABLE)
		return 0;

	/* Get write indexer. */
	liscr_pushdata (lua, data);
	lua_getfield (lua, -1, "__newindex");
	if (lua_type (lua, -1) != LUA_TFUNCTION)
	{
		lua_pop (lua, 2);
		assert (0);
		return 0;
	}

	/* Call it for each table value. */
	lua_pushnil (lua);
	while (lua_next (lua, arg) != 0)
	{
		lua_pushvalue (lua, -3);
		lua_pushvalue (lua, -5);
		lua_pushvalue (lua, -4);
		lua_pushvalue (lua, -4);
		if (lua_pcall (lua, 3, 0, 0))
		{
			lisys_error_set (EINVAL, lua_tostring (lua, -1));
			lisys_error_report ();
			lua_pop (lua, 1);
		}
		lua_pop (lua, 1);
	}
	lua_pop (lua, 3);

	return 1;
}

/**
 * \brief Pushes a reference to the stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param owner Userdata whose reference to fetch.
 * \param slot Integer slot holding the reference.
 * \return Nonzero if the value in the reference slot was not nil.
 */
int
liscr_getref (lua_State* lua,
              liscrData* owner,
              int        slot)
{
	liscr_pushpriv (lua, owner);
	lua_pushlightuserdata (lua, NULL + 1 + slot);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);

	return !lua_isnil (lua, -1);
}

/**
 * \brief Pops a value from stack and stores it as a reference.
 *
 * Setting a reference with this function effectively ensures that the
 * referenced value doesn't get garbage collected as long it is needed by
 * the userdata. A referenced value can only become subject to garbage
 * collection if all userdata referencing it become subject to garbage
 * collection or unreference it.
 *
 * Consumes: 1.
 * Returns: 0.
 *
 * \param lua Lua state.
 * \param owner Userdata whose reference the value will be.
 * \param slot Integer slot to hold the reference.
 */
void
liscr_setref (lua_State* lua,
              liscrData* owner,
              int        slot)
{
	liscr_pushpriv (lua, owner);
	lua_pushlightuserdata (lua, NULL + 1 + slot);
	lua_pushvalue (lua, -3);
	lua_settable (lua, -3);
	lua_pop (lua, 2);
}

/**
 * \brief Pushes the userdata to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param object Pointer to script userdata.
 */
void
liscr_pushdata (lua_State* lua,
                liscrData* object)
{
	lua_rawgeti (lua, LUA_REGISTRYINDEX, object->script->userdata.lookup);
	lua_pushlightuserdata (lua, object);
	lua_gettable (lua, -2);
	lua_remove (lua, -2);
	assert (lua_isuserdata (lua, -1));
}

/**
 * \brief Pushes the environment table of the userdata to stack.
 *
 * Consumes: 0.
 * Returns: 1.
 *
 * \param lua Lua state.
 * \param object Pointer to script userdata.
 */
void
liscr_pushpriv (lua_State* lua,
                liscrData* object)
{
	liscr_pushdata (lua, object);
	lua_getfenv (lua, -1);
	lua_remove (lua, -2);
	assert (lua_istable (lua, -1));
}

/**
 * \brief Returns the current script.
 *
 * \param lua Lua state.
 * \return Script.
 */
liscrScript*
liscr_script (lua_State* lua)
{
	liscrScript* script;

	lua_pushlightuserdata (lua, LISCR_SCRIPT_SELF);
	lua_gettable (lua, LUA_REGISTRYINDEX);
	script = lua_touserdata (lua, -1);
	lua_pop (lua, 1);

	return script;
}

/** @} */
/** @} */
