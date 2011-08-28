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
 * \addtogroup LIWdgElement Element
 * @{
 */

#include "lipsofsuna/system.h"
#include "widget.h"
#include "widget-element.h"

typedef struct _LIWdgElement LIWdgElement;
struct _LIWdgElement
{
	int dst_clip_enabled;
	int dst_clip[4];
	int dst_clip_screen[4];
	int dst_pos[2];
	int dst_size[2];
	int src_pos[2];
	int src_tiling_enabled;
	int src_tiling[6];
	int img_size[2];
	float rotation;
	const char* font;
	const char* text;
	const char* image_name;
	float text_align[2];
	float text_color[4];
	LIMatVector center;
};

static void private_pack_quad (
	LIWdgWidget*  self,
	LIWdgElement* elem,
	float         u0,
	float         v0,
	float         x0,
	float         y0,
	float         u1,
	float         v1,
	float         x1,
	float         y1);

static void private_pack_scaled (
	LIWdgWidget*  self,
	LIWdgElement* elem);

static void private_pack_text (
	LIWdgWidget*  self,
	LIWdgElement* elem);

static void private_pack_tiled (
	LIWdgWidget*  self,
	LIWdgElement* elem);

static void private_pack_verts (
	LIWdgWidget*  self,
	LIWdgElement* elem,
	LIRenVertex*  verts,
	int           count);

/*****************************************************************************/

void liwdg_widget_canvas_clear (
	LIWdgWidget* self)
{
	liren_render_overlay_clear (self->manager->render, self->overlay);
}

void liwdg_widget_canvas_compile (
	LIWdgWidget* self)
{
}

int liwdg_widget_canvas_insert_image (
	LIWdgWidget*       self,
	const char*        image,
	const float*       color,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const int*         src_pos,
	const int*         src_tiling,
	float              rotation_angle,
	const LIMatVector* rotation_center)
{
	const float white[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	LIWdgElement elem;

	memset (&elem, 0, sizeof (LIWdgElement));
	elem.image_name = image;
	if (!liren_render_get_image_size (self->manager->render, image, elem.img_size))
		return 0;
	if (color != NULL)
		memcpy (elem.text_color, color, 4 * sizeof (float));
	else
		memcpy (elem.text_color, white, 4 * sizeof (float));
	if (dst_clip != NULL)
	{
		elem.dst_clip_enabled = 1;
		memcpy (elem.dst_clip, dst_clip, 4 * sizeof (int));
	}
	if (dst_pos != NULL)
		memcpy (elem.dst_pos, dst_pos, 2 * sizeof (int));
	if (dst_size != NULL)
		memcpy (elem.dst_size, dst_size, 2 * sizeof (int));
	if (src_pos != NULL)
		memcpy (elem.src_pos, src_pos, 2 * sizeof (int));
	if (src_tiling != NULL)
	{
		elem.src_tiling_enabled = 1;
		memcpy (elem.src_tiling, src_tiling, 6 * sizeof (int));
	}
	else
	{
		elem.src_tiling[1] = elem.img_size[0];
		elem.src_tiling[4] = elem.img_size[1];
	}
	elem.rotation = rotation_angle;
	if (rotation_center != NULL)
		elem.center = *rotation_center;

	if (elem.src_tiling_enabled)
		private_pack_tiled (self, &elem);
	else
		private_pack_scaled (self, &elem);

	return 1;
}

int liwdg_widget_canvas_insert_text (
	LIWdgWidget*       self,
	const char*        font,
	const char*        text,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const float*       text_align,
	const float*       text_color,
	float              rotation_angle,
	const LIMatVector* rotation_center)
{
	LIWdgElement elem;

	memset (&elem, 0, sizeof (LIWdgElement));
	elem.font = font;
	elem.text = text;
	if (elem.text == NULL)
	{
		lisys_free (self);
		return 0;
	}
	if (dst_clip != NULL)
	{
		elem.dst_clip_enabled = 1;
		memcpy (elem.dst_clip, dst_clip, 4 * sizeof (int));
	}
	if (dst_pos != NULL)
		memcpy (elem.dst_pos, dst_pos, 2 * sizeof (int));
	if (dst_size != NULL)
		memcpy (elem.dst_size, dst_size, 2 * sizeof (int));
	if (text_align != NULL)
		memcpy (elem.text_align, text_align, 2 * sizeof (float));
	if (text_color != NULL)
		memcpy (elem.text_color, text_color, 4 * sizeof (float));
	elem.rotation = rotation_angle;
	if (rotation_center != NULL)
		elem.center = *rotation_center;

	private_pack_text (self, &elem);

	return 1;
}

/*****************************************************************************/

static void private_pack_quad (
	LIWdgWidget*  self,
	LIWdgElement* elem,
	float         u0,
	float         v0,
	float         x0,
	float         y0,
	float         u1,
	float         v1,
	float         x1,
	float         y1)
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

	private_pack_verts (self, elem, vertices, 6);
}

static void private_pack_scaled (
	LIWdgWidget*  self,
	LIWdgElement* elem)
{
	float center;
	float size;
	float xs;
	float ys;
	float tx[2];
	float ty[2];

	/* Calculate texture coordinates. */
	tx[0] = (float)(elem->src_pos[0]) / elem->img_size[0];
	tx[1] = (float)(elem->src_pos[0] + elem->src_tiling[1]) / elem->img_size[0];
	ty[0] = (float)(elem->src_pos[1]) / elem->img_size[1];
	ty[1] = (float)(elem->src_pos[1] + elem->src_tiling[4]) / elem->img_size[1];

	/* Calculate pixels per texture unit. */
	xs = tx[1] - tx[0];
	ys = ty[1] - ty[0];
	if (xs < LIMAT_EPSILON || ys < LIMAT_EPSILON)
		return;
	xs = elem->dst_size[0] / xs;
	ys = elem->dst_size[1] / ys;

	/* Scale and translate to fill widget area. */
	if (ty[1] - ty[0] >= elem->dst_size[1] / xs)
	{
		center = 0.5f * (ty[0] + ty[1]);
		size = ty[1] - ty[0];
		ty[0] = center - 0.5f * elem->dst_size[1] / xs;
		ty[1] = center + 0.5f * elem->dst_size[1] / xs;
	}
	else
	{
		center = 0.5f * (tx[0] + tx[1]);
		size = tx[1] - tx[0];
		tx[0] = center - 0.5f * elem->dst_size[0] / ys;
		tx[1] = center + 0.5f * elem->dst_size[0] / ys;
	}

	/* Pack fill. */
	private_pack_quad (self, elem, tx[0], ty[0],
		elem->dst_pos[0], elem->dst_pos[1], tx[1], ty[1],
		elem->dst_pos[0] + elem->dst_size[0],
		elem->dst_pos[0] + elem->dst_size[1]);
}

static void private_pack_text (
	LIWdgWidget*  self,
	LIWdgElement* elem)
{
	liren_render_overlay_add_text (self->manager->render, self->overlay, "widget",
		elem->font, elem->text, elem->text_color,
		elem->dst_clip_enabled? elem->dst_clip : NULL,
		elem->dst_pos, elem->dst_size, elem->text_align);
}

static void private_pack_tiled (
	LIWdgWidget*  self,
	LIWdgElement* elem)
{
	int px;
	int py;
	int iw;
	int ih;
	float fw;
	float fh;
	float fu;
	float fv;
	float w[3];
	float h[3];
	float tx[4];
	float ty[4];
	LIWdgRect r;

	/* Calculate destination rectangle. */
	r.x = elem->dst_pos[0];
	r.y = elem->dst_pos[1];
	r.width = elem->dst_size[0];
	r.height = elem->dst_size[1];

	/* Calculate repeat counts. */
	w[0] = elem->src_tiling[0];
	w[1] = LIMAT_MAX (1, elem->src_tiling[1]);
	w[2] = elem->src_tiling[2];
	h[0] = elem->src_tiling[3];
	h[1] = LIMAT_MAX (1, elem->src_tiling[4]);
	h[2] = elem->src_tiling[5];

	/* Calculate texture coordinates. */
	iw = elem->img_size[0];
	ih = elem->img_size[1];
	tx[0] = (float)(elem->src_pos[0]) / iw;
	tx[1] = (float)(elem->src_pos[0] + elem->src_tiling[0]) / iw;
	tx[2] = (float)(elem->src_pos[0] + elem->src_tiling[0] + elem->src_tiling[1]) / iw;
	tx[3] = (float)(elem->src_pos[0] + elem->src_tiling[0] + elem->src_tiling[1] + elem->src_tiling[2]) / iw;
	ty[0] = (float)(elem->src_pos[1]) / ih;
	ty[1] = (float)(elem->src_pos[1] + elem->src_tiling[3]) / ih;
	ty[2] = (float)(elem->src_pos[1] + elem->src_tiling[3] + elem->src_tiling[4]) / ih;
	ty[3] = (float)(elem->src_pos[1] + elem->src_tiling[3] + elem->src_tiling[4] + elem->src_tiling[5]) / ih;

	/* Pack corners. */
	px = r.x;
	py = r.y;
	private_pack_quad (self, elem,
		tx[0], ty[0], px, py,
		tx[1], ty[1], px + w[0], py + h[0]);
	px = r.x + r.width - w[2] - 1;
	private_pack_quad (self, elem,
		tx[2], ty[0], px, py,
		tx[3], ty[1], px + w[2], py + h[0]);
	py = r.y + r.height - h[2] - 1;
	private_pack_quad (self, elem,
		tx[2], ty[2], px, py,
		tx[3], ty[3], px + w[2], py + h[2]);
	px = r.x;
	private_pack_quad (self, elem,
		tx[0], ty[2], px, py,
		tx[1], ty[3], px + w[0], py + h[2]);

	/* Pack horizontal borders. */
	for (px = r.x + w[0] ; px < r.x + r.width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], r.x + r.width - px - w[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		py = r.y;
		private_pack_quad (self, elem,
			tx[1], ty[0], px, py,
			tx[1] + fu, ty[1], px + fw, py + h[0]);
		py = r.y + r.height - h[2] - 1;
		private_pack_quad (self, elem,
			tx[1], ty[2], px, py,
			tx[1] + fu, ty[3], px + fw, py + h[2]);
	}

	/* Pack vertical borders. */
	for (py = r.y + h[0] ; py < r.y + r.height - h[2] ; py += h[1])
	{
		fh = LIMAT_MIN (h[1], r.y + r.height - py - h[2] - 1);
		fv = (ty[2] - ty[1]) * fh / h[1];
		px = r.x;
		private_pack_quad (self, elem,
			tx[0], ty[1], px, py,
			tx[1], ty[1] + fv, px + w[0], py + fh);
		px = r.x + r.width - w[2] - 1;
		private_pack_quad (self, elem,
			tx[2], ty[1], px, py,
			tx[3], ty[1] + fv, px + w[2], py + fh);
	}

	/* Pack fill. */
	for (py = r.y + h[0] ; py < r.y + r.height - h[2] ; py += h[1])
	for (px = r.x + w[0] ; px < r.x + r.width - w[2] ; px += w[1])
	{
		fw = LIMAT_MIN (w[1], r.x + r.width - px - w[2] - 1);
		fh = LIMAT_MIN (h[1], r.y + r.height - py - h[2] - 1);
		fu = (tx[2] - tx[1]) * fw / w[1];
		fv = (ty[2] - ty[1]) * fh / h[1];
		private_pack_quad (self, elem,
			tx[1], ty[1], px, py,
			tx[1] + fu, ty[1] + fv, px + fw, py + fh);
	}
}

static void private_pack_verts (
	LIWdgWidget*  self,
	LIWdgElement* elem,
	LIRenVertex*  verts,
	int           count)
{
	int i;
	LIMatQuaternion q;
	LIMatVector v;
	LIRenVertex* tmp;

	/* Apply rotation. */
	if (elem->rotation != 0.0f)
	{
		v = limat_vector_init (0.0f, 0.0f, -1.0f);
		q = limat_quaternion_rotation (elem->rotation, v);
		for (i = 0 ; i < count ; i++)
		{
			tmp = verts + i;
			v = limat_vector_init (tmp->coord[0], tmp->coord[1], tmp->coord[2]);
			v = limat_vector_subtract (v, elem->center);
			v = limat_quaternion_transform (q, v);
			v = limat_vector_add (v, elem->center);
			tmp->coord[0] = v.x;
			tmp->coord[1] = v.y;
			tmp->coord[2] = v.z;
		}
	}

	/* Add to the overlay. */
	liren_render_overlay_add_triangles (self->manager->render, self->overlay, "widget",
		elem->image_name, elem->text_color, elem->dst_clip_enabled? elem->dst_clip : NULL, verts, count);
}

/** @} */
/** @} */
