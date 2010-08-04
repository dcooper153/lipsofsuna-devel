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
 * module "Extension.Common.Generator"
 * --- Generate random dungeons.
 * -- @name Generator
 * -- @class table
 */

/* @luadoc
 * --- Creates a brush object.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>brush: Brush ID. (required)</li>
 * --   <li>extra: User defined extra string.</li>
 * --   <li>flags: Creation flags.</li>
 * --   <li>model: Model string.</li>
 * --   <li>position: Position vector relative to brush origin.</li>
 * --   <li>prob: Creation probability.</li>
 * --   <li>type: User defined type string.</li>
 * --   <li>rotation: Rotation quaternion.</li></ul>
 * function Generator.create_objects(clss, args)
 */
static void Generator_create_object (LIScrArgs* args)
{
	int id;
	int flags = 0;
	float prob = 1.0f;
	const char* type = "";
	const char* model = "";
	const char* extra = "";
	LIMatTransform transform = limat_transform_identity ();
	LIExtModule* module;
	LIGenBrush* brush;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_int (args, "brush", &id))
	{
		brush = ligen_generator_find_brush (module->generator, id);
		if (brush == NULL)
			return;
		liscr_args_gets_int (args, "flags", &flags);
		liscr_args_gets_float (args, "prob", &prob);
		liscr_args_gets_string (args, "type", &type);
		liscr_args_gets_string (args, "model", &model);
		liscr_args_gets_string (args, "extra", &extra);
		liscr_args_gets_vector (args, "position", &transform.position);
		liscr_args_gets_quaternion (args, "rotation", &transform.rotation);
		ligen_brush_insert_object (brush, flags, prob, type, model, extra, &transform);
	}
}

/* @luadoc
 * --- Deletes objects from a brush.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>brush: Brush ID.</li>
 * --   <li>object: Object index.</li></ul>
 * function Generator.delete_objects(clss, args)
 */
