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
#include "ext-block.h"
#include "ext-build-task.h"

static int private_block_free (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

static int private_block_load (
	LIExtModule*      self,
	LIVoxUpdateEvent* event);

static int private_process_result (
	LIExtModule*    self,
	LIExtBuildTask* task);

static void private_remove_redundant_tasks (
	LIExtBuildTask** tasks);

static int private_tick (
	LIExtModule* self,
	float        secs);

static void private_worker_thread (
	LISysAsyncCall* call,
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
	self->tasks.mutex = lisys_mutex_new ();
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

	/* Find the render manager. */
	self->render = limai_program_find_component (program, "render");
	if (self->render == NULL)
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
		lisys_async_call_stop (self->tasks.worker);
		lisys_async_call_free (self->tasks.worker);
	}
	if (self->tasks.mutex != NULL)
		lisys_mutex_free (self->tasks.mutex);

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
	LIExtBuildTask* ptr;
	LIExtBuildTask* task;

	/* Allocate a new task. */
	task = liext_tiles_build_task_new (self, event);
	if (task == NULL)
		return 1;

	/* Add the task to the pending queue. */
	lisys_mutex_lock (self->tasks.mutex);
	if (self->tasks.pending != NULL)
	{
		for (ptr = self->tasks.pending ; ptr->next != NULL ; ptr = ptr->next) {}
		ptr->next = task;
	}
	else
		self->tasks.pending = task;
	lisys_mutex_unlock (self->tasks.mutex);

	return 1;
}

static int private_process_result (
	LIExtModule*    self,
	LIExtBuildTask* task)
{
	LIExtBlock* block;
	LIMatTransform transform;

	/* Delete emptied blocks. */
	if (task->model == NULL)
	{
		block = lialg_memdic_find (self->blocks, &task->addr, sizeof (LIVoxBlockAddr));
		if (block != NULL)
		{
			lialg_memdic_remove (self->blocks, &task->addr, sizeof (LIVoxBlockAddr));
			liext_tiles_render_block_free (block);
		}
		return 0;
	}

	/* Find or create the block. */
	block = lialg_memdic_find (self->blocks, &task->addr, sizeof (LIVoxBlockAddr));
	if (block == NULL)
	{
		block = liext_tiles_render_block_new (self);
		if (block == NULL)
			return 0;
		if (!lialg_memdic_insert (self->blocks, &task->addr, sizeof (LIVoxBlockAddr), block))
		{
			liext_tiles_render_block_free (block);
			return 0;
		}
	}

	/* Start creating the render model. */
	if (block->model_next)
		liren_render_model_free (self->render, block->model_next);
	block->model_next = liren_render_model_new (self->render, task->model, 0);

	/* Start creating the render object. */
	if (block->object_next)
		liren_render_object_free (self->render, block->object_next);
	block->object_next = liren_render_object_new (self->render, 0);
	if (block->object_next)
	{
		transform = limat_transform_init (task->offset, limat_quaternion_identity ());
		liren_render_object_set_model (self->render, block->object_next, block->model_next);
		liren_render_object_set_transform (self->render, block->object_next, &transform);
		liren_render_object_set_realized (self->render, block->object_next, 1);
	}
	else
	{
		liren_render_model_free (self->render, block->model_next);
		block->model_next = 0;
	}

	return 1;
}

static void private_remove_redundant_tasks (
	LIExtBuildTask** tasks)
{
	LIExtBuildTask* task1;
	LIExtBuildTask* task1_next;
	LIExtBuildTask* task1_prev;
	LIExtBuildTask* task2;

	task1_prev = NULL;
	for (task1 = *tasks ; task1 != NULL ; task1 = task1_next)
	{
		/* Search for a duplicate. */
		task1_next = task1->next;
		for (task2 = task1->next ; task2 != NULL ; task2 = task2->next)
		{
			if (liext_tiles_build_task_compare (task1, task2))
				break;
		}
		if (task2 == NULL)
		{
			task1_prev = task1;
			continue;
		}

		/* Remove from the list. */
		if (task1_prev == NULL)
			*tasks = task1->next;
		else
			task1_prev->next = task1->next;

		/* Free. */
		liext_tiles_build_task_free (task1);
	}
}

static int private_tick (
	LIExtModule* self,
	float        secs)
{
	int i;
	LIAlgMemdicIter iter;
	LIExtBlock* block;
	LIExtBuildTask* task;
	LIExtBuildTask* task_next;

	/* Build blocks in another thread. */
	/* Without this, there'd be major stuttering when multiple blocks are
	   loaded quickly. That can happen when, for example, the player moves fast,
	   teleports to a new area, or witnesses a lot of terrain destruction. */
	lisys_mutex_lock (self->tasks.mutex);
	if (self->tasks.worker != NULL && lisys_async_call_get_done (self->tasks.worker))
	{
		lisys_async_call_free (self->tasks.worker);
		self->tasks.worker = NULL;
	}
	for (i = 0, task = self->tasks.completed ; task != NULL && i < 5 ; task = task_next, i++)
	{
		task_next = task->next;
		private_process_result (self, task);
		liext_tiles_build_task_free (task);
	}
	self->tasks.completed = task;
	if (self->tasks.worker == NULL && self->tasks.pending != NULL)
		self->tasks.worker = lisys_async_call_new (private_worker_thread, NULL, self);
	lisys_mutex_unlock (self->tasks.mutex);

	/* Clear replaced blocks when the replacement has finished loading. */
	LIALG_MEMDIC_FOREACH (iter, self->blocks)
	{
		/* Filter out up-to-date blocks. */
		block = iter.value;
		if (!block->model_next || !block->object_next)
			continue;
		if (!liren_render_object_get_loaded (self->render, block->object_next))
			continue;

		/* Replace the old model. */
		if (block->model)
			liren_render_model_free (self->render, block->model);
		block->model = block->model_next;
		block->model_next = 0;

		/* Replace the old object. */
		if (block->object)
			liren_render_object_free (self->render, block->object);
		block->object = block->object_next;
		block->object_next = 0;
	}

	return 1;
}

static void private_worker_thread (
	LISysAsyncCall* call,
	void*           data)
{
	LIExtBuildTask* ptr;
	LIExtBuildTask* task;
	LIExtModule* self = data;

	lisys_mutex_lock (self->tasks.mutex);
	while (!lisys_async_call_get_stop (call))
	{
		/* Get the next task. */
		private_remove_redundant_tasks (&self->tasks.pending);
		task = self->tasks.pending;
		if (task == NULL)
			break;
		self->tasks.pending = task->next;
		task->next = NULL;
		lisys_mutex_unlock (self->tasks.mutex);

		/* Process the task. */
		if (!liext_tiles_build_task_process (task))
		{
			liext_tiles_build_task_free (task);
			lisys_mutex_lock (self->tasks.mutex);
			continue;
		}

		/* Publish the result. */
		lisys_mutex_lock (self->tasks.mutex);
		if (self->tasks.completed != NULL)
		{
			for (ptr = self->tasks.completed ; ptr->next != NULL ; ptr = ptr->next) {}
			ptr->next = task;
			private_remove_redundant_tasks (&self->tasks.completed);
		}
		else
			self->tasks.completed = task;
	}
	lisys_mutex_unlock (self->tasks.mutex);
}

/** @} */
/** @} */
