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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include "ext-module.h"
#include "ext-block.h"

static int private_block_free (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

static int private_block_load (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

static int private_tick (
	LIExtModule* self,
	float        secs);

/*****************************************************************************/

LIMaiExtensionInfo liext_tiles_render_info =
{
	LIMAI_EXTENSION_VERSION, "tiles-render",
	liext_tiles_render_new,
	liext_tiles_render_free
};

LIExtModule* liext_tiles_render_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");

	/* Allocate block list. */
	self->blocks = lialg_memdic_new ();
	if (self->blocks == NULL)
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	/* Make sure the tiles extension is loaded. */
	if (!limai_program_insert_extension (program, "tiles"))
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	/* Find the voxel manager. */
	self->voxels = limai_program_find_component (program, "voxels");
	if (self->voxels == NULL)
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 1, private_tick, self, self->calls + 0) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-free", 1, private_block_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->voxels->callbacks, self->voxels, "block-load", 1, private_block_load, self, self->calls + 2))
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	return self;
}

void liext_tiles_render_free (
	LIExtModule* self)
{
	LIAlgMemdicIter iter;

	/* Free callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free render blocks. */
	if (self->blocks != NULL)
	{
		LIALG_MEMDIC_FOREACH (iter, self->blocks)
			liext_tiles_render_block_free (iter.value);
		lialg_memdic_free (self->blocks);
	}

	lisys_free (self);
}

int liext_tiles_render_build_all (
	LIExtModule* self)
{
	int ret = 1;
	LIAlgMemdicIter iter;
	LIExtBlock* block;
	LIVoxBlockAddr* addr;

	LIALG_MEMDIC_FOREACH (iter, self->blocks)
	{
		addr = iter.key;
		block = iter.value;
		ret &= liext_tiles_render_build_block (self, addr);
	}

	return ret;
}

int liext_tiles_render_build_block (
	LIExtModule*    self,
	LIVoxBlockAddr* addr)
{
	LIExtBlock* block;

	/* Find the block. */
	block = lialg_memdic_find (self->blocks, addr, sizeof (LIVoxBlockAddr));
	if (block == NULL)
	{
		block = liext_tiles_render_block_new (self);
		if (block == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, addr, sizeof (LIVoxBlockAddr), block))
		{
			liext_tiles_render_block_free (block);
			return 0;
		}
	}

	/* Build the block. */
	if (!liext_tiles_render_block_build (block, addr))
	{
		lialg_memdic_remove (self->blocks, addr, sizeof (LIVoxBlockAddr));
		liext_tiles_render_block_free (block);
		return 0;
	}

	return 1;
}

void liext_tiles_render_clear_all (
	LIExtModule* self)
{
	LIAlgMemdicIter iter;
	LIExtBlock* block;

	LIALG_MEMDIC_FOREACH (iter, self->blocks)
	{
		block = iter.value;
		liext_tiles_render_block_clear (block);
	}
}

/*****************************************************************************/

static int private_block_free (
	LIExtModule*      self,
	LIVoxUpdateEvent* event)
{
	LIExtBlock* block;
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	block = lialg_memdic_find (self->blocks, &addr, sizeof (addr));
	if (block != NULL)
	{
		lialg_memdic_remove (self->blocks, &addr, sizeof (addr));
		liext_tiles_render_block_free (block);
	}

	return 1;
}

static int private_block_load (
	LIExtModule*      self,
	LIVoxUpdateEvent* event)
{
	LIVoxBlockAddr addr;

	addr.sector[0] = event->sector[0];
	addr.sector[1] = event->sector[1];
	addr.sector[2] = event->sector[2];
	addr.block[0] = event->block[0];
	addr.block[1] = event->block[1];
	addr.block[2] = event->block[2];
	if (!liext_tiles_render_build_block (self, &addr))
		lisys_error_report ();

	return 1;
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	livox_manager_update (self->voxels, secs);

	return 1;
}

/** @} */
/** @} */
/** @} */
