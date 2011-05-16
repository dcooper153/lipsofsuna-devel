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
 * \addtogroup LIExtTilesRender TilesRender
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

static int private_process_result (
	LIExtModule*    self,
	LIExtBuildTask* task);

static int private_tick (
	LIExtModule* self,
	float        secs);

static void private_worker_thread (
	LIThrAsyncCall* call,
	void*           data);

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
	self->tasks.mutex = lithr_mutex_new ();
	if (self->tasks.mutex == NULL)
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	/* Allocate block list. */
	self->blocks = lialg_memdic_new ();
	if (self->blocks == NULL)
	{
		liext_tiles_render_free (self);
		return NULL;
	}

	/* Make sure that the required extensions are loaded. */
	if (!limai_program_insert_extension (program, "render") ||
	    !limai_program_insert_extension (program, "tiles"))
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
	if (!lical_callbacks_insert (program->callbacks, "tick", 1, private_tick, self, self->calls + 0) ||
	    !lical_callbacks_insert (self->voxels->callbacks, "block-free", 1, private_block_free, self, self->calls + 1) ||
	    !lical_callbacks_insert (self->voxels->callbacks, "block-load", 1, private_block_load, self, self->calls + 2))
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
	LIExtBuildTask* task;
	LIExtBuildTask* task_next;

	/* Free callbacks. */
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));

	/* Free render blocks. */
	if (self->blocks != NULL)
	{
		LIALG_MEMDIC_FOREACH (iter, self->blocks)
			liext_tiles_render_block_free (iter.value);
		lialg_memdic_free (self->blocks);
	}

	/* Stop the worker thread. */
	if (self->tasks.worker != NULL)
	{
		lithr_async_call_stop (self->tasks.worker);
		lithr_async_call_free (self->tasks.worker);
	}
	if (self->tasks.mutex != NULL)
		lithr_mutex_free (self->tasks.mutex);

	/* Free unhandled build tasks. */
	for (task = self->tasks.pending ; task != NULL ; task = task_next)
	{
		task_next = task->next;
		livox_builder_free (task->builder);
		lisys_free (task);
	}
	for (task = self->tasks.completed ; task != NULL ; task = task_next)
	{
		task_next = task->next;
		if (task->model != NULL)
			limdl_model_free (task->model);
		lisys_free (task);
	}

	lisys_free (self);
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
	int blockw;
	LIVoxManager* manager;
	LIExtBuildTask* ptr;
	LIExtBuildTask* task;

	/* Allocate a new task. */
	task = lisys_calloc (1, sizeof (LIExtBuildTask));
	if (task == NULL)
		return 1;
	task->addr.sector[0] = event->sector[0];
	task->addr.sector[1] = event->sector[1];
	task->addr.sector[2] = event->sector[2];
	task->addr.block[0] = event->block[0];
	task->addr.block[1] = event->block[1];
	task->addr.block[2] = event->block[2];
	manager = self->voxels;
	blockw = manager->tiles_per_line / manager->blocks_per_line;

	/* Initialize a new terrain builder. */
	task->builder = livox_builder_new (self->voxels,
		manager->tiles_per_line * event->sector[0] + blockw * event->block[0],
		manager->tiles_per_line * event->sector[1] + blockw * event->block[1],
		manager->tiles_per_line * event->sector[2] + blockw * event->block[2],
		blockw, blockw, blockw);
	if (task->builder == NULL)
	{
		lisys_free (task);
		return 1;
	}

	/* Add the task to the pending queue. */
	lithr_mutex_lock (self->tasks.mutex);
	if (self->tasks.pending != NULL)
	{
		for (ptr = self->tasks.pending ; ptr->next != NULL ; ptr = ptr->next) {}
		ptr->next = task;
	}
	else
		self->tasks.pending = task;
	lithr_mutex_unlock (self->tasks.mutex);

	return 1;
}

static int private_process_result (
	LIExtModule*    self,
	LIExtBuildTask* task)
{
	LIExtBlock* block;

	/* Find or create the affected block. */
	block = lialg_memdic_find (self->blocks, &task->addr, sizeof (LIVoxBlockAddr));
	if (block == NULL)
	{
		if (task->model == NULL)
			return 0;
		block = liext_tiles_render_block_new (self);
		if (block == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, &task->addr, sizeof (LIVoxBlockAddr), block))
		{
			liext_tiles_render_block_free (block);
			return 0;
		}
	}
	else
	{
		if (task->model == NULL)
		{
			lialg_memdic_remove (self->blocks, &task->addr, sizeof (LIVoxBlockAddr));
			liext_tiles_render_block_free (block);
			return 0;
		}
	}

	/* Replace the model of the block. */
	liext_tiles_render_block_clear (block);
	if (task->model != NULL)
	{
		block->model = liren_model_new (self->client->render, task->model, 0);
		if (block->model != NULL)
		{
			block->object = liren_object_new (self->client->scene, 0);
			if (block->object != NULL)
				liren_object_set_model (block->object, block->model);
		}
	}
	if (block->object != NULL)
		liren_object_set_realized (block->object, 1);

	return 1;
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	LIExtBuildTask* task;
	LIExtBuildTask* task_next;

	/* Build blocks in another thread. */
	/* Without this, there'd be major stuttering when multiple blocks are
	   loaded quickly. That can happen when, for example, the player moves fast,
	   teleports to a new area, or witnesses a lot of terrain destruction. */
	lithr_mutex_lock (self->tasks.mutex);
	if (self->tasks.worker != NULL && lithr_async_call_get_done (self->tasks.worker))
	{
		lithr_async_call_free (self->tasks.worker);
		self->tasks.worker = NULL;
	}
	for (task = self->tasks.completed ; task != NULL ; task = task_next)
	{
		task_next = task->next;
		private_process_result (self, task);
		if (task->model != NULL)
			limdl_model_free (task->model);
		lisys_free (task);
	}
	self->tasks.completed = NULL;
	if (self->tasks.worker == NULL && self->tasks.pending != NULL)
		self->tasks.worker = lithr_async_call_new (private_worker_thread, NULL, self);
	lithr_mutex_unlock (self->tasks.mutex);

	/* FIXME: Should be in the tiles module? */
	livox_manager_update (self->voxels, secs);

	return 1;
}

static void private_worker_thread (
	LIThrAsyncCall* call,
	void*           data)
{
	LIExtBuildTask* ptr;
	LIExtBuildTask* task;
	LIExtModule* self = data;

	lithr_mutex_lock (self->tasks.mutex);
	while (!lithr_async_call_get_stop (call))
	{
		/* Get the next task. */
		task = self->tasks.pending;
		if (task == NULL)
			break;
		self->tasks.pending = task->next;
		lithr_mutex_unlock (self->tasks.mutex);

		/* Process the task. */
		if (!livox_builder_build (task->builder, &task->model))
		{
			livox_builder_free (task->builder);
			lithr_mutex_lock (self->tasks.mutex);
			continue;
		}
		livox_builder_free (task->builder);
		task->builder = NULL;
		task->next = NULL;

		/* Publish the result. */
		lithr_mutex_lock (self->tasks.mutex);
		if (self->tasks.completed != NULL)
		{
			for (ptr = self->tasks.completed ; ptr->next != NULL ; ptr = ptr->next) {}
			ptr->next = task;
		}
		else
			self->tasks.completed = task;
	}
	lithr_mutex_unlock (self->tasks.mutex);
}

/** @} */
/** @} */
