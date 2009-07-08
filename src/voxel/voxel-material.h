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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup livoxMaterial Material
 * @{
 */

#ifndef __VOXEL_MATERIAL_H__
#define __VOXEL_MATERIAL_H__

#include <archive/lips-archive.h>
#include <model/lips-model.h>
#include "voxel-types.h"

struct _livoxMaterial
{
	int id;
	char* name;
	float friction;
	float scale;
	limdlMaterial model;
};

livoxMaterial*
livox_material_new (liarcSql*     sql,
                    sqlite3_stmt* stmt);

livoxMaterial*
livox_material_new_from_stream (liReader* reader);

void
livox_material_free (livoxMaterial* self);

int
livox_material_write_to_stream (livoxMaterial* self,
                                liarcWriter*   writer);

#endif

/** @} */
/** @} */
