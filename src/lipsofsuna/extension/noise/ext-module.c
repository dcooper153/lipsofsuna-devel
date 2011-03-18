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

static float private_noise (
	float x,
	float y,
	float z);

static float private_smooth_noise (
	float x,
	float y,
	float z);

static float private_interpolated_noise (
	float x,
	float y,
	float z);

/*****************************************************************************/

LIMaiExtensionInfo liext_noise_info =
{
	LIMAI_EXTENSION_VERSION, "Noise",
	liext_noise_new,
	liext_noise_free
};

LIExtModule* liext_noise_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;

	/* Register classes. */
	liscr_script_create_class (program->script, "Noise", liext_script_noise, self);

	return self;
}

void liext_noise_free (
	LIExtModule* self)
{
	lisys_free (self);
}

float liext_noise_perlin_noise (
	LIExtModule* self,
	float        x,
	float        y,
	float        z,
	int          seed,
	int          octaves,
	float        frequency,
	float        persistence)
{
	int i;
	int s[3];
	float a;
	float f;
	float total = 0;
	LIAlgRandom random;

	lialg_random_init (&random, seed);
	s[0] = lialg_random_range (&random, 0, 1024);
	s[1] = lialg_random_range (&random, 0, 1024);
	s[2] = lialg_random_range (&random, 0, 1024);

	for (i = 0 ; i < octaves ; i++)
	{
		a = powf (persistence, i);
		f = powf (frequency, i);
		total = total + a * private_interpolated_noise(s[0] + x * f, s[1] + y * f, s[2] + z * f);
	}

	return total;
}

/*****************************************************************************/

static float private_noise (
	float x,
	float y,
	float z)
{
	uint32_t m = x + y * 57 + z * 7919;
	uint32_t n = (m << 13) ^ m;
	uint32_t o = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7FFFFFFF;
	return (1 - o / 1073741824);    
}

static float private_smooth_noise (
	float x,
	float y,
	float z)
{
	return 0.4f * private_noise(x, y, z) + 0.1 * (
		private_noise(x-1, y, z) + private_noise(x+1, y, z) +
		private_noise(x, y-1, z) + private_noise(x, y+1, z) +
		private_noise(x, y, z-1) + private_noise(x, y, z+1));
}

static float private_interpolated_noise (
	float x,
	float y,
	float z)
{
	int ix = (int) x;
	int iy = (int) y;
	int iz = (int) z;
	float v000 = private_smooth_noise (ix, iy, iz);
	float v100 = private_smooth_noise (ix + 1, iy, iz);
	float v010 = private_smooth_noise (ix, iy + 1, iz);
	float v110 = private_smooth_noise (ix + 1, iy + 1, iz);
	float v001 = private_smooth_noise (ix, iy, iz + 1);
	float v101 = private_smooth_noise (ix + 1, iy, iz + 1);
	float v011 = private_smooth_noise (ix, iy + 1, iz + 1);
	float v111 = private_smooth_noise (ix + 1, iy + 1, iz + 1);
	float fx = x - ix;
	float v00 = limat_mix (v000, v100, fx);
	float v10 = limat_mix (v010, v110, fx);
	float v01 = limat_mix (v001, v101, fx);
	float v11 = limat_mix (v011, v111, fx);
	float fy = y - iy;
	float v0 = limat_mix (v00, v10, fy);
	float v1 = limat_mix (v01, v11, fy);
	return limat_mix (v0, v1, z - iz);
}

/** @} */
/** @} */
