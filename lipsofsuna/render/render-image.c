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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndImage Image
 * @{
 */

#include "render.h"
#include "render-image.h"

static const uint8_t missing_image[16] =
{
	0, 0, 0, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 0, 0, 0, 255
};

static int
private_init (lirndImage* self,
              const char* name);

/*****************************************************************************/

lirndImage*
lirnd_image_new (lirndRender* render,
                 const char*  name)
{
	lirndImage* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndImage));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Set name and path. */
	if (!private_init (self, name))
	{
		lirnd_image_free (self);
		return NULL;
	}

	/* Create dummy texture. */
	self->texture = liimg_texture_new_from_rgba (2, 2, missing_image);
	if (self->texture == NULL)
	{
		lirnd_image_free (self);
		return NULL;
	}

	return self;
}

lirndImage*
lirnd_image_new_from_file (lirndRender* render,
                           const char*  name)
{
	lirndImage* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (lirndImage));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Set name and path. */
	if (!private_init (self, name))
	{
		lirnd_image_free (self);
		return NULL;
	}

	/* Load texture. */
	if (!lirnd_image_load (self))
	{
		lirnd_image_free (self);
		return NULL;
	}

	return self;
}

void
lirnd_image_free (lirndImage* self)
{
	/* Remove from dictionary. */
	if (self->added)
		lialg_strdic_remove (self->render->images, self->name);

	/* Free self. */
	lisys_free (self->name);
	lisys_free (self->path);
	lisys_free (self);
}

int
lirnd_image_load (lirndImage* self)
{
	liimgTexture* texture;

	texture = liimg_texture_new_from_file (self->path);
	if (texture == NULL)
		return 0;
	if (self->texture != NULL)
		liimg_texture_free (self->texture);
	self->texture = texture;

	return 1;
}

/*****************************************************************************/

static int
private_init (lirndImage* self,
              const char* name)
{
	lialgStrdicNode* node;

	/* Allocate name. */
	self->name = listr_dup (name);
	if (self->name == NULL)
		return 0;

	/* Allocate path. */
	self->path = lisys_path_format (self->render->config.dir,
		LISYS_PATH_SEPARATOR, "graphics",
		LISYS_PATH_SEPARATOR, name, ".dds", NULL);
	if (self->path == NULL)
		return 0;

	/* Add to dictionary. */
	node = lialg_strdic_find_node (self->render->images, name);
	if (node == NULL)
	{
		if (!lialg_strdic_insert (self->render->images, name, self))
			return 0;
	}
	else
	{
		((lirndImage*) node->value)->added = 0;
		node->value = self;
	}
	self->added = 1;

	return 1;
}

/** @} */
/** @} */
