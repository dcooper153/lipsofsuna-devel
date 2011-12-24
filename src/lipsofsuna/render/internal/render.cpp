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
 * \addtogroup LIRenRender Render
 * @{
 */

#include "render-internal.h"
#include "render-container-factory.hpp"
#include <OgreLogManager.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgrePlugin.h>
#include <OgreFontManager.h>
#include <OgreBorderPanelOverlayElement.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
#include <X11/Xlib.h>
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <windows.h>
#endif

#define LIREN_RENDER_TEXTURE_UNLOAD_TIME 10

static void private_load_plugin (
	LIRenRender* self,
	const char*  name);

static int private_check_plugin (
	LIRenRender* self,
	const char*  name);

static void private_update_mode (
	LIRenRender*  self);

/*****************************************************************************/

int liren_internal_init (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	Ogre::Real w;
	Ogre::Real h;

	/* Initialize the private data. */
	self->data = new LIRenRenderData;
	if (self->data == NULL)
		return 0;

	/* Disable console output. */
	Ogre::LogManager* log = new Ogre::LogManager ();
	log->createLog ("render.log", true, false, false);

	/* Initialize the Ogre root. */
	self->data->root = new Ogre::Root("", "", "");

	/* Load plugins. */
	private_load_plugin (self, "RenderSystem_GL");
	private_load_plugin (self, "Plugin_OctreeSceneManager");
	private_load_plugin (self, "Plugin_ParticleFX");

	/* Make sure that the required plugins were loaded. */
	if (!private_check_plugin (self, "GL RenderSystem") ||
		!private_check_plugin (self, "Octree & Terrain Scene Manager"))
		return 0;

	/* Setup resource paths. */
	Ogre::String data0(self->paths->override_data);
	Ogre::String data1(self->paths->module_data);
	const Ogre::String& group = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0, "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/fonts", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/graphics", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/materials", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/meshes", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/shaders", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data0 + "/textures", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/fonts", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/graphics", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/materials", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/meshes", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/shaders", "FileSystem", group, true);
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation (data1 + "/textures", "FileSystem", group, true);

	/* Initialize the render system. */
	self->data->render_system = self->data->root->getRenderSystemByName ("OpenGL Rendering Subsystem");
	if (!(self->data->render_system->getName () == "OpenGL Rendering Subsystem"))
		return 0;

	/* Choose the video mode. */
	Ogre::String video_mode =
		Ogre::StringConverter::toString (mode->width) + " x " +
		Ogre::StringConverter::toString (mode->height);
	self->data->render_system->setConfigOption ("Full Screen", mode->fullscreen? "Yes" : "No");
	self->data->render_system->setConfigOption ("VSync", mode->sync? "Yes" : "No");
	self->data->render_system->setConfigOption ("Video Mode", video_mode);

	/* Initialize the render window. */
	self->data->root->setRenderSystem (self->data->render_system);
	self->data->render_window = self->data->root->initialise (true, "Lips of Suna");
	self->mode = *mode;
	private_update_mode (self);

	/* Initialize the scene manager. */
	self->data->scene_manager = self->data->root->createSceneManager("OctreeSceneManager", "DefaultSceneManager");
	self->data->scene_manager->setAmbientLight (Ogre::ColourValue (0.5, 0.5, 0.5));
	self->data->scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_MODULATIVE_INTEGRATED);
	self->data->scene_manager->setShadowTextureSelfShadow (false);
	self->data->scene_manager->setShowDebugShadows (true);
	self->data->scene_manager->setShadowFarDistance (100.0f);
	self->data->scene_manager->setShadowCameraSetup (Ogre::ShadowCameraSetupPtr (new Ogre::DefaultShadowCameraSetup ()));
	self->data->scene_manager->setShadowTextureCount (8);
	self->data->scene_manager->setShadowTextureSize (512);
	self->data->scene_root = self->data->scene_manager->getRootSceneNode ();

	/* Initialize resources. */
	self->data->texture_manager = &Ogre::TextureManager::getSingleton ();
	self->data->texture_manager->setDefaultNumMipmaps (5);
	self->data->material_manager = &Ogre::MaterialManager::getSingleton ();
	self->data->mesh_manager = &Ogre::MeshManager::getSingleton ();
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups ();

	/* Initialize the camera. */
	self->data->camera = self->data->scene_manager->createCamera ("Camera");
	self->data->camera->setNearClipDistance (1);
	self->data->camera->setFarClipDistance (75);
	self->data->viewport = self->data->render_window->addViewport (self->data->camera);
	self->data->viewport->setBackgroundColour (Ogre::ColourValue (0.0f, 0.0f, 0.0f));
	w = Ogre::Real (self->data->viewport->getActualWidth ());
	h = Ogre::Real (self->data->viewport->getActualHeight ());
	self->data->camera->setAspectRatio (w / h);

	/* Initialize the user interface. */
	self->data->overlay_manager = &(Ogre::OverlayManager::getSingleton ());
	self->data->container_factory = new LIRenContainerFactory;
	self->data->overlay_manager->addOverlayElementFactory (self->data->container_factory);
	self->data->image_factory = new LIRenImageOverlayFactory;
	self->data->overlay_manager->addOverlayElementFactory (self->data->image_factory);

	return 1;
}

