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

#ifndef __EXT_SOFTBODY_SOFTBODY_HPP__
#define __EXT_SOFTBODY_SOFTBODY_HPP__

#include <map>
#include <vector>
#include "lipsofsuna/extension.h"
#include "lipsofsuna/extension/physics/physics.h"
#include "lipsofsuna/render/internal/render.hpp"
#include "lipsofsuna/render/internal/render-object.hpp"
#include "lipsofsuna/render/internal/render-model.hpp"
#include <BulletSoftBody/btSoftBody.h>

class LIExtSoftbody
{
public:
	LIExtSoftbody (LIPhyPhysics* physics, LIRenRender* render, const LIMdlModel* model);
	~LIExtSoftbody ();
	void update (float secs);
	void set_position (float x, float y, float z);
	void set_rotation (float x, float y, float z, float w);
	void set_render_queue (const char* value);
	void set_visible (int value);
public:
	LIPhyPhysics* physics;
	LIRenRender* render;
	LIRenModel* model;
	LIRenObject* object;
private:
	typedef std::vector<btScalar> WeightList;
	typedef std::vector<btVector3> CoordList;
	typedef std::vector<std::vector<int> > WeldList;
	bool visible;
	float movement_deformation;
	CoordList coord_list;
	WeldList index_list;
	WeightList weight_list;
	btSoftBody* softbody;
};

LIAPICALL (void, liext_softbody_free, (
	LIExtSoftbody* self));

#endif
