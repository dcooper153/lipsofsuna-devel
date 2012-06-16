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
 * \addtogroup LIRenMeshBuilder MeshBuilder
 * @{
 */

#include "lipsofsuna/system.h"
#include "render-mesh-builder.hpp"
#include "render.h"
#include "render-object.h"
#include <OgreSubMesh.h>
#include <OgreSkeletonManager.h>

LIRenMeshBuilder::LIRenMeshBuilder (LIRenRender* render, LIMdlModel* model)
{
	step = 0;
	this->render = render;
	this->model = limdl_model_new_copy (model);
	buffer_size_0 = 0;
	buffer_size_1 = 0;
	buffer_size_2 = 0;
	buffer_data_0 = NULL;
	buffer_data_1 = NULL;
	buffer_data_2 = NULL;
	vertex_data = NULL;
	vertex_buffer_binding = NULL;
}

LIRenMeshBuilder::~LIRenMeshBuilder ()
{
	delete[] buffer_data_0;
	delete[] buffer_data_1;
	delete[] buffer_data_2;

	/* Free the model. */
	if (model != NULL)
		limdl_model_free (model);
}

void LIRenMeshBuilder::prepareResource (Ogre::Resource* resource)
{
	materials.clear ();
	if (step < 1)
	{
		step_1_bg ((Ogre::Mesh*) resource);
		step = 1;
	}
}

void LIRenMeshBuilder::loadResource (Ogre::Resource* resource)
{
	materials.clear ();
	if (step < 1)
	{
		step_1_bg ((Ogre::Mesh*) resource);
		step = 1;
	}
	if (step < 2)
	{
		step_2_fg ((Ogre::Mesh*) resource);
		step = 2;
	}
	if (step < 3)
	{
		step_3_fg ((Ogre::Mesh*) resource);
		step = 3;
	}
	step = 0;
	delete[] buffer_data_0;
	delete[] buffer_data_1;
	delete[] buffer_data_2;
	buffer_data_0 = NULL;
	buffer_data_1 = NULL;
	buffer_data_2 = NULL;
	vertex_data = NULL;
	vertex_buffer_binding = NULL;
}

bool LIRenMeshBuilder::is_idle () const
{
	return step == 0;
}

LIMdlModel* LIRenMeshBuilder::get_model () const
{
	return this->model;
}

/**
 * \brief Prepares the mesh.
 *
 * This is called from the background thread. It basically does everything
 * except material initialization, which was done before, and vertex buffer
 * initialization, which must be done in load_mesh() due to thread safe issues.
 *
 * Materials are also loaded here. They are loaded in yet another background
 * loader. The function waits for the background loading of the materials to
 * finish, but since this is done in a background thread, the main thread will
 * never stall.
 *
 * \param mesh Ogre mesh to prepare.
 */
void LIRenMeshBuilder::step_1_bg (Ogre::Mesh* mesh)
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
	vertex_data = OGRE_NEW Ogre::VertexData ();
	vertex_data->vertexCount = model->vertices.count;
	mesh->sharedVertexData = vertex_data;

	/* Initialize the vertex format. */
	/* Positions and normals need to be in their own vertex buffer for software
	   skinning to work. Let's be nice and support the software fallback too. */
	Ogre::VertexDeclaration* format = vertex_data->vertexDeclaration;
	format->addElement (0, buffer_size_0, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	buffer_size_0 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (0, buffer_size_0, Ogre::VET_FLOAT3, Ogre::VES_NORMAL);
	buffer_size_0 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (1, buffer_size_1, Ogre::VET_FLOAT3, Ogre::VES_TANGENT);
	buffer_size_1 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT3);
	format->addElement (1, buffer_size_1, Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES);
	buffer_size_1 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT2);
	format->addElement (1, buffer_size_1, Ogre::VET_COLOUR_ABGR, Ogre::VES_DIFFUSE);
	buffer_size_1 += Ogre::VertexElement::getTypeSize (Ogre::VET_COLOUR_ABGR);
	format->addElement (2, buffer_size_2, Ogre::VET_UBYTE4, Ogre::VES_BLEND_INDICES);
	buffer_size_2 += Ogre::VertexElement::getTypeSize (Ogre::VET_UBYTE4);
	format->addElement (2, buffer_size_2, Ogre::VET_FLOAT4, Ogre::VES_BLEND_WEIGHTS);
	buffer_size_2 += Ogre::VertexElement::getTypeSize (Ogre::VET_FLOAT4);
	lisys_assert (buffer_size_0 == 6 * 4);
	lisys_assert (buffer_size_1 == 6 * 4);
	lisys_assert (buffer_size_2 == 5 * 4);

	/* Create the bone index to blend index mapping. */
	/* The indices are always the same but Ogre needs the map regardless. */
	mesh->sharedBlendIndexToBoneIndexMap.resize (model->weight_groups.count + 1);
	for (int i = 0 ; i < model->weight_groups.count + 1 ; i++)
		mesh->sharedBlendIndexToBoneIndexMap[i] = i;

	/* Prepare the first vertex buffer. */
	/* Contains positions and normals only. */
	int j = 0;
	buffer_data_0 = new float[6 * model->vertices.count];
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		buffer_data_0[j++] = v->coord.x;
		buffer_data_0[j++] = v->coord.y;
		buffer_data_0[j++] = v->coord.z;
		buffer_data_0[j++] = v->normal.x;
		buffer_data_0[j++] = v->normal.y;
		buffer_data_0[j++] = v->normal.z;
	}
	lisys_assert (j == 6 * model->vertices.count);

	/* Create the second vertex buffer. */
	/* Contains the rest apart from blending information. */
	j = 0;
	buffer_data_1 = new float[6 * model->vertices.count];
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		buffer_data_1[j++] = v->tangent.x;
		buffer_data_1[j++] = v->tangent.y;
		buffer_data_1[j++] = v->tangent.z;
		buffer_data_1[j++] = v->texcoord[0];
		buffer_data_1[j++] = v->texcoord[1];
		uint8_t* color = (uint8_t*)(buffer_data_1 + j++);
		color[0] = v->color[0];
		color[1] = v->color[1];
		color[2] = v->color[2];
		color[3] = v->color[3];
	}
	lisys_assert (j == 6 * model->vertices.count);

	/* Create the third vertex buffer. */
	/* Contains blending indices and weights. */
	j = 0;
	buffer_data_2 = new float[5 * model->vertices.count];
	for (int i = 0 ; i < model->vertices.count ; i++)
	{
		LIMdlVertex* v = model->vertices.array + i;
		memcpy (buffer_data_2 + j, v->bones, LIMDL_VERTEX_WEIGHTS_MAX * sizeof (uint8_t));
		j++;
		memcpy (buffer_data_2 + j, v->weights, LIMDL_VERTEX_WEIGHTS_MAX * sizeof (float));
		j += LIMDL_VERTEX_WEIGHTS_MAX;
	}
	lisys_assert (j == 5 * model->vertices.count);

	/* Create submeshes. */
	/* The index buffer cannot be prepared yet since it doesn't exist. */
	for (int i = 0 ; i < lod->face_groups.count ; i++)
	{
		Ogre::SubMesh* submesh = mesh->createSubMesh ();
		submesh->useSharedVertices = true;
		submesh->indexData->indexStart = lod->face_groups.array[i].start;
		submesh->indexData->indexCount = lod->face_groups.array[i].count;
	}

	/* Set the bounding box. */
	mesh->_setBounds (Ogre::AxisAlignedBox (
		model->bounds.min.x, model->bounds.min.y, model->bounds.min.z,
		model->bounds.max.x, model->bounds.max.y, model->bounds.max.z));
	mesh->_setBoundingSphereRadius (
		(model->bounds.max.x - model->bounds.min.x) +
		(model->bounds.max.y - model->bounds.min.y) +
		(model->bounds.max.z - model->bounds.min.z));
}

