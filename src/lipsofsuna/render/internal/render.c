/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenRender Render
 * @{
 */

#include "render-internal.h"
#include "../render21/render-private.h"
#include "../render32/render-private.h"

#define LIREN_RENDER_TEXTURE_UNLOAD_TIME 10

static int private_init_glew (
	LIRenRender* self);

static void private_render_overlay (
	LIRenRender*  self,
	LIRenOverlay* overlay);

/*****************************************************************************/

/**
 * \brief Finds a texture by name.
 *
 * Searches for a texture from the texture cache and returns the match, if any.
 * If no match is found, NULL is returned.
 *
 * \param self Renderer.
 * \param name Name of the texture.
 * \return Texture or NULL.
 */
LIRenImage* liren_internal_find_image (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->images, name);
}

/**
 * \brief Finds a model by ID.
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel* liren_internal_find_model (
	LIRenRender* self,
	int          id)
{
	return lialg_u32dic_find (self->models, id);
}

/**
 * \brief Finds a shader by name.
 * \param self Renderer.
 * \param name Name of the shader.
 * \return Shader or NULL.
 */
LIRenShader* liren_internal_find_shader (
	LIRenRender* self,
	const char*  name)
{
	return lialg_strdic_find (self->shaders, name);
}

void liren_internal_handle_message (
	LIRenRender*  self,
	LIRenMessage* message)
{
	int i;
	LIRenObject* object;

	switch (message->type)
	{
		/* Render */
		case LIREN_MESSAGE_RENDER_LOAD_FONT:
			liren_internal_load_font (self,
				message->render_load_font.name,
				message->render_load_font.file,
				message->render_load_font.size);
			lisys_free (message->render_load_font.name);
			lisys_free (message->render_load_font.file);
			break;
		case LIREN_MESSAGE_RENDER_LOAD_IMAGE:
			liren_internal_load_image (self,
				message->render_load_image.name);
			lisys_free (message->render_load_image.name);
			break;
		case LIREN_MESSAGE_RENDER_MEASURE_TEXT:
			liren_internal_measure_text (self,
				message->render_measure_text.font,
				message->render_measure_text.text,
				message->render_measure_text.width_limit,
				message->render_measure_text.result_width,
				message->render_measure_text.result_height);
			lisys_free (message->render_measure_text.font);
			lisys_free (message->render_measure_text.text);
			break;
		case LIREN_MESSAGE_RENDER_SCREENSHOT:
			//*message->render_screenshot.result = liren_internal_screenshot (self);
			break;
		case LIREN_MESSAGE_RENDER_UPDATE:
			liren_internal_update (self,
				message->render_update.secs);
			break;
		case LIREN_MESSAGE_RENDER_GET_ANISOTROPY:
			*message->render_get_anisotropy.result = liren_internal_get_anisotropy (self);
			break;
		case LIREN_MESSAGE_RENDER_SET_ANISOTROPY:
			liren_internal_set_anisotropy (self,
				message->render_set_anisotropy.value);
			break;
		case LIREN_MESSAGE_RENDER_GET_IMAGE_SIZE:
			liren_internal_get_image_size (self,
				message->render_get_image_size.name,
				message->render_get_image_size.result);
			lisys_free (message->render_get_image_size.name);
			break;
		case LIREN_MESSAGE_RENDER_SET_VIDEOMODE:
			liren_internal_set_videomode (self,
				message->render_set_videomode.mode);
			break;

		/* Light */
		/* TODO */

		/* Model */
		case LIREN_MESSAGE_MODEL_NEW:
			break;
		case LIREN_MESSAGE_MODEL_FREE:
			break;
		case LIREN_MESSAGE_MODEL_SET_MODEL:
			break;

		/* Object */
		case LIREN_MESSAGE_OBJECT_NEW:
			break;
		case LIREN_MESSAGE_OBJECT_FREE:
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_ANIMATE:
			object = lialg_u32dic_find (self->objects, message->object_channel_animate.id);
			if (object != NULL)
			{
				liren_object_channel_animate (object,
					message->object_channel_animate.channel,
					message->object_channel_animate.name,
					message->object_channel_animate.additive,
					message->object_channel_animate.repeat,
					message->object_channel_animate.repeat_start,
					message->object_channel_animate.keep,
					message->object_channel_animate.fade_in,
					message->object_channel_animate.fade_out,
					message->object_channel_animate.weight,
					message->object_channel_animate.weight_scale,
					message->object_channel_animate.time,
					message->object_channel_animate.time_scale,
					(const char**) message->object_channel_animate.node_names,
					message->object_channel_animate.node_weights,
					message->object_channel_animate.node_count);
			}
			lisys_free (message->object_channel_animate.name);
			if (message->object_channel_animate.node_names != NULL)
			{
				for (i = 0 ; i < message->object_channel_animate.node_count ; i++)
					lisys_free (message->object_channel_animate.node_names[i]);
			}
			lisys_free (message->object_channel_animate.node_names);
			lisys_free (message->object_channel_animate.node_weights);
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_EDIT:
			object = lialg_u32dic_find (self->objects, message->object_channel_edit.id);
			if (object != NULL)
			{
				liren_object_channel_edit (object,
					message->object_channel_edit.channel,
					message->object_channel_edit.frame,
					message->object_channel_edit.node,
					&message->object_channel_edit.transform,
					message->object_channel_edit.scale);
			}
			lisys_free (message->object_channel_edit.node);
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_FADE:
			object = lialg_u32dic_find (self->objects, message->object_channel_fade.id);
			if (object != NULL)
			{
				liren_object_channel_fade (object,
					message->object_channel_fade.channel,
					message->object_channel_fade.time);
			}
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_GET_STATE:
			object = lialg_u32dic_find (self->objects, message->object_channel_get_state.id);
			if (object != NULL)
			{
				*message->object_channel_get_state.result = liren_object_channel_get_state (object,
					message->object_channel_get_state.channel);
			}
			break;
		case LIREN_MESSAGE_OBJECT_FIND_NODE:
			object = lialg_u32dic_find (self->objects, message->object_find_node.id);
			if (object != NULL)
			{
				*message->object_find_node.result = liren_object_find_node (object,
					message->object_find_node.name,
					message->object_find_node.world,
					message->object_find_node.result_transform);
			}
			lisys_free (message->object_find_node.name);
			break;
		case LIREN_MESSAGE_OBJECT_PARTICLE_ANIMATION:
			break;
		case LIREN_MESSAGE_OBJECT_SET_EFFECT:
			break;
		case LIREN_MESSAGE_OBJECT_SET_MODEL:
			break;
		case LIREN_MESSAGE_OBJECT_SET_REALIZED:
			break;
		case LIREN_MESSAGE_OBJECT_SET_TRANSFORM:
			break;
	}

	/* Tell the caller that the return value has been written. */
	if (message->mutex_caller)
		lisys_mutex_unlock (message->mutex_caller);
}

