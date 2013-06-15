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
 * \addtogroup LIImg Image
 * @{
 * \addtogroup LIImgImage Image
 * @{
 */

#include <stdlib.h>
#include <string.h>
#include "lipsofsuna/system.h"
#include "lipsofsuna/math.h"
#include "lipsofsuna/render/image/image-dds.h"
#include "color.h"
#include "image.h"
#include "image-compress.h"
#include "image-png.h"

/**
 * \brief Creates a new empty image.
 * \return New image or NULL.
 */
LIImgImage* liimg_image_new ()
{
	LIImgImage* self;

	self = lisys_calloc (1, sizeof (LIImgImage));
	if (self == NULL)
		return NULL;

	return self;
}

/**
 * \brief Loads an image from a file.
 * \param path Path to the image file.
 * \return New image or NULL.
 */
LIImgImage* liimg_image_new_from_file (
	const char* path)
{
	LIImgImage* self;

	self = liimg_image_new ();
	if (self == NULL)
		return NULL;
	if (!liimg_image_load_png (self, path) &&
	    !liimg_image_load_dds (self, path))
	{
		liimg_image_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Creates a copy of the image.
 * \param image Image.
 * \return New image or NULL.
 */
LIImgImage* liimg_image_new_from_image (
	LIImgImage* image)
{
	LIImgImage* self;

	self = lisys_calloc (1, sizeof (LIImgImage));
	if (self == NULL)
		return NULL;
	self->width = image->width;
	self->height = image->height;
	if (self->width && self->height)
	{
		self->pixels = lisys_malloc (4 * self->width * self->height * sizeof (uint8_t));
		if (self->pixels == NULL)
		{
			lisys_free (self);
			return NULL;
		}
		memcpy (self->pixels, image->pixels, 4 * self->width * self->height * sizeof (uint8_t));
	}

	return self;
}

/**
 * \brief Frees the image.
 * \param self Image.
 */
void liimg_image_free (
	LIImgImage* self)
{
	lisys_free (self->pixels);
	lisys_free (self);
}

/**
 * \brief Alters the color of the image in the HSV color space.
 * \param self Image.
 * \param hue_add Amount to add to hue. The hue range is [0,1].
 * \param sat_add Amount to add to saturation. The saturation range is [0,1].
 * \param val_add Amount to add to value. The value range is [0,1].
 */
void liimg_image_add_hsv (
	LIImgImage* self,
	float       hue_add,
	float       sat_add,
	float       val_add)
{
	int x;
	int y;
	uint8_t* dst;
	LIImgColor rgb;
	LIImgColorHSV hsv;

	for (y = 0 ; y < self->height ; y++)
	{
		for (x = 0 ; x < self->width ; x++)
		{
			/* Get the pixel. */
			dst = self->pixels + 4 * (x + y * self->width);

			/* Perform the HSV modification. */
			rgb.r = dst[0];
			rgb.g = dst[1];
			rgb.b = dst[2];
			liimg_color_rgb_to_hsv (&rgb, &hsv);
			hsv.h = fmodf (hsv.h + hue_add, 1.0f);
			hsv.s = LIMAT_CLAMP (hsv.s + sat_add, 0.0f, 1.0f);
			hsv.v = LIMAT_CLAMP (hsv.v + val_add, 0.0f, 1.0f);
			liimg_color_hsv_to_rgb (&hsv, &rgb);

			/* Replace the old pixel. */
			dst[0] = rgb.r;
			dst[1] = rgb.g;
			dst[2] = rgb.b;
		}
	}
}

/**
 * \brief Allocates or reallocates the pixels.
 * \param self Image.
 * \param w Width.
 * \param h Height.
 * \return One on success. Zero otherwise.
 */
int liimg_image_alloc (
	LIImgImage* self,
	int         w,
	int         h)
{
	void* tmp;
	if (w && h)
	{
		tmp = lisys_calloc (1, 4 * w * h);
		if (tmp == NULL)
			return 0;
		self->width = w;
		self->height = h;
		self->pixels = tmp;
	}
	else
	{
		lisys_free (self->pixels);
		self->width = w;
		self->height = h;
		self->pixels = NULL;
	}

	return 1;
}

/**
 * \brief Blits another image over this one.
 * \param self Image.
 * \param image Image.
 */
void liimg_image_blit (
	LIImgImage* self,
	LIImgImage* image)
{
	int x;
	int y;
	int a1;
	int a2;
	uint8_t* src;
	uint8_t* dst;

	for (y = 0 ; y < self->height && y < image->height ; y++)
	{
		for (x = 0 ; x < self->width && x < image->width ; x++)
		{
			src = image->pixels + 4 * (x + y * image->width);
			dst = self->pixels + 4 * (x + y * self->width);
			a1 = src[3];
			a2 = 256 - a1;
			dst[0] = (src[0] * a1 + dst[0] * a2) >> 8;
			dst[1] = (src[1] * a1 + dst[1] * a2) >> 8;
			dst[2] = (src[2] * a1 + dst[2] * a2) >> 8;
			dst[3] = LIMAT_MIN (255, dst[3] + a1);
		}
	}
}

/**
 * \brief Blits an HSV altered image over this one.
 * \param self Image.
 * \param image Image.
 * \param hue_add Amount to add to hue. The hue range is [0,1].
 * \param sat_add Amount to add to saturation. The saturation range is [0,1].
 * \param val_add Amount to add to value. The value range is [0,1].
 */
void liimg_image_blit_hsv_add (
	LIImgImage* self,
	LIImgImage* image,
	float       hue_add,
	float       sat_add,
	float       val_add)
{
	int x;
	int y;
	int a1;
	int a2;
	uint8_t* src;
	uint8_t* dst;
	LIImgColor rgb;
	LIImgColorHSV hsv;

	for (y = 0 ; y < self->height && y < image->height ; y++)
	{
		for (x = 0 ; x < self->width && x < image->width ; x++)
		{
			/* Get the pixels. */
			src = image->pixels + 4 * (x + y * image->width);
			dst = self->pixels + 4 * (x + y * self->width);

			/* Perform the HSV modification. */
			rgb.r = src[0];
			rgb.g = src[1];
			rgb.b = src[2];
			liimg_color_rgb_to_hsv (&rgb, &hsv);
			hsv.h = fmodf (hsv.h + hue_add, 1.0f);
			hsv.s = LIMAT_CLAMP (hsv.s + sat_add, 0.0f, 1.0f);
			hsv.v = LIMAT_CLAMP (hsv.v + val_add, 0.0f, 1.0f);
			liimg_color_hsv_to_rgb (&hsv, &rgb);

			/* Blit to the destination image. */
			a1 = src[3];
			a2 = 256 - a1;
			dst[0] = (rgb.r * a1 + dst[0] * a2) >> 8;
			dst[1] = (rgb.g * a1 + dst[1] * a2) >> 8;
			dst[2] = (rgb.b * a1 + dst[2] * a2) >> 8;
			dst[3] = LIMAT_MIN (255, dst[3] + a1);
		}
	}
}

/**
 * \brief Fills the image.
 * \param self Image.
 * \param color Color.
 */
void liimg_image_fill (
	LIImgImage*       self,
	const LIImgColor* color)
{
	int i;
	int n;
	uint8_t* pixel;

	n = 4 * self->width * self->height;
	for (i = 0 ; i < n ; i += 4)
	{
		pixel = self->pixels + i;
		pixel[0] = color->r;
		pixel[1] = color->g;
		pixel[2] = color->b;
		pixel[3] = color->a;
	}
}

/**
 * \brief Loads the image from a DDS file.
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int liimg_image_load_dds (
	LIImgImage* self,
	const char* path)
{
	int type;
	void* pixels;
	FILE* file;
	LIImgDDS dds;
	LIImgDDSFormat fmt;
	LIImgDDSLevel lvl;

	/* Read header. */
	file = fopen (path, "rb");
	if (file == NULL)
		return 0;
	if (!liimg_dds_read_header (&dds, file))
	{
		fclose (file);
		return 0;
	}
	fmt = dds.info;

	/* Read pixels. */
	if (!liimg_dds_read_level (&dds, file, 0, &lvl))
	{
		fclose (file);
		return 0;
	}
	fclose (file);

	/* Uncompress pixels. */
	pixels = lisys_calloc (1, 4 * lvl.width * lvl.height);
	if (pixels == NULL)
		return 0;
	switch (fmt.type)
	{
		case DDS_TYPE_DXT1: type = 1; break;
		case DDS_TYPE_DXT3: type = 3; break;
		case DDS_TYPE_DXT5: type = 5; break;
		default:
			type = 0;
			break;
	}
	if (type)
	{
#ifdef HAVE_SQUISH
		liimg_compress_uncompress (lvl.data, lvl.width, lvl.height, type, pixels);
#else
		lisys_error_set (EINVAL, "loading compressed textures requires libsquish");
		lisys_free (pixels);
		return 0;
#endif
	}
	else
		memcpy (pixels, lvl.data, 4 * lvl.width * lvl.height);
	lisys_free (lvl.data);

	/* Use the new pixels. */
	lisys_free (self->pixels);
	self->pixels = pixels;
	self->width = lvl.width;
	self->height = lvl.height;

	return 1;
}

/**
 * \brief Loads the image from a PNG file.
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int liimg_image_load_png (
	LIImgImage* self,
	const char* path)
{
	return liimg_png_load (path, &self->width, &self->height, &self->pixels);
}

/**
 * \brief Creates an uncompressed DDS file.
 * \param self Image.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int liimg_image_save_rgba (
	LIImgImage* self,
	const char* path)
{
	int w;
	int h;
	FILE* file;
	LIImgDDS dds;

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
		liimg_dds_write_level (&dds, file, 0, self->pixels, 4 * w * h);
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
int liimg_image_save_s3tc (
	LIImgImage* self,
	const char* path)
{
#ifdef HAVE_SQUISH
	int w;
	int h;
	int maps;
	int size;
	FILE* file;
	char* bytes;
	LIImgDDS dds;

	/* Allocate buffers. */
	bytes = lisys_malloc (4 * self->width * self->height);
	if (bytes == NULL)
		return 0;

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
		lisys_free (bytes);
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
	lisys_free (bytes);
	fclose (file);

	return 1;
#else
	lisys_error_set (EINVAL, "saving compressed textures requires libsquish");
	return 0;
#endif
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
void liimg_image_shrink_half (
	LIImgImage* self)
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

/**
 * \brief Gets the color of the pixel.
 * \param self Image.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \return Color.
 */
LIImgColor liimg_image_get_pixel (
	const LIImgImage* self,
	int               x,
	int               y)
{
	LIImgColor color;
	const uint8_t* pixel;

	pixel = self->pixels + 4 * (x + y * self->width);
	color.r = pixel[0];
	color.g = pixel[1];
	color.b = pixel[2];
	color.a = pixel[3];

	return color;
}

/**
 * \brief Sets the color of the pixel.
 * \param self Image.
 * \param x X coordinate.
 * \param y Y coordinate.
 * \param color Color.
 */
void liimg_image_set_pixel (
	LIImgImage*       self,
	int               x,
	int               y,
	const LIImgColor* color)
{
	uint8_t* pixel;

	pixel = self->pixels + 4 * (x + y * self->width);
	pixel[0] = color->r;
	pixel[1] = color->g;
	pixel[2] = color->b;
	pixel[3] = color->a;
}

/** @} */
/** @} */
