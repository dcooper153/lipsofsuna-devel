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

static bool private_check_material_override (
	LIRenModel*        self,
	Ogre::MaterialPtr& material);

static bool private_check_name_override (
	const Ogre::String& name);

static void private_initialize_pass (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	Ogre::Pass*    pass);

static void private_override_pass (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	Ogre::Pass*    pass);

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
	bool existing = false;
	Ogre::MaterialPtr material;
	Ogre::String unique_name = private_unique_material (self, index);

	/* Try to load an existing material. */
	if (mat->material != NULL && mat->material[0] != '\0')
	{
		Ogre::String name = Ogre::String (mat->material);
		material = self->render->data->material_manager->getByName (name);
		if (!material.isNull())
			existing = true;
	}

	/* Try to default to diff1 if no material found yet. */
	/* Although the model could specify almost all the important properties,
	   we strongly prefer to use a base material. This can be avoided by not
	   specifying the `diff1' shader, though there should be no reason for that. */
	if (!existing)
	{
		material = self->render->data->material_manager->getByName ("diff1");
		if (!material.isNull())
			existing = true;
	}

	/* Create a new material if an existing one was not found. */
	if (!existing)
	{
		Ogre::String group = LIREN_RESOURCES_TEMPORARY;
		material = self->render->data->material_manager->create (unique_name, group);
		if (mat->flags & LIMDL_MATERIAL_FLAG_TRANSPARENCY)
			material->setSceneBlending (Ogre::SBT_TRANSPARENT_ALPHA);
		if (mat->flags & LIMDL_MATERIAL_FLAG_CULLFACE)
			material->setCullingMode (Ogre::CULL_CLOCKWISE);
		else
			material->setCullingMode (Ogre::CULL_NONE);
	}

	/* Instantiate the material if it needs to be overridden. */
	/* The original material is in the group of permanent resources but
	   the instantiated material needs to be put into the temporary group
	   so that it can be removed when the model is garbage collected. */
	if (existing)
	{
		if (!private_check_material_override (self, material))
			return;
		material = material->clone (unique_name, true, LIREN_RESOURCES_TEMPORARY);
	}

	/* Override the fields of techniques. */
	for (int i = 0 ; i < material->getNumTechniques () ; i++)
	{
		Ogre::Technique* technique = material->getTechnique (i);
		Ogre::Pass* pass = technique->getPass (0);
		if (existing)
			private_override_pass (self, mat, pass);
		else
			private_initialize_pass (self, mat, pass);
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

	/* Make sure that the mesh is valid. */
#ifndef _NDEBUG
	lisys_assert (lod->indices.count % 3 == 0);
	for (int i = 0 ; i < lod->indices.count ; i++)
		lisys_assert (lod->indices.array[i] < model->vertices.count);
#endif

	/* Allocate the vertex data. */
	Ogre::VertexData* vertex_data = new Ogre::VertexData ();
	vertex_data->vertexCount = model->vertices.count;

	/* Initialize the vertex format. */
	/* Positions and normals need to be in their own vertex buffer for software
	   skinning to work. Let's be nice and support the software fallback too. */
	size_t offset0 = 0;
	size_t offset1 = 0;
	size_t offset2 = 0;
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
	format->addElement (2, offset2, Ogre::VET_UBYTE4, Ogre::VES_BLEND_INDICES);
	offset2 += Ogre::VertexElement::getTypeSize (Ogre::VET_UBYTE4);
	format->addElement (2, offset2, Ogre::VET_FLOAT4, Ogre::VES_BLEND_WEIGHTS);
	offset2 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT4);
	lisys_assert (offset0 == 6 * 4);
	lisys_assert (offset1 == 6 * 4);
	lisys_assert (offset2 == 5 * 4);

	/* Create the mesh. */
	self->data->mesh = Ogre::MeshManager::getSingleton ().createManual (private_unique_id (self), LIREN_RESOURCES_TEMPORARY);
	self->data->mesh->sharedVertexData = vertex_data;

	/* Create the bone index to blend index mapping. */
	/* The indices are always the same but Ogre needs the map regardless. */
	self->data->mesh->sharedBlendIndexToBoneIndexMap.resize (model->weight_groups.count + 1);
	for (int i = 0 ; i < model->weight_groups.count + 1 ; i++)
		self->data->mesh->sharedBlendIndexToBoneIndexMap[i] = i;

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
	/* Contains the rest apart from blending information. */
	j = 0;
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		buffer[j++] = v->tangent.x;
		buffer[j++] = v->tangent.y;
		buffer[j++] = v->tangent.z;
		buffer[j++] = v->texcoord[0];
		buffer[j++] = v->texcoord[1];
		uint8_t* color = (uint8_t*)(buffer + j++);
		color[0] = v->color[0];
		color[1] = v->color[1];
		color[2] = v->color[2];
		color[3] = v->color[3];
	}
	lisys_assert (j == 6 * model->vertices.count);
	self->data->vertex_buffer_1 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		offset1, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	self->data->vertex_buffer_1->writeData (0, self->data->vertex_buffer_1->getSizeInBytes (), buffer, true);

	/* Create the third vertex buffer. */
	/* Contains blending indices. */
	j = 0;
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		uint8_t* indices = (uint8_t*)(buffer + j++);
		for (int k = 0 ; k < LIMDL_VERTEX_WEIGHTS_MAX ; k++)
			indices[k] = v->bones[k];
		for (int k = 0 ; k < LIMDL_VERTEX_WEIGHTS_MAX ; k++)
		{
#ifdef LIMDL_VERTEX_WEIGHT_UINT16
			buffer[j++] = v->weights[k] / 65535.0f;
#else
			buffer[j++] = v->weights[k] / 255.0f;
#endif
		}
	}
	lisys_assert (j == 5 * model->vertices.count);
	self->data->vertex_buffer_2 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		offset2, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
	self->data->vertex_buffer_2->writeData (0, self->data->vertex_buffer_2->getSizeInBytes (), buffer, true);
	delete[] buffer;

	/* Bind the vertex buffers. */
	self->data->vertex_buffer_binding = vertex_data->vertexBufferBinding;
	self->data->vertex_buffer_binding->setBinding (0, self->data->vertex_buffer_0);
	self->data->vertex_buffer_binding->setBinding (1, self->data->vertex_buffer_1);
	self->data->vertex_buffer_binding->setBinding (2, self->data->vertex_buffer_2);

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

	/* Make sure that there aren't too many bones. */
	/* Ogre seems to occasionally fail an assertion when it tries to
	   upload bone matrices to shaders. This check seems to have eliminated
	   the issue so it probably can't handle too many matrices. */
	if (model->weight_groups.count > 32)
	{
		printf ("WARNING: too many weighted bones: %d/%d!\n", model->weight_groups.count, 32);
		return;
	}

	/* Create the skeleton. */
	Ogre::String name (private_unique_id (self));
	Ogre::ResourcePtr resource = Ogre::SkeletonManager::getSingleton ().create (name, LIREN_RESOURCES_TEMPORARY, true);
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
}

