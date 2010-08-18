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
 * module "Extension.Render"
 * ---
 * -- Specify shaders.
 * -- @name Scene
 * -- @class table
 */

/* @luadoc
 * --- Creates a new shader.
 * --
 * -- @param clss Shader class.
 * -- @param args Arguments.<ul>
 * --   <li>config: Shader configuration code.</li>
 * --   <li>fragment: Fragment program code.</li>
 * --   <li>name: Unique shader name.</li>
 * --   <li>vertex: Vertex program code.</li></ul>
 * function Shader.new(clss, args)
 */
static void Shader_new (LIScrArgs* args)
{
	const char* name = "forward-default";
	const char* config = "";
	const char* fragment = "void main()\n{\ngl_FragColor = vec4(1.0,1.0,1.0,1.0);\n}";
	const char* vertex = "void main()\n{\ngl_Position=ftransform();\n}";
	LIExtModule* module;
	LIRenShader* shader;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SHADER);
	liscr_args_gets_string (args, "config", &config);
	liscr_args_gets_string (args, "fragment", &fragment);
	liscr_args_gets_string (args, "name", &name);
	liscr_args_gets_string (args, "vertex", &vertex);

	/* Avoid duplicate names. */
	shader = lialg_strdic_find (module->client->render->shaders, name);
	if (shader != NULL)
		return;

	/* Allocate self. */
	shader = liren_shader_new (module->client->render, name, config, vertex, fragment);
	if (shader == NULL)
	{
		lisys_error_report ();
		return;
	}

	/* Insert to dictionary. */
	if (!lialg_strdic_insert (module->client->render->shaders, name, shader))
	{
		liren_shader_free (shader);
		return;
	}
}

/*****************************************************************************/

void liext_script_shader (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, liscr_script_data, data);
	liscr_class_set_userdata (self, LIEXT_SCRIPT_SHADER, data);
	liscr_class_insert_cfunc (self, "new", Shader_new);
}

/** @} */
/** @} */
