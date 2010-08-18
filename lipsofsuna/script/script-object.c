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
 * \addtogroup liscrObject Object
 * @{
 */

#include <lipsofsuna/engine.h>
#include <lipsofsuna/main.h>
#include <lipsofsuna/script.h>

/* @luadoc
 * module "Core.Object"
 * ---
 * -- Manipulate objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- Adds an additional model mesh to the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>model: Model name. (required)</li></ul>
 * function Object.add_model(self, args)
 */
static void Object_add_model (LIScrArgs* args)
{
	const char* model;

	if (liscr_args_gets_string (args, "model", &model))
	{
		if (!lieng_object_merge_model (args->self, model))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Sets or clears an animation.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>animation: Animation name.</li>
 * --   <li>channel: Channel number.</li>
 * --   <li>weight: Blending weight.</li>
 * --   <li>time: Starting time.</li>
 * --   <li>permanent: True if should keep repeating.</li></ul>
 * -- @return True if started a new animation.
 * function Object.animate(self, args)
 */
static void Object_animate (LIScrArgs* args)
{
	int ret;
	int repeat = 0;
	int channel = -1;
	float weight = 1.0f;
	float time = 0.0f;
	const char* animation = NULL;

	liscr_args_gets_string (args, "animation", &animation);
	liscr_args_gets_int (args, "channel", &channel);
	liscr_args_gets_float (args, "weight", &weight);
	liscr_args_gets_float (args, "time", &time);
	liscr_args_gets_bool (args, "permanent", &repeat);
	if (channel < 1 || channel > 255)
		channel = -1;
	else
		channel--;
	ret = lieng_object_animate (args->self, channel, animation, repeat, weight, time);
	liscr_args_seti_bool (args, ret);
}

/* @luadoc
 * --- Recalculates the bounding box of the model of the object.
 * -- @param self Object.
 * function Object.calculate_bounds(self)
 */
static void Object_calculate_bounds (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->model != NULL)
		lieng_model_calculate_bounds (self->model);
}

/* @luadoc
 * --- Finds an object by ID.
 * --
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Object ID. (required)</li>
 * --   <li>point: Center point for radius check.</li>
 * --   <li>radius: Maximum distance from center point.</li></ul>
 * -- @return Object or nil.
 * function Object.find(clss, args)
 */
static void Object_find (LIScrArgs* args)
{
	int id;
	float radius;
	LIEngObject* object;
	LIMaiProgram* program;
	LIMatTransform transform;
	LIMatVector center;

	if (liscr_args_gets_int (args, "id", &id))
	{
		/* Find object. */
		program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
		object = lieng_engine_find_object (program->engine, id);
		if (object == NULL)
			return;

		/* Optional radius check. */
		if (liscr_args_gets_vector (args, "point", &center) &&
		    liscr_args_gets_float (args, "radius", &radius))
		{
			if (!lieng_object_get_realized (object))
				return;
			lieng_object_get_transform (object, &transform);
			center = limat_vector_subtract (center, transform.position);
			if (limat_vector_get_length (center) > radius)
				return;
		}

		/* Return object. */
		liscr_args_seti_data (args, object->script);
	}
}

/* @luadoc
 * --- Finds a bone or an anchor by name.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>name: Node name. (required)</li>
 * --   <li>space: Coordinate space. ("local"/"world")</li></ul>
 * -- @return Position and rotation, or nil if not found.
 * function Object.find_node(self, args)
 */
static void Object_find_node (LIScrArgs* args)
{
	const char* name;
	const char* space = "local";
	LIMatTransform transform;
	LIMatTransform transform1;
	LIMdlNode* node;
	LIEngObject* self;

	if (!liscr_args_gets_string (args, "name", &name))
		return;
	liscr_args_gets_string (args, "space", &space);

	/* Find the node. */
	self = args->self;
	node = limdl_pose_find_node (self->pose, name);
	if (node == NULL)
		return;

	/* Get the transformation. */
	if (!strcmp (space, "world"))
	{
		limdl_node_get_world_transform (node, &transform);
		lieng_object_get_transform (self, &transform1);
		transform = limat_transform_multiply (transform1, transform);
	}
	else
		limdl_node_get_world_transform (node, &transform);

	/* Return the transformation. */
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- Finds all objects inside a sphere.
 * --
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Center point. (required)</li>
 * --   <li>radius: Search radius.</li>
 * --   <li>sector: Return all object in this sector.</li></ul>
 * -- @return Table of matching objects and their IDs.
 * function Object.find_objects(clss, args)
 */
static void Object_find_objects (LIScrArgs* args)
{
	int id;
	float radius = 32.0f;
	LIAlgU32dicIter iter1;
	LIEngObjectIter iter;
	LIEngObject* object;
	LIEngSector* sector;
	LIMatVector center;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Radial find mode. */
	if (liscr_args_gets_vector (args, "point", &center))
	{
		liscr_args_gets_float (args, "radius", &radius);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		LIENG_FOREACH_OBJECT (iter, program->engine, &center, radius)
			liscr_args_setf_data (args, iter.object->id, iter.object->script);
	}

	/* Sector find mode. */
	else if (liscr_args_gets_int (args, "sector", &id))
	{
		sector = lialg_sectors_data_index (program->sectors, "engine", id, 0);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		if (sector != NULL)
		{
			LIALG_U32DIC_FOREACH (iter1, sector->objects)
			{
				object = iter1.value;
				liscr_args_setf_data (args, object->id, object->script);
			}
		}
	}
}

/* @luadoc
 * --- Gets animation information for the given animation channel.
 * --
 * -- If an animation is looping in the channel, a table containing the fields
 * -- animation, time, and weight is returned.
 * --
 * -- @param self Server class.
 * -- @param args Arguments.<ul>
 * --   <li>channel: Channel number. (required)</li></ul>
 * -- @return Animation info table or nil.
 * function Object.get_animation(self, args)
 */
static void Object_get_animation (LIScrArgs* args)
{
	int chan;
	LIEngObject* object;
	LIMdlAnimation* anim;

	/* Check arguments. */
	if (!liscr_args_gets_int (args, "channel", &chan))
		return;
	chan--;
	object = args->self;
	anim = limdl_pose_get_channel_animation (object->pose, chan);
	if (anim == NULL)
		return;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_sets_string (args, "animation", anim->name);
	liscr_args_sets_float (args, "time", limdl_pose_get_channel_position (object->pose, chan));
	liscr_args_sets_float (args, "weight", limdl_pose_get_channel_priority (object->pose, chan));
}

/* @luadoc
 * --- Creates a new object.
 * --
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>id: Unique ID of the object. (required)</li></ul>
 * -- @return New object.
 * function Object.new(clss, args)
 */
static void Object_new (LIScrArgs* args)
{
	int id;
	int realize = 0;
	LIEngObject* self;
	LIMaiProgram* program;
	LIScrClass* clss;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	if (!liscr_args_gets_int (args, "id", &id) || id <= 0)
		return;

	/* Check for an existing object. Reusing an existing object with the same
	   ID is a valid use case because the scripts have no way to know if the
	   object has been garbage collected by clients in networked scenarios. */
	self = lieng_engine_find_object (program->engine, id);
	if (self != NULL)
	{
		if (self->script != NULL)
		{
			lieng_object_reset (self);
			liscr_args_call_setters_except (args, self->script, "realized");
			liscr_args_gets_bool (args, "realized", &realize);
			liscr_args_seti_data (args, self->script);
			lieng_object_set_realized (self, realize);
		}
		return;
	}

	/* Get real class. */
	clss = liscr_isanyclass (args->lua, 1);
	if (clss == NULL)
		return;

	/* Allocate object. */
	self = lieng_object_new (program->engine, NULL, LIPHY_CONTROL_MODE_RIGID, id);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, self, clss->meta, lieng_object_free);
	if (self->script == NULL)
	{
		lieng_object_free (self);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters_except (args, self->script, "realized");
	liscr_args_gets_bool (args, "realized", &realize);
	liscr_args_seti_data (args, self->script);
	liscr_data_unref (self->script, NULL);
	lieng_object_set_realized (self, realize);
}

/* @luadoc
 * --- Table of channel numbers and animation names.<br/>
 * -- A list of permanent animations the object is playing back.
 * -- @name Object.animations
 */
static void Object_getter_animations (LIScrArgs* args)
{
	LIAlgU32dicIter iter;
	LIEngObject* object;
	LIMdlPoseChannel* channel;

	object = args->self;
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	LIALG_U32DIC_FOREACH (iter, object->pose->channels)
	{
		channel = iter.value;
		if (channel->repeats == -1)
			liscr_args_setf_string (args, iter.key + 1, channel->animation->name);
	}
}

/* @luadoc
 * --- Custom collision response callback.
 * --
 * -- Function to be called every time the object collides with something.
 * --
 * -- @name Object.contact_cb
 * -- @class table
 */

/* @luadoc
 * --- Unique identification number.
 * --
 * -- @name Object.id
 * -- @class table
 */
static void Object_getter_id (LIScrArgs* args)
{
	liscr_args_seti_int (args, LIENG_OBJECT (args->self)->id);
}

/* @luadoc
 * --- Model string.
 * -- @name Object.model
 * -- @class table
 */
static void Object_getter_model (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->model != NULL)
		liscr_args_seti_string (args, self->model->name);
}
static void Object_setter_model (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		lieng_object_set_model_name (args->self, value);
}

/* @luadoc
 * --- Position.
 * --
 * -- @name Object.position
 * -- @class table
 */
static void Object_getter_position (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp.position);
}
static void Object_setter_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		lieng_object_get_target (args->self, &transform);
		transform.position = vector;
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * --- The position smoothing factor of the object.
 * --
 * -- @name Object.position_smoothing
 * -- @class table
 */
static void Object_getter_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lieng_object_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, pos);
}
static void Object_setter_position_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lieng_object_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &pos);
	lieng_object_set_smoothing (args->self, pos, rot);
}

