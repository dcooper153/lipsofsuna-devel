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
 * \addtogroup LIRenTexture Texture
 * @{
 */

#include "render-texture.h"

void liren_texture_init (
	LIRenTexture* self)
{
	self->flags = 0;
	self->type = 0;
	self->width = 0;
	self->height = 0;
	glGenSamplers (1, &self->sampler);
	liren_texture_set_flags (self, 0);
}

void liren_texture_free (
	LIRenTexture* self)
{
	if (self->sampler)
		glDeleteSamplers (1, &self->sampler);
}

void liren_texture_set_flags (
	LIRenTexture* self,
	int           value)
{
	GLenum magfilter;
	GLenum minfilter;
	GLenum wraps;
	GLenum wrapt;

	/* Get filters. */
	if (value & LIMDL_TEXTURE_FLAG_MIPMAP)
	{
		if (value & LIMDL_TEXTURE_FLAG_BILINEAR)
		{
			magfilter = GL_LINEAR;
			minfilter = GL_LINEAR_MIPMAP_LINEAR;
		}
		else
		{
			magfilter = GL_NEAREST;
			minfilter = GL_NEAREST_MIPMAP_NEAREST;
		}
	}
	else
	{
		if (self->flags & LIMDL_TEXTURE_FLAG_BILINEAR)
		{
			magfilter = GL_LINEAR;
			minfilter = GL_NEAREST;
		}
		else
		{
			magfilter = GL_NEAREST;
			minfilter = GL_NEAREST;
		}
	}

	/* Get wrap flags. */
	if (value & LIMDL_TEXTURE_FLAG_CLAMP)
	{
		wraps = GL_CLAMP_TO_EDGE;
		wrapt = GL_CLAMP_TO_EDGE;
	}
	else
	{
		wraps = GL_REPEAT;
		wrapt = GL_REPEAT;
	}

	/* Update the sampler parameters. */
	glSamplerParameteri (self->sampler, GL_TEXTURE_MIN_FILTER, minfilter);
	glSamplerParameteri (self->sampler, GL_TEXTURE_MAG_FILTER, magfilter);
	glSamplerParameteri (self->sampler, GL_TEXTURE_WRAP_S, wraps);
	glSamplerParameteri (self->sampler, GL_TEXTURE_WRAP_T, wrapt);

	/* Store the parameters. */
	self->params.minfilter = minfilter;
	self->params.magfilter = magfilter;
	self->params.wraps = wraps;
	self->params.wrapt = wrapt;
}

void liren_texture_set_image (
	LIRenTexture* self,
	LIRenImage*   value)
{
	self->image = value;
	if (value != NULL && value->texture != NULL)
		self->texture = value->texture->texture;
	else
		self->texture = 0;
}

/** @} */
/** @} */
