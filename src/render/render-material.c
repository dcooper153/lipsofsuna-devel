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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndMaterial Material
 * @{
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "render-material.h"

/**
 * \brief Creates a new material.
 *
 * \return Material or NULL.
 */
lirndMaterial*
lirnd_material_new ()
{
	lirndMaterial* self;

	self = calloc (1, sizeof (lirndMaterial));
	if (self == NULL)
		return NULL;
	self->shininess = 64;
	return self;
}

/**
 * \brief Frees a material.
 *
 * \param self Material.
 */
void
lirnd_material_free (lirndMaterial* self)
{
	free (self->textures.array);
	free (self);
}

/**
 * \brief Sets the rendering flags of the material.
 *
 * \param self Material.
 * \param flags Flags.
 */
void
lirnd_material_set_flags (lirndMaterial* self,
                          int            flags)
{
	self->flags = flags;
}

/**
 * \brief Assigns a shader to the material.
 *
 * A weak reference to the shader is stored to the material. Thus,
 * the shader must exist for the lifetime of the material or invalid
 * memory access will occur.
 *
 * The shader must be assigned before any textures are assigned and
 * cannot be changed after the first call.
 *
 * \param self Material.
 * \param shader Shader.
 */
int
lirnd_material_set_shader (lirndMaterial* self,
                           lirndShader*   shader)
{
	self->shader = shader;

	return 1;
}

/**
 * \brief Assigns a texture to the material.
 *
 * \param self Material.
 * \param index Texture index.
 * \param texture Texture data.
 * \param image Image.
 */
void
lirnd_material_set_texture (lirndMaterial* self,
                            int            index,
                            limdlTexture*  texture,
                            lirndImage*    image)
{
	if (index < 0 || index >= self->textures.count)
		return;
	lirnd_texture_init (self->textures.array + index, texture);
	lirnd_texture_set_image (self->textures.array + index, image);
}

int
lirnd_material_set_texture_count (lirndMaterial* self,
                                  int            value)
{
	int num;
	lirndTexture* tmp;

	if (value)
	{
		if (value > self->textures.count)
		{
			num = value - self->textures.count;
			tmp = realloc (self->textures.array, value * sizeof (lirndTexture));
			if (tmp == NULL)
				return 0;
			self->textures.array = tmp;
			self->textures.count = value;
			memset (tmp + value - num, 0, num * sizeof (lirndTexture));
		}
		else
		{
			tmp = realloc (self->textures.array, value * sizeof (lirndTexture));
			if (tmp != NULL)
				self->textures.array = tmp;
			self->textures.count = value;
		}
	}
	else
	{
		free (self->textures.array);
		self->textures.array = NULL;
		self->textures.count = 0;
	}

	return 1;
}

/** @} */
/** @} */