/* @luadoc
 * --- Visibility status.
 * --
 * -- @name Object.realized
 * -- @class table
 */
static void Object_getter_realized (LIScrArgs* args)
{
	liscr_args_seti_bool (args, lieng_object_get_realized (args->self));
}
static void Object_setter_realized (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lieng_object_set_realized (args->self, value);
}

/* @luadoc
 * --- Rotational orientation.
 * --
 * -- @name Object.rotation
 * -- @class table
 */
static void Object_getter_rotation (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_quaternion (args, &tmp.rotation);
}
static void Object_setter_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIScrData* quat;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &quat))
	{
		lieng_object_get_target (args->self, &transform);
		transform.rotation = *((LIMatQuaternion*) quat->data);
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * --- The rotation smoothing factor of the object.
 * --
 * -- @name Object.rotation_smoothing
 * -- @class table
 */
static void Object_getter_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lieng_object_get_smoothing (args->self, &pos, &rot);
	liscr_args_seti_float (args, rot);
}
static void Object_setter_rotation_smoothing (LIScrArgs* args)
{
	float pos;
	float rot;

	lieng_object_get_smoothing (args->self, &pos, &rot);
	liscr_args_geti_float (args, 0, &rot);
	lieng_object_set_smoothing (args->self, pos, rot);
}

