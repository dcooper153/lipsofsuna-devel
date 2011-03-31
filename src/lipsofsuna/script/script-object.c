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
 * \addtogroup LIScrObject Object
 * @{
 */

#include "lipsofsuna/engine.h"
#include "lipsofsuna/main.h"
#include "lipsofsuna/script.h"
#include "script-private.h"

/* @luadoc
 * module "builtin/object"
 * --- Manipulate objects.
 * -- @name Object
 * -- @class table
 */

/* @luadoc
 * --- Finds all objects inside a sphere.
 * -- @param clss Object class.
 * -- @param args Arguments.<ul>
 * --   <li>point: Center point. (required)</li>
 * --   <li>radius: Search radius.</li>
 * --   <li>sector: Return all object in this sector.</li></ul>
 * -- @return Table of matching objects.
 * function Object.find(clss, args)
 */
static void Object_find (LIScrArgs* args)
{
	int id;
	float radius = 32.0f;
	LIAlgU32dicIter iter1;
	LIEngObjectIter iter;
	LIEngObject* object;
	LIEngSector* sector;
	LIMatVector center;
	LIMatVector diff;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Radial find mode. */
	if (liscr_args_gets_vector (args, "point", &center))
	{
		liscr_args_gets_float (args, "radius", &radius);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		LIENG_FOREACH_OBJECT (iter, program->engine, &center, radius)
		{
			diff = limat_vector_subtract (center, iter.object->transform.position);
			if (limat_vector_get_length (diff) < radius)
				liscr_args_seti_data (args, iter.object->script);
		}
	}

	/* Sector find mode. */
	else if (liscr_args_gets_int (args, "sector", &id))
	{
		sector = lialg_sectors_data_index (program->sectors, "engine", id, 0);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		if (sector != NULL)
		{
			LIALG_U32DIC_FOREACH (iter1, sector->objects)
			{
				object = iter1.value;
				liscr_args_seti_data (args, object->script);
			}
		}
	}
}

/* @luadoc
 * --- Creates a new object.
 * -- @param clss Object class.
 * -- @param args Arguments.
 * -- @return New object.
 * function Object.new(clss, args)
 */
static void Object_new (LIScrArgs* args)
{
	int realize = 0;
	LIEngObject* self;
	LIMaiProgram* program;

	program = liscr_class_get_userdata (args->clss, LISCR_SCRIPT_OBJECT);

	/* Allocate object. */
	self = lieng_object_new (program->engine, 0);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, self, args->clss, lieng_object_free);
	if (self->script == NULL)
	{
		lieng_object_free (self);
		return;
	}

	/* Initialize userdata. */
	liscr_args_call_setters_except (args, self->script, "realized");
	liscr_args_gets_bool (args, "realized", &realize);
	liscr_args_seti_data (args, self->script);
	liscr_data_unref (self->script);
	lieng_object_set_realized (self, realize);
}

/* @luadoc
 * --- Adds an additional model mesh to the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>model: Model.</li></ul>
 * function Object.add_model(self, args)
 */
static void Object_add_model (LIScrArgs* args)
{
	LIScrData* model;

	if (liscr_args_gets_data (args, "model", LISCR_SCRIPT_MODEL, &model))
	{
		if (!lieng_object_merge_model (args->self, model->data))
			lisys_error_report ();
	}
}

/* @luadoc
 * --- Recalculates the bounding box of the model of the object.
 * -- @param self Object.
 * function Object.calculate_bounds(self)
 */
static void Object_calculate_bounds (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->model != NULL)
		lieng_model_calculate_bounds (self->model);
}

/* @luadoc
 * --- Prevents map sectors around the object from being unloaded.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>radius: Refresh radius.</li></ul>
 * function Object.refresh(self, args)
 */
static void Object_refresh (LIScrArgs* args)
{
	float radius = 32.0f;

	liscr_args_gets_float (args, "radius", &radius);
	lieng_object_refresh (args->self, radius);
}

