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

#ifndef __EXT_TERRAIN_STICK_H__
#define __EXT_TERRAIN_STICK_H__

#include "lipsofsuna/archive.h"
#include "lipsofsuna/math.h"
#include "terrain-vertex.h"

typedef struct _LIExtTerrainStick LIExtTerrainStick;
struct _LIExtTerrainStick
{
	LIExtTerrainStick* next;
	int material;
	float height;
	LIExtTerrainVertex vertices[2][2];
};

LIAPICALL (LIExtTerrainStick*, liext_terrain_stick_new, (
	int   material,
	float height));

LIAPICALL (void, liext_terrain_stick_free, (
	LIExtTerrainStick* self));

LIAPICALL (int, liext_terrain_stick_cast_sphere, (
	const LIExtTerrainStick* self,
	float                    grid_size,
	float                    bot00,
	float                    bot10,
	float                    bot01,
	float                    bot11,
	float                    top00,
	float                    top10,
	float                    top01,
	float                    top11,
	const LIMatVector*       sphere_rel_cast_start,
	const LIMatVector*       sphere_rel_cast_end,
	float                    sphere_radius,
	float*                   result_fraction));

LIAPICALL (void, liext_terrain_stick_clamp_vertices, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* next));

LIAPICALL (void, liext_terrain_stick_clamp_vertices_bottom, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* prev));

LIAPICALL (void, liext_terrain_stick_clear, (
	LIExtTerrainStick* self));

LIAPICALL (void, liext_terrain_stick_copy_vertices, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* src));

LIAPICALL (void, liext_terrain_stick_fix_vertices_downwards, (
	LIExtTerrainStick* self,
	LIExtTerrainStick* stick_first));

LIAPICALL (void, liext_terrain_stick_fix_vertices_upwards, (
	LIExtTerrainStick* self));

LIAPICALL (void, liext_terrain_stick_move_vertices, (
	LIExtTerrainStick* self,
	float              diff));

LIAPICALL (void, liext_terrain_stick_reset_vertices, (
	LIExtTerrainStick* self));

LIAPICALL (int, liext_terrain_stick_subtract, (
	LIExtTerrainStick* self,
	float              y,
	float              h,
	float              bot00,
	float              bot10,
	float              bot01,
	float              bot11,
	float              top00,
	float              top10,
	float              top01,
	float              top11));

LIAPICALL (int, liext_terrain_stick_get_data, (
	LIExtTerrainStick* self,
	LIArcWriter*       writer));

LIAPICALL (int, liext_terrain_stick_set_data, (
	LIExtTerrainStick* self,
	LIArcReader*       reader));

LIAPICALL (int, liext_terrain_stick_get_intersection_type, (
	const LIExtTerrainStick* self,
	float                    y,
	float                    h));

LIAPICALL (void, liext_terrain_stick_get_normal, (
	const LIExtTerrainStick* self,
	LIMatVector*             result));

LIAPICALL (void, liext_terrain_stick_get_normal_override, (
	const LIExtTerrainStick* self,
	int                      vertex_x,
	int                      vertex_y,
	float                    vertex_offset,
	LIMatVector*             result));

LIAPICALL (void, liext_terrain_stick_set_vertices, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

LIAPICALL (void, liext_terrain_stick_set_vertices_max, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

LIAPICALL (void, liext_terrain_stick_set_vertices_min, (
	LIExtTerrainStick* self,
	const float*       slope,
	float              offset));

#endif
