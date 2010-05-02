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
 * \addtogroup LIExtVoxel Voxel
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

static int
private_block_free (LIExtModule*      self,
                    LIVoxUpdateEvent* event);

static int
private_block_load (LIExtModule*      self,
                    LIVoxUpdateEvent* event);

static int
private_tick (LIExtModule* self,
              float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Voxel",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIAlgU32dicIter iter;
	LIExtModule* self;
	LISerServer* server;
	LIVoxMaterial* material;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Allocate block list. */
	self->blocks = lialg_memdic_new ();
	if (self->blocks == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Create voxel manager. */
	self->voxels = livox_manager_new (program->callbacks, program->sectors);
	if (self->voxels == NULL)
	{
		liext_module_free (self);
		return NULL;
	}

	/* Load materials if server. */
	/* FIXME: This shouldn't be hardcoded either. */
	server = limai_program_find_component (program, "server");
	if (server != NULL)
	{
		livox_manager_set_sql (self->voxels, server->sql);
		livox_manager_load_materials (self->voxels);
	}

	/* Create assign packet. */
	self->assign_packet = liarc_writer_new_packet (1);
	if (self->assign_packet == NULL)
	{
		liext_module_free (self);
		return NULL;
	}
	LIALG_U32DIC_FOREACH (iter, self->voxels->materials)
	{
		material = iter.value;
		if (!livox_material_write (material, self->assign_packet))
		{
			liext_module_free (self);
			return NULL;
		}
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 0) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-free", 0, private_block_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-load", 0, private_block_load, self, self->calls + 2))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Material", liext_script_material, self);
	liscr_script_create_class (program->script, "Tile", liext_script_tile, self);
	liscr_script_create_class (program->script, "Voxel", liext_script_voxel, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	LIAlgMemdicIter iter0;

	if (self->blocks != NULL)
	{
		LIALG_MEMDIC_FOREACH (iter0, self->blocks)
			liext_block_free (iter0.value);
		lialg_memdic_free (self->blocks);
	}
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	if (self->voxels != NULL)
		livox_manager_free (self->voxels);
	if (self->assign_packet != NULL)
		liarc_writer_free (self->assign_packet);

	lisys_free (self);
}

int
liext_module_build_block (LIExtModule*    self,
                          LIVoxBlockAddr* addr)
{
	LIExtBlock* block;

	/* Find the block. */
	block = lialg_memdic_find (self->blocks, addr, sizeof (LIVoxBlockAddr));
	if (block == NULL)
	{
		block = liext_block_new (self);
		if (block == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, addr, sizeof (LIVoxBlockAddr), block))
		{
			liext_block_free (block);
			return 0;
		}
	}

	/* Build the block. */
	if (!liext_block_build (block, addr))
	{
		lialg_memdic_remove (self->blocks, addr, sizeof (LIVoxBlockAddr));
		liext_block_free (block);
		return 0;
	}

	return 1;
}

int
liext_module_write (LIExtModule* self,
                    LIArcSql*    sql)
{
	return livox_manager_write (self->voxels);
}

/**
 * \brief Gets the voxel manager of the module.
 *
 * This function is used by other modules, such as the NPC module, to interact
 * with the voxel terrain.
 *
 * \warning Accessing the terrain from a different thread isn't safe.
 *
 * \param self Module.
 * \return Voxel manager.
 */
LIVoxManager*
liext_module_get_voxels (LIExtModule* self)
{
	return self->voxels;
}

/*****************************************************************************/

static int
private_block_free (LIExtModule*      self,
                    LIVoxUpdateEvent* event)
{
	return 1;
}

static int
private_block_load (LIExtModule*      self,
                    LIVoxUpdateEvent* event)
{
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	liext_module_build_block (self, &addr);

	return 1;
}

static int
private_tick (LIExtModule* self,
              float        secs)
{
	livox_manager_mark_updates (self->voxels);
	livox_manager_update_marked (self->voxels);

	return 1;
}

/** @} */
/** @} */
