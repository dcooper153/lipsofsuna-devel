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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliReload Reload
 * @{
 */

#include <stdio.h>
#include <image/lips-image.h>

/**
 * \brief Converts an image file to DDS.
 *
 * \param src Source file.
 * \param dst Destination file of type DDS.
 * \return Nonzero of success.
 */
int
liext_reload_image (const char* src,
                    const char* dst)
{
	liimgImage* image;

	printf ("CONVERTIMAGE %s %s\n", src, dst);
	image = liimg_image_new_from_file (src);
	if (image == NULL)
		return 0;
	if (!liimg_image_save_s3tc (image, dst) &&
	    !liimg_image_save_rgba (image, dst))
	{
		printf ("FAILED TO SAVE S3TC %s\n", dst);
	}
	liimg_image_free (image);

	return 1;
}

/** @} */
/** @} */
/** @} */
