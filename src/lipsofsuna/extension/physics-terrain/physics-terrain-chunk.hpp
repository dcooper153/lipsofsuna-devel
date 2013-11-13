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

#ifndef __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_CHUNK_HPP__
#define __EXT_PHYSICS_TERRAIN_PHYSICS_TERRAIN_CHUNK_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics/physics-private.h"
#include "physics-terrain.h"
#include <btBulletCollisionCommon.h>

class LIExtPhysicsTerrainChunk
{
public:
	LIExtPhysicsTerrainChunk(LIExtPhysicsTerrain* terrain, LIExtTerrainChunk* chunk, int x, int z, int size);
	~LIExtPhysicsTerrainChunk();
	void update();
	void set_collision_group(int value);
	void set_collision_mask(int value);
	void set_friction(float value);
	void set_visible(bool value);
private:
	void create_object ();
	void create_shape ();
	void delete_object ();
	void delete_shape ();
	void add_stick (int x, int z, const btTransform& transform, const btVector3* verts);
private:
	int x;
	int z;
	int size;
	int stamp;
	bool realized;
	btCollisionObject* object;
	btCompoundShape* shape;
	LIExtPhysicsTerrain* terrain;
	LIExtTerrainChunk* chunk;
	LIPhyPointer* pointer;
};

#endif
