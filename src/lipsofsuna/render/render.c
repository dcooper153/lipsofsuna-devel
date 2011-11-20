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
	if (!liren_internal_set_videomode (self, mode))
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

	/* TODO: Wait for the handler thread to exit. */

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
	return liren_internal_load_font (self, name, file, size);
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
	return liren_internal_load_image (self, name);
}

int liren_render_measure_text (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int*         result_width,
	int*         result_height)
{
	return liren_internal_measure_text (self, font, text, width_limit, result_width, result_height);
}

/**
 * \brief Renders the overlays.
 * \param self Renderer.
 */
void liren_render_render (
	LIRenRender* self)
{
	liren_internal_render (self);
}

int liren_render_screenshot (
	LIRenRender* self,
	const char*  path)
{
	return liren_internal_screenshot (self, path);
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
	liren_internal_update (self, secs);
}

int liren_render_get_anisotropy (
	const LIRenRender* self)
{
	return liren_internal_get_anisotropy (self);
}

void liren_render_set_anisotropy (
	LIRenRender* self,
	int          value)
{
	liren_internal_set_anisotropy (self, value);
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
	return liren_internal_get_image_size (self, name, result);
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
	return liren_internal_set_videomode (self, mode);
}

int liren_render_get_videomodes (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num)
{
	return liren_internal_get_videomodes (self, modes, modes_num);
}

/** @} */
/** @} */
