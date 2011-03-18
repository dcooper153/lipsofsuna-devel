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
 * \addtogroup LIScrArgs Args
 * @{
 */

#include "script-args.h"
#include "script-library.h"
#include "script-private.h"
#include "script-util.h"

void
liscr_args_init_func (LIScrArgs*  self,
                      lua_State*  lua,
                      LIScrClass* clss,
                      LIScrData*  data)
{
	memset (self, 0, sizeof (LIScrArgs));
	self->lua = lua;
	self->script = clss->script;
	self->clss = clss;
	self->data = data;
	if (data != NULL)
		self->self = data->data;
	self->args_start = 2;
	self->args_count = lua_gettop (lua) - 1;
	if (!liscr_isanyclass (lua, self->args_start) &&
	    lua_type (self->lua, self->args_start) == LUA_TTABLE)
	{
		self->input_mode = LISCR_ARGS_INPUT_TABLE;
		self->input_table = self->args_start;
	}
}

void
liscr_args_init_getter (LIScrArgs*  self,
                        lua_State*  lua,
                        LIScrClass* clss,
                        LIScrData*  data)
{
	memset (self, 0, sizeof (LIScrArgs));
	self->lua = lua;
	self->script = clss->script;
	self->clss = clss;
	self->data = data;
	if (data != NULL)
		self->self = data->data;
	self->args_start = 2;
	self->args_count = 0;
}

void
liscr_args_init_setter (LIScrArgs*  self,
                        lua_State*  lua,
                        LIScrClass* clss,
                        LIScrData*  data)
{
	memset (self, 0, sizeof (LIScrArgs));
	self->lua = lua;
	self->script = clss->script;
	self->clss = clss;
	self->data = data;
	if (data != NULL)
		self->self = data->data;
	self->args_start = 3;
	self->args_count = lua_gettop (lua) - 2;
	if (!liscr_isanyclass (lua, self->args_start) &&
	    lua_type (self->lua, self->args_start) == LUA_TTABLE)
	{
		self->input_mode = LISCR_ARGS_INPUT_TABLE;
		self->input_table = self->args_start;
	}
}

void
liscr_args_call_setters (LIScrArgs* self,
                         LIScrData* data)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
		liscr_copyargs (self->lua, data, self->input_table);
}

void
liscr_args_call_setters_except (LIScrArgs*  self,
                                LIScrData*  data,
                                const char* except)
{
	const char* tmp;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		/* Get write indexer. */
		liscr_pushdata (self->lua, data);
		lua_getfield (self->lua, -1, "__newindex");
		if (lua_type (self->lua, -1) != LUA_TFUNCTION)
		{
			lua_pop (self->lua, 2);
			lisys_assert (0);
			return;
		}

		/* Call it for each table value. */
		lua_pushnil (self->lua);
		while (lua_next (self->lua, self->input_table) != 0)
		{
			tmp = lua_tostring (self->lua, -2);
			if (tmp != NULL && !strcmp (tmp, except))
			{
				lua_pop (self->lua, 1);
				continue;
			}
			lua_pushvalue (self->lua, -3);
			lua_pushvalue (self->lua, -5);
			lua_pushvalue (self->lua, -4);
			lua_pushvalue (self->lua, -4);
			if (lua_pcall (self->lua, 3, 0, 0))
			{
				lisys_error_set (EINVAL, lua_tostring (self->lua, -1));
				lisys_error_report ();
				lua_pop (self->lua, 1);
			}
			lua_pop (self->lua, 1);
		}
		lua_pop (self->lua, 2);
	}
}

int liscr_args_geti_bool (
	LIScrArgs*  self,
	int         index,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, index);
			ret = 1;
		}
	}

	return ret;
}

int liscr_args_geti_bool_convert (
	LIScrArgs*  self,
	int         index,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		*result = (int) lua_toboolean (self->lua, -1);
		ret = 1;
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		*result = (int) lua_toboolean (self->lua, index);
		ret = 1;
	}

	return ret;
}

