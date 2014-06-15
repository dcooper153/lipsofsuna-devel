/* Lips of Suna
 * Copyright© 2007-2014 Lips of Suna development team.
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
 * \addtogroup LIExtQuaternion Quaternion
 * @{
 */

#include "lipsofsuna/script.h"

static void Quaternion_new (LIScrArgs* args)
{
	LIMatQuaternion quat = { 0.0f, 0.0f, 0.0f, 1.0f };

	liscr_args_geti_float (args, 0, &quat.x);
	liscr_args_geti_float (args, 1, &quat.y);
	liscr_args_geti_float (args, 2, &quat.z);
	liscr_args_geti_float (args, 3, &quat.w);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_new_from_axis (LIScrArgs* args)
{
	float angle = 0.0f;
	LIMatVector axis = { 0.0f, 1.0f, 0.0f };
	LIMatQuaternion quat;

	liscr_args_geti_float (args, 0, &axis.x);
	liscr_args_geti_float (args, 1, &axis.y);
	liscr_args_geti_float (args, 2, &axis.z);
	liscr_args_geti_float (args, 3, &angle);
	quat = limat_quaternion_rotation (angle, axis);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_new_from_dir (LIScrArgs* args)
{
	LIMatVector dir = { 0.0f, 0.0f, -1.0f };
	LIMatVector up = { 0.0f, 1.0f, 0.0f };
	LIMatQuaternion quat;

	liscr_args_geti_float (args, 0, &dir.x);
	liscr_args_geti_float (args, 1, &dir.y);
	liscr_args_geti_float (args, 2, &dir.z);
	liscr_args_geti_float (args, 3, &up.x);
	liscr_args_geti_float (args, 4, &up.y);
	liscr_args_geti_float (args, 5, &up.z);
	quat = limat_quaternion_look (dir, up);
	quat = limat_quaternion_conjugate (quat);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_new_from_euler (LIScrArgs* args)
{
	float euler[3] = { 0.0f, 0.0f, 0.0f };
	LIMatQuaternion quat;

	liscr_args_geti_float (args, 0, euler + 0);
	liscr_args_geti_float (args, 1, euler + 1);
	liscr_args_geti_float (args, 2, euler + 2);
	quat = limat_quaternion_euler (euler[0], euler[1], euler[2]);
	quat = limat_quaternion_validate (quat);
	quat = limat_quaternion_normalize (quat);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_new_from_vectors (LIScrArgs* args)
{
	LIMatQuaternion quat;
	LIMatVector v1 = { 0.0f, 0.0f, 0.0f };
	LIMatVector v2 = { 0.0f, 0.0f, 0.0f };

	liscr_args_geti_float (args, 0, &v1.x);
	liscr_args_geti_float (args, 1, &v1.y);
	liscr_args_geti_float (args, 2, &v1.z);
	liscr_args_geti_float (args, 3, &v2.x);
	liscr_args_geti_float (args, 4, &v2.y);
	liscr_args_geti_float (args, 5, &v2.z);
	quat = limat_quaternion_init_vectors (v1, v2);
	quat = limat_quaternion_validate (quat);
	quat = limat_quaternion_normalize (quat);
	liscr_args_seti_quaternion (args, &quat);
}

static void Quaternion_add (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_add (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) liscr_data_get_data (b)));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_sub (LIScrArgs* args)
{
	LIMatQuaternion tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &b))
		return;

	tmp = limat_quaternion_subtract (*((LIMatQuaternion*) args->self), *((LIMatQuaternion*) liscr_data_get_data (b)));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatQuaternion* self;

	self = args->self;
	snprintf (buffer, 256, "Quaternion(%g,%g,%g,%g)", self->x, self->y, self->z, self->w);
	liscr_args_seti_string (args, buffer);
}

static void Quaternion_concat (LIScrArgs* args)
{
	LIMatQuaternion* self;
	LIMatQuaternion quat;

	if (!liscr_args_geti_quaternion (args, 0, &quat))
		return;
	self = args->self;
	*self = limat_quaternion_multiply (*self, quat);
	*self = limat_quaternion_validate (*self);
}

static void Quaternion_multiply (LIScrArgs* args)
{
	float s;
	LIMatQuaternion* self;

	if (!liscr_args_geti_float (args, 0, &s))
		return;
	self = args->self;
	*self = limat_quaternion_init (self->x * s, self->y * s, self->z * s, self->w * s);
}

static void Quaternion_nlerp (LIScrArgs* args)
{
	float val;
	LIMatQuaternion* self;
	LIMatQuaternion q2;

	if (liscr_args_geti_quaternion(args, 0, &q2) &&
	    liscr_args_geti_float (args, 1, &val))
	{
		self = args->self;
		q2 = limat_quaternion_get_nearest (q2, *self);
		*self = limat_quaternion_nlerp (*self, q2, val);
	}
}

static void Quaternion_normalize (LIScrArgs* args)
{
	LIMatQuaternion tmp;

	tmp = limat_quaternion_normalize (*((LIMatQuaternion*) args->self));
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_set_axis (LIScrArgs* args)
{
	float angle;
	LIMatQuaternion* self;
	LIMatVector axis;

	if (liscr_args_geti_vector (args, 0, &axis) &&
	    liscr_args_geti_float (args, 1, &angle))
	{
		self = args->self;
		*self = limat_quaternion_rotation (angle, axis);
	}
}

static void Quaternion_get_conjugate (LIScrArgs* args)
{
	LIMatQuaternion* data;
	LIMatQuaternion tmp;

	data = args->self;
	tmp = limat_quaternion_conjugate (*data);
	liscr_args_seti_quaternion (args, &tmp);
}

static void Quaternion_set_dir (LIScrArgs* args)
{
	LIMatQuaternion* self;
	LIMatVector v1;
	LIMatVector v2;

	if (liscr_args_geti_vector (args, 0, &v1) &&
	    liscr_args_geti_vector (args, 1, &v2))
	{
		self = args->self;
		*self = limat_quaternion_look (v1, v2);
		*self = limat_quaternion_conjugate (*self);
	}
}

static void Quaternion_get_euler (LIScrArgs* args)
{
	float e[3];
	LIMatQuaternion* data;

	data = args->self;
	limat_quaternion_get_euler (*data, e + 0, e + 1, e + 2);
	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE);
	liscr_args_seti_float (args, limat_number_validate (e[0]));
	liscr_args_seti_float (args, limat_number_validate (e[1]));
	liscr_args_seti_float (args, limat_number_validate (e[2]));
}
static void Quaternion_set_euler (LIScrArgs* args)
{
	float e[3];
	LIMatQuaternion* self;

	if (liscr_args_geti_float (args, 0, e + 0) &&
	    liscr_args_geti_float (args, 1, e + 1) &&
	    liscr_args_geti_float (args, 2, e + 2))
	{
		self = args->self;
		*self = limat_quaternion_euler (e[0], e[1], e[2]);
	}
}

static void Quaternion_get_length (LIScrArgs* args)
{
	liscr_args_seti_float (args, limat_quaternion_get_length (*((LIMatQuaternion*) args->self)));
}

static void Quaternion_get_w (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->w);
}
static void Quaternion_set_w (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->w);
}

