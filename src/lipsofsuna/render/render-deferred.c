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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenDeferred Deferred
 * @{
 */

#include <lipsofsuna/system.h>
#include "render-deferred.h"
#include "render-error.h"

static int private_check (
	LIRenDeferred* self);

static int private_rebuild (
	LIRenDeferred* self,
	int            width,
	int            height,
	int            samples);

/*****************************************************************************/

/**
 * \brief Creates a deferred rendering framebuffer.
 * \param render Renderer.
 * \param width Framebuffer width.
 * \param height Framebuffer height.
 * \param samples Number of multisamples.
 * \return New deferred framebuffer or NULL.
 */
LIRenDeferred* liren_deferred_new (
	LIRenRender* render,
	int          width,
	int          height,
	int          samples)
{
	LIRenDeferred* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenDeferred));
	if (self == NULL)
		return NULL;
	self->render = render;
	self->width = width;
	self->height = height;

	/* Create frame buffer object. */
	for ( ; samples > 0 ; samples--)
	{
		if (!liren_deferred_resize (self, width, height, samples))
			lisys_error_report ();
		else
			break;
	}
	if (samples == 0)
	{
		liren_deferred_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the deferred framebuffer.
 * \param self Deferred framebuffer.
 */
void liren_deferred_free (
	LIRenDeferred* self)
{
	glDeleteFramebuffers (2, self->postproc_fbo);
	glDeleteTextures (1, &self->depth_texture);
	glDeleteTextures (2, self->postproc_texture);
	lisys_free (self);
}

/**
 * \brief Resizes the deferred framebuffer.
 * \param self Deferred framebuffer.
 * \param width New width.
 * \param height New height.
 * \param samples Number of multisamples.
 * \return Nonzero on success.
 */
int liren_deferred_resize (
	LIRenDeferred* self,
	int            width,
	int            height,
	int            samples)
{
	int max;
	int request;
	GLint max_color;
	GLint max_depth;

	/* Make sure that the sample count is valid. */
	/* The sample count must not be greater than the maximum sample count.
	   It must also be a power of two and greater than zero. */
	glGetIntegerv (GL_MAX_COLOR_TEXTURE_SAMPLES, &max_color);
	glGetIntegerv (GL_MAX_DEPTH_TEXTURE_SAMPLES, &max_depth);
	request = samples;
	max = LIMAT_MIN (max_color, max_depth);
	for (samples = 1 ; samples < max && samples < request ; samples <<= 1) {}
	samples = LIMAT_MIN (samples, max);

	/* Check if a resize is actually needed. */
	if (self->width == width && self->height == height && self->samples == samples)
		return 1;

	/* Recreate the framebuffer objects. */
	if (private_rebuild (self, width, height, samples))
	{
		self->width = width;
		self->height = height;
		self->samples = samples;
		return 1;
	}

	return 0;
}

/**
 * \brief Reads a pixel value from one of the textures.
 *
 * This function is horribly slow and should only be used for debugging purposes.
 * Doesn't work with multisample buffers currently.
 *
 * \param self Deferred framebuffer.
 * \param x Framebuffer position.
 * \param y Framebuffer position.
 * \param texture Texture index from 0 to 3.
 * \param result Return location for 4 floats.
 * \return Nonzero on success.
 */
void liren_deferred_read_pixel (
	LIRenDeferred* self,
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

static int private_check (
	LIRenDeferred* self)
{
	int ret;

	ret = glCheckFramebufferStatus (GL_FRAMEBUFFER);
	if (ret != GL_FRAMEBUFFER_COMPLETE)
	{
		switch (ret)
		{
			case GL_FRAMEBUFFER_UNSUPPORTED:
				lisys_error_set (ENOTSUP, "OpenGL: framebuffer object unsupported");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer draw buffer");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer dimensions");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer formats");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				lisys_error_set (ENOTSUP, "OpenGL: missing framebuffer attachment");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer multisample");
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				lisys_error_set (ENOTSUP, "OpenGL: incomplete framebuffer read buffer");
				break;
			default:
				lisys_error_set (ENOTSUP, "OpenGL: unknown framebuffer creation error");
				break;
		}
		return 0;
	}

	return 1;
}

static int private_rebuild (
	LIRenDeferred* self,
	int            width,
	int            height,
	int            samples)
{
	int i;
	GLenum fmt1;
	GLenum fmt2;
	GLenum error;
	GLuint postproc_fbo[2];
	GLuint depth_texture;
	GLuint postproc_texture[2];
	static const GLenum fragdata[] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	/* Choose pixel formats. */
	if (GLEW_ARB_texture_float)
	{
		fmt1 = GL_RGBA;
		fmt2 = GL_RGBA32F;
	}
	else
	{
		fmt1 = GL_RGBA;
		fmt2 = GL_RGBA;
	}
	error = glGetError ();

	/* Create multisample depth texture. */
	glGenTextures (1, &depth_texture);
	glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, depth_texture);
	glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, samples,
		GL_DEPTH_COMPONENT24, width, height, GL_FALSE);

	/* Create multisample postprocessing textures. */
	glGenTextures (2, postproc_texture);
	for (i = 0 ; i < 2 ; i++)
	{
		glBindTexture (GL_TEXTURE_2D_MULTISAMPLE, postproc_texture[i]);
		glTexImage2DMultisample (GL_TEXTURE_2D_MULTISAMPLE, samples, fmt2, width, height, GL_FALSE);
	}

	/* Create multisample post-processing framebuffer objects. */
	glGenFramebuffers (2, postproc_fbo);
	for (i = 0 ; i < 2 ; i++)
	{
		glBindFramebuffer (GL_FRAMEBUFFER, postproc_fbo[i]);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_2D_MULTISAMPLE, postproc_texture[i], 0);
		glFramebufferTexture2D (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D_MULTISAMPLE, depth_texture, 0);
		if (!private_check (self))
		{
			glBindFramebuffer (GL_FRAMEBUFFER, 0);
			glBindRenderbuffer (GL_RENDERBUFFER, 0);
			glDeleteFramebuffers (2, postproc_fbo);
			glDeleteTextures (1, &depth_texture);
			glDeleteTextures (2, postproc_texture);
			return 0;
		}
		glDrawBuffers (1, fragdata);
	}

	/* There's a danger of running out of memory here since the G-buffer
	   can consume a lot of video memory with high multisample settings.
	   If something goes wrong, it's better to revert to the old buffer
	   than to try to use the broken one just created. */
	if (liren_error_get ())
		return 0;

	/* Accept successful rebuild. */
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers (2, self->postproc_fbo);
	glDeleteTextures (1, &self->depth_texture);
	glDeleteTextures (2, self->postproc_texture);
	self->postproc_fbo[0] = postproc_fbo[0];
	self->postproc_fbo[1] = postproc_fbo[1];
	self->depth_texture = depth_texture;
	self->postproc_texture[0] = postproc_texture[0];
	self->postproc_texture[1] = postproc_texture[1];

	return 1;
}

/** @} */
/** @} */