int
liscr_args_geti_class (LIScrArgs*   self,
                       int          index,
                       const char*  type,
                       LIScrClass** result)
{
	LIScrClass* tmp;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (type != NULL)
			tmp = liscr_isclass (self->lua, -1, type);
		else
			tmp = liscr_isanyclass (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (type != NULL)
			tmp = liscr_isclass (self->lua, index, type);
		else
			tmp = liscr_isanyclass (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int
liscr_args_geti_data (LIScrArgs*  self,
                      int         index,
                      const char* type,
                      LIScrData** result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (type != NULL)
			tmp = liscr_isdata (self->lua, -1, type);
		else
			tmp = liscr_isanydata (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (type != NULL)
			tmp = liscr_isdata (self->lua, index, type);
		else
			tmp = liscr_isanydata (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int
liscr_args_geti_float (LIScrArgs*  self,
                       int         index,
                       float*      result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_isnumber (self->lua, -1))
		{
			*result = lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_isnumber (self->lua, index))
		{
			*result = lua_tonumber (self->lua, index);
			ret = 1;
		}
	}

	return ret;
}

int
liscr_args_geti_int (LIScrArgs*  self,
                     int         index,
                     int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_isnumber (self->lua, -1))
		{
			*result = (int) lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_isnumber (self->lua, index))
		{
			*result = (int) lua_tonumber (self->lua, index);
			ret = 1;
		}
	}

	return ret;
}

int liscr_args_geti_intv (
	LIScrArgs* self,
	int        index,
	int        count,
	int*       result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, index);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
	}

	return ret;
}

int
liscr_args_geti_quaternion (LIScrArgs*       self,
                            int              index,
                            LIMatQuaternion* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_QUATERNION);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_geti_string (LIScrArgs*   self,
                        int          index,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

/**
 * \brief Gets a table by index and pushes it to the stack.
 * \param self Arguments.
 * \param index Argument index.
 * \return Nonzero on success.
 */
int liscr_args_geti_table (
	LIScrArgs* self,
	int        index)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
			return 1;
		lua_pop (self->lua, 1);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		if (lua_type (self->lua, index) == LUA_TTABLE)
		{
			lua_pushvalue (self->lua, index);
			return 1;
		}
	}

	return 0;
}

int
liscr_args_geti_vector (LIScrArgs*   self,
                        int          index,
                        LIMatVector* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_VECTOR);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_gets_bool (LIScrArgs*  self,
                      const char* name,
                      int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TBOOLEAN)
		{
			*result = (int) lua_toboolean (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int
liscr_args_gets_class (LIScrArgs*   self,
                       const char*  name,
                       const char*  type,
                       LIScrClass** result)
{
	LIScrClass* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (type != NULL)
			tmp = liscr_isclass (self->lua, -1, type);
		else
			tmp = liscr_isanyclass (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int
liscr_args_gets_data (LIScrArgs*  self,
                      const char* name,
                      const char* type,
                      LIScrData** result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (type != NULL)
			tmp = liscr_isdata (self->lua, -1, type);
		else
			tmp = liscr_isanydata (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int liscr_args_gets_float (
	LIScrArgs*  self,
	const char* name,
	float*      result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_isnumber (self->lua, -1))
		{
			*result = lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_floatv (
	LIScrArgs*  self,
	const char* name,
	int         count,
	float*      result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_int (
	LIScrArgs*  self,
	const char* name,
	int*        result)
{
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_isnumber (self->lua, -1))
		{
			*result = (int) lua_tonumber (self->lua, -1);
			ret = 1;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int liscr_args_gets_intv (
	LIScrArgs*  self,
	const char* name,
	int         count,
	int*        result)
{
	int i;
	int ret = 0;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
		{
			for (i = 0 ; i < count ; i++)
			{
				lua_pushnumber (self->lua, i + 1);
				lua_gettable (self->lua, -2);
				if (!lua_isnumber (self->lua, -1))
				{
					lua_pop (self->lua, 1);
					break;
				}
				result[i] = lua_tonumber (self->lua, -1);
				lua_pop (self->lua, 1);
			}
			ret = i;
		}
		lua_pop (self->lua, 1);
	}

	return ret;
}

int
liscr_args_gets_quaternion (LIScrArgs*       self,
                            const char*      name,
                            LIMatQuaternion* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatQuaternion*) tmp->data);
	}

	return tmp != NULL;
}

int
liscr_args_gets_string (LIScrArgs*   self,
                        const char*  name,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TSTRING)
			tmp = lua_tostring (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

/**
 * \brief Gets a table by name and pushes it to the stack.
 * \param self Arguments.
 * \param name Argument name.
 * \return Nonzero on success.
 */
int liscr_args_gets_table (
	LIScrArgs*  self,
	const char* name)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		if (lua_type (self->lua, -1) == LUA_TTABLE)
			return 1;
		lua_pop (self->lua, 1);
	}

	return 0;
}

int
liscr_args_gets_vector (LIScrArgs*   self,
                        const char*  name,
                        LIMatVector* result)
{
	LIScrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((LIMatVector*) tmp->data);
	}

	return tmp != NULL;
}

void
liscr_args_set_output (LIScrArgs* self,
                       int        value)
{
	lisys_assert (self->ret == 0);
	self->output_mode = value;

	if (value == LISCR_ARGS_OUTPUT_TABLE_FORCE)
	{
		lua_newtable (self->lua);
		self->output_table = lua_gettop (self->lua);
		self->output_mode = LISCR_ARGS_OUTPUT_TABLE;
	}
	else
		self->output_mode = value;
}

void
liscr_args_setf_data (LIScrArgs* self,
                      double     name,
                      LIScrData* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, name);
			liscr_pushdata (self->lua, value);
			lua_settable (self->lua, self->output_table);
		}
	}
}

