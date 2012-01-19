/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtHeightmap Heightmap
 * @{
 */

#include "module.h"

LIExtHeightmap* liext_heightmap_new (
	LIExtHeightmapModule* module,
	LIImgImage*           image,
	const LIMatVector*    position,
	int                   size,
	float                 spacing,
	float                 scaling)
{
	int x;
	int z;
	float h;
	LIExtHeightmap* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtHeightmap));
	if (self == NULL)
		return NULL;
	self->module = module;
	self->position = *position;
	self->size = size;
	self->spacing = spacing;
	self->min = 0.0f;
	self->max = scaling;

	/* Allocate heights. */
	if (size)
	{
		self->heights = lisys_calloc (size * size, sizeof (float));
		if (self->heights == NULL)
		{
			liext_heightmap_free (self);
			return NULL;
		}
	}

	/* Add to the dictionary. */
	self->id = lialg_u32dic_insert_auto (module->heightmaps, &module->program->random, self);
	if (!self->id)
	{
		liext_heightmap_free (self);
		return NULL;
	}

	/* Copy heights from the image. */
	if (image != NULL && image->width && image->height)
	{
		for (z = 0 ; z < size && z < image->height ; z++)
		{
			for (x = 0 ; x < size && x < image->width ; x++)
			{
				h = scaling / 255.0f * ((uint8_t*) image->pixels)[4 * (x + z * image->width)];
				self->heights[x + z * size] = h;
			}
		}
	}

	/* Call hooks. */
	if (module->physics_hooks != NULL)
		module->physics_hooks->init (module->physics_hooks->data, self);
	if (module->render_hooks != NULL)
		module->render_hooks->init (module->render_hooks->data, self);

	return self;
}

void liext_heightmap_free (
	LIExtHeightmap* self)
{
	/* Call hooks. */
	if (self->module->physics_hooks != NULL)
		self->module->physics_hooks->free (self->module->physics_hooks->data, self);
	if (self->module->render_hooks != NULL)
		self->module->render_hooks->free (self->module->render_hooks->data, self);

	/* Register from the dictionary. */
	lialg_u32dic_remove (self->module->heightmaps, self->id);

	/* Free self. */
	lisys_free (self->heights);
	lisys_free (self);
}

/** @} */
/** @} */
