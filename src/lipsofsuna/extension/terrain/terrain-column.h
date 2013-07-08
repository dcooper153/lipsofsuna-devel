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

#ifndef __EXT_TERRAIN_COLUMN_H__
#define __EXT_TERRAIN_COLUMN_H__

#include "lipsofsuna/model.h"
#include "terrain-stick.h"
#include "terrain-stick-filter.h"

typedef struct _LIExtTerrainColumn LIExtTerrainColumn;
struct _LIExtTerrainColumn
{
	int stamp;
	int stamp_model;
	LIExtTerrainStick* sticks;
	LIMdlModel* model;
};

LIAPICALL (int, liext_terrain_column_add_stick, (
	LIExtTerrainColumn*     self,
	float                   world_y,
	float                   world_h,
	const float*            slope_bot,
	const float*            slope_top,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_column_add_stick_corners, (
	LIExtTerrainColumn*     self,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data));

LIAPICALL (int, liext_terrain_column_build_model, (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  sticks_back,
	LIExtTerrainStick*  sticks_front,
	LIExtTerrainStick*  sticks_left,
	LIExtTerrainStick*  sticks_right,
	float               grid_size));

LIAPICALL (void, liext_terrain_column_calculate_smooth_normals, (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11));

LIAPICALL (void, liext_terrain_column_clear, (
	LIExtTerrainColumn* self));

LIAPICALL (void, liext_terrain_column_clear_model, (
	LIExtTerrainColumn* self));

LIAPICALL (int, liext_terrain_column_intersect_ray, (
	LIExtTerrainColumn* self,
	const LIMatVector*  src,
	const LIMatVector*  dst,
	LIMatVector*        result_point,
	LIMatVector*        result_normal,
	float*              result_fraction));

LIAPICALL (void, liext_terrain_column_smoothen, (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11,
	float               y,
	float               h));

LIAPICALL (int, liext_terrain_column_get_data, (
	LIExtTerrainColumn* self,
	LIArcWriter*        writer));

LIAPICALL (int, liext_terrain_column_set_data, (
	LIExtTerrainColumn* self,
	LIArcReader*        reader));

#endif
