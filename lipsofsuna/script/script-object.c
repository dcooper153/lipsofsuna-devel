/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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

#include <engine/lips-engine.h>
#include <script/lips-script.h>

/* @luadoc
 * module "Core.Common.Object"
 * ---
 * -- Manipulate objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Use bounding box shape for collisions.
 * -- @name Object.SHAPE_MODE_BOX
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Use capsule shape for collisions.
 * -- @name Object.SHAPE_MODE_CAPSULE
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Use triangle mesh shape for collisions.
 * -- @name Object.SHAPE_MODE_CONCAVE
 * -- @class table
 */
/* @luadoc
 * ---
 * -- Use convex shape for collisions.
 * -- @name Object.SHAPE_MODE_CONVEX
 * -- @class table
 */

static int
Object___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lieng_object_free (self->data);
	liscr_data_free (self);
	return 0;
}

static int
Object_approach (lua_State* lua)
{
	liscrData* object;
	liscrData* vector;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	/* FIXME: Speed not supported. */
	lieng_object_approach (object->data, vector->data, 1.0f);
	return 0;
}

/* @luadoc
 * ---
 * -- Finds a bone or an anchor by name.
 * --
 * -- @param self Object.
 * -- @param name Node name.
 * function Object.find_node(self, name)
 */
static int
Object_find_node (lua_State* lua)
{
	const char* name;
	limatTransform transform;
	limdlNode* node;
	liscrData* tmp;
	liscrData* object;
	liscrScript* script;

	script = liscr_script (lua);
	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	name = luaL_checkstring (lua, 2);

	/* Find node. */
	node = limdl_pose_find_node (LIENG_OBJECT (object->data)->pose, name);
	if (node == NULL)
		return 0;

	/* Return transform. */
	limdl_node_get_world_transform (node, &transform);
	tmp = liscr_vector_new (script, &transform.position);
	if (tmp != NULL)
	{
		liscr_pushdata (lua, tmp);
		liscr_data_unref (tmp, NULL);
	}
	else
		lua_pushnil (lua);
	tmp = liscr_quaternion_new (script, &transform.rotation);
	if (tmp != NULL)
	{
		liscr_pushdata (lua, tmp);
		liscr_data_unref (tmp, NULL);
	}
	else
		lua_pushnil (lua);

	return 2;
}

/* @luadoc
 * ---
 * -- Lets an impulse force affect the object.
 * --
 * -- @param self Object.
 * -- @param point Point of impulse.
 * -- @param impulse Force of impulse.
 * function Object.impulse(self, point, impulse)
 */
static int
Object_impulse (lua_State* lua)
{
	liscrData* object;
	liscrData* point;
	liscrData* impulse;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	point = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
	impulse = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	lieng_object_impulse (object->data, point->data, impulse->data);
	return 0;
}

/* @luadoc
 * ---
 * -- Causes the object to jump.
 * --
 * -- @param self Object.
 * -- @param impulse Jump force.
 * function Object.jump(self, impulse)
 */
