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
 * \addtogroup LIRenOverlay Overlay
 * @{
 */

#include "render-internal.h"

static void private_free_element (
	LIRenOverlay*        self,
	LIRenOverlayElement* element);

static LIRenOverlayElement* private_add_vertices (
	LIRenOverlay*      self,
	const LIRenVertex* verts,
	int                count);

static void private_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* child);

/*****************************************************************************/

/**
 * \brief Creates a new overlay.
 * \param render Renderer.
 * \return Overlay or NULL on failure.
 */
LIRenOverlay* liren_overlay_new (
	LIRenRender* render)
{
	LIRenOverlay* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenOverlay));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Choose a unique ID. */
	while (!self->id)
	{
		self->id = lialg_random_range (&render->random, 0x00000000, 0x0FFFFFFF);
		if (lialg_u32dic_find (render->overlays, self->id))
			self->id = 0;
	}

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->overlays, self->id, self))
	{
		liren_overlay_free (self);
		return 0;
	}

	return self;
}

/**
 * \brief Frees the overlay.
 * \param self Overlay.
 */
void liren_overlay_free (
	LIRenOverlay* self)
{
	int i;
	LIRenOverlayElement* element;

	/* Detach children. */
	for (i = 0 ; i < self->overlays.count ; i++)
		self->overlays.array[i]->parent = NULL;

	/* Remove from the scene. */
	if (self->parent != NULL)
		private_remove_overlay (self->parent, self);
	if (self == self->render->root_overlay)
		self->render->root_overlay = NULL;
	lialg_u32dic_remove (self->render->overlays, self->id);

	/* Free element data. */
	for (i = 0 ; i < self->elements.count ; i++)
	{
		element = self->elements.array + i;
		private_free_element (self, element);
	}
	if (self->buffer != NULL)
		liren_buffer_free (self->buffer);

	/* Free vertices. */
	lisys_free (self->vertices.array);

	/* Free scene data. */
	if (self->scene.framebuffer != NULL)
		liren_framebuffer_free (self->scene.framebuffer);
	lisys_free (self->scene.render_passes);
	lisys_free (self->scene.postproc_passes);
	lisys_free (self);
}

/**
 * \brief Clears the elements of the overlay.
 * \param self Overlay.
 */
void liren_overlay_clear (
	LIRenOverlay* self)
{
	int i;
	LIRenOverlayElement* element;

	/* Free elements. */
	for (i = 0 ; i < self->elements.count ; i++)
	{
		element = self->elements.array + i;
		private_free_element (self, element);
	}
	lisys_free (self->elements.array);
	self->elements.array = NULL;
	self->elements.count = 0;

	/* Free vertices. */
	lisys_free (self->vertices.array);
	self->vertices.array = NULL;
	self->vertices.count = 0;

	/* Free the buffer. */
	if (self->buffer != NULL)
	{
		liren_buffer_free (self->buffer);
		self->buffer = NULL;
	}
}

/**
 * \brief Adds text to the overlay.
 * \param self Overlay.
 * \param shader Shader name.
 * \param font Font name.
 * \param color Diffuse color.
 * \param scissor Screen space scissor rectangle.
 * \param pos Text offset relative to the overlay origin.
 * \param size Clip rectangle size.
 * \param align Text alignment.
 */
void liren_overlay_add_text (
	LIRenOverlay* self,
	const char*   shader,
	const char*   font,
	const char*   text,
	const float*  color,
	const int*    scissor,
	const int*    pos,
	const int*    size,
	const float*  align)
{
	int i;
	int j;
	int x;
	int y;
	int w;
	int h;
	int count;
	float* vertex;
	float* vertex_data;
	LIMdlIndex* index_data;
	LIFntFont* font_;
	LIFntLayout* layout;
	LIFntLayoutGlyph* glyph;
	LIRenVertex* verts;
	LIRenOverlayElement* element;
	LIRenShader* shader_;

	if (!strlen (text))
		return;

	/* Find the shader. */
	shader_ = liren_internal_find_shader (self->render, shader);
	if (shader_ == NULL)
		return;

	/* Find the font. */
	font_ = lialg_strdic_find (self->render->fonts, font);
	if (font_ == NULL)
		return;

	/* Layout the text. */
	layout = lifnt_layout_new ();
	if (layout == NULL)
		return;
	lifnt_layout_set_width_limit (layout, size[0]);
	lifnt_layout_append_string (layout, font_, text);
	if (!lifnt_layout_get_vertices (layout, &index_data, &vertex_data))
	{
		lifnt_layout_free (layout);
		return;
	}

	/* Apply translation. */
	w = lifnt_layout_get_width (layout);
	h = lifnt_layout_get_height (layout);
	x = pos[0] + (int)(align[0] * (size[0] - w));
	y = pos[1] + (int)(align[1] * (size[1] - h));
	for (i = 2 ; i < layout->n_glyphs * 20 ; i += 5)
	{
		glyph = layout->glyphs + i;
		vertex_data[i + 0] += x;
		vertex_data[i + 1] += y;
	}

	/* Allocate room for renderer vertices. */
	count = layout->n_glyphs * 6;
	verts = lisys_calloc (count, sizeof (LIRenVertex));
	if (verts == NULL)
	{
		lifnt_layout_free (layout);
		return;
	}

	/* Pack glyphs to the vertex array. */
	for (i = 0 ; i < layout->n_glyphs ; i++)
	{
		glyph = layout->glyphs + i;
		for (j = 0 ; j < 6 ; j++)
		{
			vertex = vertex_data + 5 * index_data[6 * i + j];
			memcpy (verts[6 * i + j].coord, vertex + 2, 3 * sizeof (float));
			memset (verts[6 * i + j].normal, 0, 3 * sizeof (float));
			memcpy (verts[6 * i + j].texcoord, vertex + 0, 2 * sizeof (float));
		}
	}
	lifnt_layout_free (layout);
	lisys_free (index_data);
	lisys_free (vertex_data);

	/* Allocate the new element. */
	element = private_add_vertices (self, verts, count);
	lisys_free (verts);
	if (!element)
		return;

	/* Copy attributes. */
	element->shader = shader_;
	element->font = font_;
	memcpy (element->color, color, 4 * sizeof (float));
	if (scissor != NULL)
	{
		element->scissor_enabled = 1;
		element->scissor_rect[0] = scissor[0];
		element->scissor_rect[1] = scissor[1];
		element->scissor_rect[2] = scissor[2];
		element->scissor_rect[3] = scissor[3];
	}
}

