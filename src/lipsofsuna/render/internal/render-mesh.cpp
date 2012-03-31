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
#include "render-mesh.hpp"
#include "render-mesh-builder.hpp"

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenMesh Mesh
 * @{
 */

/**
 * \brief Initializes a completely empty mesh.
 *
 * The empty mesh can be used as a placeholder. It's treated as an unloaded
 * model, and should not be attempted to be loaded.
 */
LIRenMesh::LIRenMesh () : Ogre::Mesh (NULL, "", 0, "", true)
{
	this->render = NULL;
	this->model = NULL;
}

LIRenMesh::LIRenMesh (
	Ogre::ResourceManager* creator,
	const Ogre::String&    name,
	Ogre::ResourceHandle   handle,
	const Ogre::String&    group,
	LIRenRender*           render) :
		Ogre::Mesh (creator, name, handle, group, true, new LIRenMeshBuilder (render))
{
	this->render = render;
	this->model = NULL;
}

LIRenMesh::~LIRenMesh ()
{
	/* Free the loader. */
	delete mLoader;

	/* Free the model. */
	if (model != NULL)
		limdl_model_free (model);
}

void LIRenMesh::set_model (LIMdlModel* model)
{
	lisys_assert (model != NULL);
	lisys_assert (this->model == NULL);

	this->model = model;
	static_cast<LIRenMeshBuilder*>(mLoader)->set_model (model);
}

LIMdlModel* LIRenMesh::get_model () const
{
	return model;
}

/** @} */
/** @} */
/** @} */
