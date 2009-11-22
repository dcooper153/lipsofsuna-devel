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

static int
Vector___gc (lua_State* lua)
{
	liscrData* self;

	self = liscr_isdata (lua, 1, LICOM_SCRIPT_VECTOR);

	lisys_free (self->data);
	liscr_data_free (self);
	return 0;
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
static int
Vector_new (lua_State* lua)
{
	limatVector vec;
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
	self = liscr_vector_new (script, &vec);
	if (self == NULL)
		return 0;

	liscr_pushdata (lua, self);
	liscr_data_unref (self, NULL);
	return 1;
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
static int
Vector_cross (lua_State* lua)
{
	limatVector tmp;
	liscrData* a;
	liscrData* b;
	liscrData* c;
	liscrScript* script = liscr_script (lua);

	a = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	b = liscr_checkdata (lua, 2, LICOM_SCRIPT_VECTOR);

	tmp = limat_vector_cross (*((limatVector*) a->data), *((limatVector*) b->data));
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
 * -- Length.
 * -- @name Vector.length
 * -- @class table
 */
static int
Vector_getter_length (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);

	lua_pushnumber (lua, limat_vector_get_length (*((limatVector*) self->data)));
	return 1;
}

/* @luadoc
 * ---
 * -- X value.
 * -- @name Vector.x
 * -- @class table
 */
static int
Vector_getter_x (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);

	lua_pushnumber (lua, ((limatVector*) self->data)->x);
	return 1;
}
static int
Vector_setter_x (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	value = luaL_checknumber (lua, 3);

	((limatVector*) self->data)->x = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Y value.
 * -- @name Vector.y
 * -- @class table
 */
static int
Vector_getter_y (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);

	lua_pushnumber (lua, ((limatVector*) self->data)->y);
	return 1;
}
static int
Vector_setter_y (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	value = luaL_checknumber (lua, 3);

	((limatVector*) self->data)->y = value;
	return 0;
}

/* @luadoc
 * ---
 * -- Z value.
 * -- @name Vector.z
 * -- @class table
 */
static int
Vector_getter_z (lua_State* lua)
{
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);

	lua_pushnumber (lua, ((limatVector*) self->data)->z);
	return 1;
}
static int
Vector_setter_z (lua_State* lua)
{
	float value;
	liscrData* self;

	self = liscr_checkdata (lua, 1, LICOM_SCRIPT_VECTOR);
	value = luaL_checknumber (lua, 3);

	((limatVector*) self->data)->z = value;
	return 0;
}

/*****************************************************************************/

void
licomVectorScript (liscrClass* self,
                   void*       data)
{
	liscr_class_insert_func (self, "__add", Vector___add);
	liscr_class_insert_func (self, "__gc", Vector___gc);
	liscr_class_insert_func (self, "__mul", Vector___mul);
	liscr_class_insert_func (self, "__sub", Vector___sub);
	liscr_class_insert_func (self, "cross", Vector_cross);
	liscr_class_insert_func (self, "new", Vector_new);
	liscr_class_insert_getter (self, "length", Vector_getter_length);
	liscr_class_insert_getter (self, "x", Vector_getter_x);
	liscr_class_insert_getter (self, "y", Vector_getter_y);
	liscr_class_insert_getter (self, "z", Vector_getter_z);
	liscr_class_insert_setter (self, "x", Vector_setter_x);
	liscr_class_insert_setter (self, "y", Vector_setter_y);
	liscr_class_insert_setter (self, "z", Vector_setter_z);
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
	limatVector* tmp;
	liscrData* self;

	tmp = lisys_malloc (sizeof (limatVector));
	if (tmp == NULL)
		return NULL;
	*tmp = *vector;
	self = liscr_data_new (script, tmp, LICOM_SCRIPT_VECTOR);
	if (self == NULL)
	{
		lisys_free (tmp);
		return NULL;
	}

	return self;
}

/** @} */
/** @} */
