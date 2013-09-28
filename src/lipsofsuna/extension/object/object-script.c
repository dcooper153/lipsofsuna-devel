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
 * \addtogroup LIObjObject Object
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/main.h"
#include "lipsofsuna/script.h"
#include "object.h"
#include "object-manager.h"
#include "object-sector.h"

static void Object_find (LIScrArgs* args)
{
	int id;
	float radius = 32.0f;
	LIAlgU32dicIter iter1;
	LIMatVector center;
	LIMatVector diff;
	LIObjObject* object;
	LIObjManager* manager;
	LIObjSector* sector;

	/* Find class data. */
	manager = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_OBJECT);

	/* Radial find mode. */
	if (liscr_args_gets_vector (args, "point", &center))
	{
		liscr_args_gets_float (args, "radius", &radius);
		liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
		LIALG_U32DIC_FOREACH (iter1, manager->objects)
		{
			object = iter1.value;
			diff = limat_vector_subtract (center, object->transform.position);
			if (limat_vector_get_length (diff) < radius)
				liscr_args_seti_data (args, object->script);
		}
	}

	/* Sector find mode. */
	else if (liscr_args_gets_int (args, "sector", &id))
	{
		sector = lialg_sectors_data_index (manager->program->sectors, LIALG_SECTORS_CONTENT_ENGINE, id, 0);
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

static void Object_new (LIScrArgs* args)
{
	LIObjObject* self;
	LIObjManager* manager;

	/* Find class data. */
	manager = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_OBJECT);

	/* Allocate object. */
	self = liobj_object_new (manager);
	if (self == NULL)
		return;

	/* Allocate userdata. */
	self->script = liscr_data_new (args->script, args->lua, self, LISCR_SCRIPT_OBJECT, liobj_object_free);
	if (self->script == NULL)
	{
		liobj_object_free (self);
		return;
	}
	liscr_args_seti_stack (args);
}

static void Object_get_id (LIScrArgs* args)
{
	LIObjObject* self = args->self;

	liscr_args_seti_int (args, liobj_object_get_external_id (self));
}

static void Object_set_id (LIScrArgs* args)
{
	int value;
	LIObjObject* self = args->self;

	if (liscr_args_geti_int (args, 0, &value))
		liobj_object_set_external_id (self, value);
}

static void Object_get_position (LIScrArgs* args)
{
	LIMatTransform tmp;
	LIMatVector* v;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_VECTOR, &data))
	{
		liobj_object_get_transform (args->self, &tmp);
		v = liscr_data_get_data (data);
		*v = tmp.position;
	}
}

static void Object_set_position (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatVector vector;

	if (liscr_args_geti_vector (args, 0, &vector))
	{
		liobj_object_get_transform (args->self, &transform);
		transform.position = vector;
		liobj_object_set_transform (args->self, &transform);
	}
}

static void Object_get_realized (LIScrArgs* args)
{
	liscr_args_seti_bool (args, liobj_object_get_realized (args->self));
}

static void Object_set_realized (LIScrArgs* args)
{
	int value;

	if (liscr_args_geti_bool (args, 0, &value))
		liobj_object_set_realized (args->self, value);
}

static void Object_get_rotation (LIScrArgs* args)
{
	LIMatTransform tmp;
	LIMatQuaternion* q;
	LIScrData* data;

	if (liscr_args_geti_data (args, 0, LISCR_SCRIPT_QUATERNION, &data))
	{
		liobj_object_get_transform (args->self, &tmp);
		q = liscr_data_get_data (data);
		*q = tmp.rotation;
	}
}

static void Object_set_rotation (LIScrArgs* args)
{
	LIMatTransform transform;
	LIMatQuaternion quat;

	if (liscr_args_geti_quaternion (args, 0, &quat))
	{
		liobj_object_get_transform (args->self, &transform);
		transform.rotation = quat;
		limat_quaternion_normalize (transform.rotation);
		liobj_object_set_transform (args->self, &transform);
	}
}

static void Object_get_sector (LIScrArgs* args)
{
	LIObjObject* self = args->self;

	if (self->sector != NULL)
		liscr_args_seti_int (args, self->sector->sector->index);
}

static void Object_get_static (LIScrArgs* args)
{
	LIObjObject* self = args->self;

	liscr_args_seti_bool (args, liobj_object_get_static (self));
}

static void Object_set_static (LIScrArgs* args)
{
	int value;
	LIObjObject* self = args->self;

	if (liscr_args_geti_bool (args, 0, &value))
		liobj_object_set_static (self, value);
}

/*****************************************************************************/

void liext_script_object (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_OBJECT, "object_find", Object_find);
	liscr_script_insert_cfunc (self, LISCR_SCRIPT_OBJECT, "object_new", Object_new);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_id", Object_get_id);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_id", Object_set_id);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_position", Object_get_position);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_position", Object_set_position);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_realized", Object_get_realized);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_realized", Object_set_realized);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_rotation", Object_get_rotation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_rotation", Object_set_rotation);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_sector", Object_get_sector);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_get_static", Object_get_static);
	liscr_script_insert_mfunc (self, LISCR_SCRIPT_OBJECT, "object_set_static", Object_set_static);
}

/** @} */
/** @} */
