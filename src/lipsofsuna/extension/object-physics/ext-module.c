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
 * \addtogroup LIExtObjectPhysics ObjectPhysics
 * @{
 */

#include "lipsofsuna/object.h"
#include "lipsofsuna/physics.h"
#include "ext-module.h"

static void private_object_contact (
	LIExtModule*  self,
	LIPhyContact* contact);

static int private_object_free (
	LIExtModule* self,
	LIObjObject* object);

static int private_object_model (
	LIExtModule* self,
	LIObjObject* object,
	LIMdlModel*  model);

static int private_object_new (
	LIExtModule* self,
	LIObjObject* object);

static int private_object_transform (
	LIExtModule* self,
	LIObjObject* object);

static int private_object_visibility (
	LIExtModule* self,
	LIObjObject* object,
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
	if (!lical_callbacks_insert (program->callbacks, "object-contact", -65535, private_object_contact, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "object-free", -65535, private_object_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (program->callbacks, "object-model", -65535, private_object_model, self, self->calls + 2) ||
	    !lical_callbacks_insert (program->callbacks, "object-new", -65535, private_object_new, self, self->calls + 3) ||
	    !lical_callbacks_insert (program->callbacks, "object-transform", -65535, private_object_transform, self, self->calls + 4) ||
	    !lical_callbacks_insert (program->callbacks, "object-visibility", -65535, private_object_visibility, self, self->calls + 5) ||
	    !lical_callbacks_insert (program->callbacks, "physics-transform", -65535, private_physics_transform, self, self->calls + 6))
	{
		liext_object_physics_free (self);
		return NULL;
	}

	/* Extend the object class. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_PHYSICS_OBJECT, self);
	liext_script_physics_object (program->script);

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

static void private_object_contact (
	LIExtModule*  self,
	LIPhyContact* contact)
{
	LIObjObject* object0;
	LIObjObject* object1;
	LIMatVector vector;

	if (contact->object1 != NULL)
	{
		/* Object -> object */
		object0 = liphy_object_get_userdata (contact->object0);
		object1 = liphy_object_get_userdata (contact->object1);
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal, "object", LIMAI_FIELD_INT, liobj_object_get_external_id (object1), "point", LIMAI_FIELD_VECTOR, &contact->point, "self", LIMAI_FIELD_INT, liobj_object_get_external_id (object0), NULL);
	}
	else if (contact->terrain != NULL)
	{
		/* Object -> voxels */
		object0 = liphy_object_get_userdata (contact->object0);
		vector.x = contact->terrain_tile[0];
		vector.y = contact->terrain_tile[1];
		vector.z = contact->terrain_tile[2];
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal,  "point", LIMAI_FIELD_VECTOR, &contact->point, "tile", LIMAI_FIELD_VECTOR, &vector, "self", LIMAI_FIELD_INT, liobj_object_get_external_id (object0), NULL);
	}
	else
	{
		/* Object -> heightmap */
		object0 = liphy_object_get_userdata (contact->object0);
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal, "heightmap", LIMAI_FIELD_BOOL, 1, "point", LIMAI_FIELD_VECTOR, &contact->point, "self", LIMAI_FIELD_INT, liobj_object_get_external_id (object0), NULL);
	}
}

static int private_object_free (
	LIExtModule* self,
	LIObjObject* object)
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
	LIObjObject* object,
	LIMdlModel*  model)
{
	LIPhyModel* model_;
	LIPhyObject* phyobj;

	/* Find physics object. */
	phyobj = liphy_physics_find_object (self->physics, object->id);
	if (phyobj == NULL)
		return 1;

	/* Set shape from a physics model. */
	if (model != NULL)
		model_ = liphy_physics_find_model (self->physics, model->id);
	else
		model_ = NULL;
	if (model_ != NULL)
		liphy_object_set_model (phyobj, model_);
	else
		liphy_object_set_model (phyobj, NULL);

	return 1;
}

static int private_object_new (
	LIExtModule* self,
	LIObjObject* object)
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
	LIObjObject* object)
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
		liobj_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}

	return 1;
}

static void private_physics_transform (
	LIExtModule* self,
	LIPhyObject* object)
{
	LIObjObject* obj;
	LIMatTransform transform;

	/* Find the engine object. */
	obj = liphy_object_get_userdata (object);
	if (obj == NULL || !liobj_object_get_realized (obj))
		return;

	/* Copy the transformation to the engine object. We set the silence
	   variable here to prevent the object-transform event by the engine
	   object from causing a feedback loop. */
	self->silence++;
	liphy_object_get_transform (object, &transform);
	liobj_object_set_transform (obj, &transform);
	liobj_object_moved (obj);
	self->silence--;
}

static int private_object_visibility (
	LIExtModule* self,
	LIObjObject* object,
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
		liobj_object_get_transform (object, &transform);
		liphy_object_set_transform (phyobj, &transform);
	}
	liphy_object_set_realized (phyobj, value);

	return 1;
}

/** @} */
/** @} */
