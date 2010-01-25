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
 * \addtogroup liext Extension
 * @{
 * \addtogroup LIExtPackager Packager
 * @{
 */

#include <lipsofsuna/string.h>
#include <lipsofsuna/system.h>
#include "ext-resources.h"

LIExtResources*
liext_resources_new ()
{
	LIExtResources* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtResources));
	if (self == NULL)
		return NULL;

	/* Allocate ignores. */
	self->ignore = lialg_strdic_new ();
	if (self->ignore == NULL)
	{
		lisys_free (self);
		return NULL;
	}

	return self;
}

void
liext_resources_free (LIExtResources* self)
{
	int i;

	for (i = 0 ; i < self->directories.count ; i++)
		lisys_free (self->directories.array[i]);
	for (i = 0 ; i < self->files.count ; i++)
	{
		lisys_free (self->files.array[i]->src);
		lisys_free (self->files.array[i]->dst);
		lisys_free (self->files.array[i]);
	}
	lisys_free (self->directories.array);
	lisys_free (self->files.array);
	lialg_strdic_free (self->ignore);
	lisys_free (self);
}

void
liext_resources_clear (LIExtResources* self)
{
	int i;

	for (i = 0 ; i < self->directories.count ; i++)
		lisys_free (self->directories.array[i]);
	for (i = 0 ; i < self->files.count ; i++)
	{
		lisys_free (self->files.array[i]->src);
		lisys_free (self->files.array[i]->dst);
		lisys_free (self->files.array[i]);
	}
	lisys_free (self->directories.array);
	lisys_free (self->files.array);
	self->directories.array = NULL;
	self->directories.count = 0;
	self->files.array = NULL;
	self->files.count = 0;
	lialg_strdic_clear (self->ignore);
}

int
liext_resources_insert_directory (LIExtResources* self,
                                  const char*     src,
                                  const char*     dst)
{
	int i;
	int count;
	char* src1 = NULL;
	char* dst1 = NULL;
	const char* name;
	LISysDir* directory;
	LISysStat result;

	/* Add to directory list. */
	dst1 = listr_dup (dst);
	if (dst1 == NULL)
		return 0;
	if (!lialg_array_append (&self->directories, &dst1))
	{
		lisys_free (dst1);
		return 0;
	}
	dst1 = NULL;

	/* Find all files. */
	directory = lisys_dir_open (src);
	if (directory == NULL)
		return 0;
	lisys_dir_set_sorter (directory, LISYS_DIR_SORTER_ALPHA);
	if (!lisys_dir_scan (directory))
	{
		lisys_dir_free (directory);
		return 0;
	}
	count = lisys_dir_get_count (directory);

	/* Insert files and directories recursively. */
	for (i = 0 ; i < count ; i++)
	{
		name = lisys_dir_get_name (directory, i);
		if (name[0] == '.')
			continue;
		src1 = lisys_dir_get_path (directory, i);
		dst1 = lisys_path_concat (dst, name, NULL);
		if (src1 == NULL || dst1 == NULL)
			goto error;
		if (!lialg_strdic_find (self->ignore, dst1))
		{
			if (!lisys_stat (src1, &result))
				goto error;
			if (result.type == LISYS_STAT_DIRECTORY)
			{
				if (!liext_resources_insert_directory (self, src1, dst1))
					goto error;
			}
			else if (result.type == LISYS_STAT_FILE)
			{
				if (!liext_resources_insert_file (self, src1, dst1))
					goto error;
			}
		}
		lisys_free (src1);
		lisys_free (dst1);
		src1 = NULL;
		dst1 = NULL;
	}
	lisys_dir_free (directory);

	return 1;

error:
	lisys_dir_free (directory);
	lisys_free (src1);
	lisys_free (dst1);
	return 0;
}

int
liext_resources_insert_file (LIExtResources* self,
                             const char*     src,
                             const char*     dst)
{
	LIExtFile* file;

	/* Create file info. */
	file = lisys_calloc (1, sizeof (LIExtFile));
	if (file == NULL)
		return 0;
	file->src = listr_dup (src);
	file->dst = listr_dup (dst);
	if (file->src == NULL || file->dst == NULL || !lialg_array_append (&self->files, &file))
	{
		lisys_free (file->src);
		lisys_free (file->dst);
		lisys_free (file);
		return 0;
	}

	return 1;
}

int
liext_resources_insert_ignore (LIExtResources* self,
                               const char*     dst)
{
	if (!lialg_strdic_insert (self->ignore, dst, (void*) -1))
		return 0;

	return 1;
}

/** @} */
/** @} */
