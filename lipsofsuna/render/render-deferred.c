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
 * \addtogroup lirndDeferred Deferred
 * @{
 */

#include <system/lips-system.h>
#include "render-deferred.h"

static int
private_rebuild (lirndDeferred* self,
                 int            width,
                 int            height);

/*****************************************************************************/

/**
 * \brief Creates a deferred rendering framebuffer.
 *
 * \param render Renderer.
 * \param width Framebuffer width.
 * \param height Framebuffer height.
 * \return New deferred framebuffer or NULL.
 */
lirndDeferred*
lirnd_deferred_new (lirndRender* render,
                    int          width,
                    int          height)
{
	lirndDeferred* self;

	/* Check for capabilities. */
	/* TODO: Check for NPOT */
	if (!GLEW_EXT_framebuffer_object)
	{
		lisys_error_set (ENOTSUP, "EXT_framebuffer_object not supported");
		return NULL;
	}
	if (!GLEW_ARB_depth_buffer_float)
	{
		lisys_error_set (ENOTSUP, "ARB_depth_buffer_float not supported");
		return NULL;
	}

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndDeferred));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->width = width;
	self->height = height;

	/* Create frame buffer object. */
	if (!private_rebuild (self, width, height))
	{
		lirnd_deferred_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the deferred framebuffer.
 *
 * \param self Deferred framebuffer.
 */
void
lirnd_deferred_free (lirndDeferred* self)
{
	glDeleteFramebuffersEXT (1, &self->fbo);
	glDeleteTextures (1, &self->depth_texture);
	glDeleteTextures (1, &self->normal_texture);
	glDeleteTextures (1, &self->diffuse_texture);
	glDeleteTextures (1, &self->specular_texture);
	lisys_free (self);
}

/**
 * \brief Resizes the deferred framebuffer.
 *
 * \param self Deferred framebuffer.
 * \param width New width.
 * \param height New height.
 * \return Nonzero on success.
 */
int
lirnd_deferred_resize (lirndDeferred* self,
                       int            width,
                       int            height)
{
	if (private_rebuild (self, width, height))
	{
		self->width = width;
		self->height = height;
		return 1;
	}

	return 0;
}

/**
 * \brief Reads a pixel value from one of the textures.
 *
 * This function is horribly slow and should only be used for debugging purposes.
 *
 * \param self Deferred framebuffer.
 * \param x Framebuffer position.
 * \param y Framebuffer position.
 * \param texture Texture index from 0 to 3.
 * \param result Return location for 4 floats.
 * \return Nonzero on success.
 */
void
lirnd_deferred_read_pixel (lirndDeferred* self,
                           int            x,
                           int            y,
                           int            texture,
                           float*         result)
{
	int off;
	GLfloat* mem;

	switch (texture)
	{
		case 0: glBindTexture (GL_TEXTURE_2D, self->depth_texture); break;
		case 1: glBindTexture (GL_TEXTURE_2D, self->diffuse_texture); break;
		case 2: glBindTexture (GL_TEXTURE_2D, self->specular_texture); break;
		case 3: glBindTexture (GL_TEXTURE_2D, self->normal_texture); break;
	}
	mem = calloc (4 * self->width * self->height, sizeof (GLfloat));
	if (mem != NULL)
	{
		glGetTexImage (GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, mem);
		off = 4 * x + 4 * y * self->width;
		result[0] = mem[off + 0];
		result[1] = mem[off + 1];
		result[2] = mem[off + 2];
		result[3] = mem[off + 3];
		free (mem);
	}
}

/*****************************************************************************/

static int
private_rebuild (lirndDeferred* self,
                 int            width,
                 int            height)
{
	int ret;
	GLuint fbo;
	GLuint depth_texture;
	GLuint normal_texture;
	GLuint diffuse_texture;
	GLuint specular_texture;
	static const GLenum fragdata[] =
	{
		GL_COLOR_ATTACHMENT0_EXT,
		GL_COLOR_ATTACHMENT1_EXT,
		GL_COLOR_ATTACHMENT2_EXT
	};

	/* Create depth texture. */
	glGenTextures (1, &depth_texture);
	glBindTexture (GL_TEXTURE_2D, depth_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height,
		0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create normal texture. */
	glGenTextures (1, &normal_texture);
	glBindTexture (GL_TEXTURE_2D, normal_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create diffuse texture. */
	glGenTextures (1, &diffuse_texture);
	glBindTexture (GL_TEXTURE_2D, diffuse_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create specular texture. */
	glGenTextures (1, &specular_texture);
	glBindTexture (GL_TEXTURE_2D, specular_texture);
	glTexImage2D (GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	/* Create framebuffer object. */
	glGenFramebuffersEXT (1, &fbo);
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, fbo);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT,
		GL_TEXTURE_2D, depth_texture, 0);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT,
		GL_TEXTURE_2D, diffuse_texture, 0);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT1_EXT,
		GL_TEXTURE_2D, specular_texture, 0);
	glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT2_EXT,
		GL_TEXTURE_2D, normal_texture, 0);
	ret = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
	if (ret != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		switch (ret)
		{
			case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
				lisys_error_set (ENOTSUP, "framebuffer object unsupported");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer draw buffer");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer dimensions");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer formats");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
				lisys_error_set (ENOTSUP, "missing framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer multisample");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
				lisys_error_set (ENOTSUP, "incomplete framebuffer read buffer");
				break;
			default:
				lisys_error_set (ENOTSUP, "unknown framebuffer creation error");
				break;
		}
		glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
		glBindRenderbufferEXT (GL_RENDERBUFFER_EXT, 0);
		glDeleteFramebuffersEXT (1, &fbo);
		glDeleteTextures (1, &depth_texture);
		glDeleteTextures (1, &normal_texture);
		glDeleteTextures (1, &diffuse_texture);
		glDeleteTextures (1, &specular_texture);
		return 0;
	}
	glDrawBuffers (sizeof (fragdata) / sizeof (GLenum), fragdata);

	/* Accept successful rebuild. */
	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glDeleteFramebuffersEXT (1, &self->fbo);
	glDeleteTextures (1, &self->depth_texture);
	glDeleteTextures (1, &self->normal_texture);
	glDeleteTextures (1, &self->diffuse_texture);
	glDeleteTextures (1, &self->specular_texture);
	self->fbo = fbo;
	self->depth_texture = depth_texture;
	self->normal_texture = normal_texture;
	self->diffuse_texture = diffuse_texture;
	self->specular_texture = specular_texture;

	return 1;
}

/** @} */
/** @} */

