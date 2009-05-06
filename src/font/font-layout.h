/* Lips of Suna
 * Copyright© 2007-2008 Lips of Suna development team.
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
 * \addtogroup lifntLayout Layout
 * @{
 */

#ifndef __FONT_LAYOUT_H__
#define __FONT_LAYOUT_H__

#include "font.h"

#define LI_TEXT_DEFAULT_CAPACITY 32

typedef struct _lifntLayoutGlyph lifntLayoutGlyph;
struct _lifntLayoutGlyph
{
	lifntFont* font;
	wchar_t glyph;
	int x;
	int y;
	int advance;
};

typedef struct _lifntLayout lifntLayout;
struct _lifntLayout
{
	int dirty;
	int ascent;
	int width;
	int height;
	int limit_width;
	int n_glyphs;
	int c_glyphs;
	lifntLayoutGlyph* glyphs;
};

lifntLayout* lifnt_layout_new             ();
void         lifnt_layout_free            (lifntLayout*       self);
void         lifnt_layout_render          (lifntLayout*       self,
                                           int                x,
                                           int                y);
int          lifnt_layout_append_string   (lifntLayout*       self,
                                           lifntFont*         font,
                                           const char*        string);
void         lifnt_layout_clear           (lifntLayout*       self);
int          lifnt_layout_get_height      (lifntLayout*       self);
int          lifnt_layout_get_width       (lifntLayout*       self);
int          lifnt_layout_get_width_limit (const lifntLayout* self);
void         lifnt_layout_set_width_limit (lifntLayout*       self,
                                           int                width);

#endif

/** @} */
/** @} */
