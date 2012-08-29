/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#include "lipsofsuna/main.h"
#include "lipsofsuna/script.h"

static void Program_add_path (LIScrArgs* args)
{
	const char* name;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_string (args, 0, &name))
		lipth_paths_add_path (program->paths, name);
}

static void Program_collect_garbage (LIScrArgs* args)
{
	float secs;
	LISysTimeval start;
	LISysTimeval time;

	if (liscr_args_geti_float (args, 0, &secs))
	{
		start = lisys_timeval_init ();
		do
		{
			if (lua_gc (args->lua, LUA_GCSTEP, 0))
			{
				liscr_args_seti_bool (args, 1);
				return;
			}
			time = lisys_timeval_init ();
		}
		while (lisys_timeval_get_diff (start, time) < secs);
	}
}

static void Program_debug_dump (LIScrArgs* args)
{
	lisys_memstats (0);
}

static void Program_launch_mod (LIScrArgs* args)
{
	const char* name;
	LIMaiProgram* program;

	/* Clear the old module name. */
	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	lisys_free (program->launch_name);
	lisys_free (program->launch_args);
	program->launch_name = NULL;
	program->launch_args = NULL;

	/* Set the new module name, if any given. */
	if (liscr_args_gets_string (args, "name", &name) ||
	    liscr_args_geti_string (args, 0, &name))
	{
		program->launch_name = lisys_string_dup (name);
		if (liscr_args_gets_string (args, "args", &name) ||
		    liscr_args_geti_string (args, 1, &name))
		{
			program->launch_args = lisys_string_dup (name);
		}
	}
}

static void Program_load_extension (LIScrArgs* args)
{
	int ret;
	const char* name;
	LIMaiProgram* program;

	if (liscr_args_gets_string (args, "name", &name) ||
	    liscr_args_geti_string (args, 0, &name))
	{
		program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
		ret = limai_program_insert_extension (program, name);
		if (!ret)
			lisys_error_report ();
		liscr_args_seti_bool (args, ret);
	}
}

static void Program_pop_message (LIScrArgs* args)
{
	LIMaiMessage* message;
	LIMaiProgram* self;
	LIMdlModel* model;
	LIScrData* data;

	/* Only allowed for child programs. */
	self = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (self->parent == NULL)
		return;

	/* Pop the message. */
	message = limai_program_pop_message (self, LIMAI_MESSAGE_QUEUE_THREAD);
	if (message == NULL)
		return;

	/* Return the message. */
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "name", message->name);
	switch (message->type)
	{
		case LIMAI_MESSAGE_TYPE_EMPTY:
			liscr_args_sets_string (args, "type", "empty");
			break;
		case LIMAI_MESSAGE_TYPE_MODEL:
			liscr_args_sets_string (args, "type", "model");
			model = message->model;
			if (!limdl_manager_add_model (self->models, model))
				break;
			data = liscr_data_new (args->script, args->lua, model, LISCR_SCRIPT_MODEL, limdl_manager_free_model);
			if (data != NULL)
			{
				liscr_args_sets_stack (args, "model");
				message->model = NULL;
			}
			break;
		case LIMAI_MESSAGE_TYPE_STRING:
			liscr_args_sets_string (args, "type", "string");
			liscr_args_sets_string (args, "string", message->string);
			break;
	}

	/* Free the message. */
	limai_message_free (message);
}

static void Program_pump_events (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	limai_program_pump_events (program);
}

