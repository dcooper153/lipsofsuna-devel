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

typedef struct _LIRenBuffer LIRenBuffer;
typedef struct _LIRenFramebuffer LIRenFramebuffer;
typedef struct _LIRenImage LIRenImage;
typedef struct _LIRenLight LIRenLight;
typedef struct _LIRenLightData LIRenLightData;
typedef struct _LIRenMessage LIRenMessage;
typedef struct _LIRenObject LIRenObject;
typedef struct _LIRenObjectData LIRenObjectData;
typedef struct _LIRenOverlay LIRenOverlay;
typedef struct _LIRenOverlayData LIRenOverlayData;
typedef struct _LIRenOverlayElement LIRenOverlayElement;
typedef struct _LIRenQueue LIRenQueue;
typedef struct _LIRenModel LIRenModel;
typedef struct _LIRenModelData LIRenModelData;
typedef struct _LIRenRenderData LIRenRenderData;
typedef struct _LIRenShader LIRenShader;

#ifdef __cplusplus
#include <OgreBorderPanelOverlayElement.h>
#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreOverlay.h>
#include <OgreOverlayContainer.h>
#include <OgreOverlayManager.h>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreMaterialManager.h>
#include <OgreTextAreaOverlayElement.h>
#include "render-container-factory.hpp"
#include "render-image-overlay-factory.hpp"
struct _LIRenLightData
{
	Ogre::Light* light;
};
struct _LIRenModelData
{
	Ogre::MeshPtr mesh;
	Ogre::VertexDeclaration vertex_declaration;
	Ogre::VertexData* vertex_data;
	Ogre::HardwareVertexBufferSharedPtr vertex_buffer;
	Ogre::VertexBufferBinding* vertex_buffer_binding;
	Ogre::HardwareIndexBufferSharedPtr index_buffer;
};
struct _LIRenObjectData
{
	Ogre::Entity* entity;
	Ogre::SceneNode* node;
};
struct _LIRenOverlayData
{
	Ogre::Overlay* overlay;
	LIRenContainer* container;
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
	LIRenContainerFactory* container_factory;
	LIRenImageOverlayFactory* image_factory;
};
#endif

#endif
