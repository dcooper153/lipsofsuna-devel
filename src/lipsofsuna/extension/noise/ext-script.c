/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIExtNoise Noise
 * @{
 */

#include "ext-module.h"

/* @luadoc
 * module "core/noise"
 * --- Generate noise.
 * -- @name Noise
 * -- @class table
 */

/* @luadoc
 * --- Generates Perlin noise.
 * -- @param self Noise class.
 * -- @param args Arguments.<ul>
 * --   <li>1,point: Position vector.</li>
 * --   <li>2,scale: Coordinate scale factor.</li>
 * --   <li>3,frequency: Noise frequency.</li>
 * --   <li>4,octaves: Number of octaves.
 * --   <li>5,persistence: Noise persistence.</li></ul>
 * --   <li>6,seed: Noise seed.</li></ul>
 * -- @return Noise value.
 * Noise.perlin_noise(self, args)
 */
static void Noise_perlin_noise (LIScrArgs* args)
{
	int seed = 0;
	int octaves = 4;
	float frequency = 2.0f;
	float persistence = 0.25f;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector scale = { 1.0f, 1.0f, 1.0f};

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NOISE);
	if (!liscr_args_geti_vector (args, 0, &point) &&
	    !liscr_args_gets_vector (args, "point", &point))
		return;
	if (!liscr_args_geti_vector (args, 1, &scale))
		liscr_args_gets_vector (args, "scale", &scale);
	if (!liscr_args_geti_float (args, 2, &frequency))
		liscr_args_gets_float (args, "frequency", &frequency);
	if (!liscr_args_geti_int (args, 3, &octaves))
		liscr_args_gets_int (args, "octaves", &octaves);
	if (!liscr_args_geti_float (args, 4, &persistence))
		liscr_args_gets_float (args, "persistence", &persistence);
	if (!liscr_args_geti_int (args, 5, &seed))
		liscr_args_gets_int (args, "seed", &seed);

	liscr_args_seti_float (args, liext_noise_perlin_noise (module,
		scale.x * point.x, scale.y * point.y, scale.z * point.z, seed, octaves, frequency, persistence));
}

/* @luadoc
 * --- Finds all noise coordinates whose values exceed a threshold and creates voxel terrain out of them.
 * -- @param self Noise class.
 * -- @param args Arguments.<ul>
 * --   <li>1,min: Range start position vector.</li>
 * --   <li>2,max: Range end position vector.</li>
 * --   <li>3,tile: Tile type.</li>
 * --   <li>4,threshold: Threshold value.</li>
 * --   <li>5,scale: Coordinate scale factor.</li>
 * --   <li>6,frequency: Noise frequency.</li>
 * --   <li>7,octaves: Number of octaves.
 * --   <li>8,persistence: Noise persistence.</li>
 * --   <li>9,seed: Noise seed.</li></ul>
 * -- @return Table of vectors.
 * Noise.perlin_threshold(self, args)
 */
static void Noise_perlin_terrain (LIScrArgs* args)
{
	int i;
	int min[3];
	int max[3];
	int pos[3];
	int size[3];
	int tile = 0;
	int seed = 0;
	int octaves = 4;
	float threshold = 0.5f;
	float frequency = 2.0f;
	float persistence = 0.25f;
	float value;
	LIExtModule* module;
	LIMatVector point1;
	LIMatVector point2;
	LIMatVector scale = { 1.0f, 1.0f, 1.0f};
	LIVoxVoxel* tmp;
	LIVoxManager* voxels;

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NOISE);
	voxels = limai_program_find_component (module->program, "voxels");
	if (voxels == NULL)
		return;
	if (!liscr_args_geti_vector (args, 0, &point1) &&
	    !liscr_args_gets_vector (args, "min", &point1))
		return;
	if (!liscr_args_geti_vector (args, 1, &point2) &&
	    !liscr_args_gets_vector (args, "max", &point2))
		return;
	if (!liscr_args_geti_int (args, 2, &tile))
		liscr_args_gets_int (args, "tile", &tile);
	if (!liscr_args_geti_float (args, 3, &threshold))
		liscr_args_gets_float (args, "threshold", &threshold);
	if (!liscr_args_geti_vector (args, 4, &scale))
		liscr_args_gets_vector (args, "scale", &scale);
	if (!liscr_args_geti_float (args, 5, &frequency))
		liscr_args_gets_float (args, "frequency", &frequency);
	if (!liscr_args_geti_int (args, 6, &octaves))
		liscr_args_gets_int (args, "octaves", &octaves);
	if (!liscr_args_geti_float (args, 7, &persistence))
		liscr_args_gets_float (args, "persistence", &persistence);
	if (!liscr_args_geti_int (args, 8, &seed))
		liscr_args_gets_int (args, "seed", &seed);

	/* Calculate the size of the area. */
	min[0] = (int) point1.x;
	min[1] = (int) point1.y;
	min[2] = (int) point1.z;
	max[0] = (int) point2.x;
	max[1] = (int) point2.y;
	max[2] = (int) point2.z;
	size[0] = max[0] - min[0];
	size[1] = max[1] - min[1];
	size[2] = max[2] - min[2];
	if (!size[0] || !size[1] || !size[2])
		return;

	/* Batch copy terrain data. */
	/* Reading all tiles at once is faster than operating on
	   individual tiles since there are fewer sector lookups. */
	tmp = lisys_calloc (size[0] * size[1] * size[2], sizeof (LIVoxVoxel));
	if (tmp == NULL)
		return;
	livox_manager_copy_voxels (voxels, min[0], min[1], min[2],
		size[0], size[1], size[2], tmp);

	/* Apply thresholded perlin noise to the copied tiles. */
	for (i = 0, pos[2] = min[2] ; pos[2] < max[2] ; pos[2]++)
	for (pos[1] = min[1] ; pos[1] < max[1] ; pos[1]++)
	for (pos[0] = min[0] ; pos[0] < max[0] ; pos[0]++, i++)
	{
		value = liext_noise_perlin_noise (module, scale.x * pos[0],
			scale.y * pos[1], scale.z * pos[2], seed, octaves, frequency, persistence);
		if (value >= threshold)
			livox_voxel_init (tmp + i, tile);
	}

	/* Batch write the copied tiles. */
	livox_manager_paste_voxels (voxels, min[0], min[1], min[2],
		size[0], size[1], size[2], tmp);
	lisys_free (tmp);
}

