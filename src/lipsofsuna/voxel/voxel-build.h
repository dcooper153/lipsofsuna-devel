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

#ifndef __VOXEL_BUILD_H__
#define __VOXEL_BUILD_H__

#include <lipsofsuna/engine.h>
#include <lipsofsuna/model.h>
#include <lipsofsuna/physics.h>
#include <lipsofsuna/system.h>
#include "voxel-manager.h"

LIAPICALL (int, livox_build_area, (
	LIVoxManager* manager,
	LIEngEngine*  engine,
	LIPhyPhysics* physics,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIMdlModel**  result_model,
	LIPhyShape**  result_physics));

LIAPICALL (int, livox_build_block, (
	LIVoxManager*         manager,
	LIEngEngine*          engine,
	LIPhyPhysics*         physics,
	const LIVoxBlockAddr* addr,
	LIMdlModel**          result_model,
	LIPhyShape**          result_physics));

LIAPICALL (int, livox_build_occlusion, (
	LIVoxManager* manager,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels,
	char*         result));

#endif
