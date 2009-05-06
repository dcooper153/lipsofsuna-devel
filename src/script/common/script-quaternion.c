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
 * \addtogroup licom Common
 * @{
 * \addtogroup licomQuaternion Quaternion
 * @{
 */

#include <script/lips-script.h>
#include "lips-common-script.h"

/* @luadoc
 * module "Core.Common.Quaternion"
 * ---
 * -- Present rotations in three dimensions.
 * -- @name Quaternion
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Calculates the sum of two quaternions.
 * --
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @return New quaternion.
 * function Quaternion.__add(self, quat)
 */
static int
Quaternion___add (lua_State* lua)
{
	limatQuaternion tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_QUATERNION);

	tmp = limat_quaternion_add (*((limatQuaternion*) a->data), *((limatQuaternion*) b->data));
	c = liscr_quaternion_new (script, &tmp);
	if (c != NULL)
	{
		liscr_pushdata (lua, c);
		liscr_data_unref (c, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

static int
Quaternion___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	free (self->data);
	liscr_data_free (self);
	return 0;
}

/* @luadoc
 * ---
 * -- Multiplies the quaternion with another value.
 * --
 * -- The second value can be a scalar, a vector, or another quaternion.
 * -- If it is a scalar, all the components of the quaternion are multiplied by it.
 * -- If it is a vector, the vector is rotated by the quaternion, and
 * -- if it is another quaternion, the rotations of the quaternions are concatenated.
 * --
 * -- @param self Quaternion.
 * -- @param value Quaternion, vector, or number.
 * -- @return New quaternion or vector.
 * function Quaternion.__mul(self, value)
 */
static int
Quaternion___mul (lua_State* lua)
{
	float s;
	limatQuaternion q;
	limatVector v;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	if (lua_isnumber (lua, 2))
	{
		/* Multiply by scalar. */
		s = luaL_checknumber (lua, 2);
		q = limat_quaternion_init (
			((limatQuaternion*) a->data)->x * s,
			((limatQuaternion*) a->data)->y * s,
			((limatQuaternion*) a->data)->z * s,
			((limatQuaternion*) a->data)->w * s);
		c = liscr_quaternion_new (script, &q);
		if (c != NULL)
		{
			liscr_pushdata (lua, c);
			liscr_data_unref (c, NULL);
		}
		else
			lua_pushnil (lua);
	}
	else if (liscr_isdata (lua, 2, LICOM_SCRIPT_VECTOR))
	{
		/* Transform vector. */
		b = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);
		v = limat_quaternion_transform (*((limatQuaternion*) a->data), *((limatVector*) b->data));
		c = liscr_vector_new (script, &v);
		if (c != NULL)
		{
			liscr_pushdata (lua, c);
			liscr_data_unref (c, NULL);
		}
		else
			lua_pushnil (lua);
	}
	else if (liscr_isdata (lua, 2, LICOM_SCRIPT_QUATERNION))
	{
		/* Concatenate rotations. */
		b = liscr_checkdata (lua, 2, LICOM_SCRIPT_QUATERNION);
		q = limat_quaternion_multiply (*((limatQuaternion*) a->data), *((limatQuaternion*) b->data));
		c = liscr_quaternion_new (script, &q);
		if (c != NULL)
		{
			liscr_pushdata (lua, c);
			liscr_data_unref (c, NULL);
		}
		else
			lua_pushnil (lua);
	}
	else
		luaL_argerror (lua, 2, "excepted number, vector or quaternion");

	return 1;
}

/* @luadoc
 * ---
 * -- Subtracts a quaternion from another.
 * --
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @return New quaternion.
 * function Quaternion.__sub(self, quat)
 */
static int
Quaternion___sub (lua_State* lua)
{
	limatQuaternion tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_QUATERNION);

	tmp = limat_quaternion_subtract (*((limatQuaternion*) a->data), *((limatQuaternion*) b->data));
	c = liscr_quaternion_new (script, &tmp);
	if (c != NULL)
	{
		liscr_pushdata (lua, c);
		liscr_data_unref (c, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Creates a new quaternion.
 * --
 * -- @param self Quaternion class.
 * -- @param x Optional X value, default is 0.
 * -- @param y Optional Y value, default is 0.
 * -- @param z Optional Z value, default is 0.
 * -- @param w Optional W value, default is 1.
 * -- @return New quaternion.
 * function Quaternion.new(self, x, y, z, w)
 */
static int
Quaternion_new (lua_State* lua)
{
	limatQuaternion vec;
	liscrData* self;
	liscrScript* script = liscr_script (lua);

	if (!lua_isnoneornil (lua, 2))
		vec.x = luaL_checknumber (lua, 2);
	else
		vec.x = 0.0f;
	if (!lua_isnoneornil (lua, 3))
		vec.y = luaL_checknumber (lua, 3);
	else
		vec.y = 0.0f;
	if (!lua_isnoneornil (lua, 4))
		vec.z = luaL_checknumber (lua, 4);
	else
		vec.z = 0.0f;
	if (!lua_isnoneornil (lua, 5))
		vec.z = luaL_checknumber (lua, 5);
	else
		vec.z = 1.0f;
	self = liscr_quaternion_new (script, &vec);
	if (self == NULL)
		return 0;

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
}

/* @luadoc
 * ---
 * -- Normalized linear interpolation.
 * --
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @return New quaternion.
 * function Quaternion.nlerp(self, quat)
 */
static int
Quaternion_nlerp (lua_State* lua)
{
	float val;
	limatQuaternion tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_QUATERNION);
	val = luaL_checknumber (lua, 3);

	tmp = limat_quaternion_nlerp (*((limatQuaternion*) a->data), *((limatQuaternion*) b->data), val);
	c = liscr_quaternion_new (script, &tmp);
	if (c != NULL)
	{
		liscr_pushdata (lua, c);
		liscr_data_unref (c, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Calculates the normalized form of the quaternion.
 * --
 * -- @param self Quaternion.
 * -- @return New quaternion.
 * function Quaternion.normalize(self)
 */
static int
Quaternion_normalize (lua_State* lua)
{
	limatQuaternion tmp;
	liscrData* a;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	tmp = limat_quaternion_normalize (*((limatQuaternion*) a->data));
	c = liscr_quaternion_new (script, &tmp);
	if (c != NULL)
	{
		liscr_pushdata (lua, c);
		liscr_data_unref (c, NULL);
	}
	else
		lua_pushnil (lua);
	return 1;
}

/* @luadoc
 * ---
 * -- Length.
 * -- @name Quaternion.length
 * -- @class table
 */
static int
Quaternion_getter_length (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	lua_pushnumber (lua, limat_quaternion_get_length (*((limatQuaternion*) self->data)));
	return 1;
}

/* @luadoc
 * ---
 * -- W value.
 * -- @name Quaternion.w
 * -- @class table
 */
static int
Quaternion_getter_w (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	lua_pushnumber (lua, ((limatQuaternion*) self->data)->w);
	return 1;
}
static int
Quaternion_setter_w (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	value = luaL_checknumber (lua, 3);

	((limatQuaternion*) self->data)->w = value;
	return 0;
}

/* @luadoc
 * ---
 * -- X value.
 * -- @name Quaternion.x
 * -- @class table
 */
static int
Quaternion_getter_x (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	lua_pushnumber (lua, ((limatQuaternion*) self->data)->x);
	return 1;
}
static int
Quaternion_setter_x (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	value = luaL_checknumber (lua, 3);

	((limatQuaternion*) self->data)->x = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Y value.
 * -- @name Quaternion.y
 * -- @class table
 */
static int
Quaternion_getter_y (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	lua_pushnumber (lua, ((limatQuaternion*) self->data)->y);
	return 1;
}
static int
Quaternion_setter_y (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	value = luaL_checknumber (lua, 3);

	((limatQuaternion*) self->data)->y = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Z value.
 * -- @name Quaternion.z
 * -- @class table
 */
static int
Quaternion_getter_z (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);

	lua_pushnumber (lua, ((limatQuaternion*) self->data)->z);
	return 1;
}
static int
Quaternion_setter_z (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_QUATERNION);
	value = luaL_checknumber (lua, 3);

	((limatQuaternion*) self->data)->z = value;
	return 0;
}

/*****************************************************************************/

static liscrData*
private_convert (liscrScript* script,
                 void*        quaternion)
{
	liscrData* data;

	data = liscr_quaternion_new (script, quaternion);
	if (data == NULL)
		return NULL;

#warning Is it safe to unrefence here?
	liscr_data_unref (data, NULL);
	return data;
}

void
licomQuaternionScript (liscrClass* self,
                       void*       data)
{
	liscr_class_set_convert (self, private_convert);
	liscr_class_insert_func (self, "__add", Quaternion___add);
	liscr_class_insert_func (self, "__gc", Quaternion___gc);
	liscr_class_insert_func (self, "__mul", Quaternion___mul);
	liscr_class_insert_func (self, "__sub", Quaternion___sub);
	liscr_class_insert_func (self, "new", Quaternion_new);
	liscr_class_insert_func (self, "nlerp", Quaternion_nlerp);
	liscr_class_insert_func (self, "normalize", Quaternion_normalize);
	liscr_class_insert_getter (self, "length", Quaternion_getter_length);
	liscr_class_insert_getter (self, "x", Quaternion_getter_x);
	liscr_class_insert_getter (self, "y", Quaternion_getter_y);
	liscr_class_insert_getter (self, "z", Quaternion_getter_z);
	liscr_class_insert_getter (self, "w", Quaternion_getter_w);
	liscr_class_insert_setter (self, "x", Quaternion_setter_x);
	liscr_class_insert_setter (self, "y", Quaternion_setter_y);
	liscr_class_insert_setter (self, "z", Quaternion_setter_z);
	liscr_class_insert_setter (self, "w", Quaternion_setter_w);
}

/**
 * \brief Creates a new script quaternion.
 *
 * The initial reference count of the quaternion is one.
 *
 * \param script Script.
 * \param quaternion Quaternion.
 * \return New script quaternion or NULL.
 */
liscrData*
liscr_quaternion_new (liscrScript*           script,
                      const limatQuaternion* quaternion)
{
	limatQuaternion* tmp;
	liscrData* self;

	tmp = malloc (sizeof (limatQuaternion));
	if (tmp == NULL)
		return NULL;
	*tmp = *quaternion;
	self = liscr_data_new (script, tmp, LICOM_SCRIPT_QUATERNION);
	if (self == NULL)
	{
		free (tmp);
		return NULL;
	}

	return self;
}

/** @} */
/** @} */