void
liscr_args_setf_float (LIScrArgs* self,
                       double     name,
                       double     value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, name);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_setf_string (LIScrArgs*  self,
                        double      name,
                        const char* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, name);
		lua_pushstring (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_bool (LIScrArgs* self,
                      int        value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushboolean (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushboolean (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_class (LIScrArgs*  self,
                       LIScrClass* value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		liscr_pushclass (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		liscr_pushclass (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_data (LIScrArgs* self,
                      LIScrData* value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		if (value != NULL)
			liscr_pushdata (self->lua, value);
		else
			lua_pushnil (self->lua);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			liscr_pushdata (self->lua, value);
			lua_settable (self->lua, self->output_table);
		}
	}
}

void
liscr_args_seti_float (LIScrArgs* self,
                       float      value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnumber (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void
liscr_args_seti_int (LIScrArgs* self,
                     int        value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnumber (self->lua, value);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, ++self->ret);
		lua_pushnumber (self->lua, value);
		lua_settable (self->lua, self->output_table);
	}
}

void liscr_args_seti_nil (
	LIScrArgs* self)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		lua_pushnil (self->lua);
		self->ret++;
	}
	else
		self->ret++;
}

void
liscr_args_seti_quaternion (LIScrArgs*             self,
                            const LIMatQuaternion* value)
{
	LIScrData* quat;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		quat = liscr_quaternion_new (self->script, value);
		if (quat != NULL)
		{
			liscr_pushdata (self->lua, quat);
			liscr_data_unref (quat);
			self->ret++;
		}
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		quat = liscr_quaternion_new (self->script, value);
		if (quat != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			liscr_pushdata (self->lua, quat);
			lua_settable (self->lua, self->output_table);
			liscr_data_unref (quat);
		}
	}
}

void
liscr_args_seti_stack (LIScrArgs* self)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushvalue (self->lua, -3);
			lua_remove (self->lua, -4);
			self->output_table = lua_gettop (self->lua) - 2;
			lua_settable (self->lua, self->output_table);
		}
		else
		{
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushvalue (self->lua, -2);
			lua_settable (self->lua, self->output_table);
			lua_pop (self->lua, 1);
		}
	}
}

