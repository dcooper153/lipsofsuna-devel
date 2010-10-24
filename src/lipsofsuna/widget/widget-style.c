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
 * \addtogroup LIWdg Widget
 * @{
 * \addtogroup LIWdgStyle Style
 * @{
 */

#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include "widget-manager.h"
#include "widget-style.h"

static void private_pack_quad (
	LIRenVertex* array,
	int*         offset,
	float        u0,
	float        v0,
	float        x0,
	float        y0,
	float        u1,
	float        v1,
	float        x1,
	float        y1);

static void private_paint_scaled (
	LIWdgStyle* self,
	LIWdgRect*  rect);

static void private_paint_tiled (
	LIWdgStyle* self,
	LIWdgRect*  rect);

/*****************************************************************************/

/**
 * \brief Paints widget graphics.
 * \param self Style.
 * \param rect Rectangle.
 */
void liwdg_style_paint (
	LIWdgStyle* self,
	LIWdgRect*  rect)
{
	liwdg_style_paint_base (self, rect, NULL);
}

/**
 * \brief Paints widget base with optional clipping.
 * \param self Style.
 * \param rect Frame rectangle.
 * \param clip Clip rectangle or NULL.
 */
void liwdg_style_paint_base (
	LIWdgStyle* self,
	LIWdgRect*  rect,
	LIWdgRect*  clip)
{
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

	if (self->texture == NULL)
		return;
	if (clip != NULL)
	{
		glPushAttrib (GL_SCISSOR_BIT);
		glEnable (GL_SCISSOR_TEST);
		glScissor (clip->x, self->manager->height - clip->y - clip->height, clip->width, clip->height);
	}
	liren_context_set_diffuse (self->manager->context, white);
	liren_context_set_projection (self->manager->context, &self->manager->projection);
	liren_context_set_shader (self->manager->context, self->manager->shader);
	liren_context_set_textures_raw (self->manager->context, &self->texture->texture, 1);
	liren_context_bind (self->manager->context);
	if (self->scale)
		private_paint_scaled (self, rect);
	else
		private_paint_tiled (self, rect);
	if (clip != NULL)
		glPopAttrib ();
}

void liwdg_style_paint_text (
	LIWdgStyle*      self,
	LIFntLayout*     text,
	float            halign,
	float            valign,
	const LIWdgRect* rect)
{
	int i;
	int j;
	int x;
	int y;
	int w;
	int h;
	float* vertex;
	float* vertex_data;
	uint32_t* index_data;
	LIFntLayoutGlyph* glyph;
	LIRenVertex vertices[6];

	/* Get vertex data. */
	if (!text->n_glyphs)
		return;
	if (!lifnt_layout_get_vertices (text, &index_data, &vertex_data))
		return;

	/* Apply translation. */
	w = lifnt_layout_get_width (text);
	h = lifnt_layout_get_height (text);
	x = rect->x + (int)(halign * (rect->width - w));
	y = rect->y + (int)(valign * (rect->height - h));
	for (i = 2 ; i < text->n_glyphs * 20 ; i += 5)
	{
		glyph = text->glyphs + i;
		vertex_data[i + 0] += x;
		vertex_data[i + 1] += y;
	}

	/* Enable clipping. */
	glPushAttrib (GL_SCISSOR_BIT);
	glScissor (rect->x, self->manager->height - rect->y - rect->height, rect->width, rect->height);
	glEnable (GL_SCISSOR_TEST);

	/* Render glyphs. */
	liren_context_set_diffuse (self->manager->context, self->color);
	liren_context_set_buffer (self->manager->context, NULL);
	liren_context_set_projection (self->manager->context, &self->manager->projection);
	liren_context_set_shader (self->manager->context, self->manager->shader);
	for (i = 0 ; i < text->n_glyphs ; i++)
	{
		glyph = text->glyphs + i;
		liren_context_set_textures_raw (self->manager->context, &glyph->font->texture, 1);
		liren_context_bind (self->manager->context);
		for (j = 0 ; j < 6 ; j++)
		{
			vertex = vertex_data + 5 * index_data[6 * i + j];
			memcpy (vertices[j].coord, vertex + 2, 3 * sizeof (float));
			memset (vertices[j].normal, 0, 3 * sizeof (float));
			memcpy (vertices[j].texcoord, vertex + 0, 2 * sizeof (float));
		}
		liren_context_render_immediate (self->manager->context, GL_TRIANGLES, vertices, 6);
	}

	/* Disable clipping. */
	glPopAttrib ();
	lisys_free (index_data);
	lisys_free (vertex_data);
}

/**
 * \brief Paints a textured rectangle.
 * \param self Style.
 * \param u0 UV coordinate of the top-left corner.
 * \param v0 UV coordinate of the top-left corner.
 * \param x0 XY coordinate of the top-left corner.
 * \param y0 XY coordinate of the top-left corner.
 * \param u1 UV coordinate of the bottom-right corner.
 * \param v1 UV coordinate of the bottom-right corner.
 * \param x1 XY coordinate of the bottom-right corner.
 * \param y1 XY coordinate of the bottom-right corner.
 */
void liwdg_style_paint_textured_quad (
	LIWdgStyle* self,
	float       u0,
	float       v0,
	float       x0,
	float       y0,
	float       u1,
	float       v1,
	float       x1,
	float       y1)
{
	float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	LIRenVertex vertices[6] =
	{
		{ { x0, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v0 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v1 } }
	};
	liren_context_set_diffuse (self->manager->context, white);
	liren_context_set_projection (self->manager->context, &self->manager->projection);
	liren_context_set_shader (self->manager->context, self->manager->shader);
	liren_context_set_textures_raw (self->manager->context, &self->texture->texture, 1);
	liren_context_bind (self->manager->context);
	liren_context_render_immediate (self->manager->context, GL_TRIANGLES, vertices, 6);
}

/**
 * \brief Paints a textured rectangle.
 * \param self Style.
 * \param rect Rectangle.
 * \param u0 UV coordinate of the top-left corner.
 * \param v0 UV coordinate of the top-left corner.
 * \param u1 UV coordinate of the bottom-right corner.
 * \param v1 UV coordinate of the bottom-right corner.
 */
void liwdg_style_paint_textured_rect (
	LIWdgStyle*      self,
	const LIWdgRect* rect,
	float            u0,
	float            v0,
	float            u1,
	float            v1)
{
	liwdg_style_paint_textured_quad (self, u0, v0, rect->x, rect->y,
		u1, v1, rect->x + rect->width, rect->y + rect->height);
}

/*****************************************************************************/

static void private_pack_quad (
	LIRenVertex* array,
	int*         offset,
	float        u0,
	float        v0,
	float        x0,
	float        y0,
	float        u1,
	float        v1,
	float        x1,
	float        y1)
{
	LIRenVertex vertices[6] =
	{
		{ { x0, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v0 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y0, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v0 } },
		{ { x0, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u0, v1 } },
		{ { x1, y1, 0.0f }, { 0.0f, 0.0f, 0.0f }, { u1, v1 } }
	};
	memcpy (array + *offset, vertices, 6 * sizeof (LIRenVertex));
	*offset += 6;
	lisys_assert (*offset < 65536);
}

static void private_paint_scaled (
	LIWdgStyle* self,
	LIWdgRect*  rect)
{
	int offset = 0;
	float center;
	float size;
	float xs;
	float ys;
	float tx[2];
	float ty[2];
	LIRenVertex vertices[6];

	/* Calculate texture coordinates. */
	tx[0] = (float)(self->x) / self->texture->width;
	tx[1] = (float)(self->x + self->w[1]) / self->texture->width;
	ty[0] = (float)(self->y) / self->texture->height;
	ty[1] = (float)(self->y + self->h[1]) / self->texture->height;

	/* Calculate pixels per texture unit. */
	xs = tx[1] - tx[0];
	ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = rect->width / xs;
	ys = rect->height / ys;

	/* Scale and translate to fill widget area. */
	if (ty[1] - ty[0] >= rect->height / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * rect->height / xs;
		ty[1] = center + 0.5f * rect->height / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * rect->width / ys;
		tx[1] = center + 0.5f * rect->width / ys;
	}

	/* Paint fill. */
	private_pack_quad (vertices, &offset,
		tx[0], ty[0], rect->x, rect->y,
		tx[1], ty[1], rect->x + rect->width, rect->y + rect->height);
	liren_context_render_immediate (self->manager->context, GL_TRIANGLES, vertices, offset);
}

static void private_paint_tiled (
	LIWdgStyle* self,
	LIWdgRect*  rect)
{
	int px;
	int py;
	int offset = 0;
	float fw;
	float fh;
	float fu;
	float fv;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];
	LIRenVertex vertices[65536];

	/* Calculate repeat counts. */
	w[0] = self->w[0];
	w[1] = LIMAT_MAX (1, self->w[1]);
	w[2] = self->w[2];
	h[0] = self->h[0];
	h[1] = LIMAT_MAX (1, self->h[1]);
	h[2] = self->h[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(self->x) / self->texture->width;
	tx[1] = (float)(self->x + self->w[0]) / self->texture->width;
	tx[2] = (float)(self->x + self->w[0] + self->w[1]) / self->texture->width;
	tx[3] = (float)(self->x + self->w[0] + self->w[1] + self->w[2]) / self->texture->width;
	ty[0] = (float)(self->y) / self->texture->height;
	ty[1] = (float)(self->y + self->h[0]) / self->texture->height;
	ty[2] = (float)(self->y + self->h[0] + self->h[1]) / self->texture->height;
	ty[3] = (float)(self->y + self->h[0] + self->h[1] + self->h[2]) / self->texture->height;

	/* Pack corners. */
	px = rect->x;
	py = rect->y;
	private_pack_quad (vertices, &offset,
		tx[0], ty[0], px, py,
		tx[1], ty[1], px + w[0], py + h[0]);
	px = rect->x + rect->width - w[2] - 1;
	private_pack_quad (vertices, &offset,
		tx[2], ty[0], px, py,
		tx[3], ty[1], px + w[2], py + h[0]);
	py = rect->y + rect->height - h[2] - 1;
	private_pack_quad (vertices, &offset,
		tx[2], ty[2], px, py,
		tx[3], ty[3], px + w[2], py + h[2]);
	px = rect->x;
	private_pack_quad (vertices, &offset,
		tx[0], ty[2], px, py,
		tx[1], ty[3], px + w[0], py + h[2]);

	/* Pack horizontal borders. */
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		py = rect->y;
		private_pack_quad (vertices, &offset,
			tx[1], ty[0], px, py,
			tx[1] + fu, ty[1], px + fw, py + h[0]);
		py = rect->y + rect->height - h[2] - 1;
		private_pack_quad (vertices, &offset,
			tx[1], ty[2], px, py,
			tx[1] + fu, ty[3], px + fw, py + h[2]);
	}

	/* Pack vertical borders. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	{
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fv = (ty[2] - ty[1]) * fh / h[1];
		px = rect->x;
		private_pack_quad (vertices, &offset,
			tx[0], ty[1], px, py,
			tx[1], ty[1] + fv, px + w[0], py + fh);
		px = rect->x + rect->width - w[2] - 1;
		private_pack_quad (vertices, &offset,
			tx[2], ty[1], px, py,
			tx[3], ty[1] + fv, px + w[0], py + fh);
	}

	/* Pack fill. */
	for (py = rect->y + h[0] ; py < rect->y + rect->height - h[2] ; py += h[1])
	for (px = rect->x + w[0] ; px < rect->x + rect->width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], rect->x + rect->width - px - w[2] - 1);
		fh = LIMAT_MIN (h[1], rect->y + rect->height - py - h[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		fv = (ty[2] - ty[1]) * fh / h[1];
		private_pack_quad (vertices, &offset,
			tx[1], ty[1], px, py,
			tx[1] + fu, ty[1] + fv, px + fw, py + fh);
	}

	/* Render the packed vertices. */
	liren_context_render_immediate (self->manager->context, GL_TRIANGLES, vertices, offset);
}

/** @} */
/** @} */