static void Quaternion_get_x (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->x);
}
static void Quaternion_set_x (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->x);
}

static void Quaternion_get_y (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->y);
}
static void Quaternion_set_y (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->y);
}

static void Quaternion_get_z (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatQuaternion*) args->self)->z);
}
static void Quaternion_set_z (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatQuaternion*) args->self)->z);
}

/*****************************************************************************/

void liext_script_quaternion (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new", Quaternion_new);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new_from_axis", Quaternion_new_from_axis);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new_from_dir", Quaternion_new_from_dir);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new_from_euler", Quaternion_new_from_euler);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_new_from_vectors", Quaternion_new_from_vectors);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_add", Quaternion_add);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_concat", Quaternion_concat);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_multiply", Quaternion_multiply);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_sub", Quaternion_sub);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_tostring", Quaternion_tostring);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_nlerp", Quaternion_nlerp);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_normalize", Quaternion_normalize);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_axis", Quaternion_set_axis);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_conjugate", Quaternion_get_conjugate);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_dir", Quaternion_set_dir);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_euler", Quaternion_get_euler);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_euler", Quaternion_set_euler);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_length", Quaternion_get_length);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_x", Quaternion_get_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_x", Quaternion_set_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_y", Quaternion_get_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_y", Quaternion_set_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_z", Quaternion_get_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_z", Quaternion_set_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_get_w", Quaternion_get_w);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_QUATERNION, "quaternion_set_w", Quaternion_set_w);
}

/** @} */
/** @} */