static void Generator_delete_objects (LIScrArgs* args)
{
	int i;
	int id;
	LIExtModule* module;
	LIGenBrush* brush;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_int (args, "brush", &id))
	{
		brush = ligen_generator_find_brush (module->generator, id);
		if (brush == NULL)
			return;
		if (liscr_args_gets_int (args, "object", &i))
		{
			if (i >= 0 && i < brush->objects.count)
				ligen_brush_remove_object (brush, i);
		}
		else
		{
			for (i = brush->objects.count - 1 ; i >= 0 ; i--)
				ligen_brush_remove_object (brush, i);
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
 * --   <li>center: Center point of root node.</li>
 * --   <li>root: Root node name.</li></ul>
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
	center.x = module->sectors->count * module->sectors->width / 2.0f;
	center.y = center.x;
	center.z = center.x;
	liscr_args_gets_string (args, "root", &root);
	liscr_args_gets_vector (args, "center", &center);

	/* Find brush. */
	brush = ligen_generator_find_brush_by_name (module->generator, root);
	if (brush == NULL)
		return;

	/* Clear generator. */
	ligen_generator_clear_scene (module->generator);

	/* Create root stroke. */
	pos[0] = LIMAT_MAX (0, (int)(center.x / module->generator->voxels->tile_width) - brush->size[0] / 2);
	pos[1] = LIMAT_MAX (0, (int)(center.y / module->generator->voxels->tile_width) - brush->size[1] / 2);
	pos[2] = LIMAT_MAX (0, (int)(center.z / module->generator->voxels->tile_width) - brush->size[2] / 2);
	if (!ligen_generator_insert_stroke (module->generator, brush->id, pos[0], pos[1], pos[2]))
		return;

	liscr_args_seti_bool (args, 1);
}

/* @luadoc
 * --- Gets the objects of a brush.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>brush: Brush ID. (required)</li></ul>
 * -- @return Table of brush objects.
 * function Generator.get_objects(clss, args)
 */
static void Generator_get_objects (LIScrArgs* args)
{
	int i;
	int id;
	LIExtModule* module;
	LIGenBrush* brush;
	LIGenBrushobject* object;
	LIScrData* data;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_int (args, "brush", &id))
	{
		brush = ligen_generator_find_brush (module->generator, id);
		if (brush == NULL)
			return;
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		for (i = 0 ; i < brush->objects.count ; i++)
		{
			object = brush->objects.array[i];
			lua_pushnumber (args->lua, i + 1);
			lua_newtable (args->lua);
			lua_pushnumber (args->lua, object->id);
			lua_setfield (args->lua, -2, "id");
			lua_pushnumber (args->lua, object->flags);
			lua_setfield (args->lua, -2, "flags");
			lua_pushnumber (args->lua, object->probability);
			lua_setfield (args->lua, -2, "prob");
			lua_pushstring (args->lua, object->type);
			lua_setfield (args->lua, -2, "type");
			lua_pushstring (args->lua, object->model);
			lua_setfield (args->lua, -2, "model");
			lua_pushstring (args->lua, object->extra);
			lua_setfield (args->lua, -2, "extra");
			data = liscr_vector_new (args->script, &object->transform.position);
			if (data != NULL)
			{
				liscr_pushdata (args->lua, data);
				lua_setfield (args->lua, -2, "position");
			}
			data = liscr_quaternion_new (args->script, &object->transform.rotation);
			if (data != NULL)
			{
				liscr_pushdata (args->lua, data);
				lua_setfield (args->lua, -2, "rotation");
			}
			lua_settable (args->lua, args->output_table);
		}
	}
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
 * --- Copies voxel data from a packet to a brush.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>brush: Brush ID. (required)</li>
 * --   <li>point: Position vector.</li>
 * --   <li>packet: Packet. (required)</li></ul>
 * -- @return True on success.
 * function Generator.paste_voxels(clss, args)
 */
static void Generator_paste_voxels (LIScrArgs* args)
{
	int id;
	int pos[3];
	uint8_t dummy;
	uint32_t size[3];
	int x;
	int y;
	int z;
	LIArcReader* reader;
	LIExtModule* module;
	LIGenBrush* brush;
	LIMatVector point;
	LIScrData* data;
	LIScrPacket* packet;
	LIVoxVoxel voxel;

	if (liscr_args_gets_int (args, "brush", &id) &&
	    liscr_args_gets_data (args, "packet", LISCR_SCRIPT_PACKET, &data))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
		point = limat_vector_init (0.0f, 0.0f, 0.0f);
		liscr_args_gets_vector (args, "point", &point);
		pos[0] = (int) point.x / module->generator->voxels->tile_width;
		pos[1] = (int) point.y / module->generator->voxels->tile_width;
		pos[2] = (int) point.z / module->generator->voxels->tile_width;

		/* Find brush. */
		brush = ligen_generator_find_brush (module->generator, id);
		if (brush == NULL)
			return;

		/* Get packet data. */
		packet = data->data;
		if (packet->writer != NULL)
		{
			reader = liarc_reader_new (
				liarc_writer_get_buffer (packet->writer),
				liarc_writer_get_length (packet->writer));
		}
		else
			reader = packet->reader;

		/* Read size. */
		if (!liarc_reader_get_uint8 (reader, &dummy) ||
		    !liarc_reader_get_uint32 (reader, size + 0) ||
		    !liarc_reader_get_uint32 (reader, size + 1) ||
		    !liarc_reader_get_uint32 (reader, size + 2))
		{
			if (packet->writer != NULL)
				liarc_reader_free (reader);
			return;
		}

		/* Copy voxels. */
		for (z = 0 ; z < size[2] ; z++)
		for (y = 0 ; y < size[1] ; y++)
		for (x = 0 ; x < size[0] ; x++)
		{
			if (!livox_voxel_read (&voxel, reader))
			{
				if (packet->writer != NULL)
					liarc_reader_free (reader);
				return;
			}
			if (0 <= x + pos[0] && x + pos[0] < brush->size[0] &&
			    0 <= y + pos[1] && y + pos[1] < brush->size[1] &&
			    0 <= z + pos[2] && z + pos[2] < brush->size[2])
				ligen_brush_set_voxel (brush, x + pos[0], y + pos[1], z + pos[2], voxel);
		}

		if (packet->writer != NULL)
			liarc_reader_free (reader);
		liscr_args_seti_bool (args, 1);
	}
}

/* @luadoc
 * --- Saves the current world map.
 * --
 * -- @param clss Generator class.
 * function Generator.save(clss)
 */
static void Generator_save (LIScrArgs* args)
{
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	ligen_generator_rebuild_scene (module->generator);
	ligen_generator_write (module->generator);
}

/* @luadoc
 * --- Saves a brush.
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>brush: Brush ID.</li></ul>
 * function Generator.save(clss)
 */
static void Generator_save_brush (LIScrArgs* args)
{
	int id;
	LIExtModule* module;

	if (liscr_args_gets_int (args, "brush", &id))
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
		ligen_generator_write_brush (module->generator, id);
	}
	else
	{
		module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
		ligen_generator_write_brushes (module->generator);
	}
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

/*****************************************************************************/

void
liext_script_generator (LIScrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_insert_cfunc (self, "create_object", Generator_create_object);
	liscr_class_insert_cfunc (self, "delete_objects", Generator_delete_objects);
	liscr_class_insert_cfunc (self, "disable_brush", Generator_disable_brush);
	liscr_class_insert_cfunc (self, "enable_brush", Generator_enable_brush);
	liscr_class_insert_cfunc (self, "expand", Generator_expand);
	liscr_class_insert_cfunc (self, "format", Generator_format);
	liscr_class_insert_cfunc (self, "get_objects", Generator_get_objects);
	liscr_class_insert_cfunc (self, "paste_voxels", Generator_paste_voxels);
	liscr_class_insert_cfunc (self, "save", Generator_save);
	liscr_class_insert_cfunc (self, "save_brush", Generator_save_brush);
	liscr_class_insert_cvar (self, "brushes", Generator_getter_brushes, NULL);
}

/** @} */
/** @} */