int liren_internal_load_font (
	LIRenRender* self,
	const char*  name,
	const char*  file,
	int          size)
{
	char* path;
	char* file_;
	LIFntFont* font;

	/* Check for existing. */
	font = lialg_strdic_find (self->fonts, name);
	if (font != NULL)
		return 0;

	/* Load the font. */
	file_ = lisys_string_concat (file, ".ttf");
	if (file == NULL)
		return 0;
	path = lipth_paths_get_font (self->paths, file_);
	lisys_free (file_);
	if (path == NULL)
		return 0;
	font = lifnt_font_new (path, size);
	lisys_free (path);
	if (font == NULL)
		return 0;

	/* Add to the dictionary. */
	if (!lialg_strdic_insert (self->fonts, name, font))
	{
		lifnt_font_free (font);
		return 0;
	}

	return 1;
}

/**
 * \brief Forces the renderer to load or reload a texture image.
 *
 * Reloads the requested texture and updates any materials that reference it
 * to point to the new texture. Any other references to the texture become
 * invalid and need to be manually replaced.
 *
 * \param self Renderer.
 * \param name Texture name.
 * \return Nonzero on success.
 */
int liren_internal_load_image (
	LIRenRender* self,
	const char*  name)
{
	LIRenImage* image;

	if (self->v32 != NULL)
	{
		image = liren_internal_find_image (self, name);
		if (image != NULL)
			return liren_render32_reload_image (self->v32, image->v32);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}
	else
	{
		image = liren_internal_find_image (self, name);
		if (image != NULL)
			return liren_render21_reload_image (self->v21, image->v21);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}

	return 1;
}

