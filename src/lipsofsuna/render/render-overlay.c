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
 * \addtogroup LIRenOverlay Overlay
 * @{
 */

#include "render-overlay.h"
#include "internal/render-internal.h"

static void private_free (
	LIRenOverlay* self);

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
 * \return Overlay ID or zero on failure.
 */
int liren_render_overlay_new (
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
		private_free (self);
		return 0;
	}

	return self->id;
}

/**
 * \brief Frees the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 */
void liren_render_overlay_free (
	LIRenRender* self,
	int          id)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	private_free (overlay);
}

/**
 * \brief Clears the elements of the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 */
void liren_render_overlay_clear (
	LIRenRender* self,
	int          id)
{
	int i;
	LIRenOverlay* overlay;
	LIRenOverlayElement* element;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	/* Free elements. */
	for (i = 0 ; i < overlay->elements.count ; i++)
	{
		element = overlay->elements.array + i;
		private_free_element (overlay, element);
	}
	lisys_free (overlay->elements.array);
	overlay->elements.array = NULL;
	overlay->elements.count = 0;

	/* Free vertices. */
	lisys_free (overlay->vertices.array);
	overlay->vertices.array = NULL;
	overlay->vertices.count = 0;

	/* Free the buffer. */
	if (overlay->buffer != NULL)
	{
		liren_buffer_free (overlay->buffer);
		overlay->buffer = NULL;
	}
}

/**
 * \brief Adds text to the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param shader Shader name.
 * \param font Font name.
 * \param color Diffuse color.
 * \param scissor Screen space scissor rectangle.
 * \param pos Text offset relative to the overlay origin.
 * \param size Clip rectangle size.
 * \param align Text alignment.
 */
void liren_render_overlay_add_text (
	LIRenRender* self,
	int          id,
	const char*  shader,
	const char*  font,
	const char*  text,
	const float* color,
	const GLint* scissor,
	const int*   pos,
	const int*   size,
	const float* align)
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
	LIRenOverlay* overlay;
	LIRenOverlayElement* element;
	LIRenShader* shader_;

	if (!strlen (text))
		return;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	/* Find the shader. */
	shader_ = liren_render_find_shader (self, shader);
	if (shader_ == NULL)
		return;

	/* Find the font. */
	font_ = lialg_strdic_find (self->fonts, font);
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
	element = private_add_vertices (overlay, verts, count);
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
		memcpy (element->scissor_rect, scissor, 4 * sizeof (GLint));
	}
}

/**
 * \brief Adds triangles to the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param shader Shader name.
 * \param image Texture image name.
 * \param color Diffuse color.
 * \param scissor Screen space scissor rectangle.
 * \param verts Array of vertices.
 * \param count Number of vertices.
 */
void liren_render_overlay_add_triangles (
	LIRenRender*       self,
	int                id,
	const char*        shader,
	const char*        image,
	const float*       color,
	const GLint*       scissor,
	const LIRenVertex* verts,
	int                count)
{
	LIRenOverlay* overlay;
	LIRenOverlayElement* element;
	LIRenImage* image_;
	LIRenShader* shader_;

	if (!count)
		return;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	/* Find the shader. */
	shader_ = liren_render_find_shader (self, shader);
	if (shader_ == NULL)
		return;

	/* Find the image. */
	image_ = liren_render_find_image (self, image);
	if (image_ == NULL)
	{
		liren_render_load_image (self, image);
		image_ = liren_render_find_image (self, image);
		if (image == NULL)
			return;
	}

	/* Allocate the new element. */
	element = private_add_vertices (overlay, verts, count);
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
		memcpy (element->scissor_rect, scissor, 4 * sizeof (GLint));
	}
}

/**
 * \brief Adds a child overlay to the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param overlay Overlay ID.
 */
void liren_render_overlay_add_overlay (
	LIRenRender* self,
	int          id,
	int          overlay)
{
	LIRenOverlay* overlay1;
	LIRenOverlay* overlay2;
	LIRenOverlay** tmp;

	overlay1 = lialg_u32dic_find (self->overlays, id);
	if (overlay1 == NULL)
		return;
	overlay2 = lialg_u32dic_find (self->overlays, overlay);
	if (overlay2 == NULL)
		return;

	/* Detach the overlay. */
	if (overlay2->parent == overlay1)
		return;
	if (overlay2->parent != NULL)
		private_remove_overlay (overlay2->parent, overlay2);

	/* Attach the overlay. */
	tmp = lisys_realloc (overlay1->overlays.array, (overlay1->overlays.count + 1) * sizeof (LIRenOverlay*));
	if (tmp == NULL)
		return;
	tmp[overlay1->overlays.count] = overlay2;
	overlay1->overlays.array = tmp;
	overlay1->overlays.count++;
	overlay2->parent = overlay1;
}

