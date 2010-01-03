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
 * \addtogroup LIScrArgs Args
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

struct _LIScrArgs
{
	lua_State* lua;
	LIScrScript* script;
	LIScrClass* clss;
	LIScrData* data;
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
liscr_args_init_func (LIScrArgs*  self,
                      lua_State*  lua,
                      LIScrClass* clss,
                      LIScrData*  data);

void
liscr_args_init_getter (LIScrArgs*  self,
                        lua_State*  lua,
                        LIScrClass* clss,
                        LIScrData*  data);

void
liscr_args_init_setter (LIScrArgs*  self,
                        lua_State*  lua,
                        LIScrClass* clss,
                        LIScrData*  data);

void
liscr_args_call_setters (LIScrArgs* self,
                         LIScrData* data);

void
liscr_args_call_setters_except (LIScrArgs*  self,
                                LIScrData*  data,
                                const char* except);

int
liscr_args_geti_bool (LIScrArgs*  self,
                      int         index,
                      int*        result);

int
liscr_args_geti_class (LIScrArgs*   self,
                       int          index,
                       const char*  type,
                       LIScrClass** result);

int
liscr_args_geti_data (LIScrArgs*  self,
                      int         index,
                      const char* type,
                      LIScrData** result);

int
liscr_args_geti_float (LIScrArgs*  self,
                       int         index,
                       float*      result);

int
liscr_args_geti_int (LIScrArgs*  self,
                     int         index,
                     int*        result);

int
liscr_args_geti_quaternion (LIScrArgs*       self,
                            int              index,
                            LIMatQuaternion* result);

int
liscr_args_geti_string (LIScrArgs*   self,
                        int          index,
                        const char** result);

int
liscr_args_geti_vector (LIScrArgs*   self,
                        int          index,
                        LIMatVector* result);

int
liscr_args_gets_bool (LIScrArgs*  self,
                      const char* name,
                      int*        result);

int
liscr_args_gets_class (LIScrArgs*   self,
                       const char*  name,
                       const char*  type,
                       LIScrClass** result);

int
liscr_args_gets_data (LIScrArgs*  self,
                      const char* name,
                      const char* type,
                      LIScrData** result);

int
liscr_args_gets_float (LIScrArgs*  self,
                       const char* name,
                       float*      result);

int
liscr_args_gets_floatv (LIScrArgs*  self,
                        const char* name,
                        int         count,
                        float*      result);

int
liscr_args_gets_int (LIScrArgs*  self,
                     const char* name,
                     int*        result);

int
liscr_args_gets_quaternion (LIScrArgs*       self,
                            const char*      name,
                            LIMatQuaternion* result);

int
liscr_args_gets_string (LIScrArgs*   self,
                        const char*  name,
                        const char** result);

int
liscr_args_gets_vector (LIScrArgs*   self,
                        const char*  name,
                        LIMatVector* result);

void
liscr_args_set_output (LIScrArgs* self,
                       int        value);

void
liscr_args_seti_bool (LIScrArgs* self,
                      int        value);

void
liscr_args_seti_class (LIScrArgs*  self,
                       LIScrClass* value);

void
liscr_args_seti_data (LIScrArgs* self,
                      LIScrData* value);

void
liscr_args_seti_float (LIScrArgs* self,
                       float      value);

void
liscr_args_seti_int (LIScrArgs* self,
                     int        value);

void
liscr_args_seti_quaternion (LIScrArgs*             self,
                            const LIMatQuaternion* value);

void
liscr_args_seti_stack (LIScrArgs* self);

void
liscr_args_seti_string (LIScrArgs*  self,
                        const char* value);

void
liscr_args_seti_vector (LIScrArgs*         self,
                        const LIMatVector* value);

void
liscr_args_sets_bool (LIScrArgs*  self,
                      const char* name,
                      int         value);

void
liscr_args_sets_class (LIScrArgs*  self,
                       const char* name,
                       LIScrClass* value);

void
liscr_args_sets_data (LIScrArgs*  self,
                      const char* name,
                      LIScrData*  value);

void
liscr_args_sets_float (LIScrArgs*  self,
                       const char* name,
                       float       value);

void
liscr_args_sets_int (LIScrArgs*  self,
                     const char* name,
                     int         value);

void
liscr_args_sets_quaternion (LIScrArgs*             self,
                            const char*            name,
                            const LIMatQuaternion* value);

void
liscr_args_sets_stack (LIScrArgs*  self,
                       const char* name);

void
liscr_args_sets_string (LIScrArgs*  self,
                        const char* name,
                        const char* value);

void
liscr_args_sets_vector (LIScrArgs*         self,
                        const char*        name,
                        const LIMatVector* value);

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
