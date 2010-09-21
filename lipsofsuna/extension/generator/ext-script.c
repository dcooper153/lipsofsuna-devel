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
 * \addtogroup LIExtGenerator Generator
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "Extension.Generator"
 * --- Generate random dungeons.
 * -- @name Generator
 * -- @class table
 */

/* @luadoc
 * --- Adds a new region brush.
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>name: Brush name.</li>
 * --   <li>size: Size vector.</li></ul>
 * function Generator.disable_brush(clss, args)
 */
static void Generator_add_brush (LIScrArgs* args)
{
	const char* name;
	LIExtModule* module;
	LIGenBrush* brush;
	LIMatVector size = limat_vector_init (5.0f, 5.0f, 5.0f);

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_string (args, "name", &name))
	{
		liscr_args_gets_vector (args, "size", &size);
		size.x = LIMAT_MAX (1, size.x);
		size.y = LIMAT_MAX (1, size.y);
		size.z = LIMAT_MAX (1, size.z);
		brush = ligen_brush_new (module->generator, (int) size.x, (int) size.y, (int) size.z);
		if (brush == NULL)
			return;
		if (!ligen_brush_set_name (brush, name) ||
		    !ligen_generator_insert_brush (module->generator, brush))
		{
			ligen_brush_free (brush);
			return;
		}
	}
}

/* @luadoc
 * --- Disables a brush type.
 * --
 * -- You can use this function to temporarily disable certain brush types so
 * -- that they'll never appear in the map. This is typically used to control
 * -- where special rooms should appear in the map.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>name: Brush name.</li></ul>
 * function Generator.disable_brush(clss, args)
 */
static void Generator_disable_brush (LIScrArgs* args)
{
	const char* name;
	LIExtModule* module;
	LIGenBrush* brush;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_string (args, "name", &name))
	{
		brush = ligen_generator_find_brush_by_name (module->generator, name);
		if (brush == NULL)
			return;
		brush->disabled = 1;
	}
}

/* @luadoc
 * --- Enables a brush type.
 * --
 * -- Enables a previously disabled brush type.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>name: Brush name.</li></ul>
 * function Generator.enable_brush(clss, args)
 */
static void Generator_enable_brush (LIScrArgs* args)
{
	const char* name;
	LIExtModule* module;
	LIGenBrush* brush;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_string (args, "name", &name))
	{
		brush = ligen_generator_find_brush_by_name (module->generator, name);
		if (brush == NULL)
			return;
		brush->disabled = 0;
	}
}

/* @luadoc
 * --- Creates the root node of the map.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>blocks_per_line: Number of blocks per sector edge.</li>
 * --   <li>center: Center point of root node.</li>
 * --   <li>root: Root node name.</li>
 * --   <li>tiles_per_line: Number of tiles per sector edge.</li></ul>
 * -- @return True if could create root node.
 * function Generator.format(clss, args)
 */
static void Generator_format (LIScrArgs* args)
{
	int pos[3];
	const char* root = "root";
	LIExtModule* module;
	LIGenBrush* brush;
	LIMatVector center;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	center.x = module->program->sectors->count / 2;
	center.y = center.x;
	center.z = center.x;
	liscr_args_gets_string (args, "root", &root);
	liscr_args_gets_vector (args, "center", &center);

	/* Find brush. */
	brush = ligen_generator_find_brush_by_name (module->generator, root);
	if (brush == NULL)
		return;

	/* Clear generator. */
	ligen_generator_clear (module->generator);

	/* Create root stroke. */
	pos[0] = LIMAT_MAX (0, (int) center.x - brush->size[0] / 2);
	pos[1] = LIMAT_MAX (0, (int) center.y - brush->size[1] / 2);
	pos[2] = LIMAT_MAX (0, (int) center.z - brush->size[2] / 2);
	if (!ligen_generator_insert_stroke (module->generator, brush->id, pos[0], pos[1], pos[2]))
		return;

	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Expands the map.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>count: Number of nodes to create.</li></ul>
 * -- @return True on success.
 * function Generator.expand(clss, args)
 */
static void Generator_expand (LIScrArgs* args)
{
	int i;
	int count = 1;
	LIExtModule* module;

	liscr_args_gets_int (args, "count", &count);
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	for (i = 0 ; i < count ; i++)
	{
		if (!ligen_generator_step (module->generator))
			return;
	}

	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- List of brush names.
 * -- @name Generator.brushes
 * -- @class table
 */
static void Generator_getter_brushes (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtModule* module;
	LIGenBrush* brush;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, module->generator->brushes)
	{
		brush = iter.value;
		liscr_args_sets_int (args, brush->name, brush->id);
	}
}

/* @luadoc
 * --- List of generated regions.
 * -- @name Generator.regions
 * -- @class table
 */
static void Generator_getter_regions (LIScrArgs* args)
{
	int i;
	LIExtModule* module;
	LIGenStroke* stroke;
	LIScrData* data;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	for (i = 0 ; i < module->generator->strokes.count ; i++)
	{
		stroke = module->generator->strokes.array + i;
		data = liscr_data_new_alloc (args->script, sizeof (LIGenStroke), LIEXT_SCRIPT_REGION);
		if (data != NULL)
		{
			memcpy (data->data, stroke, sizeof (LIGenStroke));
			liscr_args_seti_data (args, data);
			liscr_data_unref (data, NULL);
		}
	}
}

/*****************************************************************************/

void
liext_script_generator (LIScrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "add_brush", Generator_add_brush);
	liscr_class_insert_cfunc (self, "disable_brush", Generator_disable_brush);
	liscr_class_insert_cfunc (self, "enable_brush", Generator_enable_brush);
	liscr_class_insert_cfunc (self, "expand", Generator_expand);
	liscr_class_insert_cfunc (self, "format", Generator_format);
	liscr_class_insert_cvar (self, "brushes", Generator_getter_brushes, NULL);
	liscr_class_insert_cvar (self, "regions", Generator_getter_regions, NULL);
}

/** @} */
/** @} */