int liren_internal_measure_text (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int*         result_width,
	int*         result_height)
{
	LIFntFont* font_;
	LIFntLayout* layout;

	font_ = lialg_strdic_find (self->fonts, font);
	if (font_ == NULL)
		return 0;

	layout = lifnt_layout_new ();
	if (layout == NULL)
		return 0;

	if (width_limit != -1)
		lifnt_layout_set_width_limit (layout, width_limit);
	lifnt_layout_append_string (layout, font_, text);
	*result_width = lifnt_layout_get_width (layout);
	*result_height = lifnt_layout_get_height (layout);
	lifnt_layout_free (layout);

	return 1;
}

/**
 * \brief Reloads all images, shaders and other graphics state.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads all data that was lost when the context was erased.
 *
 * \param self Renderer.
 * \param pass Reload pass.
 */
void liren_internal_reload (
	LIRenRender* self,
	int          pass)
{
	LIAlgU32dicIter iter;
	LIAlgStrdicIter iter1;
	LIRenOverlay* overlay;

	/* Reload fonts. */
	LIALG_STRDIC_FOREACH (iter1, self->fonts)
		lifnt_font_reload (iter1.value, pass);

	/* Reload overlays. */
	LIALG_U32DIC_FOREACH (iter, self->overlays)
	{
		overlay = iter.value;
		if (overlay->buffer != NULL)
		{
			liren_buffer_free (overlay->buffer);
			overlay->buffer = NULL;
		}
	}

	/* Reload others. */
	if (self->v32 != NULL)
		liren_render32_reload (self->v32, pass);
	else
		liren_render21_reload (self->v21, pass);
}

/**
 * \brief Renders the overlays.
 * \param self Renderer.
 */
void liren_internal_render (
	LIRenRender* self)
{
	glViewport (0, 0, self->mode.width, self->mode.height);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	if (self->root_overlay != NULL)
		private_render_overlay (self, self->root_overlay);
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	SDL_GL_SwapBuffers ();
}