static bool private_check_material_override (
	LIRenModel*        self,
	Ogre::MaterialPtr& material)
{
	for (int k = 0 ; k < material->getNumTechniques () ; k++)
	{
		/* Check if the pass needs to be overridden. */
		Ogre::Technique* tech = material->getTechnique (k);
		Ogre::Pass* pass = tech->getPass (0);
		if (private_check_name_override (pass->getName ()))
			return true;

		/* Check if any textures need to be overridden. */
		for (int i = 0 ; i < pass->getNumTextureUnitStates () ; i++)
		{
			Ogre::TextureUnitState* state = pass->getTextureUnitState (i);
			if (private_check_name_override (state->getName ()))
				return true;
		}
	}

	return false;
}

static bool private_check_name_override (
	const Ogre::String& name)
{
	if (name.size () < 3)
		return 0;
	if (name[0] != 'L' || name[1] != 'O' || name[2] != 'S')
		return 0;
	return 1;
}

static void private_initialize_pass (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	Ogre::Pass*    pass)
{
	/* Initialize pass properties. */
	pass->setSelfIllumination (mat->emission, mat->emission, mat->emission);
	pass->setShininess (mat->shininess);
	pass->setDiffuse (Ogre::ColourValue (mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->diffuse[3]));
	pass->setSpecular (Ogre::ColourValue (mat->specular[0], mat->specular[1], mat->specular[2], mat->specular[3]));
	pass->setVertexColourTracking (Ogre::TVC_DIFFUSE);

	/* Initialize texture units. */
	for (int i = 0 ; i < mat->textures.count && i < 1 ; i++)
	{
		Ogre::String tex = Ogre::String (mat->textures.array[i].string);
		pass->createTextureUnitState (tex + ".dds");
	}
}

static void private_override_pass (
	LIRenModel*    self,
	LIMdlMaterial* mat,
	Ogre::Pass*    pass)
{
	/* Set pass properties. */
	/* If this is a newly created material or the name of the first pass
	   starts with the string "LOS", we override some of the parameters. */
	if (private_check_name_override (pass->getName ()))
	{
		pass->setSelfIllumination (mat->emission, mat->emission, mat->emission);
		pass->setShininess (mat->shininess);
		Ogre::TrackVertexColourType track = pass->getVertexColourTracking ();
		if (track != Ogre::TVC_DIFFUSE)
			pass->setDiffuse (Ogre::ColourValue (mat->diffuse[0], mat->diffuse[1], mat->diffuse[2], mat->diffuse[3]));
		if (track != Ogre::TVC_SPECULAR)
			pass->setSpecular (Ogre::ColourValue (mat->specular[0], mat->specular[1], mat->specular[2], mat->specular[3]));
	}

	/* Override texture units. */
	/* Texture units whose names start with "LOS" are considered overridable. */
	int j = 0;
	for (int i = 0 ; i < pass->getNumTextureUnitStates () ; i++)
	{
		if (j >= mat->textures.count)
			break;
		Ogre::TextureUnitState* state = pass->getTextureUnitState (i);
		if (private_check_name_override (state->getName ()))
		{
			/* Check if a PNG version is available. */
			Ogre::String texname = Ogre::String (mat->textures.array[j].string);
			Ogre::String pngname = texname + ".png";
			const char* path = lipth_paths_find_file (self->render->paths, pngname.c_str ());
			int gotpng = lisys_filesystem_access (path, LISYS_ACCESS_READ);

			/* Use either a PNG or a DDS file. */
			/* PNG is favored over DDS so that artists don't need to bother
			   with converting their textures when testing them. */
			if (gotpng)
				state->setTextureName (pngname);
			else
				state->setTextureName (texname + ".dds");
			j++;
		}
	}
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
