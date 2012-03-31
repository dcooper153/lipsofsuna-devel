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

#include "render-types.h"
#include "render.h"
#include "render-mesh.hpp"
#include "render-mesh-manager.hpp"

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenMeshManager MeshManager
 * @{
 */

LIRenMeshManager::LIRenMeshManager (LIRenRender* render)
{
	this->render = render;
}

LIRenMeshManager::~LIRenMeshManager ()
{
}

Ogre::ResourcePtr LIRenMeshManager::create_mesh (
	LIMdlModel* model)
{
	LIMdlModel* model_copy = limdl_model_new_copy (model);
	Ogre::String name = render->data->id.next ();
	Ogre::String group = LIREN_RESOURCES_TEMPORARY;
	Ogre::ResourcePtr res (create (name, group, false, 0));

	static_cast<LIRenMesh*>(res.get ())->set_model (model_copy);

	return res;
}

Ogre::Resource* LIRenMeshManager::createImpl (
	const Ogre::String&            name,
	Ogre::ResourceHandle           handle, 
	const Ogre::String&            group,
	bool                           isManual,
	Ogre::ManualResourceLoader*    loader, 
	const Ogre::NameValuePairList* createParams)
{
	return new LIRenMesh (this, name, handle, group, this->render);
}

/** @} */
/** @} */
/** @} */
