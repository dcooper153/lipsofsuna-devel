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
	int feedback = 0;
	const char* name = "forward-default";
	const char* fragment = "void main()\n{\ngl_FragColor = vec4(1.0,1.0,1.0,1.0);\n}";
	const char* geometry = NULL;
	const char* vertex = "void main()\n{\ngl_Position=vec4(LOS_coord,1.0);\n}";
	LIExtModule* module;
	LIScrData* data;
	LIRenShader* shader;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SHADER);
	liscr_args_gets_string (args, "fragment", &fragment);
	liscr_args_gets_string (args, "geometry", &geometry);
	liscr_args_gets_string (args, "name", &name);
	liscr_args_gets_string (args, "vertex", &vertex);
	liscr_args_gets_bool (args, "transform_feedback", &feedback);

	/* Avoid duplicate names. */
	shader = lialg_strdic_find (module->client->render->shaders, name);
	if (shader != NULL)
		return;

	/* Allocate self. */
	shader = liren_shader_new (module->client->render, name, vertex, geometry, fragment, feedback);
	if (shader == NULL)
	{
		lisys_error_report ();
		return;
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
	int feedback = 0;
	const char* fragment = "void main()\n{\ngl_FragColor = vec4(1.0,1.0,1.0,1.0);\n}";
	const char* geometry = NULL;
	const char* vertex = "void main()\n{\ngl_Position=vec4(LOS_coord,1.0);\n}";

	liscr_args_gets_string (args, "fragment", &fragment);
	liscr_args_gets_string (args, "geometry", &geometry);
	liscr_args_gets_string (args, "vertex", &vertex);
	liscr_args_gets_bool (args, "transform_feedback", &feedback);
	if (!liren_shader_compile (args->self, vertex, geometry, fragment, feedback))
		lisys_error_report ();
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
