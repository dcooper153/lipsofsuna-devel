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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtWidgets Widgets
 * @{
 */

#include <lipsofsuna/render.h>
#include "ext-module.h"

static const char* default_fragment_shader = "void main()\n{\ngl_FragColor = vec4(1.0,1.0,1.0,1.0);\n}";
static const char* default_vertex_shader = "void main()\n{\ngl_Position=vec4(LOS_coord,1.0);\n}";
static const char* shader_pass_fields[3 * LIREN_SHADER_PASS_COUNT] =
{
	"deferred_pass1_fragment",
	"deferred_pass1_geometry",
	"deferred_pass1_vertex",
	"deferred_pass2_fragment",
	"deferred_pass2_geometry",
	"deferred_pass2_vertex",
	"forward_pass1_fragment",
	"forward_pass1_geometry",
	"forward_pass1_vertex",
	"forward_pass2_fragment",
	"forward_pass2_geometry",
	"forward_pass2_vertex",
	"transparent_pass1_fragment",
	"transparent_pass1_geometry",
	"transparent_pass1_vertex",
	"transparent_pass2_fragment",
	"transparent_pass2_geometry",
	"transparent_pass2_vertex"
};

/* @luadoc
 * module "core/render"
 * ---
 * -- Specify shaders.
 * -- @name Shader
 * -- @class table
 */

/* @luadoc
 * --- Creates a new shader.
 * -- @param clss Shader class.
 * -- @param args Arguments.<ul>
 * --   <li>config: Shader configuration code.</li>
 * --   <li>fragment: Fragment program code.</li>
 * --   <li>geometry: Geometry program code.</li>
 * --   <li>name: Unique shader name.</li>
 * --   <li>transform_feedback: True to enable transform feedback.</li>
 * --   <li>vertex: Vertex program code.</li></ul>
 * function Shader.new(clss, args)
 */
static void Shader_new (LIScrArgs* args)
{
	int i;
	int feedback = 0;
	const char* name = "default";
	const char* fragment;
	const char* geometry;
	const char* vertex;
	LIExtModule* module;
	LIScrData* data;
	LIRenShader* shader;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SHADER);
    liscr_args_gets_string (args, "name", &name);
	liscr_args_gets_bool (args, "transform_feedback", &feedback);

	/* Avoid duplicate names. */
	shader = lialg_strdic_find (module->client->render->shaders, name);
	if (shader != NULL)
		return;

	/* Allocate self. */
	shader = liren_shader_new (module->client->render, name);
	if (shader == NULL)
	{
		lisys_error_report ();
		return;
	}

	/* Compile passes. */
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
	{
		fragment = default_fragment_shader;
		geometry = NULL;
		vertex = default_vertex_shader;
		if (liscr_args_gets_string (args, shader_pass_fields[3 * i + 0], &fragment))
		{
			liscr_args_gets_string (args, shader_pass_fields[3 * i + 1], &geometry);
			liscr_args_gets_string (args, shader_pass_fields[3 * i + 2], &vertex);
			if (!liren_shader_compile (shader, i, vertex, geometry, fragment, feedback))
				lisys_error_report ();
		}
	}

	/* Allocate userdata. */
	data = liscr_data_new (args->script, shader, args->clss, liren_shader_free);
	if (data == NULL)
	{
		liren_shader_free (shader);
		return;
	}
	liscr_args_seti_data (args, data);
	liscr_data_unref (data);
}

/* @luadoc
 * --- Recompiles the shader from new source code.
 * -- @param self Shader.
 * -- @param args Arguments.<ul>
 * --   <li>config: Shader configuration code.</li>
 * --   <li>fragment: Fragment program code.</li>
 * --   <li>geometry: Geometry program code.</li>
 * --   <li>transform_feedback: True to enable transform feedback.</li>
 * --   <li>vertex: Vertex program code.</li></ul>
 * function Shader.compile(self, args)
 */
static void Shader_compile (LIScrArgs* args)
{
	int i;
	int feedback = 0;
	const char* fragment;
	const char* geometry;
	const char* vertex;
	LIRenShader* shader = args->self;

	/* Compile passes. */
	liscr_args_gets_bool (args, "transform_feedback", &feedback);
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
	{
		liren_shader_clear_pass (shader, i);
		fragment = default_fragment_shader;
		geometry = NULL;
		vertex = default_vertex_shader;
		if (liscr_args_gets_string (args, shader_pass_fields[3 * i + 0], &fragment))
		{
			liscr_args_gets_string (args, shader_pass_fields[3 * i + 1], &geometry);
			liscr_args_gets_string (args, shader_pass_fields[3 * i + 2], &vertex);
			if (!liren_shader_compile (shader, i, vertex, geometry, fragment, feedback))
				lisys_error_report ();
		}
	}
}

/*****************************************************************************/

void liext_script_shader (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SHADER, data);
	liscr_class_insert_cfunc (self, "new", Shader_new);
	liscr_class_insert_mfunc (self, "compile", Shader_compile);
}

/** @} */
/** @} */
