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

/**
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtSoftbody Softbody
 * @{
 */

#include "lipsofsuna/extension/physics/physics-private.h"
#include "lipsofsuna/render/internal/render-model-data.hpp"
#include "softbody.hpp"

struct Vertex
{
	float x, y, z;
	bool operator<(const Vertex& f) const
	{
		if (x < f.x) return true;
		if (x > f.x) return false;
		if (y < f.y) return true;
		if (y > f.y) return false;
		if (z < f.z) return true;
		return false;
	}
};

struct Edge
{
	int i1, i2;
	bool operator<(const Edge& f) const
	{
		if (i1 < f.i1) return true;
		if (i1 > f.i1) return false;
		if (i2 < f.i2) return true;
		return false;
	}
	Edge reversed() const
	{
		Edge res = {i2, i1};
		return res;
	}
};

struct Face
{
	int i1, i2, i3;
	bool operator<(const Face& f) const
	{
		if (i1 < f.i1) return true;
		if (i1 > f.i1) return false;
		if (i2 < f.i2) return true;
		if (i2 > f.i2) return false;
		if (i3 < f.i3) return true;
		return false;
	}
};

/*****************************************************************************/

LIExtSoftbody::LIExtSoftbody (LIPhyPhysics* physics, LIRenRender* render, const LIMdlModel* model) :
	physics(physics), render(render), visible(false), movement_deformation(1.0f)
{
	// Create the render model.
	this->model = new LIRenModel (render, model, 0, true);

	// Create the render object.
	object = new LIRenObject (render, 0);
	object->add_model (this->model);

	// Join vertices whose coordinates match each other.
	std::map<Vertex, int> weld_map;
	std::vector<int> index_list_rev;
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMatVector v = model->vertices.array[i].coord;
		Vertex key = { v.x, v.y, v.z };
		std::map<Vertex, int>::iterator iter = weld_map.find(key);
		if (iter == weld_map.end())
		{
			size_t index = index_list.size();
			index_list.push_back(std::vector<int>());
			index_list[index].push_back(i);
			coord_list.push_back(btVector3(v.x, v.y, v.z));
			weight_list.push_back(0.0f);
			weld_map[key] = index;
			lisys_assert(weld_map.find(key) != weld_map.end());
			index_list_rev.push_back(index);
		}
		else
		{
			index_list[iter->second].push_back(i);
			index_list_rev.push_back(iter->second);
		}
	}

	// Read the weights from the model partition.
	LIMdlPartition* partition = limdl_model_find_partition (model, "softbody");
	if (partition)
	{
		for (int i = 0 ; i < partition->vertices.count ; i++)
		{
			int mdl_index = partition->vertices.array[i].index;
			int sb_index = index_list_rev[mdl_index];
			float weight = partition->vertices.array[i].weight;
			weight_list[sb_index] = weight;
		}
	}

	// Create the softbody.
	btSoftBodyWorldInfo& info = physics->dynamics->getWorldInfo();
	info.m_gravity.setValue(0, -10, 0);
	softbody = new btSoftBody (&info, coord_list.size(),
		(btVector3*) &(coord_list[0]), (btScalar*) &(weight_list[0]));

	// Create the softbody links.
	std::map<Edge, bool> created;
	std::map<Face, bool> created_faces;
	if (model->lod.count)
	{
		const LIMdlLod* lod = model->lod.array;
		for (int i = 0 ; i < lod->indices.count ; i += 3)
		{
			int i1 = index_list_rev[lod->indices.array[i]];
			int i2 = index_list_rev[lod->indices.array[i + 1]];
			int i3 = index_list_rev[lod->indices.array[i + 2]];
			if (i1 == i2 || i2 == i3 || i3 == i1)
				continue;
			Edge edges[3] = {{i1,i2}, {i2,i3}, {i3,i1}};
			for (int j = 0 ; j < 3 ; j++)
			{
				if (created.find(edges[j]) == created.end())
				{
					created[edges[j]] = true;
					created[edges[j].reversed()] = true;
					lisys_assert(created.find(edges[j]) != created.end());
					lisys_assert(created.find(edges[j].reversed()) != created.end());
					softbody->appendLink(edges[j].i1, edges[j].i2);
				}
			}
			Face f = { i1,i2,i3 };
			if (created_faces.find(f) == created_faces.end())
			{
				softbody->appendFace(i1,i2,i3);
				created_faces[f] = true;
			}
		}
	}

	// Set the simulation parameters.
	btSoftBody::Material* mat = softbody->appendMaterial();
	mat->m_kLST = 0.5;
	mat->m_kAST = 0.5;
	mat->m_flags &= ~btSoftBody::fMaterial::DebugDraw;
	softbody->m_cfg.aeromodel = btSoftBody::eAeroModel::V_TwoSided;
	softbody->m_cfg.kDP = 0.005;
	softbody->m_cfg.kDF = 1.0;
	softbody->m_cfg.kMT = 0.005;
	softbody->m_cfg.collisions = btSoftBody::fCollision::CL_SS | btSoftBody::fCollision::CL_RS;
	softbody->m_cfg.piterations = 5;
	softbody->m_cfg.diterations = 5;
	softbody->m_cfg.viterations = 5;
	softbody->generateBendingConstraints(2, mat);
	softbody->randomizeConstraints();
	softbody->setTotalMass(1.0f);
	softbody->generateClusters(10);
	softbody->setPose(false, true);

	transform.setIdentity();
}

