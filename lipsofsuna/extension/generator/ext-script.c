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
 * --- Adds or replaces a material definition.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>flags: Flags.</li>
 * --   <li>friction: Friction coefficient.</li>
 * --   <li>diffuse0: Top surface diffuse color.</li>
 * --   <li>diffuse1: Side surface diffuse color.</li>
 * --   <li>id: ID number. (required)</li>
 * --   <li>name: Name string.</li>
 * --   <li>model: Model string.</li>
 * --   <li>shader0: Top surface shader.</li>
 * --   <li>shader1: Side surface shader.</li>
 * --   <li>shininess0: Top surface shininess.</li>
 * --   <li>shininess1: Side surface shininess.</li>
 * --   <li>specular0: Top surface specular color.</li>
 * --   <li>specular1: Side surface specular color.</li>
 * --   <li>texture0: Table of top surface texture names.</li>
 * --   <li>texture1: Table of side surface texture names.</li>
 * --   <li>type: Material type ("height"/"tile")</li></ul>
 * function Generator.add_material(clss, args)
 */
static void Generator_add_material (LIScrArgs* args)
{
	int i;
	int id;
	int flags = LIMDL_MATERIAL_FLAG_COLLISION | LIMDL_MATERIAL_FLAG_CULLFACE;
	int texflags0 = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	int texflags1 = LIMDL_TEXTURE_FLAG_BILINEAR | LIMDL_TEXTURE_FLAG_MIPMAP | LIMDL_TEXTURE_FLAG_REPEAT;
	float friction = 1.0f;
	float shininess0 = 1.0f;
	float shininess1 = 1.0f;
	float diffuse0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diffuse1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular0[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float specular1[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const char* type = "tile";
	const char* name = "";
	const char* model = NULL;
	const char* shader0 = "default";
	const char* shader1 = "default";
	LIExtModule* module;
	LIVoxMaterial* material;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_int (args, "id", &id) && id > 0)
	{
		liscr_args_gets_int (args, "flags", &flags);
		liscr_args_gets_float (args, "friction", &friction);
		liscr_args_gets_float (args, "shininess0", &shininess0);
		liscr_args_gets_float (args, "shininess1", &shininess1);
		liscr_args_gets_string (args, "type", &type);
		liscr_args_gets_string (args, "name", &name);
		liscr_args_gets_string (args, "model", &model);
		liscr_args_gets_string (args, "shader0", &shader0);
		liscr_args_gets_string (args, "shader1", &shader1);
		liscr_args_gets_floatv (args, "diffuse0", 4, diffuse0);
		liscr_args_gets_floatv (args, "diffuse1", 4, diffuse1);
		liscr_args_gets_floatv (args, "specular0", 4, specular0);
		liscr_args_gets_floatv (args, "specular1", 4, specular1);

		/* Create the material. */
		material = livox_material_new ();
		if (material == NULL)
			return;
		material->id = id;
		material->flags = flags;
		material->friction = friction;
		livox_material_set_name (material, name);
		if (model != NULL)
			livox_material_set_model (material, model);
		if (!strcmp (type, "height"))
			material->type = LIVOX_MATERIAL_TYPE_HEIGHT;
		else
			material->type = LIVOX_MATERIAL_TYPE_TILE;
		material->mat_top.shininess = shininess0;
		material->mat_side.shininess = shininess1;
		memcpy (material->mat_top.diffuse, diffuse0, 4 * sizeof (float));
		memcpy (material->mat_side.diffuse, diffuse1, 4 * sizeof (float));
		memcpy (material->mat_top.specular, specular0, 4 * sizeof (float));
		memcpy (material->mat_side.specular, specular1, 4 * sizeof (float));
		limdl_material_set_shader (&material->mat_top, shader0);
		limdl_material_set_shader (&material->mat_side, shader1);

		/* Append top surface textures. */
		if (liscr_args_gets_table (args, "texture0"))
		{
			for (i = 1 ; i > 0 ; i++)
			{
				lua_pushnumber (args->lua, i);
				lua_gettable (args->lua, -2);
				if (lua_type (args->lua, -1) == LUA_TSTRING)
				{
					limdl_material_append_texture (&material->mat_top,
						LIMDL_TEXTURE_TYPE_IMAGE, texflags0, lua_tostring (args->lua, -1));
				}
				else
					i = -1;
				lua_pop (args->lua, 1);
			}
			lua_pop (args->lua, 1);
		}

		/* Append side surface textures. */
		if (liscr_args_gets_table (args, "texture1"))
		{
			for (i = 1 ; i > 0 ; i++)
			{
				lua_pushnumber (args->lua, i);
				lua_gettable (args->lua, -2);
				if (lua_type (args->lua, -1) == LUA_TSTRING)
				{
					limdl_material_append_texture (&material->mat_side,
						LIMDL_TEXTURE_TYPE_IMAGE, texflags1, lua_tostring (args->lua, -1));
				}
				else
					i = -1;
				lua_pop (args->lua, 1);
			}
			lua_pop (args->lua, 1);
		}

		/* Insert to the generator. */
		ligen_generator_remove_material (module->generator, id);
		if (!ligen_generator_insert_material (module->generator, material))
		{
			livox_material_free (material);
			return;
		}
		liscr_args_seti_bool (args, 1);
	}
}

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
	int tmp;
	int tiles_per_line;
	int blocks_per_line;
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

	/* Configure tile size. */
	blocks_per_line = module->blocks_per_line;
	tiles_per_line = module->tiles_per_line;
	if (liscr_args_gets_int (args, "blocks_per_line", &tmp) && tmp >= 1)
		blocks_per_line = tmp;
	if (liscr_args_gets_int (args, "tiles_per_line", &tmp) && tmp >= 1)
		tiles_per_line = tmp;
	liext_generator_configure (module, blocks_per_line, tiles_per_line);

	/* Find brush. */
	brush = ligen_generator_find_brush_by_name (module->generator, root);
	if (brush == NULL)
		return;

	/* Clear generator. */
	ligen_generator_clear_scene (module->generator);

	/* Create root stroke. */
	pos[0] = LIMAT_MAX (0, (int)(center.x / module->tile_width) - brush->size[0] / 2);
	pos[1] = LIMAT_MAX (0, (int)(center.y / module->tile_width) - brush->size[1] / 2);
	pos[2] = LIMAT_MAX (0, (int)(center.z / module->tile_width) - brush->size[2] / 2);
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
 * --- Gets the material configuration of the voxel system.
 * --
 * -- @param clss Voxel class.
 * -- @param args Arguments.<ul>
 * --   <li>type: Packet type.</li></ul>
 * -- @return Packet writer.
 * function Generator.get_materials(clss, args)
 */
static void Generator_get_materials (LIScrArgs* args)
{
	int type = 1;
	LIExtModule* module;
	LIScrData* data;
	LIScrPacket* packet;

	/* Create packet. */
	liscr_args_gets_int (args, "type", &type);
	data = liscr_packet_new_writable (args->script, type);
	if (data == NULL)
		return;
	packet = data->data;

	/* Build packet. */
	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	ligen_generator_write_materials (module->generator, packet->writer);
	liscr_args_seti_data (args, data);
	liscr_data_unref (data, NULL);
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
		pos[0] = (int) point.x / module->tile_width;
		pos[1] = (int) point.y / module->tile_width;
		pos[2] = (int) point.z / module->tile_width;

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
 * --- Removes a material definition.
 * --
 * -- @param clss Generator class.
 * -- @param args Arguments.<ul>
 * --   <li>id: ID number. (required)</li></ul>
 * function Generator.remove_material(clss, args)
 */
static void Generator_remove_material (LIScrArgs* args)
{
	int id;
	LIExtModule* module;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	if (liscr_args_gets_int (args, "id", &id))
		ligen_generator_remove_material (module->generator, id);
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
	if (!ligen_generator_write (module->generator, module->blocks_per_line, module->tiles_per_line))
		lisys_error_report ();
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

/* @luadoc
 * --- List of brush IDs.
 * -- @name Generator.materials
 * -- @class table
 */
static void Generator_getter_materials (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIExtModule* module;
	LIVoxMaterial* material;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_GENERATOR);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, module->generator->materials)
	{
		material = iter.value;
		liscr_args_seti_int (args, material->id);
	}
}

/*****************************************************************************/

void
liext_script_generator (LIScrClass* self,
                        void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_GENERATOR, data);
	liscr_class_inherit (self, liscr_script_class, NULL);
	liscr_class_insert_cfunc (self, "add_material", Generator_add_material);
	liscr_class_insert_cfunc (self, "create_object", Generator_create_object);
	liscr_class_insert_cfunc (self, "delete_objects", Generator_delete_objects);
	liscr_class_insert_cfunc (self, "disable_brush", Generator_disable_brush);
	liscr_class_insert_cfunc (self, "enable_brush", Generator_enable_brush);
	liscr_class_insert_cfunc (self, "expand", Generator_expand);
	liscr_class_insert_cfunc (self, "format", Generator_format);
	liscr_class_insert_cfunc (self, "get_materials", Generator_get_materials);
	liscr_class_insert_cfunc (self, "get_objects", Generator_get_objects);
	liscr_class_insert_cfunc (self, "paste_voxels", Generator_paste_voxels);
	liscr_class_insert_cfunc (self, "remove_material", Generator_remove_material);
	liscr_class_insert_cfunc (self, "save", Generator_save);
	liscr_class_insert_cfunc (self, "save_brush", Generator_save_brush);
	liscr_class_insert_cvar (self, "brushes", Generator_getter_brushes, NULL);
	liscr_class_insert_cvar (self, "materials", Generator_getter_materials, NULL);
}

/** @} */
/** @} */
