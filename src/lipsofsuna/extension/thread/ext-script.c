/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtThread Thread
 * @{
 */

#include "ext-module.h"

static void Thread_new (LIScrArgs* args)
{
	const char* args_ = "";
	const char* file = "main.lua";
	const char* code = NULL;
	LIExtModule* module;

	/* Read arguments. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_THREAD);
	if (!liscr_args_geti_string (args, 0, &file))
	    liscr_args_gets_string (args, "file", &file);
	if (!liscr_args_geti_string (args, 1, &args_))
	    liscr_args_gets_string (args, "args", &args_);
	if (!liscr_args_geti_string (args, 2, &code))
	    liscr_args_gets_string (args, "code", &code);

	/* Create the thread to the stack. */
	if (liext_thread_inst_new (module->program, args->lua, file, code, args_) != NULL)
		liscr_args_seti_stack (args);
}

static void Thread_set_quit (LIScrArgs* args)
{
	liext_thread_inst_set_quit (args->self);
}

/*****************************************************************************/

void liext_script_thread (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_THREAD, "thread_new", Thread_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_THREAD, "thread_set_quit", Thread_set_quit);
}

/** @} */
/** @} */
