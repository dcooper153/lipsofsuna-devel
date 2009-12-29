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
 * \addtogroup liscrQuaternion Quaternion
 * @{
 */

#include <script/lips-script.h>

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
static void Quaternion_new (liscrArgs* args)
{
	limatQuaternion quat = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (!liscr_args_gets_float (args, "x", &quat.x))
		liscr_args_geti_float (args, 0, &quat.x);
	if (!liscr_args_gets_float (args, "y", &quat.y))
		liscr_args_geti_float (args, 1, &quat.y);
	if (!liscr_args_gets_float (args, "z", &quat.z))
		liscr_args_geti_float (args, 2, &quat.z);
	if (!liscr_args_gets_float (args, "w", &quat.w))
		liscr_args_geti_float (args, 3, &quat.w);
	liscr_args_seti_quaternion (args, &quat);
}

/* @luadoc
 * ---
 * -- Creates a new quaternion.
 * --
 * -- @param self Quaternion class.
 * -- @param euler Euler angles.
 * -- @return New quaternion.
 * function Quaternion.new(self, euler)
 */
static void Quaternion_new_euler (liscrArgs* args)
{
	float euler[3] = { 0.0f, 0.0f, 0.0f };
	limatQuaternion quat;

	liscr_args_geti_float (args, 0, euler + 0);
	liscr_args_geti_float (args, 1, euler + 1);
	liscr_args_geti_float (args, 2, euler + 2);
	quat = limat_quaternion_euler (euler[0], euler[1], euler[2]);
	liscr_args_seti_quaternion (args, &quat);
}

/* @luadoc
 * ---
 * -- Normalized linear interpolation.
 * --
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @param blend Interpolation factor.
 * -- @return New quaternion.
 * function Quaternion.nlerp(self, quat, blend)
 */
static void Quaternion_nlerp (liscrArgs* args)
{
	float val;
	limatQuaternion tmp;
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LICOM_SCRIPT_QUATERNION, &data) &&
	    liscr_args_geti_float (args, 1, &val))
	{
		tmp = limat_quaternion_nlerp (*((limatQuaternion*) args->self), *((limatQuaternion*) data->data), val);
		liscr_args_seti_quaternion (args, &tmp);
	}
}

/* @luadoc
 * ---
 * -- Calculates the normalized form of the quaternion.
 * --
 * -- @param self Quaternion.
 * -- @return New quaternion.
 * function Quaternion.normalize(self)
 */
static void Quaternion_normalize (liscrArgs* args)
{
	limatQuaternion tmp;

	tmp = limat_quaternion_normalize (*((limatQuaternion*) args->self));
	liscr_args_seti_quaternion (args, &tmp);
}

/* @luadoc
 * ---
 * -- Euler angle presentation of the quaternion.
 * -- @name Quaternion.euler
 * -- @class table
 */
static void Quaternion_getter_euler (liscrArgs* args)
{
	float e[3];
	limatQuaternion* data;

	data = args->self;
	limat_quaternion_get_euler (*data, e + 0, e + 1, e + 2);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, e[0]);
	liscr_args_seti_float (args, e[1]);
	liscr_args_seti_float (args, e[2]);
}

/* @luadoc
 * ---
 * -- Length.
 * -- @name Quaternion.length
 * -- @class table
 */
static void Quaternion_getter_length (liscrArgs* args)
{
	liscr_args_seti_float (args, limat_quaternion_get_length (*((limatQuaternion*) args->self)));
}

/* @luadoc
 * ---
 * -- W value.
 * -- @name Quaternion.w
 * -- @class table
 */
static void Quaternion_getter_w (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatQuaternion*) args->self)->w);
}
static void Quaternion_setter_w (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatQuaternion*) args->self)->w);
}

/* @luadoc
 * ---
 * -- X value.
 * -- @name Quaternion.x
 * -- @class table
 */
static void Quaternion_getter_x (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatQuaternion*) args->self)->x);
}
static void Quaternion_setter_x (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatQuaternion*) args->self)->x);
}

/* @luadoc
 * ---
 * -- Y value.
 * -- @name Quaternion.y
 * -- @class table
 */
static void Quaternion_getter_y (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatQuaternion*) args->self)->y);
}
static void Quaternion_setter_y (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatQuaternion*) args->self)->y);
}

/* @luadoc
 * ---
 * -- Z value.
 * -- @name Quaternion.z
 * -- @class table
 */
static void Quaternion_getter_z (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatQuaternion*) args->self)->z);
}
static void Quaternion_setter_z (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatQuaternion*) args->self)->z);
}

/*****************************************************************************/

void
licomQuaternionScript (liscrClass* self,
                       void*       data)
{
	liscr_class_insert_func (self, "__add", Quaternion___add);
	liscr_class_insert_func (self, "__mul", Quaternion___mul);
	liscr_class_insert_func (self, "__sub", Quaternion___sub);
	liscr_class_insert_cfunc (self, "new", Quaternion_new);
	liscr_class_insert_cfunc (self, "new_euler", Quaternion_new_euler);
	liscr_class_insert_mfunc (self, "nlerp", Quaternion_nlerp);
	liscr_class_insert_mfunc (self, "normalize", Quaternion_normalize);
	liscr_class_insert_mvar (self, "euler", Quaternion_getter_euler, NULL);
	liscr_class_insert_mvar (self, "length", Quaternion_getter_length, NULL);
	liscr_class_insert_mvar (self, "x", Quaternion_getter_x, Quaternion_setter_x);
	liscr_class_insert_mvar (self, "y", Quaternion_getter_y, Quaternion_setter_y);
	liscr_class_insert_mvar (self, "z", Quaternion_getter_z, Quaternion_setter_z);
	liscr_class_insert_mvar (self, "w", Quaternion_getter_w, Quaternion_setter_w);
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
	liscrData* self;

	self = liscr_data_new_alloc (script, sizeof (limatQuaternion), LICOM_SCRIPT_QUATERNION);
	if (self == NULL)
		return NULL;
	*((limatQuaternion*) self->data) = *quaternion;

	return self;
}

/** @} */
/** @} */
