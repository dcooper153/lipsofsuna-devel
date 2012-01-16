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

#ifndef __WIDGET_ELEMENT_H__
#define __WIDGET_ELEMENT_H__

#include "lipsofsuna/render.h"
#include "widget.h"
#include "widget-types.h"

LIAPICALL (void, liwdg_widget_canvas_clear, (
	LIWdgWidget* self));

LIAPICALL (void, liwdg_widget_canvas_compile, (
	LIWdgWidget* self));

LIAPICALL (int, liwdg_widget_canvas_insert_image, (
	LIWdgWidget*       self,
	const char*        image,
	const float*       color,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const int*         src_pos,
	const int*         src_tiling,
	float              rotation_angle,
	const LIMatVector* rotation_center));

LIAPICALL (int, liwdg_widget_canvas_insert_text, (
	LIWdgWidget*       self,
	const char*        font,
	const char*        text,
	const int*         dst_clip,
	const int*         dst_pos,
	const int*         dst_size,
	const float*       text_align,
	const float*       text_color,
	float              rotation_angle,
	const LIMatVector* rotation_center));

#endif
