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
 * \addtogroup liscr Script
 * @{
 * \addtogroup liscrArgs Args
 * @{
 */

#include "script-args.h"
#include "script-library.h"
#include "script-util.h"

void
liscr_args_init_func (liscrArgs*  self,
                      lua_State*  lua,
                      liscrClass* clss,
                      liscrData*  data)
{
	memset (self, 0, sizeof (liscrArgs));
	self->lua = lua;
	self->script = clss->script;
	self->clss = clss;
	self->data = data;
	if (data != NULL)
		self->self = data->data;
	self->args_start = 2;
	self->args_count = lua_gettop (lua) - 1;
	if (lua_type (self->lua, self->args_start) == LUA_TTABLE)
	{
		self->input_mode = LISCR_ARGS_INPUT_TABLE;
		self->input_table = self->args_start;
	}
}

void
liscr_args_init_getter (liscrArgs*  self,
                        lua_State*  lua,
                        liscrClass* clss,
                        liscrData*  data)
{
	memset (self, 0, sizeof (liscrArgs));
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
liscr_args_init_setter (liscrArgs*  self,
                        lua_State*  lua,
                        liscrClass* clss,
                        liscrData*  data)
{
	memset (self, 0, sizeof (liscrArgs));
	self->lua = lua;
	self->script = clss->script;
	self->clss = clss;
	self->data = data;
	if (data != NULL)
		self->self = data->data;
	self->args_start = 3;
	self->args_count = lua_gettop (lua) - 2;
}

void
liscr_args_call_setters (liscrArgs* self,
                         liscrData* data)
{
	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
		liscr_copyargs (self->lua, data, self->input_table);
}

void
liscr_args_call_setters_except (liscrArgs*  self,
                                liscrData*  data,
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
			assert (0);
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
		lua_pop (self->lua, 3);
	}
}

int
liscr_args_geti_bool (liscrArgs*  self,
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

int
liscr_args_geti_class (liscrArgs*   self,
                       int          index,
                       const char*  type,
                       liscrClass** result)
{
	liscrClass* tmp;

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
liscr_args_geti_data (liscrArgs*  self,
                      int         index,
                      const char* type,
                      liscrData** result)
{
	liscrData* tmp = NULL;

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
liscr_args_geti_float (liscrArgs*  self,
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
liscr_args_geti_int (liscrArgs*  self,
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

int
liscr_args_geti_quaternion (liscrArgs*       self,
                            int              index,
                            limatQuaternion* result)
{
	liscrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((limatQuaternion*) tmp);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_QUATERNION);
		if (tmp != NULL)
			*result = *((limatQuaternion*) tmp);
	}

	return tmp != NULL;
}

int
liscr_args_geti_string (liscrArgs*   self,
                        int          index,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
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
		tmp = lua_tostring (self->lua, index);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int
liscr_args_geti_vector (liscrArgs*   self,
                        int          index,
                        limatVector* result)
{
	liscrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_pushnumber (self->lua, index + 1);
		lua_gettable (self->lua, self->input_table);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((limatVector*) tmp);
	}
	else
	{
		if (index < 0 || index >= self->args_count)
			return 0;
		index += self->args_start;
		tmp = liscr_isdata (self->lua, index, LISCR_SCRIPT_VECTOR);
		if (tmp != NULL)
			*result = *((limatVector*) tmp);
	}

	return tmp != NULL;
}

int
liscr_args_gets_bool (liscrArgs*  self,
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
liscr_args_gets_class (liscrArgs*   self,
                       const char*  name,
                       const char*  type,
                       liscrClass** result)
{
	liscrClass* tmp;

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
liscr_args_gets_data (liscrArgs*  self,
                      const char* name,
                      const char* type,
                      liscrData** result)
{
	liscrData* tmp = NULL;

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

int
liscr_args_gets_float (liscrArgs*  self,
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

int
liscr_args_gets_floatv (liscrArgs*  self,
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

int
liscr_args_gets_int (liscrArgs*  self,
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

int
liscr_args_gets_quaternion (liscrArgs*       self,
                            const char*      name,
                            limatQuaternion* result)
{
	liscrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_QUATERNION);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((limatQuaternion*) tmp);
	}

	return tmp != NULL;
}

int
liscr_args_gets_string (liscrArgs*   self,
                        const char*  name,
                        const char** result)
{
	const char* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = lua_tostring (self->lua, -1);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = tmp;
	}

	return tmp != NULL;
}

int
liscr_args_gets_vector (liscrArgs*   self,
                        const char*  name,
                        limatVector* result)
{
	liscrData* tmp = NULL;

	if (self->input_mode == LISCR_ARGS_INPUT_TABLE)
	{
		lua_getfield (self->lua, self->input_table, name);
		tmp = liscr_isdata (self->lua, -1, LISCR_SCRIPT_VECTOR);
		lua_pop (self->lua, 1);
		if (tmp != NULL)
			*result = *((limatVector*) tmp);
	}

	return tmp != NULL;
}

void
liscr_args_set_output (liscrArgs* self,
                       int        value)
{
	assert (self->ret == 0);
	self->output_mode = value;
}

void
liscr_args_seti_bool (liscrArgs* self,
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
liscr_args_seti_class (liscrArgs*  self,
                       liscrClass* value)
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
liscr_args_seti_data (liscrArgs* self,
                      liscrData* value)
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
liscr_args_seti_float (liscrArgs* self,
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
liscr_args_seti_int (liscrArgs* self,
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

void
liscr_args_seti_quaternion (liscrArgs*             self,
                            const limatQuaternion* value)
{
	liscrData* quat;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		quat = liscr_quaternion_new (self->script, value);
		if (quat != NULL)
		{
			liscr_pushdata (self->lua, quat);
			liscr_data_unref (quat, NULL);
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
			liscr_data_unref (quat, NULL);
		}
	}
}

void
liscr_args_seti_stack (liscrArgs* self)
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
liscr_args_seti_string (liscrArgs*  self,
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
liscr_args_seti_vector (liscrArgs*         self,
                        const limatVector* value)
{
	liscrData* vector;

	if (self->output_mode != LISCR_ARGS_OUTPUT_TABLE)
	{
		vector = liscr_vector_new (self->script, value);
		if (vector != NULL)
		{
			liscr_pushdata (self->lua, vector);
			liscr_data_unref (vector, NULL);
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
			liscr_data_unref (vector, NULL);
		}
	}
}

void
liscr_args_sets_bool (liscrArgs*  self,
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
liscr_args_sets_class (liscrArgs*  self,
                       const char* name,
                       liscrClass* value)
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
liscr_args_sets_data (liscrArgs*  self,
                      const char* name,
                      liscrData*  value)
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
liscr_args_sets_float (liscrArgs*  self,
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
liscr_args_sets_int (liscrArgs*  self,
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
liscr_args_sets_quaternion (liscrArgs*             self,
                            const char*            name,
                            const limatQuaternion* value)
{
	liscrData* quat;

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
			liscr_data_unref (quat, NULL);
		}
	}
}

void
liscr_args_sets_stack (liscrArgs*  self,
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
liscr_args_sets_string (liscrArgs*  self,
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
liscr_args_sets_vector (liscrArgs*         self,
                        const char*        name,
                        const limatVector* value)
{
	liscrData* vector;

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
			liscr_data_unref (vector, NULL);
		}
	}
}

/*****************************************************************************/

int
liscr_marshal_CLASS (lua_State* lua)
{
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));

	if (!liscr_isclass (lua, 1, clss->meta))
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
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));
	liscrData* data;

	data = liscr_isdata (lua, 1, clss->meta);
	if (data == NULL)
		return 0;
	liscr_args_init_func (&args, lua, clss, data);
	func (&args);

	if (args.output_table)
		return 1;
	else
		return args.ret;
}

int
liscr_marshal_BOOL__CLASS_ARGS (lua_State* lua)
{
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	int (*func)(liscrClass*, liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));

	if (!liscr_isclass (lua, 1, clss->meta))
		return 0;
	liscr_args_init_func (&args, lua, clss, NULL);
	lua_pushboolean (lua, func (clss, &args));

	return 1;
}

int
liscr_marshal_CLASS__CLASS_ARGS (lua_State* lua)
{
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	liscrClass* (*func)(liscrClass*, liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));

	if (!liscr_isclass (lua, 1, clss->meta))
		return 0;
	liscr_args_init_func (&args, lua, clss, NULL);
	clss = func (clss, &args);
	if (clss == NULL)
		return 0;
	liscr_pushclass (lua, clss);

	return 1;
}

int
liscr_marshal_VOID__CLASS_ARGS (lua_State* lua)
{
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(liscrClass*, liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));

	if (!liscr_isclass (lua, 1, clss->meta))
		return 0;
	liscr_args_init_func (&args, lua, clss, NULL);
	func (clss, &args);

	return 0;
}

int
liscr_marshal_VOID__DATA_ARGS (lua_State* lua)
{
	liscrArgs args;
	liscrClass* clss = lua_touserdata (lua, lua_upvalueindex (1));
	void (*func)(liscrData*, liscrArgs*) = lua_touserdata (lua, lua_upvalueindex (2));
	liscrData* data;

	data = liscr_isdata (lua, 1, clss->meta);
	if (data == NULL)
		return 0;
	liscr_args_init_func (&args, lua, clss, data);
	func (data, &args);

	return 0;
}

/** @} */
/** @} */