/* @luadoc
 * --- Custom collision response callback.<br/>
 * -- Function to be called every time the object collides with something.
 * -- @name Object.contact_cb
 * -- @class table
 */

/* @luadoc
 * --- Gets the model of the object.
 * -- @param self Object.
 * -- @return Model or nil.
 * function Object.get_model(self)
 */
static void Object_get_model (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->model != NULL)
		liscr_args_seti_data (args, self->model->script);
}

/* @luadoc
 * --- Sets the model of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1: Model.</li></ul>
 * function Object.set_model(self, args)
 */
static void Object_set_model (LIScrArgs* args)
{
	LIScrData* value;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_MODEL, &value))
		lieng_object_set_model (args->self, value->data);
}

/* @luadoc
 * --- Gets the position of the object.
 * -- @param self Object.
 * -- @return Vector.
 * function Object.get_position(self)
 */
static void Object_get_position (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_vector (args, &tmp.position);
}

/* @luadoc
 * --- Sets the position of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1: Position vector.</li></ul>
 * function Object.set_position(self, args)
 */
static void Object_set_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		lieng_object_get_transform (args->self, &transform);
		transform.position = vector;
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * --- Gets the realization status of the object.
 * -- @param self Object.
 * -- @return Boolean.
 * function Object.get_realized(self)
 */
static void Object_get_realized (LIScrArgs* args)
{
	liscr_args_seti_bool (args, lieng_object_get_realized (args->self));
}

/* @luadoc
 * --- Sets the realization status of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1: boolean.</li></ul>
 * function Object.set_realized(self, args)
 */
static void Object_set_realized (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		lieng_object_set_realized (args->self, value);
}

/* @luadoc
 * --- Gets the rotation of the object.
 * -- @param self Object.
 * -- @return Quaternion.
 * function Object.get_rotation(self)
 */
static void Object_get_rotation (LIScrArgs* args)
{
	LIMatTransform tmp;

	lieng_object_get_transform (args->self, &tmp);
	liscr_args_seti_quaternion (args, &tmp.rotation);
}

/* @luadoc
 * --- Sets the rotation of the object.
 * -- @param self Object.
 * -- @param args Arguments.<ul>
 * --   <li>1: Quaternion.</li></ul>
 * function Object.set_rotation(self, args)
 */
static void Object_set_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIScrData* quat;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &quat))
	{
		lieng_object_get_transform (args->self, &transform);
		transform.rotation = *((LIMatQuaternion*) quat->data);
		lieng_object_set_transform (args->self, &transform);
	}
}

/* @luadoc
 * --- Gets the map sector of the object.
 * -- @param self Object.
 * -- @return Integer.
 * function Object.get_sector(self)
 */
static void Object_get_sector (LIScrArgs* args)
{
	LIEngObject* self = args->self;

	if (self->sector != NULL)
		liscr_args_seti_int (args, self->sector->sector->index);
}

/*****************************************************************************/

void liscr_script_object (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LISCR_SCRIPT_OBJECT, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "find", Object_find);
	liscr_class_insert_cfunc (self, "new", Object_new);
	liscr_class_insert_mfunc (self, "add_model", Object_add_model);
	liscr_class_insert_mfunc (self, "calculate_bounds", Object_calculate_bounds);
	liscr_class_insert_mfunc (self, "refresh", Object_refresh);
	liscr_class_insert_mfunc (self, "get_model", Object_get_model);
	liscr_class_insert_mfunc (self, "set_model", Object_set_model);
	liscr_class_insert_mfunc (self, "get_position", Object_get_position);
	liscr_class_insert_mfunc (self, "set_position", Object_set_position);
	liscr_class_insert_mfunc (self, "get_realized", Object_get_realized);
	liscr_class_insert_mfunc (self, "set_realized", Object_set_realized);
	liscr_class_insert_mfunc (self, "get_rotation", Object_get_rotation);
	liscr_class_insert_mfunc (self, "set_rotation", Object_set_rotation);
	liscr_class_insert_mfunc (self, "get_sector", Object_get_sector);
}

/** @} */
/** @} */
