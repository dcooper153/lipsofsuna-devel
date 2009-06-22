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
 * \addtogroup lirel Reload
 * @{
 * \addtogroup lirelGimp Gimp
 * @{
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <image/lips-image.h>
#include <string/lips-string.h>
#include <system/lips-system.h>

/**
 * \brief Converts an XCF image to DDS.
 *
 * \param src Source file of type XCF.
 * \param dst Destination file of type DDS.
 * \return Nonzero of success.
 */
int
lirel_reload_gimp (const char* src,
                   const char* dst)
{
	int ret;
	int len0;
	int len1;
	char* tmp;
	char* script;
	liimgImage* image;
	const char* format =
		"(let* ((image (car (gimp-file-load 1 \"%s\" \"%s\")))"
		"(drawable (car (gimp-image-merge-visible-layers image 1))))"
		"(file-png-save 1 image drawable \"%s\" \"%s\" 1 0 0 0 0 0 0))"
		"(gimp-quit 1)";

	/* Check if source file is missing for sure. */
	if (access (src, R_OK) == -1)
		return 0;

	/* Choose temporary file. */
	/* FIXME: Might conflict with another application. */
	tmp = lisys_path_format ("/tmp",
		LISYS_PATH_SEPARATOR, LISYS_PATH_BASENAME, dst, ".png", NULL);
	if (tmp == NULL)
		return 0;

	/* Format script. */
	len0 = strlen (src);
	len1 = strlen (tmp);
	script = calloc (2 * len0 + 2 * len1 + strlen (format) + 9, sizeof (char));
	if (script == NULL)
	{
		free (tmp);
		return 0;
	}
	sprintf (script, format, src, src, tmp, tmp);

	/* Convert to PNG. */
	ret = lisys_execvl ("gimp", "gimp", "--no-interface", "--no-data", "-b", script, NULL);
	free (script);

	/* Convert to DDS. */
	image = liimg_image_new_from_file (tmp);
	if (image == NULL)
	{
		remove (tmp);
		free (tmp);
		return 0;
	}
	if (!liimg_image_save_s3tc (image, dst) &&
	    !liimg_image_save_rgba (image, dst))
	{
		printf ("FAILED TO SAVE S3TC %s\n", dst);
	}
	liimg_image_free (image);
	remove (tmp);
	free (tmp);

	return ret;
}

/** @} */
/** @} */
