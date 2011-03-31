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
 * \addtogroup LIScr Script
 * @{
 * \addtogroup LIScrQuaternion Quaternion
 * @{
 */

#include "lipsofsuna/script.h"
#include "script-private.h"

/* @luadoc
 * module "builtin/quaternion"
 * --- Present rotations in three dimensions.
 * -- @name Quaternion
 * -- @class table
 */

/* @luadoc
 * --- Creates a new quaternion.
 * -- @param self Quaternion class.
 * -- @param args Arguments.<ul>
 * --   <li>1,x: Optional X value, default is 0.</li>
 * --   <li>2,y: Optional Y value, default is 0.</li>
 * --   <li>3,z: Optional Z value, default is 0.</li>
 * --   <li>4,w: Optional W value, default is 1.</li>
 * --   <li>angle: Angle of rotation in radians.</li>
 * --   <li>axis: Axis of rotation.</li>
 * --   <li>dir: Look direction vector.</li>
 * --   <li>up: Up direction vector.</li></ul>
 * -- @return New quaternion.
 * function Quaternion.new(self, args)
 */
static void Quaternion_new (LIScrArgs* args)
{
	float angle;
	LIMatVector axis;
	LIMatVector dir;
	LIMatVector up;
	LIMatQuaternion quat = { 0.0f, 0.0f, 0.0f, 1.0f };

	if (liscr_args_gets_vector (args, "axis", &axis) &&
	    liscr_args_gets_float (args, "angle", &angle))
	{
		quat = limat_quaternion_rotation (angle, axis);
	}
	else if (liscr_args_gets_vector (args, "dir", &dir) &&
	         liscr_args_gets_vector (args, "up", &up))
	{
		quat = limat_quaternion_look (dir, up);
		quat = limat_quaternion_conjugate (quat);
	}
	else
	{
		if (!liscr_args_gets_float (args, "x", &quat.x))
			liscr_args_geti_float (args, 0, &quat.x);
		if (!liscr_args_gets_float (args, "y", &quat.y))
			liscr_args_geti_float (args, 1, &quat.y);
		if (!liscr_args_gets_float (args, "z", &quat.z))
			liscr_args_geti_float (args, 2, &quat.z);
		if (!liscr_args_gets_float (args, "w", &quat.w))
			liscr_args_geti_float (args, 3, &quat.w);
	}
	liscr_args_seti_quaternion (args, &quat);
}

/* @luadoc
 * --- Creates a new quaternion.
 * -- @param self Quaternion class.
 * -- @param euler Euler angles.
 * -- @return New quaternion.
 * function Quaternion.new_euler(self, euler)
 */
static void Quaternion_new_euler (LIScrArgs* args)
{
	float euler[3] = { 0.0f, 0.0f, 0.0f };
	LIMatQuaternion quat;

	liscr_args_geti_float (args, 0, euler + 0);
	liscr_args_geti_float (args, 1, euler + 1);
	liscr_args_geti_float (args, 2, euler + 2);
	quat = limat_quaternion_euler (euler[0], euler[1], euler[2]);
	liscr_args_seti_quaternion (args, &quat);
}

/* @luadoc
 * --- Calculates the sum of two quaternions.
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @return New quaternion.
 * function Quaternion.__add(self, quat)
 */
static void Quaternion___add (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_add (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
	liscr_args_seti_quaternion (args, &tmp);
}

/* @luadoc
 * --- Multiplies the quaternion with another value.<br/>
 * -- The second value can be a scalar, a vector, or another quaternion.
 * -- If it is a scalar, all the components of the quaternion are multiplied by it.
 * -- If it is a vector, the vector is rotated by the quaternion, and
 * -- if it is another quaternion, the rotations of the quaternions are concatenated.
 * -- @param self Quaternion.
 * -- @param value Quaternion, vector, or number.
 * -- @return New quaternion or vector.
 * function Quaternion.__mul(self, value)
 */
static void Quaternion___mul (LIScrArgs* args)
{
	float s;
	LIMatQuaternion q;
	LIMatVector v;
	LIScrData* b;

	if (liscr_args_geti_float (args, 0, &s))
	{
		/* Multiply by scalar. */
		q = limat_quaternion_init (
			((LIMatQuaternion*) args->self)->x * s,
			((LIMatQuaternion*) args->self)->y * s,
			((LIMatQuaternion*) args->self)->z * s,
			((LIMatQuaternion*) args->self)->w * s);
		liscr_args_seti_quaternion (args, &q);
	}
	else if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
	{
		/* Transform vector. */
		v = limat_quaternion_transform (*((LIMatQuaternion*) args->self), *((LIMatVector*) b->data));
		liscr_args_seti_vector (args, &v);
	}
	else if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
	{
		/* Concatenate rotations. */
		q = limat_quaternion_multiply (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
		liscr_args_seti_quaternion (args, &q);
	}
}

/* @luadoc
 * --- Subtracts a quaternion from another.
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @return New quaternion.
 * function Quaternion.__sub(self, quat)
 */
static void Quaternion___sub (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_subtract (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) b->data));
	liscr_args_seti_quaternion (args, &tmp);
}

/* @luadoc
 * --- Converts the quaternion to a string.
 * -- @param self Quaternion.
 * -- @return String.
 * function Quaternion.__tostring(self)
 */
static void Quaternion___tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatQuaternion* self;

	self = args->self;
	snprintf (buffer, 256, "Quaternion(%g,%g,%g,%g)", self->x, self->y, self->z, self->w);
	liscr_args_seti_string (args, buffer);
}

/* @luadoc
 * --- Normalized linear interpolation.
 * -- @param self Quaternion.
 * -- @param quat Quaternion.
 * -- @param blend Interpolation factor.
 * -- @return New quaternion.
 * function Quaternion.nlerp(self, quat, blend)
 */
static void Quaternion_nlerp (LIScrArgs* args)
{
	float val;
	LIMatQuaternion q1;
	LIMatQuaternion q2;

	if (liscr_args_geti_quaternion(args, 0, &q2) &&
	    liscr_args_geti_float (args, 1, &val))
	{
		q1 = *((LIMatQuaternion*) args->self);
		q2 = limat_quaternion_get_nearest (q2, q1);
		q2 = limat_quaternion_nlerp (q1, q2, val);
		liscr_args_seti_quaternion (args, &q2);
	}
}

/* @luadoc
 * --- Calculates the normalized form of the quaternion.
 * -- @param self Quaternion.
 * -- @return New quaternion.
 * function Quaternion.normalize(self)
 */
static void Quaternion_normalize (LIScrArgs* args)
{
	LIMatQuaternion tmp;

	tmp = limat_quaternion_normalize (*((LIMatQuaternion*) args->self));
	liscr_args_seti_quaternion (args, &tmp);
}

/* @luadoc
 * --- The conjugate of the quaternion.
 * -- @name Quaternion.conjugate
 * -- @class table
 */
static void Quaternion_get_conjugate (LIScrArgs* args)
{
	LIMatQuaternion* data;
	LIMatQuaternion tmp;

	data = args->self;
	tmp = limat_quaternion_conjugate (*data);
	liscr_args_seti_quaternion (args, &tmp);
}

/* @luadoc
 * --- Euler angle presentation of the quaternion.
 * -- @name Quaternion.euler
 * -- @class table
 */
static void Quaternion_get_euler (LIScrArgs* args)
{
	float e[3];
	LIMatQuaternion* data;

	data = args->self;
	limat_quaternion_get_euler (*data, e + 0, e + 1, e + 2);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, e[0]);
	liscr_args_seti_float (args, e[1]);
	liscr_args_seti_float (args, e[2]);
}

