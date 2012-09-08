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
 * \addtogroup LIExtPhysicsObject PhysicsObject
 * @{
 */

#include "lipsofsuna/extension/physics/ext-module.h"
#include "ext-module.h"

static void private_object_contact (
	LIExtPhysicsObjectModule* self,
	LIPhyContact*             contact);

static void private_physics_transform (
	LIExtPhysicsObjectModule* self,
	LIPhyObject*              object);

/*****************************************************************************/

LIMaiExtensionInfo liext_object_physics_info =
{
	LIMAI_EXTENSION_VERSION, "ObjectPhysics",
	liext_object_physics_new,
	liext_object_physics_free
};

LIExtPhysicsObjectModule* liext_object_physics_new (
	LIMaiProgram* program)
{
	LIExtPhysicsObjectModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtPhysicsObjectModule));
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
	    !lical_callbacks_insert (program->callbacks, "physics-transform", -65535, private_physics_transform, self, self->calls + 1))
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
	LIExtPhysicsObjectModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	lisys_free (self);
}

/*****************************************************************************/

static void private_object_contact (
	LIExtPhysicsObjectModule* self,
	LIPhyContact*             contact)
{
	LIMatVector vector;

	if (contact->object1_id)
	{
		/* Object -> object */
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal, "object", LIMAI_FIELD_INT, contact->object1_id, "point", LIMAI_FIELD_VECTOR, &contact->point, "self", LIMAI_FIELD_INT, contact->object_id, NULL);
	}
	else if (contact->voxels_id)
	{
		/* Object -> voxels */
		vector.x = contact->terrain_tile[0];
		vector.y = contact->terrain_tile[1];
		vector.z = contact->terrain_tile[2];
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal,  "point", LIMAI_FIELD_VECTOR, &contact->point, "tile", LIMAI_FIELD_VECTOR, &vector, "self", LIMAI_FIELD_INT, contact->object_id, NULL);
	}
	else if (contact->terrain_id)
	{
		/* Object -> terrain */
		vector.x = contact->terrain_tile[0];
		vector.y = contact->terrain_tile[1];
		vector.z = contact->terrain_tile[2];
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal,  "point", LIMAI_FIELD_VECTOR, &contact->point, "tile", LIMAI_FIELD_VECTOR, &vector, "self", LIMAI_FIELD_INT, contact->object_id, NULL);
	}
	else
	{
		/* Object -> heightmap */
		limai_program_event (self->program, "object-contact", "impulse", LIMAI_FIELD_FLOAT, contact->impulse, "normal", LIMAI_FIELD_VECTOR, &contact->normal, "heightmap", LIMAI_FIELD_BOOL, 1, "point", LIMAI_FIELD_VECTOR, &contact->point, "self", LIMAI_FIELD_INT, contact->object_id, NULL);
	}
}

static void private_physics_transform (
	LIExtPhysicsObjectModule* self,
	LIPhyObject*              object)
{
	/* Emit an object-motion event. */
	limai_program_event (self->program, "object-motion", "id", LIMAI_FIELD_INT, liphy_object_get_external_id (object), NULL);
}

/** @} */
/** @} */