void liren_internal_deinit (
	LIRenRender* self)
{
	if (self->data != NULL)
	{
		delete self->data->root;
		delete self->data->container_factory;
		delete self->data->image_factory;
		delete self->data;
		self->data = NULL;
	}
}

/**
 * \brief Finds a model by ID.
 * \param self Renderer.
 * \param id Model ID.
 * \return Model.
 */
LIRenModel* liren_internal_find_model (
	LIRenRender* self,
	int          id)
{
	return (LIRenModel*) lialg_u32dic_find (self->models, id);
}

void liren_internal_handle_message (
	LIRenRender*  self,
	LIRenMessage* message)
{
	int i;
	LIRenObject* object;

	switch (message->type)
	{
		/* Render */
		case LIREN_MESSAGE_RENDER_LOAD_FONT:
			liren_internal_load_font (self,
				message->render_load_font.name,
				message->render_load_font.file,
				message->render_load_font.size);
			lisys_free (message->render_load_font.name);
			lisys_free (message->render_load_font.file);
			break;
		case LIREN_MESSAGE_RENDER_MEASURE_TEXT:
			liren_internal_measure_text (self,
				message->render_measure_text.font,
				message->render_measure_text.text,
				message->render_measure_text.width_limit,
				message->render_measure_text.result_width,
				message->render_measure_text.result_height);
			lisys_free (message->render_measure_text.font);
			lisys_free (message->render_measure_text.text);
			break;
		case LIREN_MESSAGE_RENDER_SCREENSHOT:
			//*message->render_screenshot.result = liren_internal_screenshot (self);
			break;
		case LIREN_MESSAGE_RENDER_UPDATE:
			liren_internal_update (self,
				message->render_update.secs);
			break;
		case LIREN_MESSAGE_RENDER_GET_ANISOTROPY:
			*message->render_get_anisotropy.result = liren_internal_get_anisotropy (self);
			break;
		case LIREN_MESSAGE_RENDER_SET_ANISOTROPY:
			liren_internal_set_anisotropy (self,
				message->render_set_anisotropy.value);
			break;
		case LIREN_MESSAGE_RENDER_SET_VIDEOMODE:
			liren_internal_set_videomode (self,
				message->render_set_videomode.mode);
			break;

		/* Light */
		/* TODO */

		/* Model */
		case LIREN_MESSAGE_MODEL_NEW:
			break;
		case LIREN_MESSAGE_MODEL_FREE:
			break;
		case LIREN_MESSAGE_MODEL_SET_MODEL:
			break;

		/* Object */
		case LIREN_MESSAGE_OBJECT_NEW:
			break;
		case LIREN_MESSAGE_OBJECT_FREE:
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_ANIMATE:
			object = (LIRenObject*) lialg_u32dic_find (self->objects, message->object_channel_animate.id);
			if (object != NULL)
			{
				liren_object_channel_animate (object,
					message->object_channel_animate.channel,
					message->object_channel_animate.name,
					message->object_channel_animate.additive,
					message->object_channel_animate.repeat,
					message->object_channel_animate.repeat_start,
					message->object_channel_animate.keep,
					message->object_channel_animate.fade_in,
					message->object_channel_animate.fade_out,
					message->object_channel_animate.weight,
					message->object_channel_animate.weight_scale,
					message->object_channel_animate.time,
					message->object_channel_animate.time_scale,
					(const char**) message->object_channel_animate.node_names,
					message->object_channel_animate.node_weights,
					message->object_channel_animate.node_count);
			}
			lisys_free (message->object_channel_animate.name);
			if (message->object_channel_animate.node_names != NULL)
			{
				for (i = 0 ; i < message->object_channel_animate.node_count ; i++)
					lisys_free (message->object_channel_animate.node_names[i]);
			}
			lisys_free (message->object_channel_animate.node_names);
			lisys_free (message->object_channel_animate.node_weights);
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_EDIT:
			object = (LIRenObject*) lialg_u32dic_find (self->objects, message->object_channel_edit.id);
			if (object != NULL)
			{
				liren_object_channel_edit (object,
					message->object_channel_edit.channel,
					message->object_channel_edit.frame,
					message->object_channel_edit.node,
					&message->object_channel_edit.transform,
					message->object_channel_edit.scale);
			}
			lisys_free (message->object_channel_edit.node);
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_FADE:
			object = (LIRenObject*) lialg_u32dic_find (self->objects, message->object_channel_fade.id);
			if (object != NULL)
			{
				liren_object_channel_fade (object,
					message->object_channel_fade.channel,
					message->object_channel_fade.time);
			}
			break;
		case LIREN_MESSAGE_OBJECT_CHANNEL_GET_STATE:
			object = (LIRenObject*) lialg_u32dic_find (self->objects, message->object_channel_get_state.id);
			if (object != NULL)
			{
				*message->object_channel_get_state.result = liren_object_channel_get_state (object,
					message->object_channel_get_state.channel);
			}
			break;
		case LIREN_MESSAGE_OBJECT_FIND_NODE:
			object = (LIRenObject*) lialg_u32dic_find (self->objects, message->object_find_node.id);
			if (object != NULL)
			{
				*message->object_find_node.result = liren_object_find_node (object,
					message->object_find_node.name,
					message->object_find_node.world,
					message->object_find_node.result_transform);
			}
			lisys_free (message->object_find_node.name);
			break;
		case LIREN_MESSAGE_OBJECT_PARTICLE_ANIMATION:
			break;
		case LIREN_MESSAGE_OBJECT_SET_EFFECT:
			break;
		case LIREN_MESSAGE_OBJECT_SET_MODEL:
			break;
		case LIREN_MESSAGE_OBJECT_SET_REALIZED:
			break;
		case LIREN_MESSAGE_OBJECT_SET_TRANSFORM:
			break;
	}

	/* Tell the caller that the return value has been written. */
	if (message->mutex_caller)
		lisys_mutex_unlock (message->mutex_caller);
}

