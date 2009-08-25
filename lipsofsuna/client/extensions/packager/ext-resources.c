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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliPackager Packager
 * @{
 */

#include <model/lips-model.h>
#include <string/lips-string.h>
#include <system/lips-system.h>
#include "ext-resources.h"

static int
private_compare_models (const void* a,
                        const void* b);

static int
private_compare_strings (const void* a,
                         const void* b);

static int
private_read_animations (liextResources*   self,
                         const limdlModel* model);

static int
private_read_shaders (liextResources*   self,
                      const limdlModel* model);

static int
private_read_textures (liextResources*   self,
                       const limdlModel* model);

static void
private_write (liextResources* self,
               liarcWriter*    writer);

/*****************************************************************************/

liextResources*
liext_resources_new ()
{
	liextResources* self;

	self = lisys_calloc (1, sizeof (liextResources));
	if (self == NULL)
		return 0;

	return self;
}

void
liext_resources_free (liextResources* self)
{
	int i;

	for (i = 0 ; i < self->animations.count ; i++)
		lisys_free (self->animations.array[i]);
	for (i = 0 ; i < self->models.count ; i++)
		lisys_free (self->models.array[i].name);
	for (i = 0 ; i < self->shaders.count ; i++)
		lisys_free (self->shaders.array[i]);
	for (i = 0 ; i < self->textures.count ; i++)
		lisys_free (self->textures.array[i]);
	lisys_free (self->animations.array);
	lisys_free (self->models.array);
	lisys_free (self->shaders.array);
	lisys_free (self->textures.array);
	lisys_free (self);
}

void
liext_resources_clear (liextResources* self)
{
	int i;

	for (i = 0 ; i < self->animations.count ; i++)
		lisys_free (self->animations.array[i]);
	for (i = 0 ; i < self->models.count ; i++)
		lisys_free (self->models.array[i].name);
	for (i = 0 ; i < self->shaders.count ; i++)
		lisys_free (self->shaders.array[i]);
	for (i = 0 ; i < self->textures.count ; i++)
		lisys_free (self->textures.array[i]);
	lisys_free (self->animations.array);
	lisys_free (self->models.array);
	lisys_free (self->shaders.array);
	lisys_free (self->textures.array);
	memset (self, 0, sizeof (liextResources));
}

int
liext_resources_insert_model (liextResources*   self,
                              const char*       name,
                              const limdlModel* model)
{
	liextModel tmp;

	/* Create model. */
	tmp.name = listr_dup (name);
	tmp.bounds = model->bounds;
	if (tmp.name == NULL)
		return 0;

	/* Append model. */
	if (!lialg_array_append (&self->models, &tmp))
	{
		lisys_free (tmp.name);
		return 0;
	}

	/* Append animations and materials. */
	if (!private_read_animations (self, model) ||
	    !private_read_shaders (self, model) ||
	    !private_read_textures (self, model))
		return 0;

	return 1;
}

int
liext_resources_insert_texture (liextResources* self,
                                const char*     name)
{
	int k;
	int count;

	count = self->textures.count;
	printf ("EXTRATEXT %s\n", name);

	/* Check for duplicate. */
	for (k = 0 ; k < count ; k++)
	{
		if (!strcmp (name, self->textures.array[k]))
			return 1;
	}

	/* Append texture. */
	if (!lialg_array_resize (&self->textures, count + 1))
		return 0;
	self->textures.array[count] = listr_dup (name);
	if (self->textures.array[count] == NULL)
		return 0;

	return 1;
}

int
liext_resources_save (liextResources* self,
                      const char*     name)
{
	liarcWriter* writer;

	writer = liarc_writer_new_file (name);
	if (writer == NULL)
		return 0;
	private_write (self, writer);
	liarc_writer_free (writer);

	return 1;
}

/*****************************************************************************/

static int
private_compare_models (const void* a,
                        const void* b)
{
	const liextModel* aa = a;
	const liextModel* bb = b;

	return strcmp (aa->name, bb->name);
}

static int
private_compare_strings (const void* a,
                         const void* b)
{
	const char* const* aa = a;
	const char* const* bb = b;

	return strcmp (*aa, *bb);
}

static int
private_read_animations (liextResources*   self,
                         const limdlModel* model)
{
	int i;
	int j;
	int count;
	const limdlAnimation* animation;

	for (i = 0 ; i < model->animation.count ; i++)
	{
		animation = model->animation.animations + i;
		count = self->animations.count;

		/* Check for duplicate. */
		for (j = 0 ; j < count ; j++)
		{
			if (!strcmp (animation->name, self->animations.array[j]))
				break;
		}
		if (j != count)
			continue;

		/* Append animation. */
		if (!lialg_array_resize (&self->animations, count + 1))
			return 0;
		self->animations.array[count] = listr_dup (animation->name);
		if (self->animations.array[count] == NULL)
			return 0;
	}

	return 1;
}

static int
private_read_shaders (liextResources*   self,
                      const limdlModel* model)
{
	int i;
	int j;
	int count;
	const limdlMaterial* material;

	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array + i;
		count = self->shaders.count;

		/* Check for duplicate. */
		for (j = 0 ; j < count ; j++)
		{
			if (!strcmp (material->shader, self->shaders.array[j]))
				break;
		}
		if (j != count)
			continue;

		/* Append shader. */
		if (!lialg_array_resize (&self->shaders, count + 1))
			return 0;
		self->shaders.array[count] = listr_dup (material->shader);
		if (self->shaders.array[count] == NULL)
			return 0;
	}

	return 1;
}

