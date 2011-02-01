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
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenImage Image
 * @{
 */

#include "render.h"
#include "render-image.h"

static const uint8_t missing_image[16] =
{
	255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255
};

static int
private_init (LIRenImage* self,
              const char* name);

/*****************************************************************************/

LIRenImage*
liren_image_new (LIRenRender* render,
                 const char*  name)
{
	LIRenImage* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenImage));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Set name and path. */
	if (!private_init (self, name))
	{
		liren_image_free (self);
		return NULL;
	}

	/* Create dummy texture. */
	self->texture = liimg_texture_new_from_rgba (2, 2, missing_image);
	if (self->texture == NULL)
	{
		liren_image_free (self);
		return NULL;
	}

	return self;
}

LIRenImage*
liren_image_new_from_file (LIRenRender* render,
                           const char*  name)
{
	LIRenImage* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIRenImage));
	if (self == NULL)
		return NULL;
	self->render = render;

	/* Set name and path. */
	if (!private_init (self, name))
	{
		liren_image_free (self);
		return NULL;
	}

	/* Load texture. */
	if (!liren_image_load (self))
	{
		liren_image_free (self);
		return NULL;
	}

	return self;
}

void liren_image_free (
	LIRenImage* self)
{
	/* Remove from dictionary. */
	if (self->added)
		lialg_strdic_remove (self->render->images, self->name);

	if (self->texture != NULL)
		liimg_texture_free (self->texture);
	lisys_free (self->name);
	lisys_free (self->path);
	lisys_free (self);
}

int
liren_image_load (LIRenImage* self)
{
	LIImgTexture* texture;

	texture = liimg_texture_new_from_file (self->path);
	if (texture == NULL)
		return 0;
	if (self->texture != NULL)
		liimg_texture_free (self->texture);
	self->texture = texture;

	return 1;
}

/*****************************************************************************/

static int private_init (
	LIRenImage* self,
	const char* name)
{
	char* file;
	LIAlgStrdicNode* node;

	/* Allocate name. */
	self->name = listr_dup (name);
	if (self->name == NULL)
		return 0;

	/* Allocate path. */
	file = listr_concat (name, ".dds");
	if (file == NULL)
		return 0;
	self->path = lipth_paths_get_graphics (self->render->paths, file);
	free (file);
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
		((LIRenImage*) node->value)->added = 0;
		node->value = self;
	}
	self->added = 1;

	return 1;
}

/** @} */
/** @} */
