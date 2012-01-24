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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtGraphics Graphics
 * @{
 */

#include "ext-module.h"

static void Program_measure_text (LIScrArgs* args)
{
	int width_limit = -1;
	int width;
	int height;
	const char* font_name;
	const char* text;
	LIExtGraphics* module;

	if (liscr_args_geti_string (args, 0, &font_name) &&
	    liscr_args_geti_string (args, 1, &text))
	{
		liscr_args_geti_int (args, 2, &width_limit);
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
		if (liren_render_measure_text (module->render, font_name, text, width_limit, &width, &height))
		{
			liscr_args_seti_int (args, width);
			liscr_args_seti_int (args, height);
		}
	}
}

static void Program_screenshot (LIScrArgs* args)
{
	char* home;
	char* file;
	char* path;
	LIExtGraphics* module;

	/* Construct file path. */
	home = lisys_paths_get_home ();
	file = lisys_string_format ("screenshot-%d.bmp", (int) time (NULL));
	if (home == NULL || file == NULL)
	{
		lisys_free (home);
		lisys_free (file);
		return;
	}
	path = lisys_path_concat (home, file, NULL);
	lisys_free (home);
	if (path == NULL)
	{
		lisys_free (file);
		return;
	}

	/* Capture the screen. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	if (liren_render_screenshot (module->render, path))
		liscr_args_seti_string (args, file);
	lisys_free (path);
	lisys_free (file);
}

static void Program_get_fps (LIScrArgs* args)
{
	LIExtGraphics* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	liscr_args_seti_float (args, module->program->fps);
}

static void Program_get_opengl_version (LIScrArgs* args)
{
	float v;
	LIExtGraphics* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	v = liren_render_get_opengl_version (module->render);
	liscr_args_seti_float (args, v);
}

static void Program_set_title (LIScrArgs* args)
{
	const char* value;
	LIExtGraphics* module;

	if (liscr_args_geti_string (args, 0, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
		liren_render_set_title (module->render, value);
	}
}

static void Program_get_video_mode (LIScrArgs* args)
{
	LIExtGraphics* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	liscr_args_seti_int (args, module->mode.width);
	liscr_args_seti_int (args, module->mode.height);
	liscr_args_seti_bool (args, module->mode.fullscreen);
	liscr_args_seti_bool (args, module->mode.sync);
}

static void Program_set_video_mode (LIScrArgs* args)
{
	int width = 1024;
	int height = 768;
	int fullscreen = 0;
	int vsync = 0;
	LIExtGraphics* module;

	if (!liscr_args_gets_int (args, "width", &width))
		liscr_args_geti_int (args, 0, &width);
	if (!liscr_args_gets_int (args, "height", &height))
		liscr_args_geti_int (args, 1, &height);
	if (!liscr_args_gets_bool (args, "fullscreen", &fullscreen))
		liscr_args_geti_bool (args, 2, &fullscreen);
	if (!liscr_args_gets_bool (args, "vsync", &vsync))
		liscr_args_geti_bool (args, 3, &vsync);
	width = LIMAT_MAX (320, width);
	height = LIMAT_MAX (240, height);

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	if (!liext_graphics_set_videomode (module, width, height, fullscreen, vsync))
	{
		lisys_error_report ();
		return;
	}
	liscr_args_seti_bool (args, 1);
}

static void Program_get_video_modes (LIScrArgs* args)
{
	int i;
	int num;
	LIExtGraphics* module;
	LIRenVideomode* modes;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_GRAPHICS);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	if (liren_render_get_videomodes (module->render, &modes, &num))
	{
		for (i = 0 ; i < num ; i++)
		{
			lua_newtable (args->lua);
			lua_pushnumber (args->lua, 1);
			lua_pushnumber (args->lua, modes[i].width);
			lua_settable (args->lua, -3);
			lua_pushnumber (args->lua, 2);
			lua_pushnumber (args->lua, modes[i].height);
			lua_settable (args->lua, -3);
			liscr_args_seti_stack (args);
		}
		lisys_free (modes);
	}
}

/*****************************************************************************/

void liext_script_graphics (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_measure_text", Program_measure_text);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_screenshot", Program_screenshot);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_set_video_mode", Program_set_video_mode);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_fps", Program_get_fps);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_opengl_version", Program_get_opengl_version);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_set_title", Program_set_title);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_video_mode", Program_get_video_mode);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_PROGRAM, "program_get_video_modes", Program_get_video_modes);
}

/** @} */
/** @} */
