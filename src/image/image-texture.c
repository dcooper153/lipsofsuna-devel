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
 * \addtogroup liimgTexture Texture
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <system/lips-system.h>
#include "image-compress.h"
#include "image-dds.h"
#include "image-texture.h"

/**
 * \brief Creates a new empty texture.
 *
 * \return New texture or NULL.
 */
liimgTexture*
liimg_texture_new ()
{
	liimgTexture* self;

	self = calloc (1, sizeof (liimgTexture));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	return self;
}

/**
 * \brief Loads a texture from a file.
 *
 * \param path Path to the texture file.
 * \return New texture or NULL.
 */
liimgTexture*
liimg_texture_new_from_file (const char* path)
{
	liimgTexture* self;

	self = liimg_texture_new ();
	if (self == NULL)
		return NULL;
	if (!liimg_texture_load (self, path))
	{
		liimg_texture_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the texture.
 *
 * \param self Texture.
 */
void
liimg_texture_free (liimgTexture* self)
{
	if (self->texture)
		glDeleteTextures (1, &self->texture);
	free (self);
}

/**
 * \brief Loads texture data from a DDS file.
 *
 * \param self Texture.
 * \param path Path to the file.
 * \return Nonzero on success.
 */
int
liimg_texture_load (liimgTexture* self,
                    const char*   path)
{
	FILE* file;
	GLuint tex;
	liimgDDS dds;

	/* Load the DDS file. */
	file = fopen (path, "rb");
	if (file == NULL)
	{
		lisys_error_set (EIO, "cannot open file `%s'", path);
		return 0;
	}
	tex = liimg_dds_load_texture (file, &dds);
	fclose (file);
	if (!tex)
	{
		lisys_error_set (EIO, "invalid texture file `%s'", path);
		return 0;
	}
	glDeleteTextures (1, &self->texture);
	self->texture = tex;
	self->width = dds.header.width;
	self->height = dds.header.height;

	return 1;
}

/** @} */
/** @} */
