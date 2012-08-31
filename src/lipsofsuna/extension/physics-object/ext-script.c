/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExtPhysicsObject PhysicsObject
 * @{
 */

#include "lipsofsuna/extension/physics/ext-module.h"
#include "ext-module.h"

static void PhysicsObject_new (LIScrArgs* args)
{
	LIExtPhysicsObjectModule* module;
	LIPhyObject* self;
	LIScrData* data;

	/* Find the class data. */
	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);

	/* Allocate the object. */
	self = liphy_object_new (module->physics);
	if (self == NULL)
		return;

	/* Allocate the userdata. */
	data = liscr_data_new (args->script, args->lua, self, LIEXT_SCRIPT_PHYSICS_OBJECT, liphy_object_free);
	if (data == NULL)
	{
		liphy_object_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void PhysicsObject_approach (LIScrArgs* args)
{
	float dist = 0.0f;
	float speed = 1.0f;
	LIMatVector vector;

	if (liscr_args_gets_vector (args, "point", &vector))
	{
		liscr_args_gets_float (args, "dist", &dist);
		liscr_args_gets_float (args, "speed", &speed);
		liphy_object_approach (args->self, &vector, speed, dist);
	}
}

static void PhysicsObject_impulse (LIScrArgs* args)
{
	LIMatVector impulse;
	LIMatVector point = { 0.0f, 0.0f, 0.0f };

	if (liscr_args_geti_vector (args, 0, &impulse) ||
	    liscr_args_gets_vector (args, "impulse", &impulse))
	{
		if (!liscr_args_geti_vector (args, 1, &point))
			liscr_args_gets_vector (args, "point", &point);
		liphy_object_impulse (args->self, &point, &impulse);
	}
}

static void PhysicsObject_insert_hinge_constraint (LIScrArgs* args)
{
	LIMatVector pos;
	LIMatVector axis = { 0.0f, 1.0f, 0.0f };
	LIExtPhysicsObjectModule* module;

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
	if (liscr_args_geti_vector (args, 0, &pos) ||
	    liscr_args_gets_vector (args, "position", &pos))
	{
		if (!liscr_args_geti_vector (args, 1, &axis))
			liscr_args_gets_vector (args, "axis", &axis);
		liphy_constraint_new_hinge (module->physics, args->self, &pos, &axis, 0, 0.0f, 0.0f);
	}
}

static void PhysicsObject_jump (LIScrArgs* args)
{
	LIMatVector impulse;

	if (liscr_args_geti_vector (args, 0, &impulse))
		liphy_object_jump (args->self, &impulse);
}

static void PhysicsObject_get_activated (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liphy_object_get_activated (args->self));
}
static void PhysicsObject_set_activated (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liphy_object_set_activated (args->self, value);
}

static void PhysicsObject_get_angular (LIScrArgs* args)
{
	LIMatVector tmp;

	liphy_object_get_angular (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void PhysicsObject_set_angular (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_angular (args->self, &vector);
}

static void PhysicsObject_get_bounding_box (LIScrArgs* args)
{
	LIMatAabb aabb;

	liphy_object_get_bounds (args->self, &aabb);
	liscr_args_seti_vector (args, &aabb.min);
	liscr_args_seti_vector (args, &aabb.max);
}

static void PhysicsObject_get_center_offset (LIScrArgs* args)
{
	LIMatAabb aabb;
	LIMatVector ctr;

	liphy_object_get_bounds (args->self, &aabb);
	ctr = limat_vector_add (aabb.min, aabb.max);
	ctr = limat_vector_multiply (ctr, 0.5f);
	liscr_args_seti_vector (args, &ctr);
}

static void PhysicsObject_get_collision_group (LIScrArgs* args)
{
	liscr_args_seti_int (args, liphy_object_get_collision_group (args->self));
}
static void PhysicsObject_set_collision_group (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_group (args->self, value);
}

static void PhysicsObject_get_collision_mask (LIScrArgs* args)
{
	liscr_args_seti_int (args, liphy_object_get_collision_mask (args->self));
}
static void PhysicsObject_set_collision_mask (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_collision_mask (args->self, value);
}

static void PhysicsObject_get_contact_events (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liphy_object_get_contact_events (args->self));
}
static void PhysicsObject_set_contact_events (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liphy_object_set_contact_events (args->self, value);
}

static void PhysicsObject_get_gravity (LIScrArgs* args)
{
	LIMatVector tmp;

	liphy_object_get_gravity (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void PhysicsObject_set_gravity (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity (args->self, &vector);
}

static void PhysicsObject_get_friction_liquid (LIScrArgs* args)
{
	float tmp;

	tmp = liphy_object_get_friction_liquid (args->self);
	liscr_args_seti_float (args, tmp);
}
static void PhysicsObject_set_friction_liquid (LIScrArgs* args)
{
	float tmp;

	if (liscr_args_geti_float (args, 0, &tmp))
		liphy_object_set_friction_liquid (args->self, tmp);
}

static void PhysicsObject_get_gravity_liquid (LIScrArgs* args)
{
	LIMatVector tmp;

	liphy_object_get_gravity_liquid (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void PhysicsObject_set_gravity_liquid (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_gravity_liquid (args->self, &vector);
}

static void PhysicsObject_get_ground (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liphy_object_get_ground (args->self));
}

static void PhysicsObject_get_id (LIScrArgs* args)
{
	liscr_args_seti_int (args, liphy_object_get_external_id (args->self));
}

static void PhysicsObject_set_id (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_int (args, 0, &value))
		liphy_object_set_external_id (args->self, value);
}

static void PhysicsObject_get_mass (LIScrArgs* args)
{
	liscr_args_seti_float (args, liphy_object_get_mass (args->self));
}
static void PhysicsObject_set_mass (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_mass (args->self, value);
}

static void PhysicsObject_set_model (LIScrArgs* args)
{
	LIExtPhysicsObjectModule* module;
	LIMdlModel* mdl_model;
	LIPhyModel* phy_model;
	LIScrData* value;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
	{
		module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_PHYSICS_OBJECT);
		mdl_model = liscr_data_get_data (value);
		phy_model = liphy_physics_find_model (module->physics, mdl_model->id);
		liphy_object_set_model (args->self, phy_model);
	}
	else
		liphy_object_set_model (args->self, NULL);
}

static void PhysicsObject_get_movement (LIScrArgs* args)
{
	liscr_args_seti_float (args, liphy_object_get_movement (args->self));
}
static void PhysicsObject_set_movement (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_movement (args->self, value);
}

static void PhysicsObject_get_physics (LIScrArgs* args)
{
	switch (liphy_object_get_control_mode (args->self))
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
static void PhysicsObject_set_physics (LIScrArgs* args)
{
	const char* str;

	if (liscr_args_geti_string (args, 0, &str))
	{
		if (!strcmp (str, "none"))
			liphy_object_set_control_mode (args->self, LIPHY_CONTROL_MODE_NONE);
		else if (!strcmp (str, "kinematic"))
			liphy_object_set_control_mode (args->self, LIPHY_CONTROL_MODE_CHARACTER);
		else if (!strcmp (str, "rigid"))
			liphy_object_set_control_mode (args->self, LIPHY_CONTROL_MODE_RIGID);
		else if (!strcmp (str, "static"))
			liphy_object_set_control_mode (args->self, LIPHY_CONTROL_MODE_STATIC);
		else if (!strcmp (str, "vehicle"))
			liphy_object_set_control_mode (args->self, LIPHY_CONTROL_MODE_VEHICLE);
	}
}

static void PhysicsObject_get_position (LIScrArgs* args)
{
	LIMatTransform tmp;
	LIMatVector* v;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		liphy_object_get_transform (args->self, &tmp);
		v = liscr_data_get_data (data);
		*v = tmp.position;
	}
}

static void PhysicsObject_set_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		liphy_object_get_transform (args->self, &transform);
		transform.position = vector;
		liphy_object_set_transform (args->self, &transform);
	}
}

static void PhysicsObject_get_visible (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liphy_object_get_realized (args->self));
}

