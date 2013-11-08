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
 * \addtogroup LIImg Image
 * @{
 * \addtogroup LIImgImageAsyncMerger ImageAsyncMerger
 * @{
 */

#include "lipsofsuna/system.h"
#include "async-merger.h"

static LIImgImage* private_task_handle (
	LIImgAsyncMergerTask* task);

static void private_task_free (
	LIImgAsyncMergerTask* task);

/*****************************************************************************/

/**
 * \brief Creates a new merger.
 * \return New merger.
 */
LIImgAsyncMerger* liimg_async_merger_new ()
{
	LIImgAsyncMerger* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIImgAsyncMerger));
	if (self == NULL)
		return NULL;

	/* Allocate the image. */
	/* This is the currently built image. A pointer to it is kept until the
	   finish command is pushed to the command queue. At that point, a new
	   image is created and ownership of the old one is delegated to the
	   worker thread, until it returns it as a result. */
	self->image = liimg_image_new ();
	if (self->image == NULL)
	{
		liimg_async_merger_free (self);
		return NULL;
	}

	/* Create the worker. */
	self->worker = lisys_serial_worker_new (
		(LISysSerialWorkerTaskHandleFunc) private_task_handle,
		(LISysSerialWorkerTaskFreeFunc) private_task_free,
		(LISysSerialWorkerResultFreeFunc) liimg_image_free);
	if (self->worker == NULL)
	{
		liimg_async_merger_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the merger.
 * \param self Merger.
 */
void liimg_async_merger_free (
	LIImgAsyncMerger* self)
{
	if (self->worker != NULL)
		lisys_serial_worker_free (self->worker);
	if (self->image != NULL)
		liimg_image_free (self->image);
	lisys_free (self);
}

/**
 * \brief Queues an HSV addition task.
 * \param self Merger.
 * \param hsv HSV color.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_add_hsv (
	LIImgAsyncMerger*    self,
	const LIImgColorHSV* hsv)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->add_hsv.type = LIIMG_ASYNC_MERGER_ADD_HSV;
	task->add_hsv.image = self->image;
	task->add_hsv.hsv = *hsv;

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/**
 * \brief Queues an HSV addition task.
 * \param self Merger.
 * \param hsv HSV color.
 * \param val_range Value ranged affected by weighting.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_add_hsv_weightv (
	LIImgAsyncMerger*    self,
	const LIImgColorHSV* hsv,
	float                val_range)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->add_hsv_weightv.type = LIIMG_ASYNC_MERGER_ADD_HSV_WEIGHTV;
	task->add_hsv_weightv.image = self->image;
	task->add_hsv_weightv.hsv = *hsv;

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/**
 * \brief Queues an image blit task.
 * \param self Merger.
 * \param image Image.
 * \param dst_rect Destination rectangle. NULL for default.
 * \param src_rect Source rectangle. NULL for default.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_blit (
	LIImgAsyncMerger*   self,
	const LIImgImage*   image,
	const LIMatRectInt* dst_rect,
	const LIMatRectInt* src_rect)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->blit.type = LIIMG_ASYNC_MERGER_BLIT;
	task->blit.image = self->image;
	task->blit.blit = liimg_image_new_from_image (image);
	if (dst_rect != NULL)
		task->blit.dst_rect = *dst_rect;
	else
		limat_rect_int_set (&task->blit.dst_rect, 0, 0, image->width, image->height);
	if (src_rect != NULL)
		task->blit.src_rect = *src_rect;
	else
		limat_rect_int_set (&task->blit.src_rect, 0, 0, image->width, image->height);

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/**
 * \brief Queues an HSV altered image blit task.
 * \param self Merger.
 * \param image Image.
 * \param dst_rect Destination rectangle. NULL for default.
 * \param src_rect Source rectangle. NULL for default.
 * \param hsv HSV color.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_blit_hsv_add (
	LIImgAsyncMerger*    self,
	const LIImgImage*    image,
	const LIMatRectInt*  dst_rect,
	const LIMatRectInt*  src_rect,
	const LIImgColorHSV* hsv)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->blit_hsv_add.type = LIIMG_ASYNC_MERGER_BLIT_HSV_ADD;
	task->blit_hsv_add.image = self->image;
	task->blit_hsv_add.blit = liimg_image_new_from_image (image);
	task->blit_hsv_add.hsv = *hsv;
	if (dst_rect != NULL)
		task->blit_hsv_add.dst_rect = *dst_rect;
	else
		limat_rect_int_set (&task->blit_hsv_add.dst_rect, 0, 0, image->width, image->height);
	if (src_rect != NULL)
		task->blit_hsv_add.src_rect = *src_rect;
	else
		limat_rect_int_set (&task->blit_hsv_add.src_rect, 0, 0, image->width, image->height);

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/**
 * \brief Queues an HSV altered image blit task.
 * \param self Merger.
 * \param image Image.
 * \param dst_rect Destination rectangle. NULL for default.
 * \param src_rect Source rectangle. NULL for default.
 * \param hsv HSV color.
 * \param val_range Value ranged affected by weighting.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_blit_hsv_add_weightv (
	LIImgAsyncMerger*    self,
	const LIImgImage*    image,
	const LIMatRectInt*  dst_rect,
	const LIMatRectInt*  src_rect,
	const LIImgColorHSV* hsv,
	float                val_range)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->blit_hsv_add_weightv.type = LIIMG_ASYNC_MERGER_BLIT_HSV_ADD_WEIGHTV;
	task->blit_hsv_add_weightv.image = self->image;
	task->blit_hsv_add_weightv.blit = liimg_image_new_from_image (image);
	task->blit_hsv_add_weightv.hsv = *hsv;
	task->blit_hsv_add_weightv.val_range = val_range;
	if (dst_rect != NULL)
		task->blit_hsv_add_weightv.dst_rect = *dst_rect;
	else
		limat_rect_int_set (&task->blit_hsv_add_weightv.dst_rect, 0, 0, image->width, image->height);
	if (src_rect != NULL)
		task->blit_hsv_add_weightv.src_rect = *src_rect;
	else
		limat_rect_int_set (&task->blit_hsv_add_weightv.src_rect, 0, 0, image->width, image->height);

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
int liimg_async_merger_finish (
	LIImgAsyncMerger* self)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	task->finish.type = LIIMG_ASYNC_MERGER_FINISH;
	task->finish.image = self->image;

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	/* Create the next image. */
	/* The old image is not leaked. Its ownership is transferred to the
	   worker thread. Eventually, the image will be either pushed to
	   the result queue or freed with the task at cleanup. */
	self->image = liimg_image_new ();

	return 1;
}

/**
 * \brief Pops a build result.
 * \param self Merger.
 * \return Model if built. NULL otherwise.
 */
LIImgImage* liimg_async_merger_pop_image (
	LIImgAsyncMerger* self)
{
	return lisys_serial_worker_pop_result (self->worker);
}

/**
 * \brief Queues an image replace task.
 * \param self Merger.
 * \param image Image.
 * \return One if succeeded. False otherwise.
 */
int liimg_async_merger_replace (
	LIImgAsyncMerger* self,
	const LIImgImage* image)
{
	LIImgAsyncMergerTask* task;

	/* Create the task. */
	task = lisys_calloc (1, sizeof (LIImgAsyncMergerTask));
	if (task == NULL)
		return 0;
	task->replace.type = LIIMG_ASYNC_MERGER_REPLACE;
	task->replace.image = self->image;
	task->replace.replace = liimg_image_new_from_image (image);

	/* Add the task. */
	if (!lisys_serial_worker_push_task (self->worker, task))
	{
		private_task_free (task);
		return 0;
	}

	return 1;
}

/*****************************************************************************/

static LIImgImage* private_task_handle (
	LIImgAsyncMergerTask* task)
{
	LIImgImage* res = NULL;

	switch (task->type)
	{
		case LIIMG_ASYNC_MERGER_ADD_HSV:
			liimg_image_add_hsv (task->add_hsv.image, task->add_hsv.hsv.h,
				task->add_hsv.hsv.s, task->add_hsv.hsv.v);
			break;
		case LIIMG_ASYNC_MERGER_ADD_HSV_WEIGHTV:
			liimg_image_add_hsv_weightv (task->add_hsv_weightv.image,
				task->add_hsv_weightv.hsv.h, task->add_hsv_weightv.hsv.s,
				task->add_hsv_weightv.hsv.v, task->add_hsv_weightv.val_range);
			break;
		case LIIMG_ASYNC_MERGER_BLIT:
			liimg_image_blit (task->blit.image, task->blit.blit,
				&task->blit.dst_rect, &task->blit.src_rect);
			break;
		case LIIMG_ASYNC_MERGER_BLIT_HSV_ADD:
			liimg_image_blit_hsv_add (task->blit_hsv_add.image, task->blit_hsv_add.blit,
				&task->blit_hsv_add.dst_rect, &task->blit_hsv_add.src_rect,
				task->blit_hsv_add.hsv.h, task->blit_hsv_add.hsv.s, task->blit_hsv_add.hsv.v);
			break;
		case LIIMG_ASYNC_MERGER_BLIT_HSV_ADD_WEIGHTV:
			liimg_image_blit_hsv_add_weightv (
				task->blit_hsv_add_weightv.image, task->blit_hsv_add_weightv.blit,
				&task->blit_hsv_add_weightv.dst_rect, &task->blit_hsv_add_weightv.src_rect,
				task->blit_hsv_add_weightv.hsv.h, task->blit_hsv_add_weightv.hsv.s, task->blit_hsv_add_weightv.hsv.v,
				task->blit_hsv_add_weightv.val_range);
			break;
		case LIIMG_ASYNC_MERGER_FINISH:
			res = task->finish.image;
			task->finish.image = NULL;
			break;
		case LIIMG_ASYNC_MERGER_REPLACE:
			liimg_image_replace (task->replace.image, task->replace.replace);
			break;
		default:
			lisys_assert (0);
			break;
	}

	return res;
}

static void private_task_free (
	LIImgAsyncMergerTask* task)
{
	switch (task->type)
	{
		case LIIMG_ASYNC_MERGER_ADD_HSV:
			break;
		case LIIMG_ASYNC_MERGER_ADD_HSV_WEIGHTV:
			break;
		case LIIMG_ASYNC_MERGER_BLIT:
			liimg_image_free (task->blit.blit);
			break;
		case LIIMG_ASYNC_MERGER_BLIT_HSV_ADD:
			liimg_image_free (task->blit_hsv_add.blit);
			break;
		case LIIMG_ASYNC_MERGER_BLIT_HSV_ADD_WEIGHTV:
			liimg_image_free (task->blit_hsv_add_weightv.blit);
			break;
		case LIIMG_ASYNC_MERGER_FINISH:
			if (task->finish.image != NULL)
				liimg_image_free (task->finish.image);
			break;
		case LIIMG_ASYNC_MERGER_REPLACE:
			liimg_image_free (task->replace.replace);
			break;
		default:
			lisys_assert (0);
			break;
	}
	lisys_free (task);
}

/** @} */
/** @} */
