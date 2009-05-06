/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liimg Image
 * @{
 * \addtogroup liimgImage Image
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <system/lips-system.h>
#include "image-compress.h"
#include "image-dds.h"
#include "image.h"

/**
 * \brief Creates a new empty image.
 *
 * \return New image or NULL.
 */
liimgImage*
liimg_image_new ()
{
	liimgImage* self;

	self = calloc (1, sizeof (liimgImage));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	return self;
}

/**
 * \brief Loads an image from a file.
 *
 * \param path Path to the image file.
 * \return New image or NULL.
 */
liimgImage*
liimg_image_new_from_file (const char* path)
{
	liimgImage* self;

	self = liimg_image_new ();
	if (self == NULL)
		return NULL;
	if (!liimg_image_load (self, path))
	{
		liimg_image_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the image.
 *
 * \param self Image.
 */
void
liimg_image_free (liimgImage* self)
{
	free (self->pixels);
	free (self);
}

/**
 * \brief Loads the image from a file.
 *
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liimg_image_load (liimgImage* self,
                  const char* path)
{
	SDL_Surface* tmp;
	SDL_Surface* image;
	SDL_PixelFormat fmt =
	{
		NULL, 32, 4, 0, 0, 0, 0,
#if __BYTE_ORDER == __BIT_ENDIAN
		24, 16, 8, 0, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
#else
		0, 8, 16, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
#endif
		0, 255
	};

	/* Load the image. */
	image = IMG_Load (path);
	if (image == NULL)
	{
		lisys_error_set (EIO, "cannot load `%s'", path);
		return 0;
	}
	tmp = SDL_ConvertSurface (image, &fmt, SDL_SWSURFACE);
	SDL_FreeSurface (image);
	if (tmp == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	image = tmp;

	/* Copy pixels. */
	self->width = image->w;
	self->height = image->h;
	self->pixels = malloc (4 * image->w * image->h);
	if (self->pixels == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		SDL_FreeSurface (image);
		return 0;
	}
	memcpy (self->pixels, image->pixels, 4 * image->w * image->h);
	SDL_FreeSurface (image);

	return 1;
}

/**
 * \brief Creates an uncompressed DDS file.
 *
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liimg_image_save_rgba (liimgImage* self,
                       const char* path)
{
	int w;
	int h;
	FILE* file;
	liimgDDS dds;

	/* Create a DDS file. */
	file = fopen (path, "wb");
	if (file == NULL)
	{
		lisys_error_set (EIO, NULL);
		return 0;
	}
	liimg_dds_init_rgba (&dds, self->width, self->height, 0); /* FIXME: No mipmaps. */
	liimg_dds_write_header (&dds, file);

	/* Write each mipmap level. */
	w = self->width;
	h = self->height;
	while (w || h)
	{
		w = w? w : 1;
		h = h? h : 1;
		liimg_dds_write_level (&dds, file, 0, /*pixels*/self->pixels, 4 * w * h);
		w >>= 1;
		h >>= 1;
		break; /* FIXME: No mipmaps. */
	}
	fclose (file);

	return 1;
}

/**
 * \brief Creates an S3TC/DXT5 compressed DDS file.
 *
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liimg_image_save_s3tc (liimgImage* self,
                       const char* path)
{
	int w;
	int h;
	int size;
	FILE* file;
	char* bytes;
	char* pixels;
	liimgDDS dds;

	/* Allocate buffers. */
	bytes = malloc (4 * self->width * self->height);
	pixels = malloc (4 * self->width * self->height);
	if (bytes == NULL || pixels == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		free (bytes);
		return 0;
	}

	/* Create a DDS file. */
	file = fopen (path, "wb");
	if (file == NULL)
	{
		lisys_error_set (EIO, NULL);
		free (bytes);
		free (pixels);
		return 0;
	}
	size = liimg_compress_storage (self->width, self->height, 5);
	liimg_dds_init_s3tc (&dds, self->width, self->height, size, 0); /* FIXME: No mipmaps. */
	liimg_dds_write_header (&dds, file);

	/* Write each mipmap level. */
	w = self->width;
	h = self->height;
	while (w || h)
	{
		w = w? w : 1;
		h = h? h : 1;
		liimg_compress_compress (/*pixels*/self->pixels, w, h, 5, bytes);
		liimg_dds_write_level (&dds, file, 0, bytes, liimg_compress_storage (w, h, 5));
		w >>= 1;
		h >>= 1;
		break; /* FIXME: No mipmaps. */
	}
	free (bytes);
	free (pixels);
	fclose (file);

	return 1;
}

/** @} */
/** @} */
