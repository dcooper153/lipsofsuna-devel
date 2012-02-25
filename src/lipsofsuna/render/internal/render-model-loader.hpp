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

#ifndef __RENDER_MODEL_LOADER_HPP__
#define __RENDER_MODEL_LOADER_HPP__

#include "lipsofsuna/system.h"
#include "lipsofsuna/model.h"
class LIRenModelLoader;
#include "render-types.h"
#include <OgreResource.h>
#include <OgreResourceBackgroundQueue.h>

class LIRenModelLoader : public Ogre::ManualResourceLoader, Ogre::ResourceBackgroundQueue::Listener
{
public:
	LIRenModelLoader (const Ogre::String& id, LIRenModel* model_dst, LIMdlModel* model_src);
	virtual ~LIRenModelLoader ();
public:
	void abort ();
	void start (bool background);
	bool get_aborted ();
	LIMdlModel* get_model ();
public:
	virtual void loadResource (Ogre::Resource* resource);
	virtual void prepareResource (Ogre::Resource* resource);
	virtual void operationCompleted (
		Ogre::BackgroundProcessTicket ticket,
		const Ogre::BackgroundProcessResult& result);
private:
	void init ();
	void prepare_mesh (Ogre::Mesh* mesh);
	void create_mesh (Ogre::Mesh* mesh);
	void create_material (LIMdlMaterial* mat, int index, Ogre::SubMesh* submesh);
	bool create_skeleton (Ogre::Mesh* mesh);
	bool check_material_override (Ogre::MaterialPtr& material);
	bool check_name_override (const Ogre::String& name);
	void initialize_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	void override_pass (LIMdlMaterial* mat, Ogre::Pass* pass);
	void update_entities ();
private:
	bool aborted;
	bool completed;
	bool prepared;
	Ogre::String id;
	LIRenRender* render;
	LIMdlModel* model;
	LIRenModel* model_dst;
	Ogre::BackgroundProcessTicket ticket;
	/* Prepared data. */
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
};

#endif
