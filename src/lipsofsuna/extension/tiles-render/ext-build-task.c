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
 * \addtogroup LIExtTilesRender TilesRender
 * @{
 */

#include "ext-module.h"
#include "ext-build-task.h"

LIExtBuildTask* liext_tiles_build_task_new (
	LIExtModule*      module,
	LIVoxUpdateEvent* event)
{
	int blockw;
	LIExtBuildTask* self;
	LIVoxManager* manager;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBuildTask));
	if (self == NULL)
		return NULL;
	manager = module->voxels;

	/* Initialize the block address. */
	self->addr.sector[0] = event->sector[0];
	self->addr.sector[1] = event->sector[1];
	self->addr.sector[2] = event->sector[2];
	self->addr.block[0] = event->block[0];
	self->addr.block[1] = event->block[1];
	self->addr.block[2] = event->block[2];

	/* Create a new terrain builder. */
	blockw = manager->tiles_per_line / manager->blocks_per_line;
	self->builder = livox_builder_new (manager,
		manager->tiles_per_line * event->sector[0] + blockw * event->block[0],
		manager->tiles_per_line * event->sector[1] + blockw * event->block[1],
		manager->tiles_per_line * event->sector[2] + blockw * event->block[2],
		blockw, blockw, blockw);
	if (self->builder == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void liext_tiles_build_task_free (
	LIExtBuildTask* self)
{
	if (self->builder != NULL)
		livox_builder_free (self->builder);
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self);
}

int liext_tiles_build_task_compare (
	LIExtBuildTask* self,
	LIExtBuildTask* task)
{
	return !memcmp (&self->addr, &task->addr, sizeof (LIVoxBlockAddr));
}

int liext_tiles_build_task_process (
	LIExtBuildTask* self)
{
	int ret;

	/* Preprocess the voxels. */
	livox_builder_preprocess (self->builder);

	/* Build the model. */
	ret = livox_builder_build_model (self->builder, &self->offset, &self->model);

	/* Free the builder. */
	livox_builder_free (self->builder);
	self->builder = NULL;

	return ret;
}

/** @} */
/** @} */
