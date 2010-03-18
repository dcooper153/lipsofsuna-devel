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
 * \addtogroup liren Render
 * @{
 * \addtogroup LIRenMaterial Material
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-material.h"

/**
 * \brief Creates a new material.
 *
 * \return Material or NULL.
 */
LIRenMaterial*
liren_material_new ()
{
	LIRenMaterial* self;

	self = lisys_calloc (1, sizeof (LIRenMaterial));
	if (self == NULL)
		return NULL;
	self->shininess = 64;

	return self;
}

LIRenMaterial*
liren_material_new_from_model (LIRenRender*         render,
                               const LIMdlMaterial* material)
{
	int j;
	char* name;
	LIMdlTexture* texture;
	LIRenImage* image;
	LIRenMaterial* self;

	self = lisys_calloc (1, sizeof (LIRenMaterial));
	if (self == NULL)
		return NULL;
	if (material->flags & LIMDL_MATERIAL_FLAG_BILLBOARD)
		self->flags |= LIREN_MATERIAL_FLAG_BILLBOARD;
	if (material->flags & LIMDL_MATERIAL_FLAG_CULLFACE)
		self->flags |= LIREN_MATERIAL_FLAG_CULLFACE;
	if (material->flags & LIMDL_MATERIAL_FLAG_TRANSPARENCY)
		self->flags |= LIREN_MATERIAL_FLAG_TRANSPARENCY;
	self->parameters[0] = material->emission;
	self->shininess = material->shininess;
	self->diffuse[0] = material->diffuse[0];
	self->diffuse[1] = material->diffuse[1];
	self->diffuse[2] = material->diffuse[2];
	self->diffuse[3] = material->diffuse[3];
	self->specular[0] = material->specular[0];
	self->specular[1] = material->specular[1];
	self->specular[2] = material->specular[2];
	self->specular[3] = material->specular[3];
	self->strand_start = material->strand_start;
	self->strand_end = material->strand_end;
	self->strand_shape = material->strand_shape;

	/* Find deferred shader. */
	name = listr_concat ("deferred-", material->shader);
	if (name == NULL)
	{
		liren_material_free (self);
		return 0;
	}
	self->shader_deferred = liren_render_find_shader (render, name);
	lisys_free (name);

	/* Find forward shader. */
	name = listr_concat ("forward-", material->shader);
	if (name == NULL)
	{
		liren_material_free (self);
		return 0;
	}
	self->shader_forward = liren_render_find_shader (render, name);
	lisys_free (name);

	/* Set textures. */
	if (!liren_material_set_texture_count (self, material->textures.count))
	{
		liren_material_free (self);
		return 0;
	}
	for (j = 0 ; j < material->textures.count ; j++)
	{
		texture = material->textures.array + j;

		/* Find or load. */
		if (texture->type == LIMDL_TEXTURE_TYPE_IMAGE)
		{
			image = liren_render_find_image (render, texture->string);
			if (image == NULL)
			{
				liren_render_load_image (render, texture->string);
				image = liren_render_find_image (render, texture->string);
			}
		}
		else
			image = render->helpers.empty_image;

		/* Set texture. */
		liren_material_set_texture (self, j, texture, image);
	}

	return self;
}

/**
 * \brief Frees a material.
 *
 * \param self Material.
 */
void
liren_material_free (LIRenMaterial* self)
{
	lisys_free (self->textures.array);
	lisys_free (self);
}

/**
 * \brief Sets the rendering flags of the material.
 *
 * \param self Material.
 * \param flags Flags.
 */
void
liren_material_set_flags (LIRenMaterial* self,
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
 * \param deferred Shader for deferred rendering.
 * \param forward Shader for forward rendering.
 */
int
liren_material_set_shader (LIRenMaterial* self,
                           LIRenShader*   deferred,
                           LIRenShader*   forward)
{
	self->shader_deferred = deferred;
	self->shader_forward = forward;

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
liren_material_set_texture (LIRenMaterial* self,
                            int            index,
                            LIMdlTexture*  texture,
                            LIRenImage*    image)
{
	if (index < 0 || index >= self->textures.count)
		return;
	liren_texture_init (self->textures.array + index, texture);
	liren_texture_set_image (self->textures.array + index, image);
}

int
liren_material_set_texture_count (LIRenMaterial* self,
                                  int            value)
{
	int num;
	LIRenTexture* tmp;

	if (value)
	{
		if (value > self->textures.count)
		{
			num = value - self->textures.count;
			tmp = realloc (self->textures.array, value * sizeof (LIRenTexture));
			if (tmp == NULL)
				return 0;
			self->textures.array = tmp;
			self->textures.count = value;
			memset (tmp + value - num, 0, num * sizeof (LIRenTexture));
		}
		else
		{
			tmp = realloc (self->textures.array, value * sizeof (LIRenTexture));
			if (tmp != NULL)
				self->textures.array = tmp;
			self->textures.count = value;
		}
	}
	else
	{
		lisys_free (self->textures.array);
		self->textures.array = NULL;
		self->textures.count = 0;
	}

	return 1;
}

/** @} */
/** @} */
