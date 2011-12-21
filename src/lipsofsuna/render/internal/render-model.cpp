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
#include <OgreSkeletonManager.h>

static void private_create_material (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	int            index,
	Ogre::SubMesh* submesh);

static void private_create_mesh (
	LIRenModel* self,
	LIMdlModel* model);

static void private_create_skeleton (
	LIRenModel* self,
	LIMdlModel* model);

static bool private_check_override (
	const Ogre::String& name);

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
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		if (object->model == self)
			liren_object_set_model (object, NULL);
	}

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

	/* Create a new mesh. */
	private_create_mesh (self, self->model);

	/* Tell objects to update their model bindings. */
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->render->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		if (object->model == self)
			liren_object_model_changed (object);
	}

	return 1;
}

/*****************************************************************************/

static void private_create_material (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	int            index,
	Ogre::SubMesh* submesh)
{
	bool newmat = true;
	bool overridden = false;
	Ogre::MaterialPtr material;
	Ogre::String unique_name = private_unique_material (self, index);

	/* Load or create a material. */
	/* If the model specifies the name of the material to be used, we use
	   an Ogre material. Some properties of loaded materials may be subject
	   to overriding. If no material is loaded, a new one is created. */
	if (mat->material != NULL && mat->material[0] != '\0')
	{
		Ogre::String name = Ogre::String (mat->material);
		material = self->render->data->material_manager->getByName (name);
		if (!material.isNull())
			newmat = false;
	}
	if (newmat)
	{
		const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
		material = self->render->data->material_manager->create (unique_name, group);
	}

	/* Set material properties. */
	/* Loaded materials get them from the material script so only newly
	   created materials need to use the properties from the model file. */
	if (newmat)
	{
		if (mat->flags & LIMDL_MATERIAL_FLAG_TRANSPARENCY)
			material->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
		if (mat->flags & LIMDL_MATERIAL_FLAG_CULLFACE)
			material->setCullingMode (Ogre::CULL_CLOCKWISE);
		else
			material->setCullingMode (Ogre::CULL_NONE);
	}

	/* Get the first pass. */
	Ogre::Technique* technique = material->getTechnique (0);
	Ogre::Pass* pass = technique->getPass (0);

	/* Set pass properties. */
	/* If this is a newly created material or the name of the first pass
	   starts with the string "LOS", we override some of the parameters. */
	if (newmat || private_check_override (pass->getName ()))
	{
		if (!newmat && !overridden)
		{
			material = material->clone (unique_name);
			technique = material->getTechnique (0);
			pass = technique->getPass (0);
			overridden = true;
		}
		pass->setSelfIllumination (mat->emission, mat->emission, mat->emission);
		pass->setShininess (mat->shininess);
		pass->setDiffuse (Ogre::ColourValue (mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->diffuse[3]));
		pass->setSpecular (Ogre::ColourValue (mat->specular[0], mat->specular[1], mat->specular[2], mat->specular[3]));
		pass->setVertexColourTracking (Ogre::TVC_DIFFUSE);
	}

	/* Setup texture units. */
	/* If this is a new material, recreate the texture units from scratch.
	   Otherwise, override texture units whose names start with "LOS". */
	if (newmat)
	{
		pass->removeAllTextureUnitStates ();
		for (int i = 0 ; i < mat->textures.count && i < 1 ; i++)
		{
			Ogre::String tex = Ogre::String (mat->textures.array[i].string);
			pass->createTextureUnitState (tex + ".dds");
		}
	}
	else
	{
		int j = 0;
		for (int i = 0 ; i < pass->getNumTextureUnitStates () ; i++)
		{
			if (j >= mat->textures.count)
				break;
			Ogre::TextureUnitState* state = pass->getTextureUnitState (i);
			if (private_check_override (state->getName ()))
			{
				if (!overridden)
				{
					material = material->clone (unique_name);
					technique = material->getTechnique (0);
					pass = technique->getPass (0);
					state = pass->getTextureUnitState (i);
					overridden = true;
				}
				Ogre::String tex = Ogre::String (mat->textures.array[j].string);
				state->setTextureName (tex + ".dds");
				j++;
			}
		}
	}

	/* Assign the material to the submesh. */
	submesh->setMaterialName (material->getName ());
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
	/* Positions and normals need to be in their own vertex buffer for software
	   skinning to work. Let's be nice and support the software fallback too. */
	size_t offset0 = 0;
	size_t offset1 = 0;
	Ogre::VertexDeclaration* format = vertex_data->vertexDeclaration;
	format->addElement (0, offset0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	offset0 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (0, offset0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	offset0 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (1, offset1, Ogre::VET_FLOAT3, Ogre::VES_TANGENT);
	offset1 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (1, offset1, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	offset1 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT2);
	format->addElement (1, offset1, Ogre::VET_COLOUR_ABGR, Ogre::VES_DIFFUSE);
	offset1 += Ogre::VertexElement::getTypeSize (Ogre::VET_COLOUR_ABGR);
	lisys_assert (offset0 == 6 * 4);
	lisys_assert (offset1 == 6 * 4);

	/* Create the mesh. */
	self->data->mesh = Ogre::MeshManager::getSingleton ().createManual (private_unique_id (self), "General");
	self->data->mesh->sharedVertexData = vertex_data;

	/* Allocate a temporary buffer for vertex data. */
	/* Unfortunately the vertex data from the model can't be used as is for a
	   multitude of reasons. We need to rearrange the data with this buffer. */
	float* buffer = new float[6 * model->vertices.count];

	/* Create the first vertex buffer. */
	/* Contains positions and normals only. */
	int j = 0;
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		buffer[j++] = v->coord.x;
		buffer[j++] = v->coord.y;
		buffer[j++] = v->coord.z;
		buffer[j++] = v->normal.x;
		buffer[j++] = v->normal.y;
		buffer[j++] = v->normal.z;
	}
	lisys_assert (j == 6 * model->vertices.count);
	self->data->vertex_buffer_0 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		offset0, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	self->data->vertex_buffer_0->writeData (0, self->data->vertex_buffer_0->getSizeInBytes (), buffer, true);

	/* Create the second vertex buffer. */
	/* Contains everything else. */
	j = 0;
	buffer = new float[6 * model->vertices.count];
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		buffer[j++] = v->tangent.x;
		buffer[j++] = v->tangent.y;
		buffer[j++] = v->tangent.z;
		buffer[j++] = v->texcoord[0];
		buffer[j++] = v->texcoord[1];
		uint8_t* color = (uint8_t*)(buffer + j++);
		color[0] = v->color[3];
		color[1] = v->color[2];
		color[2] = v->color[1];
		color[3] = v->color[0];
	}
	lisys_assert (j == 6 * model->vertices.count);
	self->data->vertex_buffer_1 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		offset1, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	self->data->vertex_buffer_1->writeData (0, self->data->vertex_buffer_1->getSizeInBytes (), buffer, true);
	delete[] buffer;

	/* Bind the vertex buffers. */
	self->data->vertex_buffer_binding = vertex_data->vertexBufferBinding;
	self->data->vertex_buffer_binding->setBinding (0, self->data->vertex_buffer_0);
	self->data->vertex_buffer_binding->setBinding (1, self->data->vertex_buffer_1);

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

	/* Create a skeleton if needed. */
	if (model->weight_groups.count)
		private_create_skeleton (self, model);
}

