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

#ifndef __EXT_RESOURCES_H__
#define __EXT_RESOURCES_H__

#include <model/lips-model.h>

typedef struct _liextModel liextModel;
struct _liextModel
{
	char* name;
	limatAabb bounds;
};

typedef struct _liextResources liextResources;
struct _liextResources
{
	struct
	{
		int count;
		char** array;
	} animations;
	struct
	{
		int count;
		liextModel* array;
	} models;
	struct
	{
		int count;
		char** array;
	} shaders;
	struct
	{
		int count;
		char** array;
	} textures;
};

liextResources*
liext_resources_new ();

void
liext_resources_free (liextResources* self);

void
liext_resources_clear (liextResources* self);

int
liext_resources_insert_model (liextResources*   self,
                              const char*       name,
                              const limdlModel* model);

int
liext_resources_insert_texture (liextResources* self,
                                const char*     name);

int
liext_resources_save (liextResources* self,
                      const char*     name);

#endif

/** @} */
/** @} */
/** @} */
