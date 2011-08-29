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
 * \addtogroup LIRenRender Render
 * @{
 */

#include "lipsofsuna/system.h"
#include "render.h"
#include "render-overlay.h"
#include "internal/render-internal.h"
#include "render21/render-private.h"
#include "render32/render-private.h"

#define LIREN_RENDER_TEXTURE_UNLOAD_TIME 10

static int private_init_glew (
	LIRenRender* self);

static void private_render_overlay (
	LIRenRender*  self,
	LIRenOverlay* overlay,
	int           width,
	int           height);

static int private_resize_window (
	LIRenRender*    self,
	LIRenVideomode* mode);

/*****************************************************************************/

LIRenRender* liren_render_new (
	LIPthPaths*     paths,
	LIRenVideomode* mode)
{
	LIRenRender* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenRender));
	if (self == NULL)
		return NULL;
	self->paths = paths;
	lialg_random_init (&self->random, lisys_time (NULL));

	/* Initialize the image dictionary. */
	self->images = lialg_strdic_new ();
	if (self->images == NULL)
		return 0;

	/* Initialize the shader dictionary. */
	self->shaders = lialg_strdic_new ();
	if (self->shaders == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the light dictionary. */
	self->lights = lialg_u32dic_new ();
	if (self->lights == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the model dictionary. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the object dictionary. */
	self->objects = lialg_u32dic_new ();
	if (self->objects == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the overlay dictionary. */
	self->overlays = lialg_u32dic_new ();
	if (self->overlays == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Allocate the font dictionary. */
	self->fonts = lialg_strdic_new ();
	if (self->fonts == NULL)
	{
		liren_render_free (self);
		return NULL;
	}

	/* Initialize the videomode. */
	if (!private_resize_window (self, mode))
	{
		liren_render_free (self);
		return NULL;
	}
	if (TTF_Init () == -1)
	{
		lisys_error_set (LISYS_ERROR_UNKNOWN, "cannot initialize SDL_ttf");
		liren_render_free (self);
		return NULL;
	}
	SDL_ShowCursor (SDL_DISABLE);

	/* Initialize the backend. */
	if (GLEW_VERSION_3_2 && getenv ("LOS_OPENGL21") == NULL)
	{
		self->v32 = liren_render32_new (self, paths);
		if (self->v32 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}
	else
	{
		self->v21 = liren_render21_new (self, paths);
		if (self->v21 == NULL)
		{
			lisys_free (self);
			return NULL;
		}
	}

	return self;
}

void liren_render_free (
	LIRenRender* self)
{
	LIAlgStrdicIter iter1;
	LIAlgU32dicIter iter2;

	/* Free lights. */
	if (self->lights != NULL)
		lialg_u32dic_free (self->lights);

	/* Free objects. */
	if (self->objects != NULL)
		lialg_u32dic_free (self->objects);

	/* Free models. */
	if (self->models != NULL)
	{
		LIALG_U32DIC_FOREACH (iter2, self->models)
			liren_model_free (iter2.value);
		lialg_u32dic_free (self->models);
	}

	/* Free overlays. */
	if (self->overlays != NULL)
	{
		LIALG_U32DIC_FOREACH (iter2, self->overlays)
			liren_render_overlay_free (self, iter2.key);
		lialg_u32dic_free (self->overlays);
	}

	/* Free images. */
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->images)
			liren_image_free (iter1.value);
		lialg_strdic_free (self->images);
	}

	/* Free shaders. */
	if (self->shaders != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->shaders)
			liren_shader_free (iter1.value);
		lialg_strdic_free (self->shaders);
	}

	/* Free fonts. */
	if (self->fonts != NULL)
	{
		LIALG_STRDIC_FOREACH (iter1, self->fonts)
			lifnt_font_free (iter1.value);
		lialg_strdic_free (self->fonts);
	}

	/* Free the backend. */
	if (self->v21 != NULL)
		liren_render21_free (self->v21);
	if (self->v32 != NULL)
		liren_render32_free (self->v32);

	/* Uninitialize the videomode. */
	if (self->screen != NULL)
		SDL_FreeSurface (self->screen);
	if (TTF_WasInit ())
		TTF_Quit ();

	lisys_free (self);
}

int liren_render_load_font (
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
int liren_render_load_image (
	LIRenRender* self,
	const char*  name)
{
	LIRenImage* image;

	if (self->v32 != NULL)
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render32_reload_image (self->v32, image->v32);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}
	else
	{
		image = liren_render_find_image (self, name);
		if (image != NULL)
			return liren_render21_reload_image (self->v21, image->v21);
		image = liren_image_new (self, name);
		if (image == NULL)
			return 0;
	}

	return 1;
}

int liren_render_measure_text (
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
void liren_render_reload (
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
 * \param width Screen width.
 * \param height Screen height.
 */
void liren_render_render (
	LIRenRender* self,
	int          width,
	int          height)
{
	glViewport (0, 0, width, height);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);
	if (self->root_overlay != NULL)
		private_render_overlay (self, self->root_overlay, width, height);
	glBindFramebuffer (GL_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer (GL_READ_FRAMEBUFFER, 0);
	SDL_GL_SwapBuffers ();
}

SDL_Surface* liren_render_screenshot (
	LIRenRender* self)
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
		return NULL;
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
		return NULL;
	}

	return surface;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 */
void liren_render_update (
	LIRenRender* self,
	float        secs)
{
	Uint32 now;
	LIAlgStrdicIter iter;
	LIRenImage* image;

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

int liren_render_get_anisotropy (
	const LIRenRender* self)
{
	if (self->v32 != NULL)
		return liren_render32_get_anisotropy (self->v32);
	else
		return liren_render21_get_anisotropy (self->v21);
}

void liren_render_set_anisotropy (
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
int liren_render_get_image_size (
	LIRenRender* self,
	const char*  name,
	int*         result)
{
	LIRenImage* image;

	/* Load the image. */
	image = lialg_strdic_find (self->images, name);
	if (image == NULL)
	{
		liren_render_load_image (self, name);
		image = lialg_strdic_find (self->images, name);
		if (image == NULL)
			return 0;
	}

	/* Return the size. */
	result[0] = liren_image_get_width (image);
	result[1] = liren_image_get_height (image);

	return 1;
}

float liren_render_get_opengl_version (
	LIRenRender* self)
{
	if (self->v32 != NULL)
		return 3.2f;
	else
		return 2.1f;
}

int liren_render_set_videomode (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	return private_resize_window (self, mode);
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
	LIRenOverlay* overlay,
	int           width,
	int           height)
{
	int i;
	GLuint texture;
	GLint scissor[4];
	LIMatMatrix modelview;
	LIMatMatrix projection;
	LIRenOverlayElement* element;
	static const LIRenFormat overlay_format = { 32, GL_FLOAT, 24, GL_FLOAT, 12, GL_FLOAT, 0 };

	if (!overlay->visible)
		return;
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
				if (element->image != NULL)
					texture = liren_image_get_handle (element->image);
				else
					texture = element->font->texture;
				liren_render_draw_clipped_buffer (self, element->shader, &modelview,
					&projection, texture, element->color, scissor,
					element->buffer_start, element->buffer_count, overlay->buffer);
			}
		}
	}

	/* Render child overlays. */
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i], width, height);
	}
	for (i = overlay->overlays.count - 1 ; i >= 0 ; i--)
	{
		if (!overlay->overlays.array[i]->behind)
			private_render_overlay (self, overlay->overlays.array[i], width, height);
	}
}

static int private_resize_window (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	int i;
	Uint32 flags;
	SDL_Rect* best = NULL;
	SDL_Rect** modes;

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

/** @} */
/** @} */
