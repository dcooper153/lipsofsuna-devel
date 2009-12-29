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
 * \addtogroup liscrArgs Args
 * @{
 */

#ifndef __SCRIPT_ARGS_H__
#define __SCRIPT_ARGS_H__

#include "script-types.h"

enum
{
	LISCR_ARGS_INPUT_NORMAL,
	LISCR_ARGS_INPUT_TABLE
};

enum
{
	LISCR_ARGS_OUTPUT_NORMAL,
	LISCR_ARGS_OUTPUT_TABLE
};

struct _liscrArgs
{
	lua_State* lua;
	liscrScript* script;
	liscrClass* clss;
	liscrData* data;
	void* self;
	int ret;
	int args_start;
	int args_count;
	int input_mode;
	int input_table;
	int output_mode;
	int output_table;
};

void
liscr_args_init_func (liscrArgs*  self,
                      lua_State*  lua,
                      liscrClass* clss,
                      liscrData*  data);

void
liscr_args_init_getter (liscrArgs*  self,
                        lua_State*  lua,
                        liscrClass* clss,
                        liscrData*  data);

void
liscr_args_init_setter (liscrArgs*  self,
                        lua_State*  lua,
                        liscrClass* clss,
                        liscrData*  data);

void
liscr_args_call_setters (liscrArgs* self,
                         liscrData* data);

void
liscr_args_call_setters_except (liscrArgs*  self,
                                liscrData*  data,
                                const char* except);

int
liscr_args_geti_bool (liscrArgs*  self,
                      int         index,
                      int*        result);

int
liscr_args_geti_class (liscrArgs*   self,
                       int          index,
                       const char*  type,
                       liscrClass** result);

int
liscr_args_geti_data (liscrArgs*  self,
                      int         index,
                      const char* type,
                      liscrData** result);

int
liscr_args_geti_float (liscrArgs*  self,
                       int         index,
                       float*      result);

int
liscr_args_geti_int (liscrArgs*  self,
                     int         index,
                     int*        result);

int
liscr_args_geti_quaternion (liscrArgs*       self,
                            int              index,
                            limatQuaternion* result);

int
liscr_args_geti_string (liscrArgs*   self,
                        int          index,
                        const char** result);

int
liscr_args_geti_vector (liscrArgs*   self,
                        int          index,
                        limatVector* result);

int
liscr_args_gets_bool (liscrArgs*  self,
                      const char* name,
                      int*        result);

int
liscr_args_gets_class (liscrArgs*   self,
                       const char*  name,
                       const char*  type,
                       liscrClass** result);

int
liscr_args_gets_data (liscrArgs*  self,
                      const char* name,
                      const char* type,
                      liscrData** result);

int
liscr_args_gets_float (liscrArgs*  self,
                       const char* name,
                       float*      result);

int
liscr_args_gets_floatv (liscrArgs*  self,
                        const char* name,
                        int         count,
                        float*      result);

int
liscr_args_gets_int (liscrArgs*  self,
                     const char* name,
                     int*        result);

int
liscr_args_gets_quaternion (liscrArgs*       self,
                            const char*      name,
                            limatQuaternion* result);

int
liscr_args_gets_string (liscrArgs*   self,
                        const char*  name,
                        const char** result);

int
liscr_args_gets_vector (liscrArgs*   self,
                        const char*  name,
                        limatVector* result);

void
liscr_args_set_output (liscrArgs* self,
                       int        value);

void
liscr_args_seti_bool (liscrArgs* self,
                      int        value);

void
liscr_args_seti_class (liscrArgs*  self,
                       liscrClass* value);

void
liscr_args_seti_data (liscrArgs* self,
                      liscrData* value);

void
liscr_args_seti_float (liscrArgs* self,
                       float      value);

void
liscr_args_seti_int (liscrArgs* self,
                     int        value);

void
liscr_args_seti_quaternion (liscrArgs*             self,
                            const limatQuaternion* value);

void
liscr_args_seti_stack (liscrArgs* self);

void
liscr_args_seti_string (liscrArgs*  self,
                        const char* value);

void
liscr_args_seti_vector (liscrArgs*         self,
                        const limatVector* value);

void
liscr_args_sets_bool (liscrArgs*  self,
                      const char* name,
                      int         value);

void
liscr_args_sets_class (liscrArgs*  self,
                       const char* name,
                       liscrClass* value);

void
liscr_args_sets_data (liscrArgs*  self,
                      const char* name,
                      liscrData*  value);

void
liscr_args_sets_float (liscrArgs*  self,
                       const char* name,
                       float       value);

void
liscr_args_sets_int (liscrArgs*  self,
                     const char* name,
                     int         value);

void
liscr_args_sets_quaternion (liscrArgs*             self,
                            const char*            name,
                            const limatQuaternion* value);

void
liscr_args_sets_stack (liscrArgs*  self,
                       const char* name);

void
liscr_args_sets_string (liscrArgs*  self,
                        const char* name,
                        const char* value);

void
liscr_args_sets_vector (liscrArgs*         self,
                        const char*        name,
                        const limatVector* value);

/*****************************************************************************/

int
liscr_marshal_CLASS (lua_State* lua);

int
liscr_marshal_DATA (lua_State* lua);
/*
int
liscr_marshal_BOOL__CLASS_ARGS (lua_State* lua);

int
liscr_marshal_CLASS__CLASS_ARGS (lua_State* lua);

int
liscr_marshal_VOID__CLASS_ARGS (lua_State* lua);

int
liscr_marshal_VOID__DATA_ARGS (lua_State* lua);
*/
#endif

/** @} */
/** @} */