/**
 * \brief Adds triangles to the overlay.
 * \param self Overlay.
 * \param shader Shader name.
 * \param image Texture image name.
 * \param color Diffuse color.
 * \param scissor Screen space scissor rectangle.
 * \param verts Array of vertices.
 * \param count Number of vertices.
 */
void liren_overlay_add_triangles (
	LIRenOverlay*      self,
	const char*        shader,
	const char*        image,
	const float*       color,
	const int*         scissor,
	const LIRenVertex* verts,
	int                count)
{
	LIRenOverlayElement* element;
	LIRenImage* image_;
	LIRenShader* shader_;

	if (!count)
		return;

	/* Find the shader. */
	shader_ = liren_internal_find_shader (self->render, shader);
	if (shader_ == NULL)
		return;

	/* Find the image. */
	image_ = liren_internal_find_image (self->render, image);
	if (image_ == NULL)
	{
		liren_internal_load_image (self->render, image);
		image_ = liren_internal_find_image (self->render, image);
		if (image == NULL)
			return;
	}

	/* Allocate the new element. */
	element = private_add_vertices (self, verts, count);
	if (!element)
		return;

	/* Copy attributes. */
	element->shader = shader_;
	element->image = image_;
	liren_image_ref (image_);
	memcpy (element->color, color, 4 * sizeof (float));
	if (scissor != NULL)
	{
		element->scissor_enabled = 1;
		element->scissor_rect[0] = scissor[0];
		element->scissor_rect[1] = scissor[1];
		element->scissor_rect[2] = scissor[2];
		element->scissor_rect[3] = scissor[3];
	}
}

/**
 * \brief Adds a child overlay to the overlay.
 * \param self Overlay.
 * \param overlay Overlay ID.
 */
void liren_overlay_add_overlay (
	LIRenOverlay* self,
	LIRenOverlay* overlay)
{
	LIRenOverlay** tmp;

	/* Detach the self. */
	if (overlay->parent == self)
		return;
	if (overlay->parent != NULL)
		private_remove_overlay (overlay->parent, overlay);

	/* Attach the self. */
	tmp = lisys_realloc (self->overlays.array, (self->overlays.count + 1) * sizeof (LIRenOverlay*));
	if (tmp == NULL)
		return;
	tmp[self->overlays.count] = overlay;
	self->overlays.array = tmp;
	self->overlays.count++;
	overlay->parent = self;
}

/**
 * \brief Removes a child overlay from the overlay.
 * \param self Overlay.
 * \param overlay Overlay ID.
 */
void liren_overlay_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* overlay)
{
	/* Detach the self. */
	if (overlay->parent == self)
		private_remove_overlay (self, overlay);
}

/**
 * \brief Disables scene rendering for the overlay.
 * \param self Overlay.
 */
void liren_overlay_disable_scene (
	LIRenOverlay* self)
{
	/* Disable the scene. */
	self->scene.enabled = 0;
	if (self->scene.framebuffer != NULL)
	{
		liren_framebuffer_free (self->scene.framebuffer);
		self->scene.framebuffer = NULL;
	}
	lisys_free (self->scene.render_passes);
	self->scene.render_passes = NULL;
	self->scene.render_passes_num = 0;
	lisys_free (self->scene.postproc_passes);
	self->scene.postproc_passes = NULL;
	self->scene.postproc_passes_num = 0;
}

