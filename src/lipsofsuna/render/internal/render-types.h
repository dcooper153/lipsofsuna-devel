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

#ifndef __RENDER_INTERNAL_TYPES_H__
#define __RENDER_INTERNAL_TYPES_H__

#include "lipsofsuna/math.h"
#include "../render-types.h"

#define LIREN_RESOURCES_PERMANENT "permanent"
#define LIREN_RESOURCES_TEMPORARY "temporary"

typedef struct _LIRenImage LIRenImage;
typedef struct _LIRenLight LIRenLight;
typedef struct _LIRenMessage LIRenMessage;
typedef struct _LIRenObject LIRenObject;
typedef struct _LIRenOverlay LIRenOverlay;
typedef struct _LIRenQueue LIRenQueue;
typedef struct _LIRenModel LIRenModel;
typedef struct _LIRenRenderData LIRenRenderData;

#ifdef __cplusplus
#include <OgreBorderPanelOverlayElement.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayManager.h>
#include <OgreParticleSystem.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextAreaOverlayElement.h>
#include "render-container-factory.hpp"
#include "render-image-overlay-factory.hpp"
#include "render-resource-loading-listener.hpp"
struct _LIRenLight
{
	int id;
	int directional;
	LIMatTransform transform;
	LIRenRender* render;
	Ogre::Light* light;
};
struct _LIRenModel
{
	int id;
	LIMdlModel* model;
	LIRenRender* render;
	Ogre::MeshPtr mesh;
	Ogre::VertexDeclaration vertex_declaration;
	Ogre::VertexData* vertex_data;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_0;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_1;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer_2;
	Ogre::VertexBufferBinding* vertex_buffer_binding;
	Ogre::HardwareIndexBufferSharedPtr index_buffer;
};
struct _LIRenObject
{
	int id;
	int shadow_casting;
	LIMatTransform transform;
	LIMdlPose* pose;
	LIRenModel* model;
	LIRenRender* render;
	Ogre::Entity* entity;
	Ogre::ParticleSystem* particles;
	Ogre::SceneNode* node;
};
struct _LIRenOverlay
{
	int id;
	int depth;
	int visible;
	LIMatVector position;
	LIRenOverlay* parent;
	LIRenRender* render;
	LIRenContainer* container;
	Ogre::Overlay* overlay;
	struct
	{
		int count;
		LIRenOverlay** array;
	} overlays;
};
struct _LIRenRenderData
{
	Ogre::Root* root;
	Ogre::Camera* camera;
	Ogre::MaterialManager* material_manager;
	Ogre::MeshManager* mesh_manager;
	Ogre::OverlayManager* overlay_manager;
	Ogre::RenderWindow* render_window;
	Ogre::RenderSystem* render_system;
	Ogre::SceneManager* scene_manager;
	Ogre::SceneNode* scene_root;
	Ogre::TextureManager* texture_manager;
	Ogre::Viewport* viewport;
	Ogre::LogManager* log;
	LIRenContainerFactory* container_factory;
	LIRenImageOverlayFactory* image_factory;
	LIRenResourceLoadingListener* resource_loading_listener;
};
#endif

#endif
