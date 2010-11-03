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
 * \addtogroup LIExtObjectPhysics ObjectPhysics
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/object-physics"
 * ---
 * -- Control the physics simulation of objects.
 * -- @name Object
 * -- @class table
 */

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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_gets_vector (args, "point", &vector))
	{
		liscr_args_gets_float (args, "dist", &dist);
		liscr_args_gets_float (args, "speed", &speed);
		liphy_object_approach (object, &vector, speed, dist);
	}
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_gets_vector (args, "impulse", &impulse) &&
	    liscr_args_gets_vector (args, "point", &point))
		liphy_object_impulse (object, &point, &impulse);
}

/* @luadoc
 * --- Creates a hinge constraint.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>position: Position vector.</li>
 * --   <li>axis: Axis of rotation.</li></ul>
 * function Object.insert_hinge_constraint(self, args)
 */
static void Object_insert_hinge_constraint (LIScrArgs* args)
{
	LIMatVector pos;
	LIMatVector axis = { 0.0f, 1.0f, 0.0f };
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_gets_vector (args, "position", &pos))
	{
		liscr_args_gets_vector (args, "axis", &axis);
		liphy_constraint_new_hinge (module->physics, object, &pos, &axis, 0, 0.0f, 0.0f);
	}
}

/* @luadoc
 * --- Causes the object to jump.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>impulse: Force of impulse. (required)</li></ul>
 * function Object.jump(self, args)
 */
static void Object_jump (LIScrArgs* args)
{
	LIMatVector impulse;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_gets_vector (args, "impulse", &impulse))
		liphy_object_jump (object, &impulse);
}

/* @luadoc
 * --- Sweeps a sphere relative to the object.
 * --
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>src: Start point vector. (required)</li>
 * --   <li>dst: End point vector. (required)</li>
 * --   <li>radius: Sphere radius.</li></ul>
 * -- @return Table with point, normal, and object. Nil if no collision occurred.
 * function Object.sweep_sphere(self, args)
 */