static void private_create_skeleton (
	LIRenModel* self,
	LIMdlModel* model)
{
	Ogre::Bone* bone;
	LIMdlWeightGroup* group;

	/* Create the skeleton. */
	Ogre::String name (private_unique_id (self));
	Ogre::ResourcePtr resource = Ogre::SkeletonManager::getSingleton ().create (name, "General", true);
	Ogre::SkeletonPtr skeleton (resource);

	/* Create the dummy bone. */
	bone = skeleton->createBone (0);
	bone->setManuallyControlled (true);

	/* Create weight group bones. */
	/* We disregard the hierarchy and just create a bone for each weight group.
	   This ensures that the minimal number of bones is uploaded to the GPU. */
	for (int i = 0 ; i < model->weight_groups.count ; i++)
	{
		group = model->weight_groups.array + i;
		if (group->node != NULL)
		{
			LIMatTransform t = group->node->rest_transform.global;
			bone = skeleton->createBone (i + 1);
			bone->setPosition (t.position.x, t.position.y, t.position.z);
			bone->setOrientation (t.rotation.w, t.rotation.x, t.rotation.y, t.rotation.z);
		}
		else
			bone = skeleton->createBone (i + 1);
		bone->setManuallyControlled (true);
		bone->setInitialState ();
	}

	/* Set the binding pose. */
	skeleton->setBindingPose ();
	skeleton->load ();

	/* Assign the skeleton to the mesh. */
	self->data->mesh->_notifySkeleton (skeleton);

	/* Compile vertex weights. */
	/* FIXME: The model format already has sane weight assignments but
	   Ogre seems to throw an exception we try to use them directly. */
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* vertex = model->vertices.array + i;
		Ogre::VertexBoneAssignment assignment;
		assignment.vertexIndex = i;
		for (int j = 0; j < LIMDL_VERTEX_WEIGHTS_MAX ; j++)
		{
			assignment.boneIndex = vertex->bones[j];
#ifdef LIMDL_VERTEX_WEIGHT_UINT16
			assignment.weight = vertex->weights[j] / 65535.0f;
#else
			assignment.weight = vertex->weights[j] / 255.0f;
#endif
			self->data->mesh->addBoneAssignment (assignment);
		}
	}
}

static bool private_check_override (
	const Ogre::String& name)
{
	if (name.size () < 3)
		return 0;
	if (name[0] != 'L' || name[1] != 'O' || name[2] != 'S')
		return 0;
	return 1;
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
