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
 * \addtogroup LIExtHeightmapPhysics HeightmapPhysics
 * @{
 */

#include "module.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include "heightfield-terrain-shape.hpp"

static void private_init_heightmap (
	LIExtHeightmapPhysicsModule* self,
	LIExtHeightmap*              heightmap);

static void private_free_heightmap (
	LIExtHeightmapPhysicsModule* self,
	LIExtHeightmap*              heightmap);

/*****************************************************************************/

extern "C"
{
	LIMaiExtensionInfo liext_heightmap_physics_info =
	{
		LIMAI_EXTENSION_VERSION, "HeightmapPhysics",
		(void*) liext_heightmap_physics_module_new,
		(void*) liext_heightmap_physics_module_free,
		(void*) liext_heightmap_physics_module_get_memstat
	};
}

LIExtHeightmapPhysicsModule* liext_heightmap_physics_module_new (
	LIMaiProgram* program)
{
	LIExtHeightmapPhysicsModule* self;

	/* Allocate self. */
	self = (LIExtHeightmapPhysicsModule*) lisys_calloc (1, sizeof (LIExtHeightmapPhysicsModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Make sure the needed extensions are loaded. */
	if (!limai_program_insert_extension (program, "heightmap") ||
	    !limai_program_insert_extension (program, "physics"))
	{
		liext_heightmap_physics_module_free (self);
		return NULL;
	}

	/* Find the required components. */
	self->heightmap = (LIExtHeightmapModule*) limai_program_find_component (program, "heightmap");
	self->physics = (LIPhyPhysics*) limai_program_find_component (program, "physics");
	if (self->heightmap == NULL || self->physics == NULL)
	{
		liext_heightmap_physics_module_free (self);
		return NULL;
	}

	/* Add the heightmap hook. */
	lisys_assert (self->heightmap->physics_hooks == NULL);
	self->hooks.data = self;
	self->hooks.init = (LIExtHeightmapInitFunc) private_init_heightmap;
	self->hooks.free = (LIExtHeightmapFreeFunc) private_free_heightmap;
	self->heightmap->physics_hooks = &self->hooks;

	return self;
}

void liext_heightmap_physics_module_free (
	LIExtHeightmapPhysicsModule* self)
{
	LIAlgU32dicIter iter;
	LIExtHeightmap* heightmap;

	if (self->heightmap != NULL)
	{
		/* Free existing physics data. */
		LIALG_U32DIC_FOREACH (iter, self->heightmap->heightmaps)
		{
			heightmap = (LIExtHeightmap*) iter.value;
			if (heightmap->physics_data != NULL)
				private_free_heightmap (self, heightmap);
		}

		/* Remove heightmap hooks. */
		self->heightmap->physics_hooks = NULL;
	}

	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_heightmap_physics_module_get_memstat (
	LIExtHeightmapPhysicsModule* self,
	LIMaiMemstat*                stat)
{
/* TODO: memstat */
}

/*****************************************************************************/

static void private_init_heightmap (
	LIExtHeightmapPhysicsModule* self,
	LIExtHeightmap*              heightmap)
{
	LIExtHeightmapPhysics* data;

	/* Allocate data. */
	data = (LIExtHeightmapPhysics*) lisys_calloc (1, sizeof (LIExtHeightmapPhysics));
	if (data == NULL)
		return;

	/* Create the collision shape. */
	float min = heightmap->min;
	float max = heightmap->max;
	data->shape = new LIExtHeightfieldTerrainShape (heightmap->size, heightmap->size,
		heightmap->heights, 1.0f, min, max, 1, PHY_FLOAT, false);
	data->shape->setUseDiamondSubdivision (true);
	data->shape->setLocalScaling (btVector3 (heightmap->spacing, 1.0f, heightmap->spacing));

	/* Calculate the transformation. */
	btTransform transform;
	transform.setIdentity ();
	transform.setOrigin (btVector3 (
		heightmap->position.x,
		heightmap->position.y + (max - min) / 2,
		heightmap->position.z));

	/* Create the collision object. */
	data->body = new btCollisionObject ();
	data->body->setCollisionFlags (data->body->getCollisionFlags () | btCollisionObject::CF_STATIC_OBJECT);
	data->body->setCollisionShape (data->shape);
	data->body->setWorldTransform (transform);
	self->physics->dynamics->addCollisionObject (data->body, LIPHY_GROUP_HEIGHTMAP, LIPHY_DEFAULT_COLLISION_MASK & ~(LIPHY_GROUP_TILES | LIPHY_GROUP_HEIGHTMAP));

	/* Register the physics data. */
	heightmap->physics_data = data;
}

static void private_free_heightmap (
	LIExtHeightmapPhysicsModule* self,
	LIExtHeightmap*              heightmap)
{
	if (heightmap->physics_data != NULL)
	{
		LIExtHeightmapPhysics* data = (LIExtHeightmapPhysics*) heightmap->physics_data;
		if (data->body != NULL)
		{
			self->physics->dynamics->removeCollisionObject (data->body);
			delete data->body;
		}
		if (data->shape != NULL)
			delete data->shape;
		lisys_free (data);
		heightmap->physics_data = NULL;
	}
}

/** @} */
/** @} */
