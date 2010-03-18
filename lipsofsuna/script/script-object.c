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
 * module "Core.Common.Object"
 * ---
 * -- Manipulate objects.
 * -- @name Object
 * -- @class table
 */

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
	if (channel < 0 || channel > 254)
		channel = -1;
	ret = lieng_object_animate (args->self, channel, animation, repeat, weight, time);
	liscr_args_seti_bool (args, ret);
}

/* @luadoc
 * --- Makes the object approach a point.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * --   <li>dist: Distance how close to target to get.</li>
 * --   <li>point: Point vector in world space. (required)</li>
 * --   <li>speed: Movement speed multiplier.</li></ul>
 * function Object.approach(self, args)
 */
static void Object_approach (LIScrArgs* args)
{
	float dist = 0.0f;
	float speed = 1.0f;
	LIMatVector vector;

	if (liscr_args_gets_vector (args, "point", &vector))
	{
		liscr_args_gets_float (args, "dist", &dist);
		liscr_args_gets_float (args, "speed", &speed);
		lieng_object_approach (args->self, &vector, speed, dist);
	}
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
 * --   <li>name: Node name. (required)</li></ul>
 * -- @return Position and rotation, or nil if not found.
 * function Object.find_node(self, args)
 */
static void Object_find_node (LIScrArgs* args)
{
	const char* name;
	LIMatTransform transform;
	LIMdlNode* node;

	if (!liscr_args_gets_string (args, "name", &name))
		return;
	node = limdl_pose_find_node (LIENG_OBJECT (args->self)->pose, name);
	if (node == NULL)
		return;
	limdl_node_get_world_transform (node, &transform);
	liscr_args_seti_vector (args, &transform.position);
	liscr_args_seti_quaternion (args, &transform.rotation);
}

/* @luadoc
 * --- Finds all objects inside a sphere.
 * --
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Center point. (required)</li>
 * --   <li>radius: Search radius.</li></ul>
 * -- @return Table of matching objects and their IDs.
 * function Object.find_objects(clss, args)
 */
static void Object_find_objects (LIScrArgs* args)
{
	float radius = 32.0f;
	LIEngObjectIter iter;
	LIMatVector center;
	LIMaiProgram* program;

	/* Check arguments. */
	if (!liscr_args_gets_vector (args, "point", &center))
		return;
	liscr_args_gets_float (args, "radius", &radius);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Find objects. */
	LIENG_FOREACH_OBJECT (iter, program->engine, &center, radius)
		liscr_args_setf_data (args, iter.object->id, iter.object->script);
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
 * --- Lets an impulse force affect the object.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>point: Point of impulse. (required)</li>
 * --   <li>impulse: Force of impulse. (required)</li></ul>
 * function Object.impulse(self, args)
 */
static void Object_impulse (LIScrArgs* args)
{
	LIMatVector impulse;
	LIMatVector point;

	if (liscr_args_gets_vector (args, "impulse", &impulse) &&
	    liscr_args_gets_vector (args, "point", &point))
		lieng_object_impulse (args->self, &point, &impulse);
}

/* @luadoc
 * --- Causes the object to jump.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>impulse: Force of impulse. (required)</li></ul>
 * function Object.jump(self, impulse)
 */
static void Object_jump (LIScrArgs* args)
{
	LIMatVector impulse;

	if (liscr_args_gets_vector (args, "impulse", &impulse))
		lieng_object_jump (args->self, &impulse);
}

/* @luadoc
 * --- Angular velocity.
 * -- <br/>
 * -- Angular velocity specifies how the object rotates. The direction of the
 * -- vector points towards the axis of rotation and the length of the vector
 * -- specifies how fast the object rotates around its center point.
 * -- <br/>
 * -- Only supported by rigid bodies. Other kind of objects always return
 * -- a zero vector.
 * --
 * -- @name Object.angular
 * -- @class table
 */
static void Object_getter_angular (LIScrArgs* args)
{
	LIMatVector tmp;

	lieng_object_get_angular (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_angular (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		lieng_object_set_angular (args->self, &vector);
}

/* @luadoc
 * --- Table of channel numbers and animation names.
 * -- <br/>
 * -- A list of permanent animations the object is playing back.
 * --
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
			liscr_args_setf_string (args, iter.key, channel->animation_name);
	}
}

/* @luadoc
 * --- Class type.
 * --
 * -- @name Object.class
 * -- @class table
 */
static void Object_getter_class (LIScrArgs* args)
{
	liscr_args_seti_class (args, args->data->clss);
}
static void Object_setter_class (LIScrArgs* args)
{
	LIScrClass* clss;

	if (liscr_args_geti_class (args, 0, NULL, &clss))
		liscr_data_set_class (args->data, clss);
}

/* @luadoc
 * --- Class name of the object.
 * --
 * -- @name Object.class_name
 * -- @class table
 */
static void Object_getter_class_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, liscr_class_get_name (args->clss));
}

/* @luadoc
 * --- Collision group bitmask.
 * --
 * -- @name Object.collision_group
 * -- @class table
 */
static void Object_getter_collision_group (LIScrArgs* args)
{
	liscr_args_seti_int (args, lieng_object_get_collision_group (args->self));
}
static void Object_setter_collision_group (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		lieng_object_set_collision_group (args->self, value);
}

/* @luadoc
 * --- Collision bitmask.
 * --
 * -- @name Object.collision_mask
 * -- @class table
 */
static void Object_getter_collision_mask (LIScrArgs* args)
{
	liscr_args_seti_int (args, lieng_object_get_collision_mask (args->self));
}
static void Object_setter_collision_mask (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		lieng_object_set_collision_mask (args->self, value);
}

/* @luadoc
 * --- Gravity vector.
 * --
 * -- @name Object.gravity
 * -- @class table
 */
static void Object_getter_gravity (LIScrArgs* args)
{
	LIMatVector tmp;

	liphy_object_get_gravity (LIENG_OBJECT (args->self)->physics, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_gravity (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity (LIENG_OBJECT (args->self)->physics, &vector);
}

/* @luadoc
 * --- Ground contact flag.
 * -- <br/>
 * -- Only supported for creatures. Other kind of objects always return false.
 * --
 * -- @name Object.ground
 * -- @class table
 */
static void Object_getter_ground (LIScrArgs* args)
{
	liscr_args_seti_bool (args, lieng_object_get_ground (args->self));
}

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
 * --- Mass.
 * --
 * -- @name Object.mass
 * -- @class table
 */
static void Object_getter_mass (LIScrArgs* args)
{
	liscr_args_seti_float (args, lieng_object_get_mass (args->self));
}
static void Object_setter_mass (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		lieng_object_set_mass (args->self, value);
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

/* @luadoc
 * --- Movement speed.
 * --
 * -- Only used by creature objects.
 * --
 * -- @name Object.speed
 * -- @class table
 */
static void Object_getter_speed (LIScrArgs* args)
{
	liscr_args_seti_float (args, lieng_object_get_speed (args->self));
}
static void Object_setter_speed (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		lieng_object_set_speed (args->self, value);
}

/* @luadoc
 * --- Strafing direction.
 * -- <br/>
 * -- Only used by creature objects. The value of -1 means that the creature is
 * -- strafing to the left at walking speed. The value of 1 means right, and the
 * -- value of 0 means no strafing.
 * --
 * -- @name Object.strafing
 * -- @class table
 */
static void Object_getter_strafing (LIScrArgs* args)
{
	liscr_args_seti_float (args, liphy_object_get_strafing (LIENG_OBJECT (args->self)->physics));
}
static void Object_setter_strafing (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_strafing (LIENG_OBJECT (args->self)->physics, value);
}

/* @luadoc
 * --- True if the object is static, false if it is a dynamic rigid body.
 * --
 * -- @name Object.static
 * -- @class table
 */
static void Object_getter_static (LIScrArgs* args)
{
	int value;

	value = liphy_object_get_control_mode (LIENG_OBJECT (args->self)->physics);
	liscr_args_seti_bool (args, value == LIPHY_CONTROL_MODE_STATIC);
}
static void Object_setter_static (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
	{
		if (value)
			value = LIPHY_CONTROL_MODE_STATIC;
		else
			value = LIPHY_CONTROL_MODE_RIGID;
		liphy_object_set_control_mode (LIENG_OBJECT (args->self)->physics, value);
	}
}

/* @luadoc
 * --- Linear velocity.
 * --
 * -- @name Object.velocity
 * -- @class table
 */
static void Object_getter_velocity (LIScrArgs* args)
{
	LIMatVector tmp;

	lieng_object_get_velocity (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_velocity (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		lieng_object_set_velocity (args->self, &vector);
}

/*****************************************************************************/

void
liscr_script_object (LIScrClass* self,
                     void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_OBJECT, data);
	liscr_class_insert_mfunc (self, "animate", Object_animate);
	liscr_class_insert_mfunc (self, "approach", Object_approach);
	liscr_class_insert_cfunc (self, "find", Object_find);
	liscr_class_insert_mfunc (self, "find_node", Object_find_node);
	liscr_class_insert_cfunc (self, "find_objects", Object_find_objects);
	liscr_class_insert_mfunc (self, "get_animation", Object_get_animation);
	liscr_class_insert_mfunc (self, "impulse", Object_impulse);
	liscr_class_insert_mfunc (self, "jump", Object_jump);
	liscr_class_insert_mvar (self, "angular", Object_getter_angular, Object_setter_angular);
	liscr_class_insert_mvar (self, "animations", Object_getter_animations, NULL);
	liscr_class_insert_mvar (self, "class", Object_getter_class, Object_setter_class);
	liscr_class_insert_cvar (self, "class_name", Object_getter_class_name, NULL);
	liscr_class_insert_mvar (self, "collision_group", Object_getter_collision_group, Object_setter_collision_group);
	liscr_class_insert_mvar (self, "collision_mask", Object_getter_collision_mask, Object_setter_collision_mask);
	liscr_class_insert_mvar (self, "gravity", Object_getter_gravity, Object_setter_gravity);
	liscr_class_insert_mvar (self, "ground", Object_getter_ground, NULL);
	liscr_class_insert_mvar (self, "id", Object_getter_id, NULL);
	liscr_class_insert_mvar (self, "mass", Object_getter_mass, Object_setter_mass);
	liscr_class_insert_mvar (self, "model", Object_getter_model, Object_setter_model);
	liscr_class_insert_mvar (self, "position", Object_getter_position, Object_setter_position);
	liscr_class_insert_mvar (self, "realized", Object_getter_realized, Object_setter_realized);
	liscr_class_insert_mvar (self, "rotation", Object_getter_rotation, Object_setter_rotation);
	liscr_class_insert_mvar (self, "save", Object_getter_save, Object_setter_save);
	liscr_class_insert_mvar (self, "selected", Object_getter_selected, Object_setter_selected);
	liscr_class_insert_cvar (self, "selected_objects", Object_getter_selected_objects, Object_setter_selected_objects);
	liscr_class_insert_mvar (self, "speed", Object_getter_speed, Object_setter_speed);
	liscr_class_insert_mvar (self, "static", Object_getter_static, Object_setter_static);
	liscr_class_insert_mvar (self, "strafing", Object_getter_strafing, Object_setter_strafing);
	liscr_class_insert_mvar (self, "velocity", Object_getter_velocity, Object_setter_velocity);
}

/** @} */
/** @} */