static void PhysicsObject_set_visible (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liphy_object_set_realized (args->self, value);
}

static void PhysicsObject_get_rotation (LIScrArgs* args)
{
	LIMatTransform tmp;
	LIMatQuaternion* q;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &data))
	{
		liphy_object_get_transform (args->self, &tmp);
		q = liscr_data_get_data (data);
		*q = tmp.rotation;
	}
}

static void PhysicsObject_set_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatQuaternion quat;

	if (liscr_args_geti_quaternion (args, 0, &quat))
	{
		liphy_object_get_transform (args->self, &transform);
		transform.rotation = quat;
		limat_quaternion_normalize (transform.rotation);
		liphy_object_set_transform (args->self, &transform);
	}
}

static void PhysicsObject_get_shape (LIScrArgs* args)
{
	liscr_args_seti_string (args, liphy_object_get_shape (args->self));
}
static void PhysicsObject_set_shape (LIScrArgs* args)
{
	const char* value;

	if (liscr_args_geti_string (args, 0, &value))
		liphy_object_set_shape (args->self, value);
}

static void PhysicsObject_get_speed (LIScrArgs* args)
{
	liscr_args_seti_float (args, liphy_object_get_speed (args->self));
}
static void PhysicsObject_set_speed (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value) && value >= 0.0f)
		liphy_object_set_speed (args->self, value);
}

