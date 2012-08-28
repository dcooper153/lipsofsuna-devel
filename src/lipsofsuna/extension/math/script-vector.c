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
 * \addtogroup LIExtVector Vector
 * @{
 */

#include "lipsofsuna/script.h"

static void Vector_new (LIScrArgs* args)
{
	LIMatVector vec = { 0.0f, 0.0f, 0.0f };

	liscr_args_seti_vector (args, &vec);
}

static void Vector_add (LIScrArgs* args)
{
	LIScrData* b;
	LIMatVector* v1;
	LIMatVector* v2;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	v1 = args->self;
	v2 = liscr_data_get_data (b);
	*v1 = limat_vector_add (*v1, *v2);
}

static void Vector_mul (LIScrArgs* args)
{
	float s;
	LIMatVector* v1;

	if (!liscr_args_geti_float (args, 0, &s))
		return;

	lisys_assert (!isnan (s));
	v1 = args->self;
	*v1 = limat_vector_multiply (*v1, s);
}

static void Vector_sub (LIScrArgs* args)
{
	LIScrData* b;
	LIMatVector* v1;
	LIMatVector* v2;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	v1 = args->self;
	v2 = liscr_data_get_data (b);
	*v1 = limat_vector_subtract (*v1, *v2);
}

static void Vector_tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatVector* self;

	self = args->self;
	snprintf (buffer, 256, "Vector(%g,%g,%g)", self->x, self->y, self->z);
	liscr_args_seti_string (args, buffer);
}

static void Vector_cross (LIScrArgs* args)
{
	LIScrData* b;
	LIMatVector* v1;
	LIMatVector* v2;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	v1 = args->self;
	v2 = liscr_data_get_data (b);
	*v1 = limat_vector_cross (*v1, *v2);
}

static void Vector_dot (LIScrArgs* args)
{
	float tmp;
	LIScrData* b;
	LIMatVector* v1;
	LIMatVector* v2;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	v1 = args->self;
	v2 = liscr_data_get_data (b);
	tmp = limat_vector_dot (*v1, *v2);
	liscr_args_seti_float (args, tmp);
}

static void Vector_normalize (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	*v1 = limat_vector_normalize (*v1);
}

static void Vector_transform (LIScrArgs* args)
{
	LIMatQuaternion q;
	LIMatVector v;
	LIMatVector* self;

	if (!liscr_args_geti_quaternion (args, 0, &q))
		return;
	self = args->self;
	*self = limat_quaternion_transform (q, *self);
	if (liscr_args_geti_vector (args, 1, &v))
		*self = limat_vector_add (*self, v);
	*self = limat_vector_validate (*self);
}

static void Vector_get_length (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_seti_float (args, limat_vector_get_length (*v1));
}

static void Vector_get_x (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_seti_float (args, v1->x);
}
static void Vector_set_x (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_geti_float (args, 0, &v1->x);
	lisys_assert (!isnan (v1->x));
}

static void Vector_get_y (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_seti_float (args, v1->y);
}
static void Vector_set_y (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_geti_float (args, 0, &v1->y);
	lisys_assert (!isnan (v1->y));
}

static void Vector_get_z (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_seti_float (args, v1->z);
}
static void Vector_set_z (LIScrArgs* args)
{
	LIMatVector* v1;

	v1 = args->self;
	liscr_args_geti_float (args, 0, &v1->z);
	lisys_assert (!isnan (v1->z));
}

/*****************************************************************************/

void liext_script_vector (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_VECTOR, "vector_new", Vector_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_add", Vector_add);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_mul", Vector_mul);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_sub", Vector_sub);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_tostring", Vector_tostring);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_cross", Vector_cross);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_dot", Vector_dot);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_normalize", Vector_normalize);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_transform", Vector_transform);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_length", Vector_get_length);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_x", Vector_get_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_x", Vector_set_x);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_y", Vector_get_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_y", Vector_set_y);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_get_z", Vector_get_z);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_VECTOR, "vector_set_z", Vector_set_z);
}

/** @} */
/** @} */