LIExtSoftbody::~LIExtSoftbody ()
{
	if (visible)
		physics->dynamics->removeSoftBody(softbody);
	delete softbody;
	delete object;
	delete model;
}

void LIExtSoftbody::update (float secs)
{
	LIRenModelData* data = model->get_editable ();
	if (data == NULL || data->buffer_data_0 == NULL)
		return;

	btTransform it = transform.inverse();
	btSoftBody::tNodeArray& nodes (softbody->m_nodes);
	for (size_t i = 0 ; i < index_list.size() ; i++)
	{
		const std::vector<int>& indices = index_list[i];
		for (size_t j = 0 ; j < indices.size() ; j++)
		{
			float* coord = data->buffer_data_0 + 6 * indices[j];
			btVector3 v = it * nodes[i].m_x;
			coord[0] = v[0];
			coord[1] = v[1];
			coord[2] = v[2];
		}
	}

	model->replace_buffer_vtx_nml (data->buffer_data_0);
}

void LIExtSoftbody::set_position (float x, float y, float z)
{
	// Move the render object.
	object->set_position (x, y, z);

	// Move the virtual origin.
	btVector3 diff = btVector3(x, y, z) - transform.getOrigin();
	transform.setOrigin(btVector3(x, y, z));

	// Move pinned vertices.
	btSoftBody::tNodeArray& nodes (softbody->m_nodes);
	if (diff.length() < 1.0f)
	{
		for (size_t i = 0 ; i < coord_list.size() ; i++)
		{
			const btVector3& coord0 = nodes[i].m_x;
			const btVector3 coord1 = transform * coord_list[i];
			float w = weight_list[i];
			nodes[i].m_x = w * coord0 + (1.0f - w) * coord1;
		}
	}
	else
	{
		for (size_t i = 0 ; i < coord_list.size() ; i++)
		{
			const btVector3 coord1 = transform * coord_list[i];
			nodes[i].m_x = coord1;
		}
	}
}

void LIExtSoftbody::set_rotation (float x, float y, float z, float w)
{
	// Move the render object.
	object->set_rotation (x, y, z, w);

	// Move the virtual origin.
	transform.setRotation(btQuaternion(x, y, z, w));

	// Move pinned vertices.
	btSoftBody::tNodeArray& nodes (softbody->m_nodes);
	for (size_t i = 0 ; i < coord_list.size() ; i++)
	{
		const btVector3& coord0 = nodes[i].m_x;
		const btVector3 coord1 = transform * coord_list[i];
		float w = weight_list[i];
		nodes[i].m_x = w * coord0 + (1.0f - w) * coord1;
	}
}

void LIExtSoftbody::set_render_queue (const char* value)
{
	object->set_render_queue (value);
}

void LIExtSoftbody::set_visible (int value)
{
	bool v = (value != 0);
	if (v == visible)
		return;
	visible = v;

	object->set_visible (visible);
	if (visible)
		physics->dynamics->addSoftBody(softbody, 0x0001, 0xFF00);
	else
		physics->dynamics->removeSoftBody(softbody);
}

void liext_softbody_free (
	LIExtSoftbody* self)
{
	delete self;
}

/** @} */
/** @} */
