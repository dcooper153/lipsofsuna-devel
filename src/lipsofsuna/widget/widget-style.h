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
 * \addtogroup liwdg Widget
 * @{
 * \addtogroup LIWdgStyle Style
 * @{
 */

#ifndef __WIDGET_STYLE_H__
#define __WIDGET_STYLE_H__

#include <lipsofsuna/image.h>
#include <lipsofsuna/system.h>
#include "widget-types.h"

struct _LIWdgStyle
{
	LIImgTexture* texture;
	LIWdgManager* manager;
	int x;
	int y;
	int w[3];
	int h[3];
	int pad[4];
	int scale;
	char font[32];
	float color[4];
	float hover[4];
	float selection[4];
};

LIAPICALL (void, liwdg_style_paint, (
	LIWdgStyle* self,
	LIWdgRect*  rect));

void liwdg_style_paint_base (
	LIWdgStyle* self,
	LIWdgRect*  rect,
	LIWdgRect*  clip);

LIAPICALL (void, liwdg_style_paint_quad, (
	LIWdgStyle* self,
	float       u0,
	float       v0,
	float       x0,
	float       y0,
	float       u1,
	float       v1,
	float       x1,
	float       y1));

LIAPICALL (void, liwdg_style_paint_rect, (
	LIWdgStyle*      self,
	const LIWdgRect* rect,
	float            u0,
	float            v0,
	float            u1,
	float            v1));

LIAPICALL (void, liwdg_style_paint_text, (
	LIWdgStyle*      self,
	LIFntLayout*     text,
	float            halign,
	float            valign,
	const LIWdgRect* rect));

#endif

/** @} */
/** @} */