int liren_internal_load_font (
	LIRenRender* self,
	const char*  name,
	const char*  file,
	int          size)
{
	LIFntFont* font;

	/* Check for existing. */
	font = (LIFntFont*) lialg_strdic_find (self->fonts, name);
	if (font != NULL)
		return 1;

	/* Load the font. */
	font = lifnt_font_new (name, file, size);
	if (font == NULL)
		return 0;

	/* Add to the dictionary. */
	if (!lialg_strdic_insert (self->fonts, name, font))
	{
		lifnt_font_free (font);
		return 0;
	}

	return 1;
}

int liren_internal_measure_text (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int*         result_width,
	int*         result_height)
{
	LIFntFont* font_;
	LIFntLayout* layout;

	font_ = (LIFntFont*) lialg_strdic_find (self->fonts, font);
	if (font_ == NULL)
		return 0;

	layout = lifnt_layout_new ();
	if (layout == NULL)
		return 0;

	if (width_limit != -1)
		lifnt_layout_set_width_limit (layout, width_limit);
	lifnt_layout_append_string (layout, font_, text);
	lifnt_layout_update (layout);
	*result_width = lifnt_layout_get_width (layout);
	*result_height = lifnt_layout_get_height (layout);
	lifnt_layout_free (layout);

	return 1;
}

/**
 * \brief Projects a point in the world space to the screen space.
 * \param self Renderer.
 * \param world Point in the world space.
 * \param screen Return location for the point in the screen space.
 */
void liren_internal_project (
	LIRenRender*       self,
	const LIMatVector* world,
	LIMatVector*       screen)
{
	Ogre::Matrix4 mat = self->data->camera->getViewMatrix ();
	Ogre::Vector3 w (world->x, world->y, world->z);
	Ogre::Vector3 s = mat * w;
	screen->x = (s.x / s.z + 0.5f) * self->mode.width;
	screen->y = (s.y / s.z + 0.5f) * self->mode.height;
	screen->z = s.z;
}

/**
 * \brief Reloads all images, shaders and other graphics state.
 *
 * This function is called when the video mode changes in Windows. It
 * reloads all data that was lost when the context was erased.
 *
 * \param self Renderer.
 * \param pass Reload pass.
 */
void liren_internal_reload (
	LIRenRender* self,
	int          pass)
{
}

/**
 * \brief Renders the overlays.
 * \param self Renderer.
 */
void liren_internal_render (
	LIRenRender* self)
{
}

int liren_internal_screenshot (
	LIRenRender* self,
	const char*  path)
{
	self->data->render_window->writeContentsToFile (path);

	return 1;
}

/**
 * \brief Updates the renderer state.
 * \param self Renderer.
 * \param secs Number of seconds since the last update.
 * \return Nonzero when running, zero if the window was closed.
 */
int liren_internal_update (
	LIRenRender* self,
	float        secs)
{
	LIAlgU32dicIter iter1;
	LIRenObject* object;

	/* Animate objects. */
	LIALG_U32DIC_FOREACH (iter1, self->objects)
	{
		object = (LIRenObject*) iter1.value;
		if (object->pose != NULL)
		{
			limdl_pose_update (object->pose, secs);
			liren_object_update_pose (object);
		}
	}

	/* Update the backend. */
	Ogre::WindowEventUtilities::messagePump ();
	if (self->data->render_window->isClosed ())
		return 0;
	private_update_mode (self);

	/* Render a frame. */
	self->data->root->renderOneFrame ();

	return 1;
}

