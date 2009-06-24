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
 * \addtogroup lifntFont Font
 * @{
 */

#include <system/lips-system.h>
#include "font.h"

static lifntFontGlyph* self_cache_glyph (lifntFont* self,
                                         wchar_t    glyph);

/*****************************************************************************/

lifntFont* lifnt_font_new (const char* path,
                           int         size)
{
	lifntFont* self;

	/* Allocate self. */
	self = calloc (1, sizeof (lifntFont));
	if (self == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		return NULL;
	}

	/* Open the font. */
	self->font = TTF_OpenFont (path, size);
	if (self->font == NULL)
	{
		lisys_error_set (EIO, "cannot load font `%s'", path);
		lifnt_font_free (self);
		return NULL;
	}
	self->font_size = size;
	self->font_height = TTF_FontLineSkip (self->font);
	self->font_ascent = TTF_FontAscent (self->font);
	self->font_descent = TTF_FontDescent (self->font);

	/* Allocate glyph index. */
	self->index = lialg_u32dic_new ();
	if (self->index == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lifnt_font_free (self);
		return NULL;
	}

	/* Allocate glyph table. */
	self->table_glyph_width = TTF_FontHeight (self->font); /* FIXME! */
	self->table_glyph_height = TTF_FontHeight (self->font);
	self->table_width = LI_FONT_CACHE_WIDTH / self->table_glyph_width;
	self->table_height = LI_FONT_CACHE_HEIGHT / self->table_glyph_height;
	self->table_length = self->table_width * self->table_height;
	self->table_filled = 0;
	self->table = calloc (self->table_length, sizeof (lifntFontGlyph*));
	if (self->table == NULL)
	{
		lisys_error_set (ENOMEM, NULL);
		lifnt_font_free (self);
		return NULL;
	}

	/* Allocate texture. */
	glGenTextures (1, &self->texture);
	glBindTexture (GL_TEXTURE_2D, self->texture);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D (GL_TEXTURE_2D, 0, 4, LI_FONT_CACHE_WIDTH, LI_FONT_CACHE_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	return self;
}

void lifnt_font_free (lifntFont* self)
{
	lialgU32dicIter iter;

	if (self->texture != 0)
		glDeleteTextures (1, &self->texture);
	if (self->index != NULL)
	{
		LI_FOREACH_U32DIC (iter, self->index)
			free (iter.value);
		lialg_u32dic_free (self->index);
	}
	if (self->table != NULL)
		free (self->table);
	if (self->font != NULL)
		TTF_CloseFont (self->font);
	free (self);
}

/**
 * \brief Renders a single glyph.
 *
 * \param self A font.
 * \param x X coordinate of the glyph.
 * \param y Y coordinate of the glyph.
 * \param glyph A glyph.
 */
void lifnt_font_render (lifntFont* self,
                        int        x,
                        int        y,
                        wchar_t    glyph)
{
	float tex_x;
	float tex_y;
	float tex_w;
	float tex_h;
	lifntFontGlyph* cached;

	/* Get tile. */
	cached = lialg_u32dic_find (self->index, glyph);
	if (cached == NULL)
	{
		cached = self_cache_glyph (self, glyph);
		if (cached == NULL)
			return;
	}

	/* Get texture rectangle. */
	tex_x = cached->table_x * self->table_glyph_width / (float) LI_FONT_CACHE_WIDTH;
	tex_y = cached->table_y * self->table_glyph_height / (float) LI_FONT_CACHE_HEIGHT;
	tex_w = cached->width / (float) LI_FONT_CACHE_WIDTH;
	tex_h = cached->height / (float) LI_FONT_CACHE_HEIGHT;

	/* Render tile. */
	glBindTexture (GL_TEXTURE_2D, self->texture);
	glBegin (GL_TRIANGLE_STRIP);
	glTexCoord2f (tex_x, tex_y);
	glVertex2f (x + cached->bearing_x,
	            y + cached->bearing_y);
	glTexCoord2f (tex_x + tex_w, tex_y);
	glVertex2f (x + cached->bearing_x + cached->width,
	            y + cached->bearing_y);
	glTexCoord2f (tex_x, tex_y + tex_h);
	glVertex2f (x + cached->bearing_x,
	            y + cached->bearing_y - cached->height);
	glTexCoord2f (tex_x + tex_w, tex_y + tex_h);
	glVertex2f (x + cached->bearing_x + cached->width,
	            y + cached->bearing_y - cached->height);
	glEnd ();
}

/**
 * \brief Gets the horizontal advance of the glyph.
 *
 * \param self A font.
 * \param glyph A wide character.
 * \return The advance in pixels.
 */
int lifnt_font_get_advance (lifntFont* self,
                            wchar_t    glyph)
{
	lifntFontGlyph* cached;

	cached = lialg_u32dic_find (self->index, glyph);
	if (cached == NULL)
	{
		cached = self_cache_glyph (self, glyph);
		if (cached == NULL)
			return 0;
	}
	return cached->advance;
}

int lifnt_font_get_height (const lifntFont* self)
{
	return self->font_height;
}

/*****************************************************************************/

static lifntFontGlyph* self_cache_glyph (lifntFont* self,
                                         wchar_t    glyph)
{
	int index;
	int advance;
	int bearing_x;
	int bearing_y;
	lifntFontGlyph* cached;
	SDL_Color color = { 0xFF, 0xFF, 0xFF, 0xFF };
	SDL_Surface* image;

	/* Render the glyph. */
	image = TTF_RenderGlyph_Blended (self->font, glyph, color);
	if (image == NULL)
		return NULL;

	/* Choose a tile. */
	if (self->table_filled < self->table_length)
	{
		/* Allocate glyph. */
		cached = malloc (sizeof (lifntFontGlyph));
		if (cached == NULL)
			return NULL;
		if (!lialg_u32dic_insert (self->index, glyph, cached))
		{
			self->table_filled--;
			SDL_FreeSurface (image);
			free (cached);
			return NULL;
		}

		/* Select empty tile. */
		for (index = 0 ; index < self->table_length ; index++)
		{
			if (self->table[index] == NULL)
				break;
		}
		assert (index < self->table_length);
		self->table[index] = cached;
	}
	else
	{
		/* Replace random tile. */
		index = rand () % self->table_length;
		cached = self->table[index];
		lialg_u32dic_remove (self->index, cached->glyph);
		if (!lialg_u32dic_insert (self->index, glyph, cached))
		{
			self->table[index] = NULL;
			self->table_filled--;
			SDL_FreeSurface (image);
			free (cached);
			return NULL;
		}
	}

	/* Store metrics. */
	TTF_GlyphMetrics (self->font, glyph, &bearing_x, NULL, NULL, &bearing_y, &advance);
	cached->glyph = glyph;
	cached->table_index = index;
	cached->table_x = index % self->table_width;
	cached->table_y = index / self->table_width;
	cached->width = image->w;
	cached->height = image->h;
	cached->advance = advance;
	cached->bearing_x = bearing_x;
	cached->bearing_y = bearing_y - self->font_descent + 1;

	/* Upload to the tile. */
	glBindTexture (GL_TEXTURE_2D, self->texture);
	glTexSubImage2D (GL_TEXTURE_2D, 0,
		self->table_glyph_width * cached->table_x,
		self->table_glyph_height * cached->table_y,
		image->w, image->h,
		GL_RGBA, GL_UNSIGNED_BYTE, image->pixels);
	SDL_FreeSurface (image);

	return cached;
}

/** @} */
/** @} */
