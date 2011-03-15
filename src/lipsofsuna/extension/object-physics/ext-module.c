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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtObjectPhysics ObjectPhysics
 * @{
 */

#include "ext-module.h"

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model);

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model);

static void private_object_contact (
	LIExtModule*  self,
	LIPhyContact* contact);

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model);

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_transform (
	LIExtModule* self,
	LIEngObject* object);

static int private_object_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value);

static void private_physics_transform (
	LIExtModule* self,
	LIPhyObject* object);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_physics_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectPhysics",
	liext_object_physics_new,
	liext_object_physics_free
};

LIExtModule* liext_object_physics_new (
	LIMaiProgram* program)
{
	LIExtModule* self;
	LIScrClass* clss;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the physics extension is loaded. */
	if (!limai_program_insert_extension (program, "physics"))
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Find the physics manager. */
	self->physics = limai_program_find_component (program, "physics");
	if (self->physics == NULL)
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "model-changed", -65535, private_model_changed, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "model-free", -65535, private_model_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "model-new", -65535, private_model_new, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, self->physics, "object-contact", -65535, private_object_contact, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-free", -65535, private_object_free, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-model", -65535, private_object_model, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-new", -65535, private_object_new, self, self->calls + 6) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-transform", -65535, private_object_transform, self, self->calls + 7) ||
	    !lical_callbacks_insert (program->callbacks, self->physics, "object-transform", -65535, private_physics_transform, self, self->calls + 8) ||
	    !lical_callbacks_insert (program->callbacks, program->engine, "object-visibility", -65535, private_object_visibility, self, self->calls + 9))
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Extend the object class. */
	clss = liscr_script_find_class (program->script, "Object");
	if (clss != NULL)
		liext_script_object (clss, self);

	return self;
}

void liext_object_physics_free (
	LIExtModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	lisys_free (self);
}

/*****************************************************************************/

static int private_model_changed (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ != NULL)
		liphy_model_set_model (model_, model->model);

	return 1;
}

static int private_model_free (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ != NULL)
		liphy_model_free (model_);

	return 1;
}

static int private_model_new (
	LIExtModule* self,
	LIEngModel*  model)
{
	LIPhyModel* model_;

	model_ = liphy_model_new (self->physics, model->model, model->id);
	if (model_ == NULL)
		return 1;

	return 1;
}

static void private_object_contact (
	LIExtModule*  self,
	LIPhyContact* contact)
{
	LIScrData* point;
	LIScrData* normal;
	LIScrData* tile;
	LIEngObject* object0;
	LIEngObject* object1;
	LIMatVector vector;
	LIScrScript* script = self->program->script;

	if (contact->object1 != NULL)
	{
		object0 = liphy_object_get_userdata (contact->object0);
		object1 = liphy_object_get_userdata (contact->object1);
		point = liscr_vector_new (script, &contact->point);
		normal = liscr_vector_new (script, &contact->normal);
		limai_program_event (self->program, "object-contact",
			"impulse", LISCR_TYPE_FLOAT, contact->impulse,
			"normal", LISCR_SCRIPT_VECTOR, normal, 
			"object", LISCR_SCRIPT_OBJECT, object1->script,
			"point", LISCR_SCRIPT_VECTOR, point,
			"self", LISCR_SCRIPT_OBJECT, object0->script, NULL);
		liscr_data_unref (point);
		liscr_data_unref (normal);
	}
	else
	{
		object0 = liphy_object_get_userdata (contact->object0);
		vector.x = contact->terrain_tile[0];
		vector.y = contact->terrain_tile[1];
		vector.z = contact->terrain_tile[2];
		point = liscr_vector_new (script, &contact->point);
		normal = liscr_vector_new (script, &contact->normal);
		tile = liscr_vector_new (script, &vector);
		limai_program_event (self->program, "object-contact",
			"impulse", LISCR_TYPE_FLOAT, contact->impulse,
			"normal", LISCR_SCRIPT_VECTOR, normal, 
			"point", LISCR_SCRIPT_VECTOR, point,
			"tile", LISCR_SCRIPT_VECTOR, tile,
			"self", LISCR_SCRIPT_OBJECT, object0->script, NULL);
		liscr_data_unref (point);
		liscr_data_unref (normal);
		liscr_data_unref (tile);
	}
}

static int private_object_free (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Free it. */
	liphy_object_free (phyobj);

	return 1;
}

static int private_object_model (
	LIExtModule* self,
	LIEngObject* object,
	LIEngModel*  model)
{
	LIPhyModel* model_;
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set shape from a physics model. */
	model_ = liphy_physics_find_model (self->physics, model->id);
	if (model_ != NULL)
		liphy_object_set_model (phyobj, model_);
	else
		liphy_object_set_model (phyobj, NULL);

	return 1;
}

static int private_object_new (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;

	/* Initialize physics. */
	phyobj = liphy_object_new (self->physics, object->id, NULL, LIPHY_CONTROL_MODE_STATIC);
	if (phyobj != NULL)
		liphy_object_set_userdata (phyobj, object);

	return 1;
}

static int private_object_transform (
	LIExtModule* self,
	LIEngObject* object)
{
	LIPhyObject* phyobj;
	LIMatTransform transform;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set its transformation. */
	if (!self->silence)
	{
		lieng_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}

	return 1;
}

static void private_physics_transform (
	LIExtModule* self,
	LIPhyObject* object)
{
	LIEngObject* obj;
	LIMatTransform transform;

	/* Find physics object. */
	obj = liphy_object_get_userdata (object);
	if (obj == NULL || obj->sector == NULL)
		return;

	/* Copy the transformation to the engine object. We set the silence
	   variable here to prevent the object-transform event by the engine
	   object from causing a feedback loop. */
	self->silence++;
	liphy_object_get_transform (object, &transform);
	lieng_object_set_transform (obj, &transform);
	lieng_object_moved (obj);
	self->silence--;
}

static int private_object_visibility (
	LIExtModule* self,
	LIEngObject* object,
	int          value)
{
	LIMatTransform transform;
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set its visibility. */
	if (value)
	{
		lieng_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}
	liphy_object_set_realized (phyobj, value);

	return 1;
}

/** @} */
/** @} */