static void Program_push_message (LIScrArgs* args)
{
	const char* name = "";
	const char* string = NULL;
	LIMaiProgram* self;
	LIScrData* data;
	LIMdlModel* model;

	/* Only allowed for child programs. */
	self = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (self->parent == NULL)
		return;

	/* Read the name. */
	if (!liscr_args_geti_string (args, 0, &name))
		liscr_args_gets_string (args, "name", &name);

	/* Read and push the data. */
	if (liscr_args_geti_string (args, 1, &string) ||
	    liscr_args_gets_string (args, "string", &string))
	{
		if (limai_program_push_message (self, LIMAI_MESSAGE_QUEUE_PROGRAM, LIMAI_MESSAGE_TYPE_STRING, name, string))
			liscr_args_seti_bool (args, 1);
	}
	else if (liscr_args_geti_data (args, 1, LISCR_SCRIPT_MODEL, &data) ||
	         liscr_args_gets_data (args, "model", LISCR_SCRIPT_MODEL, &data))
	{
		model = liscr_data_get_data (data);
		if (limai_program_push_message (self, LIMAI_MESSAGE_QUEUE_PROGRAM, LIMAI_MESSAGE_TYPE_MODEL, name, model))
			liscr_args_seti_bool (args, 1);
	}
	else
	{
		if (limai_program_push_message (self, LIMAI_MESSAGE_QUEUE_PROGRAM, LIMAI_MESSAGE_TYPE_EMPTY, name, NULL))
			liscr_args_seti_bool (args, 1);
	}
}

static void Program_unittest (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	limai_program_unittest (program);
}

static void Program_unload_sector (LIScrArgs* args)
{
	int sector;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_gets_int (args, "sector", &sector))
		lialg_sectors_remove (program->sectors, sector);
}

static void Program_unload_world (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	lialg_sectors_clear (program->sectors);
}

static void Program_shutdown (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	limai_program_shutdown (program);
}

static void Program_update (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	limai_program_update (program);
}

static void Program_wait (LIScrArgs* args)
{
	float secs = 1.0f;

	liscr_args_geti_float (args, 0, &secs);
	if (secs > 0.0f)
		lisys_usleep ((int)(1000000 * secs));
}

static void Program_get_args (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_string (args, program->args);
}

static void Program_get_fps (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->fps);
}

static void Program_get_quit (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_bool (args, program->quit);
}
static void Program_set_quit (LIScrArgs* args)
{
	int value;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_bool (args, 0, &value) && value)
		program->quit = value;
}

static void Program_get_sectors (LIScrArgs* args)
{
	int idle;
	LIAlgSector* sector;
	LIAlgU32dicIter iter;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, program->sectors->sectors)
	{
		sector = iter.value;
		idle = lisys_time (NULL) - (time_t) sector->stamp;
		liscr_args_setf_float (args, iter.key, idle);
	}
}

static void Program_get_sector_idle (LIScrArgs* args)
{
	int id;
	LIAlgSector* sector;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (!liscr_args_geti_int (args, 0, &id))
		return;
	sector = lialg_u32dic_find (program->sectors->sectors, id);
	liscr_args_seti_int (args, lisys_time (NULL) - (time_t) sector->stamp);
}

static void Program_get_sector_size (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->sectors->width);
}

static void Program_get_sleep (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->sleep / 1000000.0f);
}
static void Program_set_sleep (LIScrArgs* args)
{
	float value;
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		program->sleep = (int)(1000000 * value);
}

static void Program_get_tick (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, program->tick);
}

static void Program_get_time (LIScrArgs* args)
{
	LIMaiProgram* program;

	program = liscr_script_get_userdata (args->script, LISCR_SCRIPT_PROGRAM);
	liscr_args_seti_float (args, limai_program_get_time (program));
}

static void Program_get_version (LIScrArgs* args)
{
	liscr_args_seti_string (args, LI_VERSION_STRING);
}

/*****************************************************************************/

void liscr_script_program (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_add_path", Program_add_path);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_collect_garbage", Program_collect_garbage);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_debug_dump", Program_debug_dump);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_launch_mod", Program_launch_mod);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_load_extension", Program_load_extension);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_pop_message", Program_pop_message);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_pump_events", Program_pump_events);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_push_message", Program_push_message);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_unittest", Program_unittest);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_unload_sector", Program_unload_sector);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_unload_world", Program_unload_world);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_shutdown", Program_shutdown);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_update", Program_update);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_wait", Program_wait);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_args", Program_get_args);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_fps", Program_get_fps);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_quit", Program_get_quit);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_set_quit", Program_set_quit);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_sectors", Program_get_sectors);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_sector_idle", Program_get_sector_idle);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_sector_size", Program_get_sector_size);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_sleep", Program_get_sleep);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_set_sleep", Program_set_sleep);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_tick", Program_get_tick);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_time", Program_get_time);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_version", Program_get_version);
}

/** @} */
/** @} */
