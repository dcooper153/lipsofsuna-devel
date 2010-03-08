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
 * ---
 * -- Manipulate program state.
 * -- @name Path
 * -- @class table
 */

/* @luadoc
 * ---
 * -- @brief Unloads the world map.
 * --
 * -- Unrealizes all objects and destroys all sectors of the world map.
 * -- You usually want to do this when you're about to create a new map with
 * -- the map generator to avoid parts of the old map being left in the game.
 * --
 * -- @param self Program class.
 * function Program.unload_world(self)
 */
static void Program_unload_world (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	lialg_sectors_clear (program->sectors);
}

/* @luadoc
 * ---
 * -- Request program shutdown.
 * --
 * -- @param self Program class.
 * function Program.shutdown(self)
 */
static void Program_shutdown (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PROGRAM);
	limai_program_shutdown (program);
}

/* @luadoc
 * ---
 * -- Short term average tick length in seconds.
 * --
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
 * ---
 * -- Number of seconds the program has been running.
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
	liscr_class_insert_cfunc (self, "unload_world", Program_unload_world);
	liscr_class_insert_cfunc (self, "shutdown", Program_shutdown);
	liscr_class_insert_cvar (self, "tick", Program_getter_tick, NULL);
	liscr_class_insert_cvar (self, "time", Program_getter_time, NULL);
}

/** @} */
/** @} */
