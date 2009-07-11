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
#include <system/lips-system.h>
#include "image-compress.h"
#include "image-dds.h"
#include "image.h"

/**
 * \brief Creates a new empty image.
 *
 * \param video Video calls.
 * \return New image or NULL.
 */
liimgImage*
liimg_image_new (lividCalls* video)
{
	liimgImage* self;

	self = calloc (1, sizeof (liimgImage));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}
	self->video = *video;

	return self;
}

/**
 * \brief Loads an image from a file.
 *
 * \param path Path to the image file.
 * \return New image or NULL.
 */
liimgImage*
liimg_image_new_from_file (lividCalls* video,
                           const char* path)
{
	liimgImage* self;

	self = liimg_image_new (video);
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
#if LI_BYTE_ORDER == LI_BIG_ENDIAN
		24, 16, 8, 0, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF,
#else
		0, 8, 16, 32, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000,
#endif
		0, 255
	};

	/* Load the image. */
	image = self->video.IMG_Load (path);
	if (image == NULL)
	{
		lisys_error_set (EIO, "cannot load `%s'", path);
		return 0;
	}
	tmp = self->video.SDL_ConvertSurface (image, &fmt, SDL_SWSURFACE);
	self->video.SDL_FreeSurface (image);
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
		self->video.SDL_FreeSurface (image);
		return 0;
	}
	memcpy (self->pixels, image->pixels, 4 * image->w * image->h);
	self->video.SDL_FreeSurface (image);

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
 * \warning Does in-place modifications to the image during mipmap generation,
 * destroying the old pixel data and setting the size of the image to 1x1.
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
	int maps;
	int size;
	FILE* file;
	char* bytes;
	liimgDDS dds;

	/* Allocate buffers. */
	bytes = malloc (4 * self->width * self->height);
	if (bytes == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Get mipmap count. */
	w = self->width;
	h = self->height;
	size = w > h? w : h;
	for (maps = 0 ; size ; size >>= 1)
		maps++;

	/* Create a DDS file. */
	file = fopen (path, "wb");
	if (file == NULL)
	{
		lisys_error_set (EIO, NULL);
		free (bytes);
		return 0;
	}
	size = liimg_compress_storage (self->width, self->height, 5);
	liimg_dds_init_s3tc (&dds, self->width, self->height, size, maps);
	liimg_dds_write_header (&dds, file);

	/* Write each mipmap level. */
	while (maps--)
	{
		liimg_compress_compress (self->pixels, w, h, 5, bytes);
		liimg_dds_write_level (&dds, file, 0, bytes, liimg_compress_storage (w, h, 5));
		w >>= 1;
		h >>= 1;
		w = w? w : 1;
		h = h? h : 1;
		liimg_image_shrink_half (self);
	}
	free (bytes);
	fclose (file);

	return 1;
}

/**
 * \brief Shinks the image to half of the original size.
 *
 * Uses a fast in-place algorithm for shrinking the image. The resulting image
 * is half of the original size and has width and height of at least one so it
 * can be used as the next mipmap level of the original image.
 *
 * \param self Image.
 */
void
liimg_image_shrink_half (liimgImage* self)
{
	int x;
	int y;
	int len;
	unsigned char tmp[16];
	unsigned char* dst;
	unsigned char* src0;
	unsigned char* src1;

	dst = self->pixels;
	src0 = self->pixels;
	if (self->width > 1 && self->height > 1)
	{
		len = 4 * self->width;
		src1 = src0 + len;
		for (y = 0 ; y < self->height ; y += 2)
		{
			for (x = 0 ; x < self->width ; x += 2)
			{
				tmp[0] = *(src0++) >> 2;
				tmp[1] = *(src0++) >> 2;
				tmp[2] = *(src0++) >> 2;
				tmp[3] = *(src0++) >> 2;
				tmp[4] = *(src0++) >> 2;
				tmp[5] = *(src0++) >> 2;
				tmp[6] = *(src0++) >> 2;
				tmp[7] = *(src0++) >> 2;
				tmp[8] = *(src1++) >> 2;
				tmp[9] = *(src1++) >> 2;
				tmp[10] = *(src1++) >> 2;
				tmp[11] = *(src1++) >> 2;
				tmp[12] = *(src1++) >> 2;
				tmp[13] = *(src1++) >> 2;
				tmp[14] = *(src1++) >> 2;
				tmp[15] = *(src1++) >> 2;
				*(dst++) = tmp[0] + tmp[4] + tmp[8] + tmp[12];
				*(dst++) = tmp[1] + tmp[5] + tmp[9] + tmp[13];
				*(dst++) = tmp[2] + tmp[6] + tmp[10] + tmp[14];
				*(dst++) = tmp[3] + tmp[7] + tmp[11] + tmp[15];
			}
			src0 += len;
			src1 += len;
		}
		self->width = self->width >> 1;
		self->height = self->height >> 1;
	}
	else if (self->width > 1 || self->height > 1)
	{
		len = 2 * self->width * self->height;
		while (len--)
		{
			tmp[0] = *(src0++) >> 1;
			tmp[1] = *(src0++) >> 1;
			tmp[2] = *(src0++) >> 1;
			tmp[3] = *(src0++) >> 1;
			tmp[4] = *(src0++) >> 1;
			tmp[5] = *(src0++) >> 1;
			tmp[6] = *(src0++) >> 1;
			tmp[7] = *(src0++) >> 1;
			*(dst++) = tmp[0] + tmp[4];
			*(dst++) = tmp[1] + tmp[5];
			*(dst++) = tmp[2] + tmp[6];
			*(dst++) = tmp[3] + tmp[7];
		}
		if (self->width > 1)
			self->width = self->width >> 1;
		else
			self->height = self->height >> 1;
	}
}

/** @} */
/** @} */