/**
 * \brief Enables scene rendering for the overlay.
 * \param self Overlay.
 * \param samples Number of multisamples.
 * \param hdr Nonzero to enable HDR.
 * \param viewport Viewport array of the camera.
 * \param modelview Modelview matrix of the camera.
 * \param projection Projection matrix of the camera.
 * \param frustum Frustum of the camera.
 * \param render_passes Array of render passes.
 * \param render_passes_num Number of render passes.
 * \param postproc_passes Array of post-processing passes.
 * \param postproc_passes_num Number of post-processing passes.
 */
void liren_overlay_enable_scene (
	LIRenOverlay*      self,
	int                samples,
	int                hdr,
	const int*         viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	/* Copy camera settings. */
	self->scene.enabled = 1;
	self->scene.viewport[0] = viewport[0];
	self->scene.viewport[1] = viewport[1];
	self->scene.viewport[2] = viewport[2];
	self->scene.viewport[3] = viewport[3];
	self->scene.modelview = *modelview;
	self->scene.projection = *projection;
	self->scene.frustum = *frustum;
	self->scene.samples = samples;
	self->scene.hdr = hdr;

	/* Copy render passes. */
	lisys_free (self->scene.render_passes);
	self->scene.render_passes = NULL;
	self->scene.render_passes_num = 0;
	if (render_passes_num)
	{
		self->scene.render_passes = lisys_calloc (render_passes_num, sizeof (LIRenPassRender));
		if (self->scene.render_passes != NULL)
		{
			memcpy (self->scene.render_passes, render_passes, render_passes_num * sizeof (LIRenPassRender));
			self->scene.render_passes_num = render_passes_num;
		}
	}

	/* Copy post-processing passes. */
	lisys_free (self->scene.postproc_passes);
	self->scene.postproc_passes = NULL;
	self->scene.postproc_passes_num = 0;
	if (postproc_passes_num)
	{
		self->scene.postproc_passes = lisys_calloc (postproc_passes_num, sizeof (LIRenPassPostproc));
		if (self->scene.postproc_passes != NULL)
		{
			memcpy (self->scene.postproc_passes, postproc_passes, postproc_passes_num * sizeof (LIRenPassPostproc));
			self->scene.postproc_passes_num = postproc_passes_num;
		}
	}
}

/**
 * \brief Sets the overlay behind other overlays.
 * \param self Overlay.
 * \param value Nonzero for behind.
 */
void liren_overlay_set_behind (
	LIRenOverlay* self,
	int           value)
{
	self->behind = value;
}

/**
 * \brief Sets the screen position of the overlay.
 * \param self Overlay.
 * \param value Position vector.
 */
void liren_overlay_set_position (
	LIRenOverlay*      self,
	const LIMatVector* value)
{
	self->position = *value;
}

/**
 * \brief Sets the visibility of the overlay.
 * \param self Overlay.
 * \param value Nonzero for visible.
 */
void liren_overlay_set_visible (
	LIRenOverlay* self,
	int           value)
{
	self->visible = value;
}

/*****************************************************************************/

static void private_free_element (
	LIRenOverlay*        self,
	LIRenOverlayElement* element)
{
	if (element->image != NULL)
		liren_image_unref (element->image);
}

static LIRenOverlayElement* private_add_vertices (
	LIRenOverlay*      self,
	const LIRenVertex* verts,
	int                count)
{
	LIRenOverlayElement* tmp;
	LIRenVertex* verts1;

	/* Allocate the new element. */
	tmp = lisys_realloc (self->elements.array, (self->elements.count + 1) * sizeof (LIRenOverlayElement));
	if (tmp == NULL)
		return NULL;
	self->elements.array = tmp;
	tmp += self->elements.count;
	self->elements.count++;
	memset (tmp, 0, sizeof (LIRenOverlayElement));

	/* Copy vertices. */
	verts1 = lisys_realloc (self->vertices.array, (self->vertices.count + count) * sizeof (LIRenVertex));
	if (verts1 == NULL)
	{
		self->elements.count--;
		return NULL;
	}
	memcpy (verts1 + self->vertices.count, verts, count * sizeof (LIRenVertex));
	self->vertices.array = verts1;
	self->vertices.count += count;

	/* Calculate the buffer range. */
	if (self->elements.count > 1)
		tmp->buffer_start = (tmp - 1)->buffer_start + (tmp - 1)->buffer_count;
	else
		tmp->buffer_start = 0;
	tmp->buffer_count = count;

	/* Invalidate the buffer. */
	if (self->buffer != NULL)
	{
		liren_buffer_free (self->buffer);
		self->buffer = NULL;
	}

	return tmp;
}

static void private_remove_overlay (
	LIRenOverlay* self,
	LIRenOverlay* child)
{
	int i;

	for (i = 0 ; i < self->overlays.count ; i++)
	{
		if (self->overlays.array[i] == child)
		{
			for ( ; i < self->overlays.count - 1 ; i++)
				self->overlays.array[i] = self->overlays.array[i + 1];
			self->overlays.count--;
			if (!self->overlays.count)
			{
				lisys_free (self->overlays.array);
				self->overlays.array = NULL;
			}
			child->parent = NULL;
			break;
		}
	}
}

/** @} */
/** @} */
/** @} */
