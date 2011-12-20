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

/**
 * \addtogroup LIRen Render
 * @{
 * \addtogroup LIRenInternal Internal
 * @{
 * \addtogroup LIRenModel Model
 * @{
 */

#include "lipsofsuna/network.h"
#include "render-internal.h"
#include <OgreSubMesh.h>

static void private_create_material (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	int            index,
	Ogre::SubMesh* submesh);

static void private_create_mesh (
	LIRenModel* self,
	LIMdlModel* model);

static Ogre::String private_unique_id (
	LIRenModel* self);

static Ogre::String private_unique_material (
	LIRenModel* self,
	int         index);

/*****************************************************************************/

/**
 * \brief Creates a new model from a loaded model buffer.
 *
 * The previous owner of the model buffer retains the ownership and must
 * ensure that the buffer is not freed before the created renderer model.
 *
 * \param render Renderer.
 * \param model Model description.
 * \param id Unique model ID.
 * \return Model or NULL on failure.
 */
LIRenModel* liren_model_new (
	LIRenRender* render,
	LIMdlModel*  model,
	int          id)
{
	LIRenModel* self;

	/* Allocate self. */
	self = (LIRenModel*) lisys_calloc (1, sizeof (LIRenModel));
	if (self == NULL)
		return 0;
	self->render = render;

	/* Copy the model. */
	if (model != NULL)
	{
		self->model = limdl_model_new_copy (model);
		if (self->model == NULL)
		{
			liren_model_free (self);
			return 0;
		}
	}

	/* Choose a unique ID. */
	while (!id)
	{
		id = lialg_random_range (&render->random, LINET_RANGE_RENDER_START, LINET_RANGE_RENDER_END);
		if (lialg_u32dic_find (render->objects, id))
			id = 0;
	}
	self->id = id;

	/* Initialize the private data. */
	self->data = new LIRenModelData;
	if (self->data == NULL)
	{
		liren_model_free (self);
		return NULL;
	}

	/* Initialize the backend. */
	if (self->model != NULL)
		private_create_mesh (self, self->model);

	/* Add to the dictionary. */
	if (!lialg_u32dic_insert (render->models, id, self))
	{
		liren_model_free (self);
		return 0;
	}

	return self;
}

void liren_model_free (
	LIRenModel* self)
{
	/* Remove from objects. */
	/* Keeping the model alive when it's assigned to objects is the job of scripts.
	   If they don't reference the model, we'll remove it even if it's in use. We
	   prevent crashing by removing it from objects in such a case. */
	/* TODO */

	/* Free the model data. */
	if (self->model != NULL)
		limdl_model_free (self->model);

	/* Free the private data. */
	if (self->data != NULL)
		delete self->data;

	lialg_u32dic_remove (self->render->models, self->id);
	lisys_free (self);
}

int liren_model_set_model (
	LIRenModel* self,
	LIMdlModel* model)
{
	LIMdlModel* copy;

	/* Copy the model. */
	copy = limdl_model_new_copy (model);
	if (copy == NULL)
		return 0;
	if (self->model != NULL)
		limdl_model_free (self->model);
	self->model = copy;

	/* TODO */
/*	if (self->model != NULL)
		private_create_mesh (self, self->model);*/

	return 1;
}

/*****************************************************************************/

static void private_create_material (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	int            index,
	Ogre::SubMesh* submesh)
{
	/* Check for an existing material. */
	if (mat->material != NULL && mat->material[0] != '\0')
	{
		Ogre::String name = Ogre::String (mat->material);
		Ogre::MaterialPtr material = self->render->data->material_manager->getByName (name);
		if (!material.isNull())
		{
			submesh->setMaterialName (name);
			return;
		}
	}

	/* Create a new the material. */
	Ogre::String name = private_unique_material (self, index);
	const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
	Ogre::MaterialPtr material = self->render->data->material_manager->create (name, group);

	/* Set material properties. */
	if (mat->flags & LIMDL_MATERIAL_FLAG_TRANSPARENCY)
		material->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
	if (mat->flags & LIMDL_MATERIAL_FLAG_CULLFACE)
		material->setCullingMode (Ogre::CULL_CLOCKWISE);
	else
		material->setCullingMode (Ogre::CULL_NONE);

	/* Get the first pass. */
	Ogre::Technique* technique = material->getTechnique (0);
	Ogre::Pass* pass = technique->getPass (0);

	/* Set pass properties. */
	pass->setSelfIllumination (mat->emission, mat->emission, mat->emission);
	pass->setShininess (mat->shininess);
	pass->setDiffuse (Ogre::ColourValue (mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->diffuse[3]));
	pass->setSpecular (Ogre::ColourValue (mat->specular[0], mat->specular[1], mat->specular[2], mat->specular[3]));
	pass->setVertexColourTracking (Ogre::TVC_DIFFUSE);

	/* Create texture units. */
	pass->removeAllTextureUnitStates ();
	for (int i = 0 ; i < mat->textures.count && i < 1 ; i++)
	{
		Ogre::String tex = Ogre::String (mat->textures.array[i].string);
		pass->createTextureUnitState (tex + ".dds");
	}

	submesh->setMaterialName (name);
}