static int
private_read_textures (liextResources*   self,
                       const limdlModel* model)
{
	int i;
	int j;
	int k;
	int count;
	const limdlTexture* texture;
	const limdlMaterial* material;

	for (i = 0 ; i < model->materials.count ; i++)
	{
		material = model->materials.array + i;
		for (j = 0 ; j < material->textures.count ; j++)
		{
			texture = material->textures.array + j;
			count = self->textures.count;

			/* Only interested in images. */
			if (texture->type != LIMDL_TEXTURE_TYPE_IMAGE)
				continue;

			/* Check for duplicate. */
			for (k = 0 ; k < count ; k++)
			{
				if (!strcmp (texture->string, self->textures.array[k]))
					break;
			}
			if (k != count)
				continue;

			/* Append to texture list. */
			if (!lialg_array_resize (&self->textures, count + 1))
				return 0;
			self->textures.array[count] = listr_dup (texture->string);
			if (self->textures.array[count] == NULL)
				return 0;
		}
	}

	return 1;
}

static void
private_write (liextResources* self,
               liarcWriter*    writer)
{
	int i;
	liextModel* model;

	/* Sort data. */
	qsort (self->models.array, self->models.count, sizeof (liextModel), private_compare_models);
	qsort (self->animations.array, self->animations.count, sizeof (char*), private_compare_strings);
	qsort (self->shaders.array, self->shaders.count, sizeof (char*), private_compare_strings);
	qsort (self->textures.array, self->textures.count, sizeof (char*), private_compare_strings);

	/* Write header. */
	liarc_writer_append_uint32 (writer, self->animations.count);
	liarc_writer_append_uint32 (writer, self->models.count);
	liarc_writer_append_uint32 (writer, self->shaders.count);
	liarc_writer_append_uint32 (writer, self->textures.count);

	/* Write animations. */
	for (i = 0 ; i < self->animations.count ; i++)
	{
		liarc_writer_append_string (writer, self->animations.array[i]);
		liarc_writer_append_nul (writer);
	}

	/* Write models. */
	for (i = 0 ; i < self->models.count ; i++)
	{
		model = self->models.array + i;
		liarc_writer_append_string (writer, model->name);
		liarc_writer_append_nul (writer);
	}

	/* Write shaders. */
	for (i = 0 ; i < self->shaders.count ; i++)
	{
		liarc_writer_append_string (writer, self->shaders.array[i]);
		liarc_writer_append_nul (writer);
	}

	/* Write textures. */
	for (i = 0 ; i < self->textures.count ; i++)
	{
		liarc_writer_append_string (writer, self->textures.array[i]);
		liarc_writer_append_nul (writer);
	}
}

/** @} */
/** @} */
/** @} */
