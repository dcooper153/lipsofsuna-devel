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
#include <lipsofsuna/script.h>

/* @luadoc
 * module "Core.Common.Object"
 * ---
 * -- Manipulate objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Instructs the object to approach a point.
 * --
 * -- Arguments:
 * -- point: Point vector in world space. (required)
 * -- speed: Movement speed multiplier.
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.approach(self, args)
 */
static void Object_approach (LIScrArgs* args)
{
	float speed = 1.0f;
	LIMatVector vector;

	if (liscr_args_gets_vector (args, "point", &vector))
	{
		liscr_args_gets_float (args, "speed", &speed);
		lieng_object_approach (args->self, &vector, speed);
	}
}

/* @luadoc
 * ---
 * -- Finds a bone or an anchor by name.
 * --
 * -- Arguments:
 * -- name: Node name. (required)
 * --
 * -- @param self Object.
 * -- @param args Arguments.
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
 * ---
 * -- Lets an impulse force affect the object.
 * --
 * -- Arguments:
 * -- point: Point of impulse. (required)
 * -- impulse: Force of impulse. (required)
 * --
 * -- @param self Object.
 * -- @param args Arguments.
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
 * ---
 * -- Causes the object to jump.
 * --
 * -- Arguments:
 * -- impulse: Force of impulse. (required)
 * --
 * -- @param self Object.
 * -- @param args Arguments.
 * function Object.jump(self, impulse)
 */
static void Object_jump (LIScrArgs* args)
{
	LIMatVector impulse;

	if (liscr_args_gets_vector (args, "impulse", &impulse))
		lieng_object_jump (args->self, &impulse);
}

/* @luadoc
 * ---
 * -- Angular velocity.
 * --
 * -- Angular velocity specifies how the object rotates. The direction of the
 * -- vector points towards the axis of rotation and the length of the vector
 * -- specifies how fast the object rotates around its center point.
 * --
 * -- Only supported by rigid bodies. Other kind of objects always return
 * -- a zero vector.
 * --
 * -- @name Object.angular_momentum
 * -- @class table
 */
static void Object_getter_angular_momentum (LIScrArgs* args)
{
	LIMatVector tmp;

	lieng_object_get_angular_momentum (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_angular_momentum (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		lieng_object_set_angular_momentum (args->self, &vector);
}

/* @luadoc
 * ---
 * -- Class type.
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
 * ---
 * -- Class name of the object.
 * --
 * -- @name Object.class_name
 * -- @class table
 */
static void Object_getter_class_name (LIScrArgs* args)
{
	liscr_args_seti_string (args, liscr_class_get_name (args->clss));
}

/* @luadoc
 * ---
 * -- Collision group bitmask.
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
 * ---
 * -- Collision bitmask.
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
 * ---
 * -- Gravity vector.
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
 * ---
 * -- Ground contact flag.
 * --
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
 * ---
 * -- Identification number.
 * --
 * -- @name Object.id
 * -- @class table
 */
static void Object_getter_id (LIScrArgs* args)
{
	liscr_args_seti_int (args, LIENG_OBJECT (args->self)->id);
}

/* @luadoc
 * ---
 * -- Mass.
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
 * ---
 * -- Model string.
 * -- @name Object.model
 * -- @class table
 */
static void Object_getter_model (LIScrArgs* args)
{
	int id;
	LIEngModel* model;
	LIEngObject* self = args->self;

	id = lieng_object_get_model_code (args->self);
	model = lieng_engine_find_model_by_code (self->engine, id);
	if (model != NULL)
		liscr_args_seti_string (args, model->name);
}
static void Object_setter_model (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		lieng_object_set_model_name (args->self, value);
}

/* @luadoc
 * ---
 * -- Position.
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
		lieng_object_get_transform (args->self, &transform);
		transform.position = vector;
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * ---
 * -- Visibility status.
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
 * ---
 * -- Rotational orientation.
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
		lieng_object_get_transform (args->self, &transform);
		transform.rotation = *((LIMatQuaternion*) quat->data);
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * ---
 * -- Save enabled flag.
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
 * ---
 * -- Selection status flag.
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
 * ---
 * -- Movement speed.
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
 * ---
 * -- Strafing direction.
 * --
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
 * ---
 * -- Linear velocity.
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
	liscr_class_insert_mfunc (self, "approach", Object_approach);
	liscr_class_insert_mfunc (self, "find_node", Object_find_node);
	liscr_class_insert_mfunc (self, "impulse", Object_impulse);
	liscr_class_insert_mfunc (self, "jump", Object_jump);
	liscr_class_insert_mvar (self, "angular_momentum", Object_getter_angular_momentum, Object_setter_angular_momentum);
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
	liscr_class_insert_mvar (self, "speed", Object_getter_speed, Object_setter_speed);
	liscr_class_insert_mvar (self, "strafing", Object_getter_strafing, Object_setter_strafing);
	liscr_class_insert_mvar (self, "velocity", Object_getter_velocity, Object_setter_velocity);
}

/** @} */
/** @} */
