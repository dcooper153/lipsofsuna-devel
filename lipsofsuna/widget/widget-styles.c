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

#include <lipsofsuna/font.h>
#include <lipsofsuna/image.h>
#include "widget-manager.h"
#include "widget-style.h"

typedef struct _PrivateFont PrivateFont;
struct _PrivateFont
{
	int size;
	char* file;
};

static LIFntFont* private_load_font (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	const char*  file,
	int          size);

static int private_load_texture (
	LIWdgStyles*   self,
	LIImgTexture** texture,
	const char*    root,
	const char*    name);

static int private_read_font_attr (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	PrivateFont* font,
	const char*  key,
	const char*  value);

static int private_read_widget_attr (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	LIWdgStyle*  widget,
	const char*  key,
	const char*  value);

/*****************************************************************************/

LIWdgStyles* liwdg_styles_new (
	LIWdgManager* manager,
	const char*   root)
{
	LIWdgStyles* self;
	unsigned char tmp[16];

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIWdgStyles));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->root = listr_dup (root);
	if (root == NULL)
	{
		liwdg_styles_free (self);
		return NULL;
	}

	/* Initialize defaults. */
	memset (tmp, 0, sizeof (tmp));
	self->fallback.manager = manager;
	self->fallback.color[0] = 0.0f;
	self->fallback.color[1] = 0.0f;
	self->fallback.color[2] = 0.0f;
	self->fallback.color[3] = 1.0f;
	self->fallback.hover[0] = 0.0f;
	self->fallback.hover[1] = 0.0f;
	self->fallback.hover[2] = 0.0f;
	self->fallback.hover[3] = 0.0f;
	self->fallback.selection[0] = 0.5f;
	self->fallback.selection[1] = 0.5f;
	self->fallback.selection[2] = 0.5f;
	self->fallback.selection[3] = 1.0f;
	strcpy (self->fallback.font, "default");

	/* Allocate resource lists. */
	self->fonts = lialg_strdic_new ();
	self->images = lialg_strdic_new ();
	self->subimgs = lialg_strdic_new ();
	if (self->fonts == NULL ||
	    self->images == NULL ||
	    self->subimgs == NULL)
	{
		liwdg_styles_free (self);
		return NULL;
	}

	return self;
}

void liwdg_styles_free (
	LIWdgStyles* self)
{
	LIAlgStrdicIter iter;

	if (self->fonts != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->fonts)
			lifnt_font_free (iter.value);
		lialg_strdic_free (self->fonts);
	}
	if (self->images != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->images)
			liimg_texture_free (iter.value);
		lialg_strdic_free (self->images);
	}
	if (self->subimgs != NULL)
	{
		LIALG_STRDIC_FOREACH (iter, self->subimgs)
			lisys_free (iter.value);
		lialg_strdic_free (self->subimgs);
	}
	if (self->fallback.texture != NULL)
		liimg_texture_free (self->fallback.texture);
	lisys_free (self->root);
	lisys_free (self);
}

int liwdg_styles_add_font (
	LIWdgStyles* self,
	const char*  name,
	LIArcReader* reader)
{
	char* line;
	char* value;
	PrivateFont font = { 12, NULL };

	while (1)
	{
		/* Read line. */
		liarc_reader_skip_chars (reader, " \t\n");
		if (liarc_reader_check_end (reader))
			break;
		if (!liarc_reader_get_text (reader, ";\n", &line))
		{
			line = NULL;
			goto error;
		}

		/* Get key and value. */
		value = strchr (line, ':');
		if (value == NULL)
		{
			lisys_error_set (EINVAL, "syntax error in font `%s'", name);
			goto error;
		}
		*value = '\0';
		value++;
		while (isspace (*value))
			value++;

		/* Process key and value. */
		if (!private_read_font_attr (self, self->root, name, &font, line, value))
			goto error;
	}

	/* Load font. */
	if (!private_load_font (self, self->root, name, font.file? font.file : "default.ttf", font.size))
	{
		lisys_free (font.file);
		return 0;
	}
	lisys_free (font.file);

	return 1;

error:
	lisys_free (line);
	return 0;
}