/**
 * \brief Removes a child overlay from the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param overlay Overlay ID.
 */
void liren_render_overlay_remove_overlay (
	LIRenRender* self,
	int          id,
	int          overlay)
{
	LIRenOverlay* overlay1;
	LIRenOverlay* overlay2;

	overlay1 = lialg_u32dic_find (self->overlays, id);
	if (overlay1 == NULL)
		return;
	overlay2 = lialg_u32dic_find (self->overlays, overlay);
	if (overlay2 == NULL)
		return;

	/* Detach the overlay. */
	if (overlay2->parent == overlay1)
		private_remove_overlay (overlay1, overlay2);
}

/**
 * \brief Disables scene rendering for the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 */
void liren_render_overlay_disable_scene (
	LIRenRender* self,
	int          id)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	/* Disable the scene. */
	overlay->scene.enabled = 0;
	if (overlay->scene.framebuffer != NULL)
	{
		liren_framebuffer_free (overlay->scene.framebuffer);
		overlay->scene.framebuffer = NULL;
	}
	lisys_free (overlay->scene.render_passes);
	overlay->scene.render_passes = NULL;
	overlay->scene.render_passes_num = 0;
	lisys_free (overlay->scene.postproc_passes);
	overlay->scene.postproc_passes = NULL;
	overlay->scene.postproc_passes_num = 0;
}

/**
 * \brief Enables scene rendering for the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
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
void liren_render_overlay_enable_scene (
	LIRenRender*       self,
	int                id,
	int                samples,
	int                hdr,
	const GLint*       viewport,
	LIMatMatrix*       modelview,
	LIMatMatrix*       projection,
	LIMatFrustum*      frustum,
	LIRenPassRender*   render_passes,
	int                render_passes_num,
	LIRenPassPostproc* postproc_passes,
	int                postproc_passes_num)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	/* Copy camera settings. */
	overlay->scene.enabled = 1;
	memcpy (overlay->scene.viewport, viewport, 4 * sizeof (GLint));
	overlay->scene.modelview = *modelview;
	overlay->scene.projection = *projection;
	overlay->scene.frustum = *frustum;
	overlay->scene.samples = samples;
	overlay->scene.hdr = hdr;

	/* Copy render passes. */
	lisys_free (overlay->scene.render_passes);
	overlay->scene.render_passes = NULL;
	overlay->scene.render_passes_num = 0;
	if (render_passes_num)
	{
		overlay->scene.render_passes = lisys_calloc (render_passes_num, sizeof (LIRenPassRender));
		if (overlay->scene.render_passes != NULL)
		{
			memcpy (overlay->scene.render_passes, render_passes, render_passes_num * sizeof (LIRenPassRender));
			overlay->scene.render_passes_num = render_passes_num;
		}
	}

	/* Copy post-processing passes. */
	lisys_free (overlay->scene.postproc_passes);
	overlay->scene.postproc_passes = NULL;
	overlay->scene.postproc_passes_num = 0;
	if (postproc_passes_num)
	{
		overlay->scene.postproc_passes = lisys_calloc (postproc_passes_num, sizeof (LIRenPassPostproc));
		if (overlay->scene.postproc_passes != NULL)
		{
			memcpy (overlay->scene.postproc_passes, postproc_passes, postproc_passes_num * sizeof (LIRenPassPostproc));
			overlay->scene.postproc_passes_num = postproc_passes_num;
		}
	}
}

/**
 * \brief Sets the overlay behind other overlays.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param value Nonzero for behind.
 */
void liren_render_overlay_set_behind (
	LIRenRender*       self,
	int                id,
	int                value)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	overlay->behind = value;
}

/**
 * \brief Sets the screen position of the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param value Position vector.
 */
void liren_render_overlay_set_position (
	LIRenRender*       self,
	int                id,
	const LIMatVector* value)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	overlay->position = *value;
}

/**
 * \brief Sets the overlay as the root overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 */
void liren_render_overlay_set_root (
	LIRenRender*       self,
	int                id)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	self->root_overlay = overlay;
}

/**
 * \brief Sets the visibility of the overlay.
 * \param render Renderer.
 * \param id Overlay ID.
 * \param value Nonzero for visible.
 */
void liren_render_overlay_set_visible (
	LIRenRender* self,
	int          id,
	int          value)
{
	LIRenOverlay* overlay;

	overlay = lialg_u32dic_find (self->overlays, id);
	if (overlay == NULL)
		return;

	overlay->visible = value;
}

/*****************************************************************************/

static void private_free (
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