/* @luadoc
 * --- Save enabled flag.
 * --
 * -- @name Object.save
 * -- @class table
 */
static void Object_getter_save (LIScrArgs* args)
{
	int flags;

	flags = lieng_object_get_flags (args->self);
	liscr_args_seti_bool (args, (flags & (LIENG_OBJECT_FLAG_SAVE)) != 0);
}
static void Object_setter_save (LIScrArgs* args)
{
	int flags;
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		flags = lieng_object_get_flags (args->self);
		if (value)
			flags |= LIENG_OBJECT_FLAG_SAVE;
		else
			flags &= ~LIENG_OBJECT_FLAG_SAVE;
		lieng_object_set_flags (args->self, flags);
	}
}

/* @luadoc
 * --- The sector index of the object, or nil if the object isn't on the map.
 * --
 * -- @name Object.sector
 * -- @class table
 */
static void Object_getter_sector (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->sector != NULL)
		liscr_args_seti_int (args, self->sector->sector->index);
}

/* @luadoc
 * --- Selection status flag.
 * --
 * -- @name Object.selected
 * -- @class table
 */
static void Object_getter_selected (LIScrArgs* args)
{
	liscr_args_seti_bool (args, lieng_object_get_selected (args->self));
}
static void Object_setter_selected (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lieng_object_set_selected (args->self, value);
}

