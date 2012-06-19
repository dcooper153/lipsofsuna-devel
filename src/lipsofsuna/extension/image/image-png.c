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
 * \addtogroup LIImgImageUtil ImageUtil
 * @{
 */

#include "image-png.h"
#ifdef HAVE_FREEIMAGE
#include <FreeImage.h>
#else
#include <png.h>
#endif

int liimg_png_load (
	const char* file,
	int*        result_width,
	int*        result_height,
	void**      result_pixels)
{
#ifdef HAVE_FREEIMAGE
	size_t i;
	size_t x;
	size_t y;
	size_t w;
	size_t h;
	uint8_t* pixels;
	FIBITMAP* image;
	RGBQUAD color;

	/* Load the image. */
	image = FreeImage_Load (FIF_PNG, file, PNG_DEFAULT);
	if (image == NULL)
		return 0;

	/* Allocate pixel data. */
	w = FreeImage_GetWidth (image);
	h = FreeImage_GetHeight (image);
	if (w > 0 && h > 0)
	{
		pixels = lisys_calloc (w * h * 4, 1);
		if (pixels == NULL)
		{
			FreeImage_Unload (image);
			return 0;
		}
	}
	else
		pixels = NULL;

	/* Copy the pixel data. */
	for (y = 0, i = 0 ; y < h ; y++)
	{
		for (x = 0 ; x < w ; x++, i++)
		{
			FreeImage_GetPixelColor (image, x, h - y - 1, &color);
			pixels[4 * i + 0] = color.rgbBlue;
			pixels[4 * i + 1] = color.rgbGreen;
			pixels[4 * i + 2] = color.rgbBlue;
			pixels[4 * i + 3] = color.rgbReserved;
		}
	}

	/* Set the results. */
	*result_width = w;
	*result_height = h;
	*result_pixels = pixels;
	FreeImage_Unload (image);

	return 1;
#else
	int x;
	int y;
	int depth;
	int width;
	int height;
	char* dst;
	void* pixels;
	FILE* file;
	png_bytepp rows;
	png_infop info;
	png_structp png;

	/* Initialize structures. */
	png = png_create_read_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}
	info = png_create_info_struct (png);
	if (info == NULL)
	{
		png_destroy_read_struct (&png, NULL, NULL);
		lisys_error_set (ENOMEM, NULL);
		return 0;
	}

	/* Open file. */
	file = fopen (path, "rb");
	if (file == NULL)
	{
		lisys_error_set (EIO, "cannot open file `%s'", path);
		png_destroy_read_struct (&png, &info, NULL);
		return 0;
	}

	/* Read data. */
	if (setjmp (png_jmpbuf (png)))
	{
		lisys_error_set (EIO, "error while reading `%s'", path);
		png_destroy_read_struct (&png, &info, NULL);
		fclose (file);
		return 0;
	}
	png_init_io (png, file);
	png_read_png (png, info, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING, NULL);
	width = png_get_image_width (png, info);
	height = png_get_image_height (png, info);
	rows = png_get_rows (png, info);
	depth = png_get_rowbytes (png, info);
	depth /= width;
	fclose (file);

	/* Allocate pixel data. */
	pixels = lisys_malloc (width * height * 4);
	if (pixels == NULL)
	{
		png_destroy_read_struct (&png, &info, NULL);
		return 0;
	}

	/* Copy pixel data. */
	if (depth == 3)
	{
		for (y = 0 ; y < height ; y++)
		{
			dst = pixels + 4 * width * y;
			for (x = 0 ; x < width ; x++)
			{
				dst[4 * x + 0] = ((char*) rows[y])[3 * x + 0];
				dst[4 * x + 1] = ((char*) rows[y])[3 * x + 1];
				dst[4 * x + 2] = ((char*) rows[y])[3 * x + 2];
				dst[4 * x + 3] = 0xFF;
			}
		}
	}
	else
	{
		for (y = 0 ; y < height ; y++)
		{
			dst = pixels + 4 * width * y;
			memcpy (dst, rows[y], 4 * width);
		}
	}
	lisys_free (self->pixels);
	*result_pixels = pixels;
	*result_width = width;
	*result_height = height;
	png_destroy_read_struct (&png, &info, NULL);

	return 1;
#endif
}

/** @} */
/** @} */