static void Object_sweep_sphere (LIScrArgs* args)
{
	float radius = 0.5f;
	LIMatVector start;
	LIMatVector end;
	LIEngObject* hitobj;
	LIExtModule* module;
	LIMatVector vector;
	LIPhyCollision result;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (!liscr_args_gets_vector (args, "src", &start) ||
	    !liscr_args_gets_vector (args, "dst", &end))
		return;
	liscr_args_gets_float (args, "radius", &radius);

	if (liphy_object_sweep_sphere (object, &start, &end, radius, &result))
	{
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
		liscr_args_sets_float (args, "fraction", result.fraction);
		liscr_args_sets_vector (args, "point", &result.point);
		liscr_args_sets_vector (args, "normal", &result.normal);
		if (result.object != NULL)
		{
			hitobj = liphy_object_get_userdata (result.object);
			if (hitobj != NULL && hitobj->script != NULL)
				liscr_args_sets_data (args, "object", hitobj->script);
		}
		if (result.terrain != NULL)
		{
			liphy_terrain_get_tile (result.terrain, result.terrain_index, &vector);
			liscr_args_sets_vector (args, "tile", &vector);
		}
	}
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_angular (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_angular (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_angular (object, &vector);
}

/* @luadoc
 * --- Collision group bitmask.
 * --
 * -- @name Object.collision_group
 * -- @class table
 */
static void Object_getter_collision_group (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_int (args, liphy_object_get_collision_group (object));
}
static void Object_setter_collision_group (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_group (object, value);
}

/* @luadoc
 * --- Collision bitmask.
 * --
 * -- @name Object.collision_mask
 * -- @class table
 */
static void Object_getter_collision_mask (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_int (args, liphy_object_get_collision_mask (object));
}
static void Object_setter_collision_mask (LIScrArgs* args)
{
	int value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_mask (object, value);
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_gravity (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_gravity (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity (object, &vector);
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_bool (args, liphy_object_get_ground (object));
}

/* @luadoc
 * --- Mass.
 * --
 * -- @name Object.mass
 * -- @class table
 */
static void Object_getter_mass (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_mass (object));
}
static void Object_setter_mass (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_mass (object, value);
}

/* @luadoc
 * --- Movement direction.
 * -- <br/>
 * -- Only used by kinematic objects. The value of -1 means that the creature is
 * -- moving forward at walking speed. The value of 1 means backward, and the
 * -- value of 0 means no strafing.
 * --
 * -- @name Object.movement
 * -- @class table
 */
static void Object_getter_movement (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_movement (object));
}
static void Object_setter_movement (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_movement (object, value);
}

/* @luadoc
 * --- Physics simulation mode.
 * -- <br/>
 * -- Specifies the physics simulation mode of the object. The recognized values are:<ul>
 * -- <li>"kinematic": Kinematic character.</li>
 * -- <li>"none": Not included to the simulation.</li>
 * -- <li>"rigid": Rigid body simulation.</li>
 * -- <li>"static": Static obstacle.</li>
 * -- <li>"vehicle": Vehicle physics.</li></ul>
 */
static void Object_getter_physics (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	switch (liphy_object_get_control_mode (object))
	{
		case LIPHY_CONTROL_MODE_NONE:
			liscr_args_seti_string (args, "none");
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			liscr_args_seti_string (args, "kinematic");
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			liscr_args_seti_string (args, "rigid");
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			liscr_args_seti_string (args, "static");
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			liscr_args_seti_string (args, "vehicle");
			break;
		default:
			lisys_assert (0 && "invalid physics control mode");
			break;
	}
}
static void Object_setter_physics (LIScrArgs* args)
{
	const char* str;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_string (args, 0, &str))
	{
		if (!strcmp (str, "none"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_NONE);
		else if (!strcmp (str, "kinematic"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_CHARACTER);
		else if (!strcmp (str, "rigid"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_RIGID);
		else if (!strcmp (str, "static"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_STATIC);
		else if (!strcmp (str, "vehicle"))
			liphy_object_set_control_mode (object, LIPHY_CONTROL_MODE_VEHICLE);
	}
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_speed (object));
}
static void Object_setter_speed (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_speed (object, value);
}

/* @luadoc
 * --- Strafing direction.
 * -- <br/>
 * -- Only used by kinematic objects. The value of -1 means that the creature is
 * -- strafing to the left at walking speed. The value of 1 means right, and the
 * -- value of 0 means no strafing.
 * --
 * -- @name Object.strafing
 * -- @class table
 */
static void Object_getter_strafing (LIScrArgs* args)
{
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liscr_args_seti_float (args, liphy_object_get_strafing (object));
}
static void Object_setter_strafing (LIScrArgs* args)
{
	float value;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_strafing (object, value);
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
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	liphy_object_get_velocity (object, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void Object_setter_velocity (LIScrArgs* args)
{
	LIMatVector vector;
	LIExtModule* module;
	LIPhyObject* object;

	/* Get physics object. */
	module = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_PHYSICS_OBJECT);
	object = liphy_physics_find_object (module->physics, ((LIEngObject*) args->self)->id);
	if (object == NULL)
		return;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_velocity (object, &vector);
}

/*****************************************************************************/

void liext_script_object (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_PHYSICS_OBJECT, data);
	liscr_class_insert_mfunc (self, "approach", Object_approach);
	liscr_class_insert_mfunc (self, "impulse", Object_impulse);
	liscr_class_insert_mfunc (self, "insert_hinge_constraint", Object_insert_hinge_constraint);
	liscr_class_insert_mfunc (self, "jump", Object_jump);
	liscr_class_insert_mfunc (self, "sweep_sphere", Object_sweep_sphere);
	liscr_class_insert_mvar (self, "angular", Object_getter_angular, Object_setter_angular);
	liscr_class_insert_mvar (self, "collision_group", Object_getter_collision_group, Object_setter_collision_group);
	liscr_class_insert_mvar (self, "collision_mask", Object_getter_collision_mask, Object_setter_collision_mask);
	liscr_class_insert_mvar (self, "gravity", Object_getter_gravity, Object_setter_gravity);
	liscr_class_insert_mvar (self, "ground", Object_getter_ground, NULL);
	liscr_class_insert_mvar (self, "mass", Object_getter_mass, Object_setter_mass);
	liscr_class_insert_mvar (self, "physics", Object_getter_physics, Object_setter_physics);
	liscr_class_insert_mvar (self, "speed", Object_getter_speed, Object_setter_speed);
	liscr_class_insert_mvar (self, "strafing", Object_getter_strafing, Object_setter_strafing);
	liscr_class_insert_mvar (self, "movement", Object_getter_movement, Object_setter_movement);
	liscr_class_insert_mvar (self, "velocity", Object_getter_velocity, Object_setter_velocity);
}

/** @} */
/** @} */
