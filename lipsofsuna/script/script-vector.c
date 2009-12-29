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
 * \addtogroup liscrVector Vector
 * @{
 */

#include <script/lips-script.h>

/* @luadoc
 * module "Core.Common.Vector"
 * ---
 * -- Do maths in three dimensions.
 * -- @name Vector
 * -- @class table
 */

/* @luadoc
 * ---
 * -- Calculates the sum of two vectors.
 * --
 * -- @param self Vector.
 * -- @param quat Vector.
 * -- @return New vector.
 * function Vector.__add(self, vector)
 */
static int
Vector___add (lua_State* lua)
{
	limatVector tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	tmp = limat_vector_add (*((limatVector*) a->data), *((limatVector*) b->data));
	c = liscr_vector_new (script, &tmp);
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
 * -- Multiplies the vector with a scalar.
 * --
 * -- @param self Vector.
 * -- @param scalar Scalar.
 * -- @return New vector.
 * function Vector.__mul(self, scalar)
 */
static int
Vector___mul (lua_State* lua)
{
	float s;
	limatVector tmp;
	liscrData* a;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	s = luaL_checknumber (lua, 2);

	tmp = limat_vector_multiply (*((limatVector*) a->data), s);
	c = liscr_vector_new (script, &tmp);
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
 * -- Subtracts a vector from another.
 * --
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return New vector.
 * function Vector.__sub(self, vector)
 */
static int
Vector___sub (lua_State* lua)
{
	limatVector tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	tmp = limat_vector_subtract (*((limatVector*) a->data), *((limatVector*) b->data));
	c = liscr_vector_new (script, &tmp);
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
 * -- Creates a new vector.
 * --
 * -- @param self Vector class.
 * -- @param x Optional X value, default is 0.
 * -- @param y Optional Y value, default is 0.
 * -- @param z Optional Z value, default is 0.
 * -- @return New vector.
 * function Vector.new(self, x, y, z)
 */
static void Vector_new (liscrArgs* args)
{
	limatVector vec;

	if (!liscr_args_gets_float (args, "x", &vec.x))
		liscr_args_geti_float (args, 0, &vec.x);
	if (!liscr_args_gets_float (args, "y", &vec.y))
		liscr_args_geti_float (args, 0, &vec.y);
	if (!liscr_args_gets_float (args, "z", &vec.z))
		liscr_args_geti_float (args, 0, &vec.z);
	liscr_args_seti_vector (args, &vec);
}

/* @luadoc
 * ---
 * -- Calculates the cross product of two vectors.
 * --
 * -- @param self Vector.
 * -- @param vector Vector.
 * -- @return New vector.
 * function Vector.cross(self, vector)
 */
static void Vector_cross (liscrArgs* args)
{
	limatVector tmp;
	liscrData* data;

	if (liscr_args_geti_data (args, 0, LICOM_SCRIPT_VECTOR, &data))
	{
		tmp = limat_vector_cross (*((limatVector*) args->self), *((limatVector*) data->data));
		liscr_args_seti_vector (args, &tmp);
	}
}

/* @luadoc
 * ---
 * -- Length.
 * -- @name Vector.length
 * -- @class table
 */
static void Vector_getter_length (liscrArgs* args)
{
	liscr_args_seti_float (args, limat_vector_get_length (*((limatVector*) args->self)));
}

/* @luadoc
 * ---
 * -- X value.
 * -- @name Vector.x
 * -- @class table
 */
static void Vector_getter_x (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatVector*) args->self)->x);
}
static void Vector_setter_x (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatVector*) args->self)->x);
}

/* @luadoc
 * ---
 * -- Y value.
 * -- @name Vector.y
 * -- @class table
 */
static void Vector_getter_y (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatVector*) args->self)->y);
}
static void Vector_setter_y (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatVector*) args->self)->y);
}

/* @luadoc
 * ---
 * -- Z value.
 * -- @name Vector.z
 * -- @class table
 */
static void Vector_getter_z (liscrArgs* args)
{
	liscr_args_seti_float (args, ((limatVector*) args->self)->z);
}
static void Vector_setter_z (liscrArgs* args)
{
	liscr_args_geti_float (args, 0, &((limatVector*) args->self)->z);
}

/*****************************************************************************/

void
licomVectorScript (liscrClass* self,
                   void*       data)
{
	liscr_class_insert_func (self, "__add", Vector___add);
	liscr_class_insert_func (self, "__mul", Vector___mul);
	liscr_class_insert_func (self, "__sub", Vector___sub);
	liscr_class_insert_mfunc (self, "cross", Vector_cross);
	liscr_class_insert_cfunc (self, "new", Vector_new);
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
liscrData*
liscr_vector_new (liscrScript*       script,
                  const limatVector* vector)
{
	liscrData* self;

	self = liscr_data_new_alloc (script, sizeof (limatVector), LICOM_SCRIPT_VECTOR);
	if (self == NULL)
		return NULL;
	*((limatVector*) self->data) = *vector;

	return self;
}

/** @} */
/** @} */
