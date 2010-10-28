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
 * \addtogroup LIScrVector Vector
 * @{
 */

#include "lipsofsuna/script.h"
#include "script-private.h"

/* @luadoc
 * module "builtin/vector"
 * --- Do maths in three dimensions.
 * -- @name Vector
 * -- @class table
 */

/* @luadoc
 * --- Creates a new vector.
 * -- @param clss Vector class.
 * -- @param args Arguments.<ul>
 * --   <li>1,x: X coordinate.</li>
 * --   <li>2,y: Y coordinate.</li>
 * --   <li>3,z: Z coordinate.</li></ul>
 * -- @return New vector.
 * function Vector.new(clss, args)
 */
static void Vector_new (LIScrArgs* args)
{
	LIMatVector vec = { 0.0f, 0.0f, 0.0f };

	if (!liscr_args_gets_float (args, "x", &vec.x))
		liscr_args_geti_float (args, 0, &vec.x);
	if (!liscr_args_gets_float (args, "y", &vec.y))
		liscr_args_geti_float (args, 1, &vec.y);
	if (!liscr_args_gets_float (args, "z", &vec.z))
		liscr_args_geti_float (args, 2, &vec.z);
	liscr_args_seti_vector (args, &vec);
}

/* @luadoc
 * --- Calculates the sum of two vectors.
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return New vector.
 * function Vector.__add(self, vector)
 */
static void Vector___add (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	tmp = limat_vector_add (*((LIMatVector*) args->self), *((LIMatVector*) b->data));
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Multiplies the vector by a scalar.
 * -- @param self Vector.
 * -- @param scalar Scalar.
 * -- @return New vector.
 * function Vector.__mul(self, scalar)
 */
static void Vector___mul (LIScrArgs* args)
{
	float s;
	LIMatVector tmp;

	if (!liscr_args_geti_float (args, 0, &s))
		return;

	tmp = limat_vector_multiply (*((LIMatVector*) args->self), s);
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Subtracts a vector from another.
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return New vector.
 * function Vector.__sub(self, vector)
 */
static void Vector___sub (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* b;

	if (!liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &b))
		return;

	tmp = limat_vector_subtract (*((LIMatVector*) args->self), *((LIMatVector*) b->data));
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Converts the vector to a string.
 * -- @param self Vector.
 * -- @return String.
 * function Vector.__tostring(self)
 */
static void Vector___tostring (LIScrArgs* args)
{
	char buffer[256];
	LIMatVector* self;

	self = args->self;
	snprintf (buffer, 256, "Vector(%g,%g,%g)", self->x, self->y, self->z);
	liscr_args_seti_string (args, buffer);
}

/* @luadoc
 * --- Calculates the cross product of two vectors.
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return New vector.
 * function Vector.cross(self, vector)
 */
static void Vector_cross (LIScrArgs* args)
{
	LIMatVector tmp;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		tmp = limat_vector_cross (*((LIMatVector*) args->self), *((LIMatVector*) data->data));
		liscr_args_seti_vector (args, &tmp);
	}
}

/* @luadoc
 * --- Calculates the dot product of two vectors.
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return Scalar.
 * function Vector.dot(self, vector)
 */
static void Vector_dot (LIScrArgs* args)
{
	float tmp;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		tmp = limat_vector_dot (*((LIMatVector*) args->self), *((LIMatVector*) data->data));
		liscr_args_seti_float (args, tmp);
	}
}

/* @luadoc
 * --- Normalizes the vector and returns it.
 * -- @param self Vector.
 * -- @return Vector.
 * function Vector.normalize(self)
 */
static void Vector_normalize (LIScrArgs* args)
{
	LIMatVector tmp;

	tmp = limat_vector_normalize (*((LIMatVector*) args->self));
	liscr_args_seti_vector (args, &tmp);
}

/* @luadoc
 * --- Length.
 * -- @name Vector.length
 * -- @class table
 */
static void Vector_getter_length (LIScrArgs* args)
{
	liscr_args_seti_float (args, limat_vector_get_length (*((LIMatVector*) args->self)));
}

/* @luadoc
 * --- X value.
 * -- @name Vector.x
 * -- @class table
 */
static void Vector_getter_x (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->x);
}
static void Vector_setter_x (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->x);
}

/* @luadoc
 * --- Y value.
 * -- @name Vector.y
 * -- @class table
 */
static void Vector_getter_y (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->y);
}
static void Vector_setter_y (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->y);
}

/* @luadoc
 * --- Z value.
 * -- @name Vector.z
 * -- @class table
 */
static void Vector_getter_z (LIScrArgs* args)
{
	liscr_args_seti_float (args, ((LIMatVector*) args->self)->z);
}
static void Vector_setter_z (LIScrArgs* args)
{
	liscr_args_geti_float (args, 0, &((LIMatVector*) args->self)->z);
}

/*****************************************************************************/

void
liscr_script_vector (LIScrClass* self,
                     void*       data)
{
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "new", Vector_new);
	liscr_class_insert_mfunc (self, "__add", Vector___add);
	liscr_class_insert_mfunc (self, "__mul", Vector___mul);
	liscr_class_insert_mfunc (self, "__sub", Vector___sub);
	liscr_class_insert_mfunc (self, "__tostring", Vector___tostring);
	liscr_class_insert_mfunc (self, "cross", Vector_cross);
	liscr_class_insert_mfunc (self, "dot", Vector_dot);
	liscr_class_insert_mfunc (self, "normalize", Vector_normalize);
	liscr_class_insert_mvar (self, "length", Vector_getter_length, NULL);
	liscr_class_insert_mvar (self, "x", Vector_getter_x, Vector_setter_x);
	liscr_class_insert_mvar (self, "y", Vector_getter_y, Vector_setter_y);
	liscr_class_insert_mvar (self, "z", Vector_getter_z, Vector_setter_z);
}

/**
 * \brief Creates a new script vector.
 *
 * The initial reference count of the vector is one.
 *
 * \param script Script.
 * \param vector Vector.
 * \return New script vector or NULL.
 */
LIScrData*
liscr_vector_new (LIScrScript*       script,
                  const LIMatVector* vector)
{
	LIScrClass* clss;
	LIScrData* self;

	clss = liscr_script_find_class (script, LISCR_SCRIPT_VECTOR);
	self = liscr_data_new_alloc (script, sizeof (LIMatVector), clss);
	if (self == NULL)
		return NULL;
	*((LIMatVector*) self->data) = *vector;

	return self;
}

/** @} */
/** @} */
