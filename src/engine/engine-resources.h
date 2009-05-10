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
 * \addtogroup lieng Engine
 * @{
 * \addtogroup liengResources Resources
 * @{
 */

#ifndef __ENGINE_RESOURCES_H__
#define __ENGINE_RESOURCES_H__

#include "engine-model.h"
#include "engine-types.h"

struct _liengAnimation
{
	int id;
	char* name;
	void* data;
};

struct _liengSample
{
	int id;
	int invalid;
	char* name;
	char* path;
	void* data;
};

struct _liengResources
{
	liengEngine* engine;
	struct
	{
		int count;
		liengAnimation* array;
	} animations;
	struct
	{
		int count;
		liengModel** array;
	} models;
	struct
	{
		int count;
		liengSample* array;
	} samples;
};

liengResources*
lieng_resources_new (liengEngine* engine);

void
lieng_resources_free (liengResources* self);

liengAnimation*
lieng_resources_find_animation_by_code (liengResources* self,
                                        int             id);

liengAnimation*
lieng_resources_find_animation_by_name (liengResources* self,
                                        const char*     name);

liengModel*
lieng_resources_find_model_by_code (liengResources* self,
                                    int             id);

liengModel*
lieng_resources_find_model_by_name (liengResources* self,
                                    const char*     name);

liengSample*
lieng_resources_find_sample_by_code (liengResources* self,
                                     int             id);

liengSample*
lieng_resources_find_sample_by_name (liengResources* self,
                                     const char*     name);

int
lieng_resources_load_from_dir (liengResources* self,
                               const char*     path);

int
lieng_resources_load_from_stream (liengResources* self,
                                  liReader*       reader);

int
lieng_resources_get_animation_count (liengResources* self);

int
lieng_resources_get_model_count (liengResources* self);

#endif

/** @} */
/** @} */
