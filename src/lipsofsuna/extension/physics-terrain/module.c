/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIExtPhysicsTerrain PhysicsTerrain
 * @{
 */

#include "module.h"
#include "physics-terrain.h"

static void private_terrain_free (
	LIExtPhysicsTerrainModule* self,
	LIExtTerrain*              terrain);

static int private_tick (
	LIExtPhysicsTerrainModule* self,
	float                      secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_physics_terrain_info =
{
	LIMAI_EXTENSION_VERSION, "PhysicsTerrain",
	liext_physics_terrain_module_new,
	liext_physics_terrain_module_free,
	liext_physics_terrain_module_get_memstat
};

LIExtPhysicsTerrainModule* liext_physics_terrain_module_new (
	LIMaiProgram* program)
{
	LIExtPhysicsTerrainModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtPhysicsTerrainModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate the terrain dictionary. */
	self->terrains = lialg_ptrdic_new ();
	if (self->terrains == NULL)
	{
		liext_physics_terrain_module_free (self);
		return NULL;
	}

	/* Make sure the physics extension is loaded. */
	if (!limai_program_insert_extension (program, "physics"))
	{
		liext_physics_terrain_module_free (self);
		return NULL;
	}

	/* Find the physics manager. */
	self->physics = limai_program_find_component (program, "physics");
	if (self->physics == NULL)
	{
		liext_physics_terrain_module_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, "terrain-free", 0, private_terrain_free, self, self->calls + 0) ||
	    !lical_callbacks_insert (program->callbacks, "tick", 1, private_tick, self, self->calls + 1))
	{
		liext_physics_terrain_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_set_userdata (program->script, LIEXT_SCRIPT_PHYSICS_TERRAIN, self);
	liext_script_physics_terrain (program->script);

	return self;
}

void liext_physics_terrain_module_free (
	LIExtPhysicsTerrainModule* self)
{
	/* Remove callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	if (self->terrains != NULL)
	{
		lisys_assert (self->terrains->size == 0);
		lialg_ptrdic_free (self->terrains);
	}
	lisys_free (self);
}

/**
 * \brief Gets the memory statistics of the module.
 * \param self Module.
 * \param stat Return location for the stats.
 */
void liext_physics_terrain_module_get_memstat (
	LIExtPhysicsTerrainModule* self,
	LIMaiMemstat*              stat)
{
/* TODO: memstat */
}

/*****************************************************************************/

static void private_terrain_free (
	LIExtPhysicsTerrainModule* self,
	LIExtTerrain*              terrain)
{
	LIAlgPtrdicIter iter;

	LIALG_PTRDIC_FOREACH (iter, self->terrains)
	{
		liext_physics_terrain_remove (iter.value, terrain);
	}
}

static int private_tick (
	LIExtPhysicsTerrainModule* self,
	float                      secs)
{
	LIAlgPtrdicIter iter;

	LIALG_PTRDIC_FOREACH (iter, self->terrains)
	{
		liext_physics_terrain_update (iter.value, secs);
	}

	return 1;
}

/** @} */
/** @} */
