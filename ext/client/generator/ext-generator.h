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
 * \addtogroup liextcliGenerator Generator
 * @{
 */

#ifndef __EXT_GENERATOR_H__
#define __EXT_GENERATOR_H__

#include <client/lips-client.h>
#include <generator/lips-generator.h>

typedef struct _liextGenerator liextGenerator;
struct _liextGenerator
{
	licliModule* module;
	ligenGenerator* generator;
	lirndScene* scene;
};

liextGenerator*
liext_generator_new (licliModule* module);

void
liext_generator_free (liextGenerator* self);

int
liext_generator_load (liextGenerator* self,
                      const char*     path);

int
liext_generator_save (liextGenerator* self);

#endif

/** @} */
/** @} */
/** @} */