/**
 * \brief Renders the scene.
 * \param self Renderer.
 * \param framebuffer Render target framebuffer.
 * \param viewport Viewport array.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projeciton matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_internal_render_scene (
	LIRenRender*       self,
	LIRenFramebuffer*  framebuffer,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	if (self->v32 != NULL)
	{
		return liren_render32_render (self->v32, framebuffer->v32, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
	else
	{
		return liren_render21_render (self->v21, framebuffer->v21, viewport,
			modelview, projection, frustum, render_passes, render_passes_num,
			postproc_passes, postproc_passes_num);
	}
}

int liren_internal_screenshot (
	LIRenRender* self,
	const char*  path)
{
	int i;
	int width;
	int height;
	int pitch;
	uint32_t rmask;
	uint32_t gmask;
	uint32_t bmask;
	uint32_t amask;
	uint8_t* pixels;
	SDL_Surface* surface;

	/* Get window size. */
	width = self->mode.width;
	height = self->mode.height;
	pitch = 4 * width;

	/* Capture pixel data. */
	/* The one extra row we allocate is used for flipping. */
	pixels = calloc ((height + 1) * pitch, sizeof (uint8_t));
	if (pixels == NULL)
		return 0;
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	glReadPixels (0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	/* Flip the image vertically. */
	/* We use the extra row as temporary storage. */
	for (i = 0 ; i < height / 2 ; i++)
	{
		memcpy (pixels + pitch * height, pixels + pitch * i, pitch);
		memcpy (pixels + pitch * i, pixels + pitch * (height - i - 1), pitch);
		memcpy (pixels + pitch * (height - i - 1), pixels + pitch * height, pitch);
	}

	/* Create a temporary SDL surface. */
	if (lisys_endian_big ())
	{
		rmask = 0xFF000000;
		gmask = 0x00FF0000;
		bmask = 0x0000FF00;
		amask = 0x000000FF;
	}
	else
	{
		rmask = 0x000000FF;
		gmask = 0x0000FF00;
		bmask = 0x00FF0000;
		amask = 0xFF000000;
	}
	surface = SDL_CreateRGBSurfaceFrom(pixels, width, height,
		32, pitch, rmask, gmask, bmask, amask);
	if (surface == NULL)
	{
		lisys_free (pixels);
		return 9;
	}

	/* Save the surface to a file. */
	pixels = surface->pixels;
	SDL_SaveBMP (surface, path);
	SDL_FreeSurface (surface);
	lisys_free (pixels);

	return 1;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_internal_update (
	LIRenRender* self,
	float        secs)
{
	Uint32 now;
	LIAlgStrdicIter iter;
	LIAlgU32dicIter iter1;
	LIRenImage* image;
	LIRenObject* object;

	/* Animate objects. */
	LIALG_U32DIC_FOREACH (iter1, self->objects)
	{
		object = iter1.value;
		if (object->pose != NULL)
			limdl_pose_update (object->pose, secs);
	}

	/* Update the backend. */
	if (self->v32 != NULL)
		liren_render32_update (self->v32, secs);
	else
		liren_render21_update (self->v21, secs);

	/* Free unused images. */
	now = SDL_GetTicks ();
	LIALG_STRDIC_FOREACH (iter, self->images)
	{
		image = iter.value;
		if (!image->refs && image->timestamp < now + 1000 * LIREN_RENDER_TEXTURE_UNLOAD_TIME)
			liren_image_free (image);
	}
}

int liren_internal_get_anisotropy (
	const LIRenRender* self)
{
	if (self->v32 != NULL)
		return liren_render32_get_anisotropy (self->v32);
	else
		return liren_render21_get_anisotropy (self->v21);
}

void liren_internal_set_anisotropy (
	LIRenRender* self,
	int          value)
{
	if (self->v32 != NULL)
		liren_render32_set_anisotropy (self->v32, value);
	else
		liren_render21_set_anisotropy (self->v21, value);
}

/**
 * \brief Gets the size of an image.
 * \param self Renderer.
 * \param name Image name.
 * \param result Return location for two integers.
 * \return Nonzero on success.
 */
int liren_internal_get_image_size (
	LIRenRender* self,
	const char*  name,
	int*         result)
{
	LIRenImage* image;

	/* Load the image. */
	image = lialg_strdic_find (self->images, name);
	if (image == NULL)
	{
		liren_internal_load_image (self, name);
		image = lialg_strdic_find (self->images, name);
		if (image == NULL)
			return 0;
	}

	/* Return the size. */
	result[0] = liren_image_get_width (image);
	result[1] = liren_image_get_height (image);

	return 1;
}

int liren_internal_set_videomode (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	int i;
	Uint32 flags;
	SDL_Rect* best = NULL;
	SDL_Rect** modes;

	/* Initialize SDL. */
	if (!SDL_WasInit (SDL_INIT_VIDEO))
	{
		if (SDL_Init (SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == -1)
		{
			lisys_error_set (ENOTSUP, "initializing SDL failed");
			lisys_free (self);
			return 0;
		}
	}

	/* Determine screen surface flags. */
	if (mode->fullscreen)
	{
		flags = SDL_OPENGL | SDL_FULLSCREEN;
		modes = SDL_ListModes (NULL, flags);
		if (modes != NULL && modes != (SDL_Rect**) -1)
		{
			/* Determine the best possible fullscreen mode. */
			for (i = 0 ; modes[i] ; i++)
			{
				if (best == NULL ||
				   (LIMAT_ABS (modes[i]->w - mode->width) < LIMAT_ABS (best->w - mode->width) &&
				    LIMAT_ABS (modes[i]->h - mode->height) < LIMAT_ABS (best->h - mode->height)))
					best = modes[i];
			}
		}
		if (best != NULL)
		{
			/* Set the resolution to the best mode found. */
			mode->width = best->w;
			mode->height = best->h;
		}
		else
		{
			/* Revert to windowed mode if no fullscreen modes. */
			flags = SDL_OPENGL | SDL_RESIZABLE;
			mode->fullscreen = 0;
		}
	}
	else
		flags = SDL_OPENGL | SDL_RESIZABLE;

	/* Unload all graphics. */
	/* Since changing the video mode erases the OpenGL context in Windows,
	   we have to unload all textures, shaders, vertex buffers, etc. */
#ifdef WIN32
	liren_render_reload (self, 0);
#endif

	/* Recreate surface. */
	/* This destroys all graphics data in Windows. */
	SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute (SDL_GL_STENCIL_SIZE, 0);
	if (mode->sync)
		SDL_GL_SetAttribute (SDL_GL_SWAP_CONTROL, 1);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	self->screen = SDL_SetVideoMode (mode->width, mode->height, 0, flags);
	if (self->screen == NULL)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot set video mode");
		return 0;
	}

	/* Initialize libraries. */
	if (!private_init_glew (self))
		return 0;

	/* Store mode. */
	self->mode = *mode;

	/* Reload all graphics. */
	/* Since changing the video mode erases the OpenGL context in Windows,
	   we have to reload all textures, shaders, vertex buffers, etc. */
#ifdef WIN32
	liren_render_reload (self, 1);
#endif

	return 1;
}

int liren_internal_get_videomodes (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num)
{
	int i;
	SDL_Rect** m;

	/* Get the list of modes. */
	m = SDL_ListModes (NULL, SDL_OPENGL | SDL_FULLSCREEN);
	if (m == NULL && m == (SDL_Rect**) -1)
		return 0;

	/* Count modes. */
	for (i = 0 ; m[i] != NULL ; i++)
		{}
	*modes = lisys_calloc (i, sizeof (LIRenVideomode));
	if (*modes == NULL)
		return 0;

	/* Convert modes. */
	*modes_num = i;
	for (i = 0 ; m[i] != NULL ; i++)
	{
		(*modes)[i].width = m[i]->w;
		(*modes)[i].height = m[i]->h;
		(*modes)[i].fullscreen = 1;
		(*modes)[i].sync = self->mode.sync;
	}

	return 1;
}

/*****************************************************************************/

/**
 * \brief Initializes the global video card information.
 * \return Nonzero on success.
 */
static int private_init_glew (
	LIRenRender* self)
{
	int ver[3];
	GLenum error;
	const GLubyte* tmp;

	/* Initialize GLEW. */
	error = glewInit ();
	if (error != GLEW_OK)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "%s", glewGetErrorString (error));
		return 0;
	}

	/* Check for OpenGL 3.2 capabilities. */
	/* GLEW versions up to 1.5.3 had a bug that completely broke OpenGL 3.2
	   support. We try to detect it and warn the user of the problem. */
	if (GLEW_VERSION_3_2)
		return 1;
	tmp = glewGetString (GLEW_VERSION);
	if (sscanf ((const char*) tmp, "%d.%d.%d", ver, ver + 1, ver + 2) == 3 &&
	   (ver[0] < 1 || ver[1] < 5 || ver[2] <= 3))
	{
		lisys_error_set (EINVAL, "OpenGL 3.2 isn't supported because it requires GLEW 1.5.4 or newer while you have %s", tmp);
		lisys_error_report ();
	}
	else
	{
		lisys_error_set (EINVAL, "OpenGL 3.2 isn't supported by your graphics card or drivers");
		lisys_error_report ();
	}

	/* Check for OpenGL 2.1 capabilities. */
	if (GLEW_VERSION_2_1)
		return 1;
	lisys_error_set (EINVAL, "OpenGL 2.1 isn't supported by your graphics card or drivers");

	return 0;
}

