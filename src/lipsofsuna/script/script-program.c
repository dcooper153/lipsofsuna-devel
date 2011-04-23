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
 * \addtogroup LIScrPath Path
 * @{
 */

#include <lipsofsuna/main.h>
#include <lipsofsuna/script.h>

/* @luadoc
 * module "builtin/program"
 * --- Manipulate program state.
 * -- @name Program
 * -- @class table
 */

/* @luadoc
 * --- Sets the name of the mod to be executed after this one quits.
 * -- @param clss Program class.
 * -- @param args Arguments.<ul>
 * --  <li>1,name: Module name.</li></ul>
 * --  <li>1,args: Argument string to pass to the module.</li></ul>
 * function Program.launch_mod(clss, args)
 */
static void Program_launch_mod (LIScrArgs* args)
{
	const char* name;
	LIMaiProgram* program;

	/* Clear the old module name. */
	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	lisys_free (program->launch_name);
	lisys_free (program->launch_args);
	program->launch_name = NULL;
	program->launch_args = NULL;

	/* Set the new module name, if any given. */
	if (liscr_args_gets_string (args, "name", &name) ||
	    liscr_args_geti_string (args, 0, &name))
	{
		program->launch_name = listr_dup (name);
		if (liscr_args_gets_string (args, "args", &name) ||
		    liscr_args_geti_string (args, 1, &name))
		{
			program->launch_args = listr_dup (name);
		}
	}
}

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
 * --- Unloads a sector.<br/>
 * -- Unrealizes all objects in the sector and clears the terrain in the sector.
 * -- The sector is then removed from the sector list.
 * -- @param clss Program class.
 * -- @param args Arguments.<ul>
 * --   <li>sector: Sector index.</li></ul>
 * function Program.unload_sector(clss, args)
 */
static void Program_unload_sector (LIScrArgs* args)
{
	int sector;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_gets_int (args, "sector", &sector))
		lialg_sectors_remove (program->sectors, sector);
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
 * --- The argument string passed to the program at startup time.
 * -- @name Program.args
 * -- @class table
 */
static void Program_get_args (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_string (args, program->args);
}

/* @luadoc
 * --- Boolean indicating whether the game needs to exit.
 * -- @name Program.quit
 * -- @class table
 */
static void Program_get_quit (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_bool (args, program->quit);
}
static void Program_set_quit (LIScrArgs* args)
{
	int value;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_bool (args, 0, &value) && value)
		program->quit = value;
}

/* @luadoc
 * --- Dictionary of indices of active sectors.
 * -- @name Program.sectors
 * -- @class table
 */
static void Program_get_sectors (LIScrArgs* args)
{
	int idle;
	LIAlgSector* sector;
	LIAlgU32dicIter iter;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, program->sectors->sectors)
	{
		sector = iter.value;
		idle = (int) time (NULL) - sector->stamp;
		liscr_args_setf_float (args, iter.key, idle);
	}
}

/* @luadoc
 * --- Sleep time between ticks, in seconds.
 * -- @name Program.quit
 * -- @class table
 */
static void Program_get_sleep (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->sleep / 1000000.0f);
}
static void Program_set_sleep (LIScrArgs* args)
{
	float value;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		program->sleep = (int)(1000000 * value);
}

/* @luadoc
 * --- Short term average tick length in seconds.
 * -- @name Program.tick
 * -- @class table
 */
static void Program_get_tick (LIScrArgs* args)
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
static void Program_get_time (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, limai_program_get_time (program));
}

/*****************************************************************************/

void liscr_script_program (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_PROGRAM, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "launch_mod", Program_launch_mod);
	liscr_class_insert_cfunc (self, "load_extension", Program_load_extension);
	liscr_class_insert_cfunc (self, "unload_sector", Program_unload_sector);
	liscr_class_insert_cfunc (self, "unload_world", Program_unload_world);
	liscr_class_insert_cfunc (self, "shutdown", Program_shutdown);
	liscr_class_insert_cfunc (self, "update", Program_update);
	liscr_class_insert_cfunc (self, "get_args", Program_get_args);
	liscr_class_insert_cfunc (self, "get_quit", Program_get_quit);
	liscr_class_insert_cfunc (self, "set_quit", Program_set_quit);
	liscr_class_insert_cfunc (self, "get_sectors", Program_get_sectors);
	liscr_class_insert_cfunc (self, "get_sleep", Program_get_sleep);
	liscr_class_insert_cfunc (self, "set_sleep", Program_set_sleep);
	liscr_class_insert_cfunc (self, "get_tick", Program_get_tick);
	liscr_class_insert_cfunc (self, "get_time", Program_get_time);
}

/** @} */
/** @} */