/**
 * \brief Creates the materials.
 */
void LIRenMeshBuilder::step_2_fg (Ogre::Mesh* mesh)
{
	if (!model->vertices.count || !model->lod.count)
		return;

	for (int i = 0 ; i < model->materials.count ; i++)
	{
		/* Create the material. */
		Ogre::MaterialPtr material = create_material (model->materials.array + i);

		/* Set the material name of the submesh. */
		Ogre::SubMesh* submesh = mesh->getSubMesh (i);
		submesh->setMaterialName (material->getName ());

		/* Reference the material. */
		/* Ogre meshes don't reference the material, instead only storing
		   its name. Since the mesh can have temporary materials that would
		   be subject to garbage collection without a reference, we need to
		   add keep one here. */
		materials.push_back (material);
	}
}

/**
 * \brief Creates the mesh.
 *
 * This is called from the main thread. It creates the vertex buffers and
 * initializes the skeleton.
 *
 * \param mesh Ogre mesh to finish.
 */
void LIRenMeshBuilder::step_3_fg (Ogre::Mesh* mesh)
{
	if (!model->vertices.count || !model->lod.count)
		return;
	LIMdlLod* lod = model->lod.array;

	/* Create the first vertex buffer. */
	/* Contains positions and normals only. */
	vertex_buffer_0 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		buffer_size_0, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	vertex_buffer_0->writeData (0, vertex_buffer_0->getSizeInBytes (), buffer_data_0, true);

	/* Create the second vertex buffer. */
	/* Contains the rest apart from blending information. */
	vertex_buffer_1 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		buffer_size_1, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	vertex_buffer_1->writeData (0, vertex_buffer_1->getSizeInBytes (), buffer_data_1, true);

	/* Create the third vertex buffer. */
	/* Contains blending indices and weights. */
	vertex_buffer_2 = Ogre::HardwareBufferManager::getSingleton ().createVertexBuffer (
		buffer_size_2, model->vertices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY, true);
	vertex_buffer_2->writeData (0, vertex_buffer_2->getSizeInBytes (), buffer_data_2, true);

	/* Bind the vertex buffers. */
	vertex_buffer_binding = vertex_data->vertexBufferBinding;
	vertex_buffer_binding->setBinding (0, vertex_buffer_0);
	vertex_buffer_binding->setBinding (1, vertex_buffer_1);
	vertex_buffer_binding->setBinding (2, vertex_buffer_2);

	/* Create the index buffer. */
	index_buffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer (
		Ogre::HardwareIndexBuffer::IT_16BIT, lod->indices.count, Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);
	index_buffer->writeData (0, index_buffer->getSizeInBytes (), lod->indices.array, true);

	/* Complete submeshes. */
	/* The index buffer needs to be setup here since prepare couldn't do it. */
	for (int i = 0 ; i < lod->face_groups.count ; i++)
	{
		Ogre::SubMesh* submesh = mesh->getSubMesh (i);
		submesh->indexData->indexBuffer = index_buffer;
	}

	/* Create a skeleton if needed. */
	if (model->weight_groups.count)
	{
		if (!create_skeleton (mesh))
		{
			/* If creating the skeleton failed due to too many bones, disable
			   the mesh completely to avoid exploding vertices. */
			for (int i = mesh->getNumSubMeshes () - 1 ; i >= 0 ; i--)
				mesh->destroySubMesh (i);
		}
	}
}

