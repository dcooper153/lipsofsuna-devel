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

typedef struct _liimgImage liimgImage;
struct _liimgImage
{
	int width;
	int height;
	void* pixels;
};

liimgImage*
liimg_image_new ();

liimgImage*
liimg_image_new_from_file (const char* path);

void
liimg_image_free (liimgImage* self);

int
liimg_image_load (liimgImage* self,
                  const char* path);

int
liimg_image_save_rgba (liimgImage* self,
                       const char* path);

int
liimg_image_save_s3tc (liimgImage* self,
                       const char* path);

/** @} */
/** @} */