/* @luadoc
 * --- Finds all noise coordinates whose values exceed a threshold.
 * -- @param self Noise class.
 * -- @param args Arguments.<ul>
 * --   <li>1,min: Range start position vector.</li>
 * --   <li>2,max: Range end position vector.</li>
 * --   <li>3,threshold: Threshold value.</li>
 * --   <li>4,scale: Coordinate scale factor.</li>
 * --   <li>5,frequency: Noise frequency.</li>
 * --   <li>6,octaves: Number of octaves.
 * --   <li>7,persistence: Noise persistence.</li>
 * --   <li>8,seed: Noise seed.</li></ul>
 * -- @return Table of vectors.
 * Noise.perlin_threshold(self, args)
 */
static void Noise_perlin_threshold (LIScrArgs* args)
{
	int min[3];
	int max[3];
	int pos[3];
	int seed = 0;
	int octaves = 4;
	float threshold = 0.5f;
	float frequency = 2.0f;
	float persistence = 0.25f;
	float value;
	LIExtModule* module;
	LIMatVector tmp;
	LIMatVector point1;
	LIMatVector point2;
	LIMatVector scale = { 1.0f, 1.0f, 1.0f};

	module = liscr_class_get_userdata (args->clss, LIEXT_SCRIPT_NOISE);
	if (!liscr_args_geti_vector (args, 0, &point1) &&
	    !liscr_args_gets_vector (args, "min", &point1))
		return;
	if (!liscr_args_geti_vector (args, 1, &point2) &&
	    !liscr_args_gets_vector (args, "max", &point2))
		return;
	if (!liscr_args_geti_float (args, 2, &threshold))
		liscr_args_gets_float (args, "threshold", &threshold);
	if (!liscr_args_geti_vector (args, 3, &scale))
		liscr_args_gets_vector (args, "scale", &scale);
	if (!liscr_args_geti_float (args, 4, &frequency))
		liscr_args_gets_float (args, "frequency", &frequency);
	if (!liscr_args_geti_int (args, 5, &octaves))
		liscr_args_gets_int (args, "octaves", &octaves);
	if (!liscr_args_geti_float (args, 6, &persistence))
		liscr_args_gets_float (args, "persistence", &persistence);
	if (!liscr_args_geti_int (args, 7, &seed))
		liscr_args_gets_int (args, "seed", &seed);

	liscr_args_set_output (args, LISCR_ARGS_OUTPUT_TABLE_FORCE);
	min[0] = (int) point1.x;
	min[1] = (int) point1.y;
	min[2] = (int) point1.z;
	max[0] = (int) point2.x;
	max[1] = (int) point2.y;
	max[2] = (int) point2.z;
	for (pos[2] = min[2] ; pos[2] < max[2] ; pos[2]++)
	for (pos[1] = min[1] ; pos[1] < max[1] ; pos[1]++)
	for (pos[0] = min[0] ; pos[0] < max[0] ; pos[0]++)
	{
		value = liext_noise_perlin_noise (module, scale.x * pos[0],
			scale.y * pos[1], scale.z * pos[2], seed, octaves, frequency, persistence);
		if (value >= threshold)
		{
			tmp = limat_vector_init (pos[0], pos[1], pos[2]);
			liscr_args_seti_vector (args, &tmp);
		}
	}
}

/*****************************************************************************/

void liext_script_noise (
	LIScrClass* self,
	void*       data)
{
	liscr_class_set_userdata (self, LIEXT_SCRIPT_NOISE, data);
	liscr_class_inherit (self, LISCR_SCRIPT_CLASS);
	liscr_class_insert_cfunc (self, "perlin_noise", Noise_perlin_noise);
	liscr_class_insert_cfunc (self, "perlin_terrain", Noise_perlin_terrain);
	liscr_class_insert_cfunc (self, "perlin_threshold", Noise_perlin_threshold);
}

/** @} */
/** @} */
