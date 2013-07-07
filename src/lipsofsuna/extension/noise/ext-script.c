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
 * \addtogroup LIExtNoise Noise
 * @{
 */

#include "ext-module.h"

static void Noise_harmonic_noise_2d (LIScrArgs* args)
{
	float x;
	float y;
	int n;
	float f;
	float p;

	if (!liscr_args_geti_float (args, 0, &x))
		return;
	if (!liscr_args_geti_float (args, 1, &y))
		return;
	if (!liscr_args_geti_int (args, 2, &n))
		return;
	if (!liscr_args_geti_float (args, 3, &f))
		return;
	if (!liscr_args_geti_float (args, 4, &p))
		return;
		
	liscr_args_seti_float (args, liext_noise_harmonic_noise_2d (x, y, n, f, p));
}

static void Noise_perlin_noise (LIScrArgs* args)
{
	int seed = 0;
	int octaves = 4;
	float frequency = 2.0f;
	float persistence = 0.25f;
	LIExtModule* module;
	LIMatVector point;
	LIMatVector scale = { 1.0f, 1.0f, 1.0f};

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NOISE);
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

	module = liscr_script_get_userdata (args->script, LIEXT_SCRIPT_NOISE);
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

static void Noise_plasma_noise_2d (LIScrArgs* args)
{
	float x;
	float y;
	float f;

	if (!liscr_args_geti_float (args, 0, &x))
		return;
	if (!liscr_args_geti_float (args, 1, &y))
		return;
	if (!liscr_args_geti_float (args, 2, &f))
		return;
		
	liscr_args_seti_float (args, liext_noise_plasma_noise_2d (x, y, f));
}

static void Noise_range_noise_2d(LIScrArgs* args)
{
	float x;
	float y;
	float min;
	float max;

	if (!liscr_args_geti_float (args, 0, &x))
		return;
	if (!liscr_args_geti_float (args, 1, &y))
		return;
	if (!liscr_args_geti_float (args, 2, &min))
		return;
	if (!liscr_args_geti_float (args, 3, &max))
		return;
		
	liscr_args_seti_float (args, liext_noise_range_noise_2d (x, y, min, max));
}

static void Noise_simplex_noise_2d (LIScrArgs* args)
{
	float x;
	float y;

	if (!liscr_args_geti_float (args, 0, &x))
		return;
	if (!liscr_args_geti_float (args, 1, &y))
		return;
		
	liscr_args_seti_float (args, liext_noise_simplex_noise_2d (x, y));
}

/*****************************************************************************/

void liext_script_noise (
	LIScrScript* self)
{
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_harmonic_noise_2d", Noise_harmonic_noise_2d);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_perlin_noise", Noise_perlin_noise);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_perlin_threshold", Noise_perlin_threshold);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_plasma_noise_2d", Noise_plasma_noise_2d);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_range_noise_2d", Noise_range_noise_2d);
	liscr_script_insert_cfunc (self, LIEXT_SCRIPT_NOISE, "noise_simplex_noise_2d", Noise_simplex_noise_2d);
}

/** @} */
/** @} */