/* @luadoc
 * --- List of selected objects.
 * -- <br/>
 * -- Class variable.
 * -- @name Object.selected_objects
 * -- @class table
 */
static void Object_getter_selected_objects (LIScrArgs* args)
{
	LIEngSelectionIter iter;
	LIMaiProgram* program;

	/* Create empty table. */
	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);

	/* Pack selected objects. */
	LIENG_FOREACH_SELECTION (iter, program->engine)
	{
		if (lieng_object_get_realized (iter.object))
			liscr_args_seti_data (args, iter.object->script);
	}
}
static void Object_setter_selected_objects (LIScrArgs* args)
{
	int i;
	LIEngSelectionIter iter;
	LIMaiProgram* program;
	LIScrData* data;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Unselect all. */
	LIENG_FOREACH_SELECTION (iter, program->engine)
		lieng_object_set_selected (iter.object, 0);

	/* Select listed. */
	for (i = 0 ; liscr_args_geti_data (args, i, LISCR_SCRIPT_OBJECT, &data) ; i++)
		lieng_object_set_selected (data->data, 1);
}

/*****************************************************************************/

void liscr_script_object (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_OBJECT, data);
	liscr_class_inherit (self, liscr_script_data, data);
	liscr_class_insert_mfunc (self, "add_model", Object_add_model);
	liscr_class_insert_mfunc (self, "animate", Object_animate);
	liscr_class_insert_mfunc (self, "calculate_bounds", Object_calculate_bounds);
	liscr_class_insert_cfunc (self, "find", Object_find);
	liscr_class_insert_mfunc (self, "find_node", Object_find_node);
	liscr_class_insert_cfunc (self, "find_objects", Object_find_objects);
	liscr_class_insert_mfunc (self, "get_animation", Object_get_animation);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mvar (self, "animations", Object_getter_animations, NULL);
	liscr_class_insert_mvar (self, "id", Object_getter_id, NULL);
	liscr_class_insert_mvar (self, "model", Object_getter_model, Object_setter_model);
	liscr_class_insert_mvar (self, "position", Object_getter_position, Object_setter_position);
	liscr_class_insert_mvar (self, "position_smoothing", Object_getter_position_smoothing, Object_setter_position_smoothing);
	liscr_class_insert_mvar (self, "realized", Object_getter_realized, Object_setter_realized);
	liscr_class_insert_mvar (self, "rotation", Object_getter_rotation, Object_setter_rotation);
	liscr_class_insert_mvar (self, "rotation_smoothing", Object_getter_rotation_smoothing, Object_setter_rotation_smoothing);
	liscr_class_insert_mvar (self, "save", Object_getter_save, Object_setter_save);
	liscr_class_insert_mvar (self, "sector", Object_getter_sector, NULL);
	liscr_class_insert_mvar (self, "selected", Object_getter_selected, Object_setter_selected);
	liscr_class_insert_cvar (self, "selected_objects", Object_getter_selected_objects, Object_setter_selected_objects);
}

/** @} */
/** @} */
