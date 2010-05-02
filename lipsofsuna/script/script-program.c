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
 * \addtogroup liscrPath Path
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/script.h>

/* @luadoc
 * module "Core.Common.Program"
 * --- Manipulate program state.
 * -- @name Program
 * -- @class table
 */

/* @luadoc
 * --- Loads an extension.
 * -- @param clss Program class.
 * -- @param args Arguments.<ul>
 * --  <li>1,name: Extension name.</li></ul>
 * -- @return True on success.
 * function Program.load_extension(clss, args)
 */
static void Program_load_extension (LIScrArgs* args)
{
	int ret;
	const char* name;
	LIMaiProgram* program;

	if (liscr_args_gets_string (args, "name", &name) ||
	    liscr_args_geti_string (args, 0, &name))
	{
		program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
		ret = limai_program_insert_extension (program, name);
		if (!ret)
			lisys_error_report ();
		liscr_args_seti_bool (args, ret);
	}
}

/* @luadoc
 * --- Pops an event from the event queue.
 * -- @param clss Program class.
 * -- @return Event or nil.
 * function Program.pop_event(clss)
 */
static void Program_pop_event (LIScrArgs* args)
{
	LIMaiProgram* program;
	LIScrData* data;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	data = limai_program_pop_event (program);
	if (data != NULL)
	{
		liscr_args_seti_data (args, data);
		liscr_data_unref (data, NULL);
	}
}

/* @luadoc
 * --- Pushes an event to the event queue.
 * -- @param clss Program class.
 * -- @param args Arguments.<ul>
 * --   <li>event: Event. (required)</li></ul>
 * function Program.push_event(clss, args)
 */
static void Program_push_event (LIScrArgs* args)
{
	LIMaiProgram* program;
	LIScrData* event;

	if (liscr_args_gets_data (args, "event", LISCR_SCRIPT_EVENT, &event) ||
	    liscr_args_geti_data (args, 0, LISCR_SCRIPT_EVENT, &event))
	{
		program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
		limai_program_push_event (program, event);
	}
}

/* @luadoc
 * --- Unloads the world map.<br/>
 * -- Unrealizes all objects and destroys all sectors of the world map.
 * -- You usually want to do this when you're about to create a new map with
 * -- the map generator to avoid parts of the old map being left in the game.
 * -- @param clss Program class.
 * function Program.unload_world(clss)
 */
static void Program_unload_world (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	lialg_sectors_clear (program->sectors);
}

/* @luadoc
 * --- Request program shutdown.
 * -- @param clss Program class.
 * function Program.shutdown(clss)
 */
static void Program_shutdown (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	limai_program_shutdown (program);
}

/* @luadoc
 * --- Updates the program state.
 * -- @param clss Program class.
 * function Program.update(clss)
 */
static void Program_update (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	limai_program_update (program);
}

/* @luadoc
 * --- Boolean indicating whether the game needs to exit.
 * -- @name Program.quit
 * -- @class table
 */
static void Program_getter_quit (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_bool (args, program->quit);
}
static void Program_setter_quit (LIScrArgs* args)
{
	int value;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_bool (args, 0, &value) && value)
		program->quit = value;
}

/* @luadoc
 * --- Short term average tick length in seconds.
 * -- @name Program.tick
 * -- @class table
 */
static void Program_getter_tick (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->tick);
}

/* @luadoc
 * --- Number of seconds the program has been running.
 * -- @name Program.time
 * -- @class table
 */
static void Program_getter_time (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, limai_program_get_time (program));
}

/*****************************************************************************/

void
liscr_script_program (LIScrClass* self,
                      void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_PROGRAM, data);
	liscr_class_insert_cfunc (self, "load_extension", Program_load_extension);
	liscr_class_insert_cfunc (self, "pop_event", Program_pop_event);
	liscr_class_insert_cfunc (self, "push_event", Program_push_event);
	liscr_class_insert_cfunc (self, "unload_world", Program_unload_world);
	liscr_class_insert_cfunc (self, "shutdown", Program_shutdown);
	liscr_class_insert_cfunc (self, "update", Program_update);
	liscr_class_insert_cvar (self, "quit", Program_getter_quit, Program_setter_quit);
	liscr_class_insert_cvar (self, "tick", Program_getter_tick, NULL);
	liscr_class_insert_cvar (self, "time", Program_getter_time, NULL);
}

/** @} */
/** @} */