int liwdg_styles_add_widget (
	LIWdgStyles* self,
	const char*  name,
	LIArcReader* reader)
{
	char* line;
	char* value;
	LIWdgStyle* widget;

	/* Allocate info. */
	widget = lisys_calloc (1, sizeof (LIWdgStyle));
	if (widget == NULL)
		return 0;
	*widget = self->fallback;

	/* Read attributes. */
	while (1)
	{
		/* Read line. */
		liarc_reader_skip_chars (reader, " \t\n");
		if (liarc_reader_check_end (reader))
			break;
		if (!liarc_reader_get_text (reader, ";\n", &line))
		{
			line = NULL;
			goto error;
		}

		/* Get key and value. */
		value = strchr (line, ':');
		if (value == NULL)
		{
			lisys_error_set (EINVAL, "syntax error in widget `%s'", name);
			goto error;
		}
		*value = '\0';
		value++;
		while (isspace (*value))
			value++;

		/* Process key and value. */
		if (!private_read_widget_attr (self, self->root, name, widget, line, value))
			goto error;
	}

	/* Add to list. */
	if (!lialg_strdic_insert (self->subimgs, name, widget))
	{
		lisys_free (widget);
		return 0;
	}

	return 1;

error:
	lisys_free (widget);
	lisys_free (line);
	return 0;
}

/*****************************************************************************/

static LIFntFont* private_load_font (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	const char*  file,
	int          size)
{
	char* path;
	LIFntFont* font;

	/* Check for duplicates. */
	if (lialg_strdic_find (self->fonts, name))
	{
		lisys_error_set (EINVAL, "duplicate font `%s'", name);
		return NULL;
	}

	/* Load font. */
	path = lisys_path_concat (root, "fonts", file, NULL);
	if (path == NULL)
		return NULL;
	font = lifnt_font_new (&self->manager->video, path, size);
	lisys_free (path);

	/* Add to list. */
	if (!lialg_strdic_insert (self->fonts, name, font))
	{
		lifnt_font_free (font);
		return 0;
	}

	return font;
}

static int private_load_texture (
	LIWdgStyles*   self,
	LIImgTexture** texture,
	const char*    root,
	const char*    name)
{
	char* path;

	path = lisys_path_concat (root, "graphics", name, NULL);
	if (path == NULL)
		return 0;
	*texture = liimg_texture_new_from_file (path);
	lisys_free (path);
	if (*texture == NULL)
		return 0;

	return 1;
}

static int private_read_font_attr (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	PrivateFont* font,
	const char*  key,
	const char*  value)
{
	if (!strcmp (key, "file"))
	{
		lisys_free (font->file);
		font->file = listr_dup (value);
	}
	else if (!strcmp (key, "size"))
	{
		sscanf (value, "%d", &font->size);
	}

	return 1;
}

static int private_read_widget_attr (
	LIWdgStyles* self,
	const char*  root,
	const char*  name,
	LIWdgStyle*  widget,
	const char*  key,
	const char*  value)
{
	LIImgTexture* image;

	if (!strcmp (key, "file"))
	{
		image = lialg_strdic_find (self->images, value);
		if (image == NULL)
		{
			if (!private_load_texture (self, &image, root, value))
				lisys_error_report ();
			else if (!lialg_strdic_insert (self->images, value, image))
			{
				liimg_texture_free (image);
				return 0;
			}
		}
		if (image != NULL)
			widget->texture = image;
	}
	else if (!strcmp (key, "source"))
	{
		sscanf (value, "%d %d", &widget->x, &widget->y);
	}
	else if (!strcmp (key, "width"))
	{
		sscanf (value, "%d %d %d", widget->w + 0, widget->w + 1, widget->w + 2);
	}
	else if (!strcmp (key, "height"))
	{
		sscanf (value, "%d %d %d", widget->h + 0, widget->h + 1, widget->h + 2);
	}
	else if (!strcmp (key, "padding"))
	{
		sscanf (value, "%d %d %d %d", widget->pad + 0, widget->pad + 1, widget->pad + 2, widget->pad + 3);
	}
	else if (!strcmp (key, "hover-color"))
	{
		sscanf (value, "%f %f %f %f", widget->hover + 0, widget->hover + 1, widget->hover + 2, widget->hover + 3);
	}
	else if (!strcmp (key, "selection-color"))
	{
		sscanf (value, "%f %f %f %f", widget->selection + 0, widget->selection + 1, widget->selection + 2, widget->selection + 3);
	}
	else if (!strcmp (key, "text-color"))
	{
		sscanf (value, "%f %f %f %f", widget->color + 0, widget->color + 1, widget->color + 2, widget->color + 3);
	}
	else if (!strcmp (key, "font"))
	{
		sscanf (value, "%32s", widget->font);
	}
	else if (!strcmp (key, "fill-mode"))
	{
		if (!strcmp (value, "resize"))
			widget->scale = 1;
		else
			widget->scale = 0;
	}
	else
	{
		lisys_error_set (EINVAL, "unknown attribute `%s' in widget `%s'", key, name);
		return 0;
	}

	return 1;
}

/** @} */
/** @} */