void
liscr_args_seti_string (LIScrArgs*  self,
                        const char* value)
{
	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		if (value != NULL)
			lua_pushstring (self->lua, value);
		else
			lua_pushnil (self->lua);
		self->ret++;
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			lua_pushstring (self->lua, value);
			lua_settable (self->lua, self->output_table);
		}
	}
}

void
liscr_args_seti_vector (LIScrArgs*         self,
                        const LIMatVector* value)
{
	LIScrData* vector;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		vector = liscr_vector_new (self->script, value);
		if (vector != NULL)
		{
			liscr_pushdata (self->lua, vector);
			liscr_data_unref (vector);
			self->ret++;
		}
	}
	else
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		vector = liscr_vector_new (self->script, value);
		if (vector != NULL)
		{
			lua_pushnumber (self->lua, ++self->ret);
			liscr_pushdata (self->lua, vector);
			lua_settable (self->lua, self->output_table);
			liscr_data_unref (vector);
		}
	}
}

void
liscr_args_sets_bool (LIScrArgs*  self,
                      const char* name,
                      int         value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushboolean (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_class (LIScrArgs*  self,
                       const char* name,
                       LIScrClass* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			liscr_pushclass (self->lua, value);
			lua_setfield (self->lua, self->output_table, name);
		}
	}
}

void
liscr_args_sets_data (LIScrArgs*  self,
                      const char* name,
                      LIScrData*  value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		if (value != NULL)
		{
			liscr_pushdata (self->lua, value);
			lua_setfield (self->lua, self->output_table, name);
		}
	}
}

void
liscr_args_sets_float (LIScrArgs*  self,
                       const char* name,
                       float       value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_int (LIScrArgs*  self,
                     const char* name,
                     int         value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushnumber (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_quaternion (LIScrArgs*             self,
                            const char*            name,
                            const LIMatQuaternion* value)
{
	LIScrData* quat;

	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		quat = liscr_quaternion_new (self->script, value);
		if (quat != NULL)
		{
			liscr_pushdata (self->lua, quat);
			lua_setfield (self->lua, self->output_table, name);
			liscr_data_unref (quat);
		}
	}
}

void
liscr_args_sets_stack (LIScrArgs*  self,
                       const char* name)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			lua_pushvalue (self->lua, -2);
			lua_remove (self->lua, -3);
			self->output_table = lua_gettop (self->lua) - 1;
		}
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_string (LIScrArgs*  self,
                        const char* name,
                        const char* value)
{
	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		lua_pushstring (self->lua, value);
		lua_setfield (self->lua, self->output_table, name);
	}
}

void
liscr_args_sets_vector (LIScrArgs*         self,
                        const char*        name,
                        const LIMatVector* value)
{
	LIScrData* vector;

	if (self->output_mode == LISCR_ARGS_OUTPUT_TABLE)
	{
		if (!self->output_table)
		{
			lua_newtable (self->lua);
			self->output_table = lua_gettop (self->lua);
		}
		vector = liscr_vector_new (self->script, value);
		if (vector != NULL)
		{
			liscr_pushdata (self->lua, vector);
			lua_setfield (self->lua, self->output_table, name);
			liscr_data_unref (vector);
		}
	}
}

/*****************************************************************************/

int
liscr_marshal_CLASS (lua_State* lua)
{
	LIScrArgs args;
	LIScrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(LIScrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));

	clss = liscr_isclass (lua, 1, clss->name);
	if (clss == NULL)
		return 0;
	liscr_args_init_func (&args, lua, clss, NULL);
	func (&args);

	if (args.output_table)
		return 1;
	else
		return args.ret;
}

int
liscr_marshal_DATA (lua_State* lua)
{
	LIScrArgs args;
	LIScrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(LIScrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));
	LIScrData* data;

	data = liscr_isdata (lua, 1, clss->name);
	if (data == NULL)
		return 0;
	liscr_args_init_func (&args, lua, data->clss, data);
	func (&args);

	if (args.output_table)
		return 1;
	else
		return args.ret;
}

/** @} */
/** @} */
