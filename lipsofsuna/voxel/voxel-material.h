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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxMaterial Material
 * @{
 */

#ifndef __VOXEL_MATERIAL_H__
#define __VOXEL_MATERIAL_H__

#include <lipsofsuna/archive.h>
#include "voxel-types.h"

enum
{
	LIVOX_MATERIAL_FLAG_OCCLUDER = 0x01
};

struct _LIVoxMaterial
{
	int id;
	int flags;
	char* name;
	char* model;
	float friction;
};

LIVoxMaterial*
livox_material_new ();

LIVoxMaterial*
livox_material_new_copy (const LIVoxMaterial* src);

LIVoxMaterial*
livox_material_new_from_sql (LIArcSql*     sql,
                             sqlite3_stmt* stmt);

LIVoxMaterial*
livox_material_new_from_stream (LIArcReader* reader);

void
livox_material_free (LIVoxMaterial* self);

int
livox_material_write_to_sql (LIVoxMaterial* self,
                             LIArcSql*      sql);

int
livox_material_write_to_stream (LIVoxMaterial* self,
                                LIArcWriter*   writer);

int
livox_material_set_name (LIVoxMaterial* self,
                         const char*    value);

int
livox_material_set_model (LIVoxMaterial* self,
                          const char*    value);

#endif

/** @} */
/** @} */
