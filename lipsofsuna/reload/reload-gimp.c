/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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

#include <lipsofsuna/image.h>
#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "reload.h"

/**
 * \brief Converts an XCF image to DDS.
 *
 * \param self Reload.
 * \param src Source file of type XCF.
 * \param dst Destination file of type DDS.
 * \return Nonzero of success.
 */
int
lirel_reload_gimp (LIRelReload* self,
                   const char*  src,
                   const char*  dst)
{
	int ret;
	int len0;
	int len1;
	char* tmp;
	char* script;
	LIImgImage* image;
	const char* format =
		"(let* ((image (car (gimp-file-load 1 \"%s\" \"%s\")))"
		"(drawable (car (gimp-image-merge-visible-layers image 1))))"
		"(file-png-save 1 image drawable \"%s\" \"%s\" 1 0 0 0 0 0 0))"
		"(gimp-quit 1)";

	/* Check if source file is missing for sure. */
	if (!lisys_access (src, LISYS_ACCESS_READ))
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
	script = lisys_calloc (2 * len0 + 2 * len1 + strlen (format) + 9, sizeof (char));
	if (script == NULL)
	{
		lisys_free (tmp);
		return 0;
	}
	sprintf (script, format, src, src, tmp, tmp);

	/* Convert to PNG. */
	ret = lisys_execvl ("gimp", "gimp", "--no-interface", "--no-data", "-b", script, NULL);
	lisys_free (script);

	/* Convert to DDS. */
	image = liimg_image_new_from_file (tmp);
	if (image == NULL)
	{
		remove (tmp);
		lisys_free (tmp);
		return 0;
	}
	if (!liimg_image_save_s3tc (image, dst) &&
	    !liimg_image_save_rgba (image, dst))
	{
		printf ("FAILED TO SAVE S3TC %s\n", dst);
	}
	liimg_image_free (image);
	remove (tmp);
	lisys_free (tmp);

	return ret;
}

/** @} */
/** @} */