static int
Object_jump (lua_State* lua)
{
	liscrData* object;
	liscrData* impulse;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	impulse = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	lieng_object_jump (object->data, impulse->data);
	return 0;
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
static int
Object_getter_angular_momentum (lua_State* lua)
{
	limatVector tmp;
	liscrData* vector;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lieng_object_get_angular_momentum (object->data, &tmp);
	vector = liscr_vector_new (script, &tmp);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Object_setter_angular_momentum (lua_State* lua)
{
	liscrData* object;
	liscrData* vector;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	lieng_object_set_angular_momentum (object->data, vector->data);
	return 0;
}

/* @luadoc
 * ---
 * -- Class type.
 * --
 * -- @name Object.class
 * -- @class table
 */
static int
Object_getter_class (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	liscr_pushclass (lua, object->clss);
	return 1;
}
static int
Object_setter_class (lua_State* lua)
{
	liscrData* object;
	liscrClass* clss;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	clss = liscr_checkanyclass (lua, 3);

	if (!liscr_data_set_class (object, clss))
		luaL_argerror (lua, 3, "incompatible class");
	return 0;
}

/* @luadoc
 * ---
 * -- Collision group bitmask.
 * --
 * -- @name Object.collision_group
 * -- @class table
 */
static int
Object_getter_collision_group (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushnumber (lua, lieng_object_get_collision_group (object->data));
	return 1;
}
static int
Object_setter_collision_group (lua_State* lua)
{
	int mask;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	mask = luaL_checkinteger (lua, 3);

	lieng_object_set_collision_group (object->data, mask);
	return 0;
}

/* @luadoc
 * ---
 * -- Collision bitmask.
 * --
 * -- @name Object.collision_mask
 * -- @class table
 */
static int
Object_getter_collision_mask (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushnumber (lua, lieng_object_get_collision_mask (object->data));
	return 1;
}
static int
Object_setter_collision_mask (lua_State* lua)
{
	int mask;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	mask = luaL_checkinteger (lua, 3);

	lieng_object_set_collision_mask (object->data, mask);
	return 0;
}

/* @luadoc
 * ---
 * -- Gravity vector.
 * --
 * -- @name Object.gravity
 * -- @class table
 */
static int
Object_getter_gravity (lua_State* lua)
{
	limatVector tmp;
	liscrData* vector;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	liphy_object_get_gravity (LIENG_OBJECT (object->data)->physics, &tmp);
	vector = liscr_vector_new (script, &tmp);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Object_setter_gravity (lua_State* lua)
{
	liscrData* object;
	liscrData* vector;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	liphy_object_set_gravity (LIENG_OBJECT (object->data)->physics, vector->data);
	return 0;
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
static int
Object_getter_ground (lua_State* lua)
{
	int tmp;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	tmp = lieng_object_get_ground (object->data);
	lua_pushboolean (lua, tmp);
	return 1;
}

/* @luadoc
 * ---
 * -- Identification number.
 * --
 * -- @name Object.id
 * -- @class table
 */
static int
Object_getter_id (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushnumber (lua, LIENG_OBJECT (object->data)->id);
	return 1;
}

/* @luadoc
 * ---
 * -- Mass.
 * --
 * -- @name Object.mass
 * -- @class table
 */
static int
Object_getter_mass (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushnumber (lua, lieng_object_get_mass (object->data));
	return 1;
}
static int
Object_setter_mass (lua_State* lua)
{
	float value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = (int) luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "invalid mass");

	lieng_object_set_mass (object->data, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Model string.
 * -- @name Object.model
 * -- @class table
 */
static int
Object_getter_model (lua_State* lua)
{
	int id;
	liengModel* model;
	liengObject* data;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	data = object->data;

	id = lieng_object_get_model_code (object->data);
	model = lieng_engine_find_model_by_code (data->engine, id);
	if (model != NULL)
		lua_pushstring (lua, model->name);
	else
		lua_pushstring (lua, "");
	return 1;
}
static int
Object_setter_model (lua_State* lua)
{
	const char* name;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	name = luaL_checkstring (lua, 3);

	lieng_object_set_model_name (object->data, name);
	return 0;
}

/* @luadoc
 * ---
 * -- Position.
 * --
 * -- @name Object.position
 * -- @class table
 */
static int
Object_getter_position (lua_State* lua)
{
	limatTransform tmp;
	liscrData* vector;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lieng_object_get_transform (object->data, &tmp);
	vector = liscr_vector_new (script, &tmp.position);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Object_setter_position (lua_State* lua)
{
	limatTransform transform;
	liscrData* object;
	liscrData* vector;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	lieng_object_get_transform (object->data, &transform);
	transform.position = *((limatVector*) vector->data);
	lieng_object_set_transform (object->data, &transform);
	return 0;
}

/* @luadoc
 * ---
 * -- Visibility status.
 * --
 * -- @name Object.realized
 * -- @class table
 */
static int
Object_getter_realized (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushboolean (lua, lieng_object_get_realized (object->data));
	return 1;
}
static int
Object_setter_realized (lua_State* lua)
{
	int value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = lua_toboolean (lua, 3);

	if (!lieng_object_set_realized (object->data, value))
		return 0;
	return 0;
}

/* @luadoc
 * ---
 * -- Rotational orientation.
 * --
 * -- @name Object.rotation
 * -- @class table
 */
static int
Object_getter_rotation (lua_State* lua)
{
	limatTransform transform;
	liscrData* quat;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lieng_object_get_transform (object->data, &transform);
	quat = liscr_quaternion_new (script, &transform.rotation);
	if (quat != NULL)
	{
		liscr_pushdata (lua, quat);
		liscr_data_unref (quat, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Object_setter_rotation (lua_State* lua)
{
	limatQuaternion tmp;
	limatTransform transform;
	liscrData* object;
	liscrData* quat;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	quat = liscr_checkdata (lua, 3, LICOM_SCRIPT_QUATERNION);

	tmp = *((limatQuaternion*) quat->data);
	lieng_object_get_transform (object->data, &transform);
	transform.rotation = tmp;
	lieng_object_set_transform (object->data, &transform);

	return 0;
}

/* @luadoc
 * ---
 * -- Save enabled flag.
 * --
 * -- @name Object.save
 * -- @class table
 */
static int
Object_getter_save (lua_State* lua)
{
	int flags;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	flags = lieng_object_get_flags (object->data);
	lua_pushboolean (lua, (flags & (LIENG_OBJECT_FLAG_SAVE)) != 0);
	return 1;
}
static int
Object_setter_save (lua_State* lua)
{
	int flags;
	int value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = lua_toboolean (lua, 3);

	flags = lieng_object_get_flags (object->data);
	if (value)
		flags |= LIENG_OBJECT_FLAG_SAVE;
	else
		flags &= ~LIENG_OBJECT_FLAG_SAVE;
	lieng_object_set_flags (object->data, flags);
	return 0;
}

/* @luadoc
 * ---
 * -- Selection status flag.
 * --
 * -- @name Object.selected
 * -- @class table
 */
static int
Object_getter_selected (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushboolean (lua, lieng_object_get_selected (object->data));
	return 1;
}
static int
Object_setter_selected (lua_State* lua)
{
	int value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = lua_toboolean (lua, 3);

	lieng_object_set_selected (object->data, value);
	return 0;
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
static int
Object_getter_speed (lua_State* lua)
{
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushnumber (lua, lieng_object_get_speed (object->data));
	return 1;
}
static int
Object_setter_speed (lua_State* lua)
{
	float value;
	liscrData* object;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	value = luaL_checknumber (lua, 3);
	luaL_argcheck (lua, value >= 0.0f, 3, "invalid speed");

	lieng_object_set_speed (object->data, value);
	return 0;
}

/* @luadoc
 * ---
 * -- Validity flag.
 * --
 * -- Evaluates to true if the object hasn't been destroyed. This is
 * -- the only operation that can be performed to destroyed objects.
 * --
 * -- @name Object.valid
 * -- @class table
 */
static int
Object_getter_valid (lua_State* lua)
{
	liscrData* object;

	object = liscr_isdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lua_pushboolean (lua, !object->invalid);
	return 1;
}

/* @luadoc
 * ---
 * -- Linear velocity.
 * --
 * -- @name Object.velocity
 * -- @class table
 */
static int
Object_getter_velocity (lua_State* lua)
{
	limatVector tmp;
	liscrData* vector;
	liscrData* object;
	liscrScript* script = liscr_script (lua);

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);

	lieng_object_get_velocity (object->data, &tmp);
	vector = liscr_vector_new (script, &tmp);
	if (vector != NULL)
	{
		liscr_pushdata (lua, vector);
		liscr_data_unref (vector, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}
static int
Object_setter_velocity (lua_State* lua)
{
	liscrData* object;
	liscrData* vector;

	object = liscr_checkdata (lua, 1, LICOM_SCRIPT_OBJECT);
	vector = liscr_checkdata (lua, 3, LICOM_SCRIPT_VECTOR);

	lieng_object_set_velocity (object->data, vector->data);
	return 0;
}

/*****************************************************************************/

void
licomObjectScript (liscrClass* self,
                   void*       data)
{
	liscr_class_insert_func (self, "__gc", Object___gc);
	liscr_class_insert_func (self, "approach", Object_approach);
	liscr_class_insert_func (self, "find_node", Object_find_node);
	liscr_class_insert_func (self, "impulse", Object_impulse);
	liscr_class_insert_func (self, "jump", Object_jump);
	liscr_class_insert_getter (self, "angular_momentum", Object_getter_angular_momentum);
	liscr_class_insert_getter (self, "class", Object_getter_class);
	liscr_class_insert_getter (self, "collision_group", Object_getter_collision_group);
	liscr_class_insert_getter (self, "collision_mask", Object_getter_collision_mask);
	liscr_class_insert_getter (self, "gravity", Object_getter_gravity);
	liscr_class_insert_getter (self, "ground", Object_getter_ground);
	liscr_class_insert_getter (self, "id", Object_getter_id);
	liscr_class_insert_getter (self, "mass", Object_getter_mass);
	liscr_class_insert_getter (self, "model", Object_getter_model);
	liscr_class_insert_getter (self, "position", Object_getter_position);
	liscr_class_insert_getter (self, "realized", Object_getter_realized);
	liscr_class_insert_getter (self, "rotation", Object_getter_rotation);
	liscr_class_insert_getter (self, "save", Object_getter_save);
	liscr_class_insert_getter (self, "selected", Object_getter_selected);
	liscr_class_insert_getter (self, "speed", Object_getter_speed);
	liscr_class_insert_getter (self, "valid", Object_getter_valid);
	liscr_class_insert_getter (self, "velocity", Object_getter_velocity);
	liscr_class_insert_setter (self, "angular_momentum", Object_setter_angular_momentum);
	liscr_class_insert_setter (self, "class", Object_setter_class);
	liscr_class_insert_setter (self, "collision_group", Object_setter_collision_group);
	liscr_class_insert_setter (self, "collision_mask", Object_setter_collision_mask);
	liscr_class_insert_setter (self, "gravity", Object_setter_gravity);
	liscr_class_insert_setter (self, "mass", Object_setter_mass);
	liscr_class_insert_setter (self, "model", Object_setter_model);
	liscr_class_insert_setter (self, "position", Object_setter_position);
	liscr_class_insert_setter (self, "realized", Object_setter_realized);
	liscr_class_insert_setter (self, "rotation", Object_setter_rotation);
	liscr_class_insert_setter (self, "save", Object_setter_save);
	liscr_class_insert_setter (self, "selected", Object_setter_selected);
	liscr_class_insert_setter (self, "speed", Object_setter_speed);
	liscr_class_insert_setter (self, "velocity", Object_setter_velocity);
}

/** @} */
/** @} */