static void private_render_overlay (
	LIRenRender*  self,
	LIRenOverlay* overlay)
{
	int i;
	int width;
	int height;
	GLuint texture;
	GLint scissor[4];
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenOverlayElement* element;
	static const LIRenFormat overlay_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };

	if (!overlay->visible)
		return;
	width = self->mode.width;
	height = self->mode.height;
	projection = limat_matrix_ortho (0, width, height, 0, -1.0f, 1.0f);

	/* Render the scene. */
	if (overlay->scene.enabled)
	{
		/* Update the framebuffer. */
		if (overlay->scene.framebuffer == NULL)
		{
			overlay->scene.framebuffer = liren_framebuffer_new (self,
				overlay->scene.viewport[2], overlay->scene.viewport[3],
				overlay->scene.samples, overlay->scene.hdr);
		}
		else
		{
			liren_framebuffer_resize (overlay->scene.framebuffer,
				overlay->scene.viewport[2], overlay->scene.viewport[3],
				overlay->scene.samples, overlay->scene.hdr);
		}

		/* Render the scene. */
		if (overlay->scene.framebuffer != NULL)
		{
			if (self->v32 != NULL)
			{
				liren_render32_render (self->v32, overlay->scene.framebuffer->v32, overlay->scene.viewport,
					&overlay->scene.modelview, &overlay->scene.projection, &overlay->scene.frustum,
					overlay->scene.render_passes, overlay->scene.render_passes_num,
					overlay->scene.postproc_passes, overlay->scene.postproc_passes_num);
			}
			else
			{
				liren_render21_render (self->v21, overlay->scene.framebuffer->v21, overlay->scene.viewport,
					&overlay->scene.modelview, &overlay->scene.projection, &overlay->scene.frustum,
					overlay->scene.render_passes, overlay->scene.render_passes_num,
					overlay->scene.postproc_passes, overlay->scene.postproc_passes_num);
			}
		}
	}

	/* Render elements. */
	if (overlay->elements.count)
	{
		/* Update the buffer. */
		if (overlay->buffer == NULL)
		{
			overlay->buffer = liren_buffer_new (self, NULL, 0, &overlay_format,
				overlay->vertices.array, overlay->vertices.count, LIREN_BUFFER_TYPE_STATIC);
		}

		/* Render each element. */
		if (overlay->buffer != NULL)
		{
			modelview = limat_matrix_translation (overlay->position.x, overlay->position.y, 0.0f);
			for (i = 0 ; i < overlay->elements.count ; i++)
			{
				/* Choose the clip rectangle. */
				element = overlay->elements.array + i;
				if (element->scissor_enabled)
				{
					scissor[0] = (int) overlay->position.x + element->scissor_rect[0];
					scissor[1] = height - (int) overlay->position.y - element->scissor_rect[1] - element->scissor_rect[3];
					scissor[2] = element->scissor_rect[2];
					scissor[3] = element->scissor_rect[3];
				}
				else
				{
					scissor[0] = 0;
					scissor[1] = 0;
					scissor[2] = width;
					scissor[3] = height;
				}

				/* Choose the texture. */
				if (element->image != NULL)
					texture = liren_image_get_handle (element->image);
				else
					texture = element->font->texture;

				/* Draw the buffer range. */
				if (self->v32 != NULL)
				{
					liren_render32_draw_clipped_buffer (self->v32, element->shader->v32,
						&modelview, &projection, texture, element->color, scissor,
						element->buffer_start, element->buffer_count, overlay->buffer->v32);
				}
				else
				{
					liren_render21_draw_clipped_buffer (self->v21, element->shader->v21,
						&modelview, &projection, texture, element->color, scissor,
						element->buffer_start, element->buffer_count, overlay->buffer->v21);
				}
			}
		}
	}

	/* Render child overlays. */
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i]);
	}
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (!overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i]);
	}
}

/** @} */
/** @} */
/** @} */