static void private_create_mesh (
	LIRenModel* self,
	LIMdlModel* model)
{
	if (!model->vertices.count || !model->lod.count)
		return;
	LIMdlLod* lod = model->lod.array;

	/* Allocate the vertex data. */
	Ogre::VertexData* vertex_data = new Ogre::VertexData ();
	vertex_data->vertexCount = model->vertices.count;

	/* Initialize the vertex format. */
	size_t offset = 0;
	Ogre::VertexDeclaration* format = vertex_data->vertexDeclaration;
	format->addElement (0, offset, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT2);
	format->addElement (0, offset, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (0, offset, Ogre::VET_FLOAT3, Ogre::VES_TANGENT);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	// FIXME: Ogre doesn't support the right format? Change in model?
	//format->addElement (0, offset, Ogre::VET_UBYTE4, Ogre::VES_DIFFUSE);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_UBYTE4);
#ifdef LIMDL_VERTEX_WEIGHT_UINT16
	format->addElement (0, offset, Ogre::VET_SHORT4, Ogre::VES_BLEND_WEIGHTS);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_SHORT4);
#else
	format->addElement (0, offset, Ogre::VET_UBYTE4, Ogre::VES_BLEND_WEIGHTS);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_UBYTE4);
#endif
	format->addElement (0, offset, Ogre::VET_UBYTE4, Ogre::VES_BLEND_INDICES);
	offset += Ogre::VertexElement::getTypeSize (Ogre::VET_UBYTE4);
#if LIMDL_VERTEX_WEIGHTS_MAX != 4
#error LIMDL_VERTEX_WEIGHTS_MAX must currently be exactly 4
#endif

	/* Create the mesh. */
	self->data->mesh = Ogre::MeshManager::getSingleton ().createManual (private_unique_id (self), "General");
	self->data->mesh->sharedVertexData = vertex_data;

	/* Create the vertex buffer. */
	self->data->vertex_buffer = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		offset, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	self->data->vertex_buffer->writeData (0, self->data->vertex_buffer->getSizeInBytes (), model->vertices.array, true);

	/* Bind the vertex buffer. */
	self->data->vertex_buffer_binding = vertex_data->vertexBufferBinding;
	self->data->vertex_buffer_binding->setBinding (0, self->data->vertex_buffer);

	/* Create the index buffer. */
	self->data->index_buffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer (
		Ogre::HardwareIndexBuffer::IT_16BIT, lod->indices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	self->data->index_buffer->writeData (0, self->data->index_buffer->getSizeInBytes (), lod->indices.array, true);

	/* Create submeshes. */
	for (int i = 0 ; i < lod->face_groups.count ; i++)
	{
		Ogre::SubMesh* submesh = self->data->mesh->createSubMesh ();
		private_create_material (self, model->materials.array + i, i, submesh);
		submesh->useSharedVertices = true;
		submesh->indexData->indexBuffer = self->data->index_buffer;
		submesh->indexData->indexStart = lod->face_groups.array[i].start;
		submesh->indexData->indexCount = lod->face_groups.array[i].count;
	}

	/* Set the bounding box. */
	self->data->mesh->_setBounds (Ogre::AxisAlignedBox (
		model->bounds.min.x, model->bounds.min.y, model->bounds.min.z,
		model->bounds.max.x, model->bounds.max.y, model->bounds.max.z));
	self->data->mesh->_setBoundingSphereRadius (
		(model->bounds.max.x - model->bounds.min.x) +
		(model->bounds.max.y - model->bounds.min.y) +
		(model->bounds.max.z - model->bounds.min.z));
 
	/* Mark the mesh as loaded. */
	self->data->mesh->load ();
}

static Ogre::String private_unique_id (
	LIRenModel* self)
{
	return Ogre::StringConverter::toString (self->id);
}

static Ogre::String private_unique_material (
	LIRenModel* self,
	int         index)
{
	return private_unique_id (self) + "." +
		Ogre::StringConverter::toString (index);
}

/** @} */
/** @} */
/** @} */