/**
 * \brief Creates a material.
 * \param mat Model material.
 */
Ogre::MaterialPtr LIRenMeshBuilder::create_material (
	LIMdlMaterial* mat)
{
	bool existing = false;
	bool override = true;
	Ogre::MaterialPtr material;
	Ogre::String unique_name = render->data->id.next ();

	/* Try to load an existing material. */
	if (mat->material != NULL && mat->material[0] != '\0')
	{
		Ogre::String name = Ogre::String (mat->material);
		material = render->data->material_manager->getByName (name);
		if (!material.isNull())
			existing = true;
	}

	/* Try to default to diff1 if no material found yet. */
	/* Although the model could specify almost all the important properties,
	   we strongly prefer to use a base material. This can be avoided by not
	   specifying the `diff1' shader, though there should be no reason for that. */
	if (!existing)
	{
		material = render->data->material_manager->getByName ("diff1");
		if (!material.isNull())
			existing = true;
	}

	/* Create a new material if an existing one was not found. */
	if (!existing)
	{
		Ogre::String group = LIREN_RESOURCES_TEMPORARY;
		material = render->data->material_manager->create (unique_name, group);
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
		if (check_material_override (material))
			material = material->clone (unique_name, true, LIREN_RESOURCES_TEMPORARY);
		else
			override = false;
	}

	/* Override the fields of techniques. */
	if (override)
	{
		for (int i = 0 ; i < material->getNumTechniques () ; i++)
		{
			Ogre::Technique* technique = material->getTechnique (i);
			if (existing)
			{
				for (int j = 0 ; j < technique->getNumPasses () ; j++)
				{
					Ogre::Pass* pass = technique->getPass (j);
					override_pass (mat, pass);
				}
			}
			else
			{
				Ogre::Pass* pass = technique->getPass (0);
				initialize_pass (mat, pass);
			}
		}
	}

	return material;
}

bool LIRenMeshBuilder::create_skeleton (Ogre::Mesh* mesh)
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
		return false;
	}

	/* Create the skeleton. */
	Ogre::String name (render->data->id.next ());
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
	mesh->_notifySkeleton (skeleton);

	return true;
}

bool LIRenMeshBuilder::check_material_override (
	Ogre::MaterialPtr& material)
{
	for (int k = 0 ; k < material->getNumTechniques () ; k++)
	{
		Ogre::Technique* tech = material->getTechnique (k);
		for (int i = 0 ; i < tech->getNumPasses () ; i++)
		{
			/* Check if the pass needs to be overridden. */
			Ogre::Pass* pass = tech->getPass (i);
			if (check_name_override (pass->getName ()))
				return true;

			/* Check if any textures need to be overridden. */
			for (int i = 0 ; i < pass->getNumTextureUnitStates () ; i++)
			{
				Ogre::TextureUnitState* state = pass->getTextureUnitState (i);
				if (check_name_override (state->getName ()))
					return true;
			}
		}
	}

	return false;
}

bool LIRenMeshBuilder::check_name_override (
	const Ogre::String& name)
{
	if (name.size () < 3)
		return 0;
	if (name[0] != 'L' || name[1] != 'O' || name[2] != 'S')
		return 0;
	return 1;
}

void LIRenMeshBuilder::initialize_pass (
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

void LIRenMeshBuilder::override_pass (
	LIMdlMaterial* mat,
	Ogre::Pass*    pass)
{
	/* Set pass properties. */
	/* If this is a newly created material or the name of the first pass
	   starts with the string "LOS", we override some of the parameters. */
	if (check_name_override (pass->getName ()))
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
		if (check_name_override (state->getName ()))
		{
			/* Check if a PNG version is available. */
			Ogre::String texname = Ogre::String (mat->textures.array[j].string);
			Ogre::String pngname = texname + ".png";
			const char* path = lipth_paths_find_file (render->paths, pngname.c_str ());
			int gotpng;
			if (path != NULL)
				gotpng = lisys_filesystem_access (path, LISYS_ACCESS_READ);
			else
				gotpng = 0;

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

/** @} */
/** @} */
/** @} */