/* @luadoc
 * --- Length.
 * -- @name Quaternion.length
 * -- @class table
 */
static void Quaternion_get_length (LIScrArgs* args)
{
	liscr_args_seti_float (args, limat_quaternion_get_length (*((LIMatQuaternion*) args->self)));
}

/* @luadoc
 * --- W value.
 * -- @name Quaternion.w
 * -- @class table
 */
static void Quaternion_get_w (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->w);
}
static void Quaternion_set_w (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->w);
}

/* @luadoc
 * --- X value.
 * -- @name Quaternion.x
 * -- @class table
 */
static void Quaternion_get_x (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->x);
}
static void Quaternion_set_x (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->x);
}

/* @luadoc
 * --- Y value.
 * -- @name Quaternion.y
 * -- @class table
 */
static void Quaternion_get_y (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->y);
}
static void Quaternion_set_y (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->y);
}

/* @luadoc
 * --- Z value.
 * -- @name Quaternion.z
 * -- @class table
 */
static void Quaternion_get_z (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->z);
}
static void Quaternion_set_z (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->z);
}

/*****************************************************************************/

void liscr_script_quaternion (
	LIScrClass* self,
	void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Quaternion_new);
	liscr_class_insert_cfunc (self, "new_euler", Quaternion_new_euler);
	liscr_class_insert_mfunc (self, "__add", Quaternion___add);
	liscr_class_insert_mfunc (self, "__mul", Quaternion___mul);
	liscr_class_insert_mfunc (self, "__sub", Quaternion___sub);
	liscr_class_insert_mfunc (self, "__tostring", Quaternion___tostring);
	liscr_class_insert_mfunc (self, "nlerp", Quaternion_nlerp);
	liscr_class_insert_mfunc (self, "normalize", Quaternion_normalize);
	liscr_class_insert_mfunc (self, "get_conjugate", Quaternion_get_conjugate);
	liscr_class_insert_mfunc (self, "get_euler", Quaternion_get_euler);
	liscr_class_insert_mfunc (self, "get_length", Quaternion_get_length);
	liscr_class_insert_mfunc (self, "get_x", Quaternion_get_x);
	liscr_class_insert_mfunc (self, "set_x", Quaternion_set_x);
	liscr_class_insert_mfunc (self, "get_y", Quaternion_get_y);
	liscr_class_insert_mfunc (self, "set_y", Quaternion_set_y);
	liscr_class_insert_mfunc (self, "get_z", Quaternion_get_z);
	liscr_class_insert_mfunc (self, "set_z", Quaternion_set_z);
	liscr_class_insert_mfunc (self, "get_w", Quaternion_get_w);
	liscr_class_insert_mfunc (self, "set_w", Quaternion_set_w);
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
LIScrData* liscr_quaternion_new (
	LIScrScript*           script,
	const LIMatQuaternion* quaternion)
{
	LIScrClass* clss;
	LIScrData* self;

	clss = liscr_script_find_class (script, LISCR_SCRIPT_QUATERNION);
	self = liscr_data_new_alloc (script, sizeof (LIMatQuaternion), clss);
	if (self == NULL)
		return NULL;
	*((LIMatQuaternion*) self->data) = *quaternion;

	return self;
}

/** @} */
/** @} */
