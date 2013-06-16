/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __EXT_MODEL_MERGE_ASYNC_MERGER_H__
#define __EXT_MODEL_MERGE_ASYNC_MERGER_H__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"

#define LIMDL_ASYNC_MERGER_SHAPE_LENGTH_MAX 32

enum
{
	LIMDL_ASYNC_MERGER_ADD_MODEL,
	LIMDL_ASYNC_MERGER_ADD_MODEL_MORPH,
	LIMDL_ASYNC_MERGER_FINISH,
	LIMDL_ASYNC_MERGER_REPLACE_MATERIAL
};

typedef struct _LIMdlAsyncMergerMorph LIMdlAsyncMergerMorph;
struct _LIMdlAsyncMergerMorph
{
	char shape[LIMDL_ASYNC_MERGER_SHAPE_LENGTH_MAX];
	float value;
};

typedef union _LIMdlAsyncMergerTask LIMdlAsyncMergerTask;
union _LIMdlAsyncMergerTask
{
	int type;
	struct
	{
		int type;
		LIMdlModel* model;
		LIMdlModel* model_add;
	} add_model;
	struct
	{
		int type;
		LIMdlModel* model;
		LIMdlModel* model_add;
		LIMdlModel* model_ref;
		struct
		{
			int count;
			LIMdlAsyncMergerMorph* array;
		} morphs;
	} add_model_morph;
	struct
	{
		int type;
		LIMdlModel* model;
	} finish;
	struct
	{
		int type;
		LIMdlModel* model;
		char* match_material;
		float* set_diffuse;
		float* set_specular;
		char* set_material;
		struct
		{
			int count;
			char** array;
		} set_textures;
	} replace_material;
};

typedef struct _LIMdlAsyncMerger LIMdlAsyncMerger;
struct _LIMdlAsyncMerger
{
	LIMdlModel* model;
	LISysSerialWorker* worker;
};

LIAPICALL (LIMdlAsyncMerger*, limdl_async_merger_new, ());

LIAPICALL (void, limdl_async_merger_free, (
	LIMdlAsyncMerger* self));

LIAPICALL (int, limdl_async_merger_add_model, (
	LIMdlAsyncMerger* self,
	const LIMdlModel* model));

LIAPICALL (int, limdl_async_merger_add_model_morph, (
	LIMdlAsyncMerger*            self,
	const LIMdlModel*            model,
	const LIMdlAsyncMergerMorph* morph_array,
	int                          morph_count));

LIAPICALL (int, limdl_async_merger_finish, (
	LIMdlAsyncMerger* self));

LIAPICALL (LIMdlModel*, limdl_async_merger_pop_model, (
	LIMdlAsyncMerger* self));

LIAPICALL (int, limdl_async_merger_replace_material, (
	LIMdlAsyncMerger* self,
	const char*       match_material,
	const float*      set_diffuse,
	const float*      set_specular,
	const char*       set_material,
	const char**      set_textures,
	int               set_textures_count));

#endif
