/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lifnt Font
 * @{
 * \addtogroup lifntFont Font
 * @{
 */

#ifndef __FONT_H__
#define __FONT_H__

#include <wchar.h>
#include <algorithm/lips-algorithm.h>
#include <video/lips-video.h>

#define LIFNT_CACHE_WIDTH 512
#define LIFNT_CACHE_HEIGHT 512

typedef struct _lifntFontGlyph lifntFontGlyph;
struct _lifntFontGlyph
{
	wchar_t glyph;
	int table_index;
	int table_x;
	int table_y;
	int bearing_x;
	int bearing_y;
	int width;
	int height;
	int advance;
};

typedef struct _lifntFont lifntFont;
struct _lifntFont
{
	int font_size;
	int font_height;
	int font_ascent;
	int font_descent;
	int table_length;
	int table_filled;
	int table_width;
	int table_height;
	int table_glyph_width;
	int table_glyph_height;
	GLuint texture;
	lialgU32dic* index;
	lifntFontGlyph** table;
	lividCalls video;
	TTF_Font* font;
};

lifntFont*
lifnt_font_new (lividCalls* video,
                const char* path,
                int         size);

void
lifnt_font_free (lifntFont* self);

void
lifnt_font_render (lifntFont* self,
                   int        x,
                   int        y,
                   wchar_t    glyph);

int
lifnt_font_get_advance (lifntFont* self,
                        wchar_t    glyph);

int
lifnt_font_get_height (const lifntFont* self);

#endif

/** @} */
/** @} */
