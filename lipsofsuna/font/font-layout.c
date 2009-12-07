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
 * \addtogroup lifntLayout Layout
 * @{
 */

#include <wctype.h>
#include <string/lips-string.h>
#include "font-layout.h"

#define LIFNT_TEXT_DEFAULT_CAPACITY 32

static void
private_layout (lifntLayout* self);

static int
private_get_line_height (lifntLayout* self,
                         int          start,
                         int          end);

static int
private_get_line_ascent (lifntLayout* self,
                         int          start,
                         int          end);

static int
private_get_line_width (lifntLayout* self,
                        int          start,
                        int*         end);

static int
private_get_white_length (lifntLayout* self,
                          int          start);

/*****************************************************************************/

lifntLayout*
lifnt_layout_new ()
{
	lifntLayout* self;

	/* Allocate self. */
	self = lisys_malloc (sizeof (lifntLayout));
	if (self == NULL)
		return NULL;
	self->dirty = 0;
	self->limit_width = 0;
	self->n_glyphs = 0;
	self->c_glyphs = LIFNT_TEXT_DEFAULT_CAPACITY;

	/* Allocate glyphs. */
	self->glyphs = lisys_malloc (self->c_glyphs * sizeof (lifntLayoutGlyph));
	if (self->glyphs == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
lifnt_layout_free (lifntLayout* self)
{
	lisys_free (self->glyphs);
	lisys_free (self);
}

void
lifnt_layout_render (lifntLayout* self,
                     int          x,
                     int          y)
{
	int i;
	lifntLayoutGlyph* glyph;

	/* Layout glyphs. */
	private_layout (self);

	/* Render glyphs. */
	for (i = 0 ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		lifnt_font_render (glyph->font, x + glyph->x, y + glyph->y, glyph->glyph);
	}
}

int
lifnt_layout_append_string (lifntLayout* self,
                            lifntFont*   font,
                            const char*  string)
{
	int i;
	int length;
	wchar_t* wstr;
	lifntLayoutGlyph* glyph;

	/* Convert to wide characters. */
	wstr = listr_utf8_to_wchar (string);
	if (wstr == NULL)
		return 0;
	length = wcslen (wstr);

	/* Allocate glyphs. */
	if (self->n_glyphs + length > self->c_glyphs)
	{
		glyph = realloc (self->glyphs, (self->n_glyphs + length) * sizeof (lifntLayoutGlyph));
		if (glyph == NULL)
		{
			lisys_free (wstr);
			return 0;
		}
		self->glyphs = glyph;
		self->c_glyphs = self->n_glyphs + length;
	}

	/* Set character data. */
	for (i = 0 ; i < length ; i++)
	{
		glyph = self->glyphs + self->n_glyphs;
		glyph->font = font;
		glyph->glyph = wstr[i];
		glyph->advance = lifnt_font_get_advance (font, wstr[i]);
		self->n_glyphs++;
	}
	lisys_free (wstr);

	/* Needs relayouting. */
	self->dirty = 1;
	return 1;
}

void
lifnt_layout_clear (lifntLayout* self)
{
	self->dirty = 1;
	self->glyphs = realloc (self->glyphs, LIFNT_TEXT_DEFAULT_CAPACITY * sizeof (lifntLayoutGlyph));
	self->n_glyphs = 0;
	self->c_glyphs = LIFNT_TEXT_DEFAULT_CAPACITY;
}

int
lifnt_layout_get_height (lifntLayout* self)
{
	private_layout (self);
	return self->height;
}

int
lifnt_layout_get_width (lifntLayout* self)
{
	private_layout (self);
	return self->width;
}

int
lifnt_layout_get_width_limit (const lifntLayout* self)
{
	return self->limit_width;
}

/**
 * \brief Sets the maximum width of the text.
 *
 * Setting the limit to zero disables the width limit.
 *
 * \param self A text object.
 * \param width The new width.
 */
void
lifnt_layout_set_width_limit (lifntLayout* self,
                              int          width)
{
	self->limit_width = width;
	self->dirty = 1;
}

/*****************************************************************************/

static void
private_layout (lifntLayout* self)
{
	int i;
	int w;
	int h = 0;
	int x;
	int y = 0;
	int start = 0;
	int end = 0;
	lifntLayoutGlyph* glyph;

	if (!self->dirty)
		return;
	self->dirty = 0;
	self->width = 0;

	/* Wrap glyphs on lines. */
	for (start = 0 ; start < self->n_glyphs ; start = end + 1)
	{
		x = 0;
		w = private_get_line_width (self, start, &end);
		for (i = start ; i <= end ; i++)
		{
			glyph = self->glyphs + i;
			glyph->x = x;
			glyph->y = y;
			x += glyph->advance;
			/* FIXME: No kerning! */
		}
		if (!start)
			self->ascent = private_get_line_ascent (self, start, end);
		if (self->width < x)
			self->width = x;
		y -= private_get_line_height (self, start, end);
		if (start)
			h += private_get_line_height (self, start, end);
	}
	self->height = -y;

	/* Position the lines. */
	for (i = 0 ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		glyph->y += h;
	}
}

static int
private_get_white_length (lifntLayout* self,
                          int          start)
{
	int i;
	lifntLayoutGlyph* glyph;

	for (i = 0 ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		if (!iswspace (glyph->glyph))
			break;
	}
	return i;
}

static int
private_get_line_ascent (lifntLayout* self,
                         int          start,
                         int          end)
{
	int i;
	int asctmp;
	int ascmax = 0;

	for (i = start ; i <= end ; i++)
	{
		asctmp = self->glyphs[i].font->font_ascent;
		if (ascmax < asctmp)
			ascmax = asctmp;
	}
	return ascmax;
}

static int
private_get_line_height (lifntLayout* self,
                         int          start,
                         int          end)
{
	int i;
	int htmp;
	int hmax = 0;

	for (i = start ; i <= end ; i++)
	{
		htmp = lifnt_font_get_height (self->glyphs[i].font);
		if (hmax < htmp)
			hmax = htmp;
	}
	return hmax;
}

static int
private_get_line_width (lifntLayout* self,
                        int          start,
                        int*         end)
{
	int i;
	int w;
	int x;
	int found = 0;
	lifntLayoutGlyph* glyph;

	/* End of line? */
	for (x = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		/* FIXME: No kerning. */
		x += glyph->advance;
		*end = i;
		if (glyph->glyph == L'\n')
			break;
	}
	if (i == self->n_glyphs - 1)
	{
		*end = i;
		return x;
	}
	if (!self->limit_width || x < self->limit_width)
		return x;

	/* End of word? */
	for (x = w = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		if (iswspace (glyph->glyph))
		{
			found = 1;
			*end = i;
			w = x;
		}
		/* FIXME: No kerning. */
		if (x + glyph->advance > self->limit_width)
			break;
		x += glyph->advance;
	}
	if (found)
		return w;

	/* End of glyph? */
	for (x = 0, i = start ; i < self->n_glyphs ; i++)
	{
		glyph = self->glyphs + i;
		/* FIXME: No kerning. */
		if (x + glyph->advance > self->limit_width)
			break;
		x += glyph->advance;
	}
	*end = i;

	return x;
}

/** @} */
/** @} */
