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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlModelAsyncMerger ModelAsyncMerger
 * @{
 */

#include "lipsofsuna/system.h"
#include "async-merger.h"

static LIMdlModel* private_task_handle (
	LIMdlAsyncMergerTask* task);

static void private_task_free (
	LIMdlAsyncMergerTask* task);

/*****************************************************************************/

/**
 * \brief Creates a new merger.
 * \return New merger.
 */
LIMdlAsyncMerger* limdl_async_merger_new ()
{
	LIMdlAsyncMerger* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIMdlAsyncMerger));
	if (self == NULL)
		return NULL;

	/* Allocate the model. */
	/* This is the currently built model. A pointer to it is kept until the
	   finish command is pushed to the command queue. At that point, a new
	   model is created and ownership of the old one is delegated to the
	   worker thread, until it returns it as a result. */
	self->model = limdl_model_new ();
	if (self->model == NULL)
	{
		limdl_async_merger_free (self);
		return NULL;
	}

	/* Create the worker. */
	self->worker = lisys_serial_worker_new (
		(LISysSerialWorkerTaskHandleFunc) private_task_handle,
		(LISysSerialWorkerTaskFreeFunc) private_task_free,
		(LISysSerialWorkerResultFreeFunc) limdl_model_free);
	if (self->worker == NULL)
	{
		limdl_async_merger_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the merger.
 * \param self Merger.
 */
void limdl_async_merger_free (
	LIMdlAsyncMerger* self)
{
	if (self->worker != NULL)
		lisys_serial_worker_free (self->worker);
	if (self->model != NULL)
		limdl_model_free (self->model);
	lisys_free (self);
}

/**
 * \brief Queues a morphed model merge task.
 * \param self Merger.
 * \param model Model.
 * \param morph_array Array of morphs.
 * \param morph_count Number of morphs. Zero to disable morphing.
 * \param partition_array Array of partitions.
 * \param partition_count Number of partitions. Zero to disable partitioning.
 * \param enable_welding One to enable welding. Zero otherwise.
 * \return One if succeeded. False otherwise.
 */
int limdl_async_merger_add_model (
	LIMdlAsyncMerger*            self,
	const LIMdlModel*            model,
	const LIMdlAsyncMergerMorph* morph_array,
	int                          morph_count,
	const LIMdlAsyncMergerMorph* partition_array,
	int                          partition_count,
	int                          enable_welding)
{
	LIMdlAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIMdlAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->add_model.type = LIMDL_ASYNC_MERGER_ADD_MODEL;
	task->add_model.model = self->model;
	task->add_model.model_add = limdl_model_new_copy (model, 0);
	if (morph_count)
	{
		task->add_model.model_ref = limdl_model_new_copy (model, 1);
		task->add_model.morphs.count = morph_count;
		task->add_model.morphs.array = lisys_calloc (morph_count, sizeof (LIMdlAsyncMergerMorph));
		memcpy (task->add_model.morphs.array, morph_array, morph_count * sizeof (LIMdlAsyncMergerMorph));
	}
	if (partition_count)
	{
		task->add_model.partitions.count = partition_count;
		task->add_model.partitions.array = lisys_calloc (partition_count, sizeof (LIMdlAsyncMergerMorph));
		memcpy (task->add_model.partitions.array, partition_array, partition_count * sizeof (LIMdlAsyncMergerMorph));
	}
	task->add_model.enable_welding = enable_welding;

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/**
 * \brief Queues a building finish task.
 * \param self Merger.
 * \return One if succeeded. False otherwise.
 */
int limdl_async_merger_finish (
	LIMdlAsyncMerger* self)
{
	LIMdlAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIMdlAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->finish.type = LIMDL_ASYNC_MERGER_FINISH;
	task->finish.model = self->model;

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	/* Create the next model. */
	/* The old model is not leaked. Its ownership is transferred to the
	   worker thread. Eventually, the model will be either pushed to
	   the result queue or freed with the task at cleanup. */
	self->model = limdl_model_new ();

	return 1;
}

/**
 * \brief Pops a build result.
 * \param self Merger.
 * \return Model if built. NULL otherwise.
 */
LIMdlModel* limdl_async_merger_pop_model (
	LIMdlAsyncMerger* self)
{
	return lisys_serial_worker_pop_result (self->worker);
}

/**
 * \brief Queues a texture replacement task.
 * \param self Merger.
 * \param match_material Material name to match.
 * \param set_diffuse Diffuse color to set, or NULL to not alter.
 * \param set_specular Specular color to set, or NULL to not alter.
 * \param set_material Material name to set, or NULL to not alter.
 * \param set_textures Texture list to set, or NULL to not alter.
 * \param set_textures_count Number of textures in the texture list.
 * \return One if succeeded. False otherwise.
 */
int limdl_async_merger_replace_material (
	LIMdlAsyncMerger* self,
	const char*       match_material,
	const float*      set_diffuse,
	const float*      set_specular,
	const char*       set_material,
	const char**      set_textures,
	int               set_textures_count)
{
	int i;
	LIMdlAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIMdlAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->replace_material.type = LIMDL_ASYNC_MERGER_REPLACE_MATERIAL;
	task->replace_material.model = self->model;
	task->replace_material.match_material = lisys_string_dup (match_material);
	if (set_diffuse != NULL)
	{
		task->replace_material.set_diffuse = lisys_calloc (4, sizeof (float));
		memcpy (task->replace_material.set_diffuse, set_diffuse, 4 * sizeof (float));
	}
	if (set_specular != NULL)
	{
		task->replace_material.set_specular = lisys_calloc (4, sizeof (float));
		memcpy (task->replace_material.set_specular, set_specular, 4 * sizeof (float));
	}
	if (set_material != NULL)
		task->replace_material.set_material = lisys_string_dup (set_material);
	if (set_textures != NULL)
	{
		task->replace_material.set_textures.count = set_textures_count;
		task->replace_material.set_textures.array = lisys_calloc (set_textures_count, sizeof (char*));
		for (i = 0 ; i < set_textures_count ; i++)
			task->replace_material.set_textures.array[i] = lisys_string_dup (set_textures[i]);
	}

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/*****************************************************************************/

static LIMdlModel* private_task_handle (
	LIMdlAsyncMergerTask* task)
{
	int i;
	LIMdlBuilder* builder;
	const char** tmp;
	LIMdlModel* res = NULL;

	switch (task->type)
	{
		case LIMDL_ASYNC_MERGER_ADD_MODEL:
			if (task->add_model.partitions.count && task->add_model.model_add->partitions.count)
			{
				tmp = lisys_calloc (task->add_model.partitions.count, sizeof (char*));
				if (tmp == NULL)
					break;
				for (i = 0 ; i < task->add_model.partitions.count ; i++)
					tmp[i] = task->add_model.partitions.array[i].shape;
				limdl_model_apply_partitions (task->add_model.model_add,
					tmp, task->add_model.partitions.count);
			}
			if (task->add_model.morphs.count)
			{
				for (i = 0 ; i < task->add_model.morphs.count ; i++)
				{
					limdl_model_morph (
						task->add_model.model_add,
						task->add_model.model_ref,
						task->add_model.model_ref,
						task->add_model.morphs.array[i].shape,
						task->add_model.morphs.array[i].value);
				}
			}
			builder = limdl_builder_new (task->add_model.model);
			if (builder != NULL)
			{
				if (task->add_model.enable_welding)
				{
					if (limdl_builder_merge_model_welded (builder, task->add_model.model_add, NULL))
						limdl_builder_finish (builder, 0);
				}
				else
				{
					if (limdl_builder_merge_model (builder, task->add_model.model_add, NULL))
						limdl_builder_finish (builder, 0);
				}
				limdl_builder_free (builder);
			}
			break;
		case LIMDL_ASYNC_MERGER_FINISH:
			res = task->finish.model;
			task->finish.model = NULL;
			break;
		case LIMDL_ASYNC_MERGER_REPLACE_MATERIAL:
			limdl_model_replace_material_by_string (task->replace_material.model,
				task->replace_material.match_material,
				task->replace_material.set_diffuse,
				task->replace_material.set_specular,
				task->replace_material.set_material,
				(const char**) task->replace_material.set_textures.array,
				task->replace_material.set_textures.count);
			break;
		default:
			lisys_assert (0);
			break;
	}

	return res;
}

static void private_task_free (
	LIMdlAsyncMergerTask* task)
{
	int i;

	switch (task->type)
	{
		case LIMDL_ASYNC_MERGER_ADD_MODEL:
			limdl_model_free (task->add_model.model_add);
			if (task->add_model.model_ref != NULL)
				limdl_model_free (task->add_model.model_ref);
			lisys_free (task->add_model.partitions.array);
			lisys_free (task->add_model.morphs.array);
			break;
		case LIMDL_ASYNC_MERGER_FINISH:
			if (task->finish.model != NULL)
				limdl_model_free (task->finish.model);
			break;
		case LIMDL_ASYNC_MERGER_REPLACE_MATERIAL:
			lisys_free (task->replace_material.match_material);
			lisys_free (task->replace_material.set_diffuse);
			lisys_free (task->replace_material.set_specular);
			lisys_free (task->replace_material.set_material);
			for (i = 0 ; i < task->replace_material.set_textures.count ; i++)
				lisys_free (task->replace_material.set_textures.array[i]);
			lisys_free (task->replace_material.set_textures.array);
			break;
		default:
			lisys_assert (0);
			break;
	}
	lisys_free (task);
}

/** @} */
/** @} */
