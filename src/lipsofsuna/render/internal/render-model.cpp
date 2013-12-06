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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "render.hpp"
#include "render-material-utils.hpp"
#include "render-mesh-builder.hpp"
#include "render-model.hpp"
#include "render-object.hpp"
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 */
LIRenModel::LIRenModel (
	LIRenRender*      render,
	const LIMdlModel* model,
	int               id)
{
	this->id = id;
	this->render = render;

	/* Choose a unique ID. */
	while (!this->id)
	{
		this->id = lialg_random_range (&render->random, 0x00000000, 0x7FFFFFFF);
		if (lialg_u32dic_find (render->objects, this->id))
			this->id = 0;
	}

	/* Load the model. */
	if (model != NULL)
		create_mesh (model);

	/* Add to the dictionary. */
	lialg_u32dic_insert (render->models, this->id, this);
}

/**
 * \brief Frees the model.
 */
LIRenModel::~LIRenModel ()
{
	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		object->remove_model (this);
	}

	/* Remove from the model dictionary. */
	lialg_u32dic_remove (render->models, id);
}

/**
 * \brief Replaces a texture.
 * \param name Name of the replaced texture.
 * \param width Width of the new texture.
 * \param height Height of the new texture.
 * \param pixels Pixels in the RGBA format.
 */
void LIRenModel::replace_texture (
	const char* name,
	int         width,
	int         height,
	const void* pixels)
{
	Ogre::TexturePtr texture;

	if (mesh.isNull ())
		return;
	if (!get_loaded ())
		return;

	for (int submesh_idx = 0 ; submesh_idx < mesh->getNumSubMeshes () ; ++submesh_idx)
	{
		// Get the material of the submesh.
		Ogre::SubMesh* submesh = mesh->getSubMesh (submesh_idx);
		const Ogre::String& submeshmatname = submesh->getMaterialName ();
		Ogre::MaterialPtr submeshmat = render->material_manager->getByName (submeshmatname);
		if (submeshmat.isNull ())
			continue;

		// Check if there are replaceable textures.
		if (!render->material_utils->has_overridable_texture (submeshmat, name))
			continue;

		// Create the replacement texture.
		// FIXME: Why does the Ogre::PF_R8G8B8A8 format not work?
		if (texture.isNull ())
		{
			Ogre::Image img;
			img.loadDynamicImage ((Ogre::uchar*) pixels, width, height, 1, Ogre::PF_A8B8G8R8);
			Ogre::String unique_name = render->id.next ();
			texture = render->texture_manager->loadImage (unique_name, LIREN_RESOURCES_TEMPORARY, img);
		}

		// Create the texture aliases.
		Ogre::String tmp(name);
		submesh->addTextureAlias(tmp + ".png", texture->getName ());
		submesh->addTextureAlias(tmp + ".dds", texture->getName ());
		submesh->updateMaterialUsingTextureAliases (); // FIXME: Needed?
	}
}

/**
 * \brief Gets the ID of the model.
 * \return ID.
 */
int LIRenModel::get_id () const
{
	return id;
}

/**
 * \brief Returns nonzero if the model has been fully background loaded.
 * \return Nonzero if background loaded, zero if not.
 */
int LIRenModel::get_loaded () const
{
	if (mesh.isNull ())
		return 0;
	return mesh->isLoaded ();
}

/**
 * \brief Sets the current LIMdlModel.
 * \param model Model.
 */
int LIRenModel::set_model (
	const LIMdlModel* model)
{
	create_mesh (model);

	return 1;
}

/*****************************************************************************/

void LIRenModel::create_mesh (
	const LIMdlModel* model)
{
	/* Create the resource loader. */
	/* Ogre doesn't free the manual resource loader, nor does it allow us to
	   store custom data to meshes. Because of those reasons, we store the
	   loader to a dictionary that is searched by mesh name. The render class
	   will also use the dictionary to garbage collect unused loaders. */
	LIRenMeshBuilder* builder = new LIRenMeshBuilder (render, model);
	Ogre::String name = render->id.next ();
	lialg_strdic_insert (render->mesh_builders, name.c_str (), builder);

	/* Create the manual mesh. */
	Ogre::String group = LIREN_RESOURCES_TEMPORARY;
	mesh = Ogre::MeshManager::getSingleton ().createManual (name, group, builder);

	/* Prevent edge list building. */
	/* Building edge lists is hideously slow. Since we don't use shadow
	   volumes, it's also completely useless for us. */
	mesh->setAutoBuildEdgeLists (false);

	/* Tell objects using the old mesh to rebuild. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		object->model_changed (this);
	}
}

/** @} */
/** @} */
/** @} */
