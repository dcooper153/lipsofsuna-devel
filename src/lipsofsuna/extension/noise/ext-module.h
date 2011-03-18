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

#ifndef __EXT_MODULE_H__
#define __EXT_MODULE_H__

#include "lipsofsuna/extension.h"

#define LIEXT_SCRIPT_NOISE "Noise"

typedef struct _LIExtModule LIExtModule;
struct _LIExtModule
{
	LIMaiProgram* program;
};

LIExtModule* liext_noise_new (
	LIMaiProgram* program);

void liext_noise_free (
	LIExtModule* self);

float liext_noise_perlin_noise (
	LIExtModule* self,
	float        x,
	float        y,
	float        z,
	int          seed,
	int          octaves,
	float        frequency,
	float        persistence);

/*****************************************************************************/

void liext_script_noise (
	LIScrClass* self,
	void*       data);

#endif