static void PhysicsObject_get_strafing (LIScrArgs* args)
{
	liscr_args_seti_float (args, liphy_object_get_strafing (args->self));
}
static void PhysicsObject_set_strafing (LIScrArgs* args)
{
	float value;

	if (liscr_args_geti_float (args, 0, &value))
		liphy_object_set_strafing (args->self, value);
}

static void PhysicsObject_get_velocity (LIScrArgs* args)
{
	LIMatVector tmp;

	liphy_object_get_velocity (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp);
}
static void PhysicsObject_set_velocity (LIScrArgs* args)
{
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
		liphy_object_set_velocity (args->self, &vector);
}

/*****************************************************************************/

void liext_script_physics_object (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_new", PhysicsObject_new);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_approach", PhysicsObject_approach);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_impulse", PhysicsObject_impulse);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_insert_hinge_constraint", PhysicsObject_insert_hinge_constraint);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_jump", PhysicsObject_jump);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_activated", PhysicsObject_get_activated);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_activated", PhysicsObject_set_activated);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_angular", PhysicsObject_get_angular);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_angular", PhysicsObject_set_angular);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_bounding_box", PhysicsObject_get_bounding_box);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_center_offset", PhysicsObject_get_center_offset);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_collision_group", PhysicsObject_get_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_collision_group", PhysicsObject_set_collision_group);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_collision_mask", PhysicsObject_get_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_collision_mask", PhysicsObject_set_collision_mask);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_contact_events", PhysicsObject_get_contact_events);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_contact_events", PhysicsObject_set_contact_events);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_friction_liquid", PhysicsObject_get_friction_liquid);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_friction_liquid", PhysicsObject_set_friction_liquid);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_gravity", PhysicsObject_get_gravity);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_gravity", PhysicsObject_set_gravity);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_gravity_liquid", PhysicsObject_get_gravity_liquid);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_gravity_liquid", PhysicsObject_set_gravity_liquid);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_ground", PhysicsObject_get_ground);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_id", PhysicsObject_get_id);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_id", PhysicsObject_set_id);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_mass", PhysicsObject_get_mass);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_mass", PhysicsObject_set_mass);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_model", PhysicsObject_set_model);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_movement", PhysicsObject_get_movement);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_movement", PhysicsObject_set_movement);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_physics", PhysicsObject_get_physics);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_physics", PhysicsObject_set_physics);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_position", PhysicsObject_get_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_position", PhysicsObject_set_position);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_rotation", PhysicsObject_get_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_rotation", PhysicsObject_set_rotation);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_shape", PhysicsObject_get_shape);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_shape", PhysicsObject_set_shape);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_speed", PhysicsObject_get_speed);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_speed", PhysicsObject_set_speed);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_strafing", PhysicsObject_get_strafing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_strafing", PhysicsObject_set_strafing);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_velocity", PhysicsObject_get_velocity);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_velocity", PhysicsObject_set_velocity);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_get_visible", PhysicsObject_get_visible);
	liscr_script_insert_mfunc (self, LIEXT_SCRIPT_PHYSICS_OBJECT, "physics_object_set_visible", PhysicsObject_set_visible);
}

/** @} */
/** @} */