int liren_internal_get_anisotropy (
	const LIRenRender* self)
{
	return self->anisotropy;
}

void liren_internal_set_anisotropy (
	LIRenRender* self,
	int          value)
{
	if (value != self->anisotropy)
	{
		self->anisotropy = value;
		self->data->material_manager->setDefaultAnisotropy (value);
		if (value)
			self->data->material_manager->setDefaultTextureFiltering (Ogre::TFO_ANISOTROPIC);
		else
			self->data->material_manager->setDefaultTextureFiltering (Ogre::TFO_BILINEAR);
		/* TODO: Update texture units? */
	}
}

/**
 * \brief Sets the far plane distance of the camera.
 * \param self Renderer.
 * \param value Distance.
 */
void liren_internal_set_camera_far (
	LIRenRender* self,
	float        value)
{
	self->data->camera->setFarClipDistance (value);
}

/**
 * \brief Sets the near plane distance of the camera.
 * \param self Renderer.
 * \param value Distance.
 */
void liren_internal_set_camera_near (
	LIRenRender* self,
	float        value)
{
	self->data->camera->setNearClipDistance (value);
}

/**
 * \brief Sets the position and orientation of the camera.
 * \param self Renderer.
 * \param value Transformation.
 */
void liren_internal_set_camera_transform (
	LIRenRender*          self,
	const LIMatTransform* value)
{
	self->data->camera->setPosition (value->position.x, value->position.y, value->position.z);
	self->data->camera->setOrientation (Ogre::Quaternion (value->rotation.w, value->rotation.x, value->rotation.y, value->rotation.z));
}

void liren_internal_set_scene_ambient (
	LIRenRender* self,
	const float* value)
{
	self->data->scene_manager->setAmbientLight (Ogre::ColourValue (value[0], value[1], value[2]));
}

void liren_internal_set_title (
	LIRenRender* self,
	const char*  value)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
	Display* display;
	Window window;
	self->data->render_window->getCustomAttribute ("DISPLAY", &display);
	self->data->render_window->getCustomAttribute ("WINDOW", &window);
	XStoreName (display, window, value);
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	HWND window;
	self->data->render_window->getCustomAttribute ("WINDOW", &window);
	SetWindowText (window, value);
#else
	/* TODO */
#endif
}

int liren_internal_set_videomode (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	if (mode->fullscreen)
		self->data->render_window->setFullscreen (true, mode->width, mode->height);
	else
		self->data->render_window->setFullscreen (false, mode->width, mode->height);
	return 1;
}

int liren_internal_get_videomodes (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num)
{
	/* Count modes. */
	const Ogre::StringVector& list = self->data->render_system->
		getConfigOptions()["Video Mode"].possibleValues;
	if (!list.size ())
		return 0;

	/* Allocate the result. */
	*modes = (LIRenVideomode*) lisys_calloc (list.size (), sizeof (LIRenVideomode));
	if (*modes == NULL)
		return 0;
	*modes_num = list.size ();

	/* Convert modes. */
	for (int i = 0 ; i < *modes_num ; i++)
	{
		int w, h;
		if (sscanf (list[i].c_str (), "%d x %d", &w, &h) == 2)
		{
			(*modes)[i].width = w;
			(*modes)[i].height = h;
			(*modes)[i].fullscreen = 1;
			(*modes)[i].sync = self->mode.sync;
		}
	}

	return 1;
}

/*****************************************************************************/

static void private_load_plugin (
	LIRenRender* self,
	const char*  name)
{
#ifdef OGRE_PLUGIN_DIR
	Ogre::String path(OGRE_PLUGIN_DIR "/");
#else
	Ogre::String path(Ogre::String(self->paths->root) + "/plugins/");
#endif

	try
	{
		self->data->root->loadPlugin (path + name);
	}
	catch (...)
	{
		self->data->root->loadPlugin (path + name + Ogre::String("_d"));
	}
}

static int private_check_plugin (
	LIRenRender* self,
	const char*  name)
{
	Ogre::Root::PluginInstanceList ip = self->data->root->getInstalledPlugins ();
	for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
	{
		if ((*k)->getName () == name)
			return 1;
	}

	return 0;
}

static void private_update_mode (
	LIRenRender*  self)
{
	unsigned int w, h, d;
	int x, y;

	self->data->render_window->getMetrics (w, h, d, x, y);
	self->mode.width = w;
	self->mode.height = h;
	self->mode.fullscreen = self->data->render_window->isFullScreen ();
}

/** @} */
/** @} */
/** @} */
