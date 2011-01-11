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

static const char* default_fragment_shader = "void main()\n{\nLOS_output_0 = vec4(1.0,1.0,1.0,1.0);\n}";
static const char* default_vertex_shader = "void main()\n{\ngl_Position=vec4(LOS_coord,1.0);\n}";

static void private_compile (
	LIRenShader* shader,
	LIScrArgs*   args)
{
	int i;
	int tmpbool;
	int feedback = 0;
	int alpha_coverage;
	int blend_enable;
	GLenum blend_src;
	GLenum blend_dst;
	int color_write;
	int depth_test;
	int depth_write;
	GLenum depth_func;
	char field_alpha_coverage[64];
	char field_blend_enable[64];
	char field_blend_src[64];
	char field_blend_dst[64];
	char field_color_write[64];
	char field_depth_test[64];
	char field_depth_write[64];
	char field_depth_func[64];
	char field_fragment[64];
	char field_vertex[64];
	char field_geometry[64];
	const char* tmpstr;
	const char* fragment;
	const char* geometry;
	const char* vertex;

	/* Compile passes. */
	liscr_args_gets_bool (args, "transform_feedback", &feedback);
	for (i = 0 ; i < LIREN_SHADER_PASS_COUNT ; i++)
	{
		fragment = default_fragment_shader;
		geometry = NULL;
		vertex = default_vertex_shader;
		alpha_coverage = 0;
		blend_enable = 0;
		blend_src = GL_SRC_ALPHA;
		blend_dst = GL_ONE_MINUS_SRC_ALPHA;
		color_write = 1;
		depth_test = 1;
		depth_write = 1;
		depth_func = GL_LEQUAL;
		snprintf (field_fragment, sizeof (field_fragment), "pass%d_fragment", i + 1);
		snprintf (field_geometry, sizeof (field_geometry), "pass%d_geometry", i + 1);
		snprintf (field_vertex, sizeof (field_vertex), "pass%d_vertex", i + 1);
		snprintf (field_alpha_coverage, sizeof (field_alpha_coverage), "pass%d_alpha_to_coverage", i + 1);
		snprintf (field_blend_enable, sizeof (field_blend_enable), "pass%d_blend", i + 1);
		snprintf (field_blend_src, sizeof (field_blend_src), "pass%d_blend_src", i + 1);
		snprintf (field_blend_dst, sizeof (field_blend_dst), "pass%d_blend_dst", i + 1);
		snprintf (field_color_write, sizeof (field_color_write), "pass%d_color_write", i + 1);
		snprintf (field_depth_test, sizeof (field_depth_test), "pass%d_depth_test", i + 1);
		snprintf (field_depth_write, sizeof (field_depth_write), "pass%d_depth_write", i + 1);
		snprintf (field_depth_func, sizeof (field_depth_func), "pass%d_depth_func", i + 1);
		if (liscr_args_gets_string (args, field_fragment, &fragment))
		{
			liscr_args_gets_string (args, field_geometry, &geometry);
			liscr_args_gets_string (args, field_vertex, &vertex);
			if (liscr_args_gets_bool (args, field_alpha_coverage, &tmpbool))
				alpha_coverage = tmpbool;
			if (liscr_args_gets_bool (args, field_blend_enable, &tmpbool))
				blend_enable = tmpbool;
			if (liscr_args_gets_string (args, field_blend_src, &tmpstr))
			{
				if (!strcmp (tmpstr, "one"))
					blend_src = GL_ONE;
				else if (!strcmp (tmpstr, "one_minus_src_alpha"))
					blend_src = GL_ONE_MINUS_SRC_ALPHA;
				else if (!strcmp (tmpstr, "src_alpha"))
					blend_src = GL_SRC_ALPHA;
			}
			if (liscr_args_gets_string (args, field_blend_dst, &tmpstr))
			{
				if (!strcmp (tmpstr, "one"))
					blend_dst = GL_ONE;
				else if (!strcmp (tmpstr, "one_minus_src_alpha"))
					blend_dst = GL_ONE_MINUS_SRC_ALPHA;
				else if (!strcmp (tmpstr, "src_alpha"))
					blend_dst = GL_SRC_ALPHA;
			}
			if (liscr_args_gets_bool (args, field_color_write, &tmpbool))
				color_write = tmpbool;
			if (liscr_args_gets_bool (args, field_depth_test, &tmpbool))
				depth_test = tmpbool;
			if (liscr_args_gets_bool (args, field_depth_write, &tmpbool))
				depth_write = tmpbool;
			if (liscr_args_gets_string (args, field_depth_func, &tmpstr))
			{
				if (!strcmp (tmpstr, "equal"))
					depth_func = GL_EQUAL;
				else if (!strcmp (tmpstr, "lequal"))
					depth_func = GL_LEQUAL;
				else if (!strcmp (tmpstr, "less"))
					depth_func = GL_LESS;
			}
			if (!liren_shader_compile (shader, i, vertex, geometry, fragment, feedback, alpha_coverage,
			     blend_enable, blend_src, blend_dst, color_write, depth_test, depth_write, depth_func))
				lisys_error_report ();
		}
	}

	/* Setup sorting. */
	if (liscr_args_gets_bool (args, "sort", &tmpbool))
		liren_shader_set_sort (shader, tmpbool);
}

/*****************************************************************************/

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
 * --   <li>blend: True to enable blending.</li>
 * --   <li>blend_src: Source blend function.</li>
 * --   <li>blend_dst: Destination blend function.</li>
 * --   <li>depth_test: False to disable depth test.</li>
 * --   <li>depth_write: False to disable depth writes.</li>
 * --   <li>depth_func: Depth test function.</li>
 * --   <li>fragment: Fragment program code.</li>
 * --   <li>geometry: Geometry program code.</li>
 * --   <li>name: Unique shader name.</li>
 * --   <li>sort: True to allow depth sorting for materials with this shader.</li>
 * --   <li>transform_feedback: True to enable transform feedback.</li>
 * --   <li>vertex: Vertex program code.</li></ul>
 * function Shader.new(clss, args)
 */
static void Shader_new (LIScrArgs* args)
{
	const char* name = "default";
	LIExtModule* module;
	LIScrData* data;
	LIRenShader* shader;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_SHADER);
    liscr_args_gets_string (args, "name", &name);

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
	private_compile (shader, args);

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
 * --   <li>blend: True to enable blending.</li>
 * --   <li>blend_src: Source blend function.</li>
 * --   <li>blend_dst: Destination blend function.</li>
 * --   <li>depth_test: False to disable depth test.</li>
 * --   <li>depth_write: False to disable depth writes.</li>
 * --   <li>depth_func: Depth test function.</li>
 * --   <li>fragment: Fragment program code.</li>
 * --   <li>geometry: Geometry program code.</li>
 * --   <li>sort: True to allow depth sorting for materials with this shader.</li>
 * --   <li>transform_feedback: True to enable transform feedback.</li>
 * --   <li>vertex: Vertex program code.</li></ul>
 * function Shader.compile(self, args)
 */
static void Shader_compile (LIScrArgs* args)
{
	private_compile (args->self, args);
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
