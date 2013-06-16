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

#ifndef __RENDER_INTERNAL_TYPES_HPP__
#define __RENDER_INTERNAL_TYPES_HPP__

#include "lipsofsuna/math.h"
#include "../render-types.h"

#define LIREN_RESOURCES_PERMANENT "permanent"
#define LIREN_RESOURCES_TEMPORARY "temporary"

typedef struct _LIRenImage LIRenImage;
typedef struct _LIRenLight LIRenLight;
typedef struct _LIRenMessage LIRenMessage;
typedef struct _LIRenOverlay LIRenOverlay;
typedef struct _LIRenQueue LIRenQueue;
typedef struct _LIRenRenderData LIRenRenderData;

class LIRenAttachment;
class LIRenMaterialUtils;
class LIRenModel;
class LIRenObject;
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
#include "render-attachment.hpp"
#include "render-container-factory.hpp"
#include "render-image-overlay-factory.hpp"
#include "render-scaled-overlay-factory.hpp"
#include "render-text-overlay-factory.hpp"
#include "render-material-utils.hpp"
#include "render-mesh-builder.hpp"
#include "render-resource-loading-listener.hpp"
#include "render-unique-id.hpp"
struct _LIRenLight
{
	int id;
	int directional;
	LIMatTransform transform;
	LIRenRender* render;
	Ogre::Light* light;
};
struct _LIRenOverlay
{
	int id;
	int depth;
	int visible;
	float alpha;
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
	float unload_timer;
	Ogre::Root* root;
	Ogre::Camera* camera;
	Ogre::MaterialManager* material_manager;
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
	LIRenScaledOverlayFactory* scaled_factory;
	LIRenTextOverlayFactory* text_factory;
	LIRenMaterialUtils* material_utils;
	LIRenResourceLoadingListener* resource_loading_listener;
	LIRenUniqueId id;
	LIAlgStrdic* mesh_builders;
};

#endif
