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
		elem.src_tiling[1] = -1;
		elem.src_tiling[4] = -1;
	}
	elem.rotation = rotation_angle;
	if (rotation_center != NULL)
		elem.center = *rotation_center;

	if (elem.src_tiling_enabled)
	{
		liren_render_overlay_add_tiled (self->manager->render, self->overlay,
			elem.image_name, elem.dst_pos, elem.dst_size, elem.src_pos, elem.src_tiling);
	}
	else
	{
		liren_render_overlay_add_scaled (self->manager->render, self->overlay,
			elem.image_name, elem.dst_pos, elem.dst_size, elem.src_pos, elem.src_tiling);
	}

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

	liren_render_overlay_add_text (self->manager->render, self->overlay, "widget",
		elem.font, elem.text, elem.text_color,
		elem.dst_clip_enabled? elem.dst_clip : NULL,
		elem.dst_pos, elem.dst_size, elem.text_align);

	return 1;
}

/** @} */
/** @} */
