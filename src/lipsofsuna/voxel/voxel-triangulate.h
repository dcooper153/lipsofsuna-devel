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

#ifndef __VOXEL_TRIANGULATE_H__
#define __VOXEL_TRIANGULATE_H__

#include <lipsofsuna/engine.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "voxel-build.h"

enum
{
	LIVOX_TRIANGULATE_NEGATIVE_X,
	LIVOX_TRIANGULATE_POSITIVE_X,
	LIVOX_TRIANGULATE_NEGATIVE_Y,
	LIVOX_TRIANGULATE_POSITIVE_Y,
	LIVOX_TRIANGULATE_NEGATIVE_Z,
	LIVOX_TRIANGULATE_POSITIVE_Z
};

typedef struct _LIVoxVoxelB LIVoxVoxelB;
struct _LIVoxVoxelB
{
	int index;
	int type;
	LIEngModel* model;
	LIMatVector position;
	LIVoxMaterial* material;
};

struct _LIVoxBuilder
{
	int model_wanted;
	int physics_wanted;
	int offset[3];
	int size[3];
	int step[3];
	float tile_width;
	float vertex_scale;
	LIEngEngine* engine;
	LIMdlBuilder* model_builder;
	LIPhyTerrain* physics;
	LIPhyPhysics* physics_manager;
	LIVoxManager* manager;
	LIVoxVoxel* voxels;
	LIVoxVoxelB* voxelsb;
};

LIAPICALL (int ,livox_triangulate_voxel, (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz,
	LIMatVector*  result,
	int*          result_faces,
	int           result_types[3][3][3]));

#endif
