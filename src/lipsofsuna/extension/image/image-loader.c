/* Lips of Suna
 * Copyright© 2007-2014 Lips of Suna development team.
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
 * \addtogroup LIImgImage Image
 * @{
 */

#include "image-loader.h"

static void private_thread_main (
	LISysThread* thread,
	void*        data);

/*****************************************************************************/

/**
 * \brief Creates a new image loader.
 * \param path Path to the image file.
 * \return New image loader or NULL.
 */
LIImgImageLoader* liimg_image_loader_new (
	const char* path)
{
	LIImgImageLoader* self;

	self = lisys_calloc (1, sizeof (LIImgImageLoader));
	if (self == NULL)
		return NULL;

	self->path = lisys_string_dup (path);
	if (self->path == NULL)
	{
		liimg_image_loader_free (self);
		return NULL;
	}

	self->mutex = lisys_mutex_new ();
	if (self->mutex == NULL)
	{
		liimg_image_loader_free (self);
		return NULL;
	}

	self->thread = lisys_thread_new (private_thread_main, self);
	if (self->thread == NULL)
	{
		liimg_image_loader_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the image loader.
 * \param self ImageLoader.
 */
void liimg_image_loader_free (
	LIImgImageLoader* self)
{
	if (self->thread != NULL)
		lisys_thread_free (self->thread);
	if (self->mutex != NULL)
		lisys_mutex_free (self->mutex);
	if (self->result != NULL)
		liimg_image_free (self->result);
	lisys_free (self->path);
	lisys_free (self);
}

/**
 * \brief Checks whether loading has finished.
 * \param self ImageLoader.
 * \return One if finished loading. Zero otherwise.
 */
int liimg_image_loader_get_done (
	LIImgImageLoader* self)
{
	return self->done;
}

/**
 * \brief Returns the image if it has finished loading.
 * \param self ImageLoader.
 * \return New image if finished loading. NULL otherwise.
 */
LIImgImage* liimg_image_loader_get_image (
	LIImgImageLoader* self)
{
	LIImgImage* res;

	lisys_mutex_lock (self->mutex);
	res = self->result;
	if (res != NULL)
		self->result = NULL;
	lisys_mutex_unlock (self->mutex);

	return res;
}

/*****************************************************************************/

static void private_thread_main (
	LISysThread* thread,
	void*        data)
{
	LIImgImage* image;
	LIImgImageLoader* self = data;

	image = liimg_image_new_from_file (self->path);
	lisys_mutex_lock (self->mutex);
	self->result = image;
	self->done = 1;
	lisys_mutex_unlock (self->mutex);
}

/** @} */
/** @} */
