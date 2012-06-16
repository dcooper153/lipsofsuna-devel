/* Lips of Suna
 * Copyright© 2007-2012 Lips of Suna development team.
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

#ifndef __RENDER_MESH_BUILDER_HPP__
#define __RENDER_MESH_BUILDER_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
#include "render-types.h"
#include <OgreResource.h>

class LIRenMeshBuilder : public Ogre::ManualResourceLoader
{
public:
	LIRenMeshBuilder (LIRenRender* render, LIMdlModel* model);
	virtual ~LIRenMeshBuilder ();
private:
	virtual void loadResource (Ogre::Resource* resource);
	virtual void prepareResource (Ogre::Resource* resource);
public:
	bool is_idle () const;
	LIMdlModel* get_model () const;
	void step_1_bg (Ogre::Mesh* mesh);
	void step_2_fg (Ogre::Mesh* mesh);
	void step_3_fg (Ogre::Mesh* mesh);
private:
	Ogre::MaterialPtr create_material (LIMdlMaterial* mat);
	bool create_skeleton (Ogre::Mesh* mesh);
	bool check_material_override (Ogre::MaterialPtr& material);
	bool check_name_override (const Ogre::String& name);
	void initialize_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	void override_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	void update_entities ();
private:
	/* Static data. */
	LIRenRender* render;
	LIMdlModel* model;
	/* Prepared data. */
	int step;
	size_t buffer_size_0;
	size_t buffer_size_1;
	size_t buffer_size_2;
	float* buffer_data_0;
	float* buffer_data_1;
	float* buffer_data_2;
	Ogre::VertexData* vertex_data;
	Ogre::VertexDeclaration vertex_declaration;
	/* Loaded data */
	Ogre::VertexBufferBinding* vertex_buffer_binding;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_0;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_1;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_2;
	Ogre::HardwareIndexBufferSharedPtr index_buffer;
	/* Referenced data. */
	/* To prevent temporary materials from being removed during the
	   period of the mesh being initialized but not used, we need to
	   keep references to the materials. */
	std::vector<Ogre::MaterialPtr> materials;
};

#endif
