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
 * \addtogroup LIRenRender Render
 * @{
 */

#include "render-internal.h"
#include "render-container-factory.hpp"
#include "render-resource-loading-listener.hpp"
#include "../font/font.h"
#include "../font/font-layout.h"
#include <OgreCompositorManager.h>
#include <OgreEntity.h>
#include <OgreFontManager.h>
#include <OgreLogManager.h>
#include <OgrePlugin.h>
#include <OgreShadowCameraSetupLiSPSM.h>
#include <OgreRenderSystemCapabilitiesManager.h>
#include <OgreRenderSystemCapabilitiesSerializer.h>
#include <OgreResourceBackgroundQueue.h>
#include <OgreSkeletonManager.h>
#include <OgreViewport.h>
#include <OgreWindowEventUtilities.h>
#ifdef HAVE_XLIB
#include <X11/Xlib.h>
#endif
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#include <windows.h>
#endif

#define LIREN_RENDER_TEXTURE_UNLOAD_TIME 10

static int private_count_resources (
	LIRenRender*           self,
	Ogre::ResourceManager& manager);

static int private_count_resources_loaded (
	LIRenRender*           self,
	Ogre::ResourceManager& manager);

static void private_garbage_collect_builders (
	LIRenRender* self);

static void private_load_plugin (
	LIRenRender* self,
	const char*  name);

static int private_check_plugin (
	LIRenRender* self,
	const char*  name);

static void private_unload_unused_resources (
	LIRenRender*           self,
	Ogre::ResourceManager& manager);

static void private_update_mode (
	LIRenRender*  self);

/*****************************************************************************/

int liren_internal_init (
	LIRenRender*    self,
	LIRenVideomode* mode)
{
	Ogre::Real w;
	Ogre::Real h;
	Ogre::String data1 (self->paths->module_data);

	/* Initialize the private data. */
	self->unload_timer = 0.0f;
	self->container_factory = NULL;
	self->image_factory = NULL;
	self->scaled_factory = NULL;
	self->text_factory = NULL;
	self->mesh_builders = lialg_strdic_new ();

	/* Disable console output. */
	self->log = new Ogre::LogManager ();
	self->log->createLog ("render.log", true, false, false);

	/* Initialize the Ogre root. */
	self->root = new Ogre::Root("", "", "");

	/* Load plugins. */
	private_load_plugin (self, "RenderSystem_GL");
	private_load_plugin (self, "Plugin_OctreeSceneManager");
	private_load_plugin (self, "Plugin_ParticleFX");

	/* Make sure that the required plugins were loaded. */
	if (!private_check_plugin (self, "GL RenderSystem"))
		return 0;

	/* Initialize the render system. */
	self->render_system = self->root->getRenderSystemByName ("OpenGL Rendering Subsystem");
	if (!(self->render_system->getName () == "OpenGL Rendering Subsystem"))
		return 0;

	/* Choose the video mode. */
	Ogre::String fsaa = Ogre::StringConverter::toString (mode->multisamples);
	Ogre::String video_mode =
		Ogre::StringConverter::toString (mode->width) + " x " +
		Ogre::StringConverter::toString (mode->height);
	self->render_system->setConfigOption ("Full Screen", mode->fullscreen? "Yes" : "No");
	self->render_system->setConfigOption ("VSync", mode->sync? "Yes" : "No");
	self->render_system->setConfigOption ("Video Mode", video_mode);
	self->render_system->setConfigOption ("FSAA", fsaa);

	/* Initialize the render window. */
	self->root->setRenderSystem (self->render_system);

	/* Initialize custom render system capabilities. */
	/* This is a debug feature that allows emulating older hardware by setting
	   the LOS_READ_RENDERCAPS environment variable. */
	if (getenv ("LOS_READ_RENDERCAPS"))
	{
		Ogre::String dirname = data1 + "/debug";
		Ogre::String capsname = getenv ("LOS_READ_RENDERCAPS");
		Ogre::RenderSystemCapabilitiesManager* rscm = Ogre::RenderSystemCapabilitiesManager::getSingletonPtr ();
		rscm->parseCapabilitiesFromArchive (dirname, "FileSystem", true);
		Ogre::RenderSystemCapabilities* caps = rscm->loadParsedCapabilities (capsname);
		if (caps != NULL)
		{
			self->root->useCustomRenderSystemCapabilities (caps);
			printf ("NOTE: read rendercaps `%s'\n", capsname.c_str ());
		}
		else
		{
			lisys_error_set (EINVAL, "could not find rendercaps `%s'", capsname.c_str ());
			lisys_error_report ();
		}
	}

	/* Create the main window. */
	self->render_window = self->root->initialise (true, "Lips of Suna");
	self->mode = *mode;
	private_update_mode (self);

	/* Dump the render system capabilities. */
	/* The rendercaps files require all the fields to be present so we can't
	   realistically expect anyone to write them manually. */
	if (getenv ("LOS_WRITE_RENDERCAPS"))
	{
		const Ogre::RenderSystemCapabilities* caps = self->render_system->getCapabilities ();
		Ogre::RenderSystemCapabilitiesSerializer s;
		Ogre::String capsname = getenv ("LOS_WRITE_RENDERCAPS");
		Ogre::String filename = data1 + "/debug/" + capsname + ".rendercaps";
		s.writeScript (caps, capsname, filename);
		printf ("NOTE: dumped rendercaps to `%s'\n", filename.c_str ());
	}

	/* Initialize the scene manager. */
	self->scene_manager = self->root->createSceneManager("OctreeSceneManager", "DefaultSceneManager");
	self->scene_manager->setAmbientLight (Ogre::ColourValue (0.5, 0.5, 0.5));
	self->scene_manager->setShadowTechnique (Ogre::SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
	self->scene_manager->setShadowTextureSelfShadow (false);
	self->scene_manager->setShadowCasterRenderBackFaces (false);
	self->scene_manager->setShadowFarDistance (50.0f);
	self->scene_manager->setShadowDirLightTextureOffset (0.6f);
	Ogre::LiSPSMShadowCameraSetup* shadow_camera = new Ogre::LiSPSMShadowCameraSetup ();
	shadow_camera->setOptimalAdjustFactor (2.0f);
	self->scene_manager->setShadowCameraSetup (Ogre::ShadowCameraSetupPtr (shadow_camera));
	self->scene_manager->setShadowTextureCount (3);
	self->scene_manager->setShadowTextureSize (1024);
	self->scene_root = self->scene_manager->getRootSceneNode ();

	/* Initialize the camera. */
	self->camera = self->scene_manager->createCamera ("Camera");
	self->camera->setNearClipDistance (1);
	self->camera->setFarClipDistance (75);
	self->viewport = self->render_window->addViewport (self->camera);
	self->viewport->setBackgroundColour (Ogre::ColourValue (0.0f, 0.0f, 0.0f));
	w = Ogre::Real (self->viewport->getActualWidth ());
	h = Ogre::Real (self->viewport->getActualHeight ());
	self->camera->setAspectRatio (w / h);

	/* Initialize the user interface. */
	self->overlay_manager = &(Ogre::OverlayManager::getSingleton ());
	self->container_factory = new LIRenContainerFactory;
	self->overlay_manager->addOverlayElementFactory (self->container_factory);
	self->image_factory = new LIRenImageOverlayFactory;
	self->overlay_manager->addOverlayElementFactory (self->image_factory);
	self->scaled_factory = new LIRenScaledOverlayFactory;
	self->overlay_manager->addOverlayElementFactory (self->scaled_factory);
	self->text_factory = new LIRenTextOverlayFactory;
	self->overlay_manager->addOverlayElementFactory (self->text_factory);

	/* Create the group for temporary resources. */
	/* This group is used for temporary resources such as meshes or
	   materials instantiated and overridden for specific meshes. We
	   want this kind of resources to be completely purged when unused. */
	Ogre::ResourceGroupManager& mgr = Ogre::ResourceGroupManager::getSingleton ();
	mgr.createResourceGroup (LIREN_RESOURCES_TEMPORARY);

	/* Allow overriding of resources. */
	self->resource_loading_listener = new LIRenResourceLoadingListener (self->paths);
	mgr.setLoadingListener (self->resource_loading_listener);

	/* Inialize texture and material managers. */
	self->texture_manager = &Ogre::TextureManager::getSingleton ();
	self->texture_manager->setDefaultNumMipmaps (5);
	self->material_manager = &Ogre::MaterialManager::getSingleton ();
	self->material_utils = new LIRenMaterialUtils(self);

	return 1;
}

void liren_internal_deinit (
	LIRenRender* self)
{
	/* Free the mesh builders. */
	/* Some meshes may be being loaded in other threads so the Ogre root
	   needs to be shut down first to guarantee clean shutdown. */
	self->root->shutdown ();
	private_garbage_collect_builders (self);
	lialg_strdic_free (self->mesh_builders);

	/* Free the Ogre root. */
	delete self->root;
	delete self->resource_loading_listener;
	delete self->material_utils;
	delete self->container_factory;
	delete self->image_factory;
	delete self->scaled_factory;
	delete self->text_factory;
	delete self->log;
}

/**
 * \brief Enables a compositor script.
 * \param self Renderer.
 * \param name Compositor script name.
 */
void liren_internal_add_compositor (
	LIRenRender* self,
	const char*  name)
{
	Ogre::CompositorManager::getSingleton ().addCompositor (self->viewport, name);
	Ogre::CompositorManager::getSingleton ().setCompositorEnabled (self->viewport, name, true);
}

/**
 * \brief Disables a compositor script.
 * \param self Renderer.
 * \param name Compositor script name.
 */
void liren_internal_remove_compositor (
	LIRenRender* self,
	const char*  name)
{
	Ogre::CompositorManager::getSingleton ().removeCompositor (self->viewport, name);
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

int liren_internal_layout_text (
	LIRenRender* self,
	const char*  font,
	const char*  text,
	int          width_limit,
	int**        result_glyphs,
	int*         result_glyphs_num)
{
	int i;
	int h;
	LIFntFont* font_;
	LIFntLayout* layout;

	font_ = (LIFntFont*) lialg_strdic_find (self->fonts, font);
	if (font_ == NULL)
		return 0;

	layout = lifnt_layout_new ();
	if (layout == NULL)
		return 0;

	lifnt_layout_set_width_limit (layout, width_limit);
	lifnt_layout_append_string (layout, font_, text);
	lifnt_layout_update (layout);
	if (layout->n_glyphs)
	{
		*result_glyphs_num = layout->n_glyphs;
		*result_glyphs = (int*) lisys_calloc (layout->n_glyphs, 3 * sizeof (int));
		if (*result_glyphs == NULL)
			return 0;
		h = lifnt_font_get_height (font_);
		for (i = 0 ; i < layout->n_glyphs ; i++)
		{
			(*result_glyphs)[3 * i] = layout->glyphs[i].x;
			(*result_glyphs)[3 * i + 1] = layout->glyphs[i].y - h;
			(*result_glyphs)[3 * i + 2] = layout->glyphs[i].advance;
		}
	}
	else
	{
		*result_glyphs_num = 0;
		*result_glyphs = NULL;
	}
	lifnt_layout_free (layout);

	return 1;
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

void liren_internal_load_resources (
	LIRenRender* self)
{
	LIAlgList* ptr;

	Ogre::ResourceGroupManager& mgr = Ogre::ResourceGroupManager::getSingleton ();

	/* Create the group for permanent resources. */
	/* This group is used for resources that are managed by Ogre. They
	   include textures and various scripts detected at initialization.
	   Out of these, we only want to unload textures and even for them
	   we want to keep the resource info available at all times. */
	Ogre::String group = LIREN_RESOURCES_PERMANENT;
	if (self->paths->paths != NULL)
	{
		for (ptr = self->paths->paths ; ptr->next != NULL ; ptr = ptr->next)
			{}
		for ( ; ptr != NULL ; ptr = ptr->prev)
		{
			const char* dir = (const char*) ptr->data;
			mgr.addResourceLocation (dir, "FileSystem", group, false);
		}
	}

	/* Load the resources. */
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups ();
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
	Ogre::Matrix4 proj = self->camera->getProjectionMatrix ();
	Ogre::Matrix4 view = self->camera->getViewMatrix ();
	Ogre::Vector3 w (world->x, world->y, world->z);
	Ogre::Vector3 s = proj * view * w;
	screen->x = (0.5f + 0.5f * s.x) * self->mode.width;
	screen->y = (0.5f - 0.5f * s.y) * self->mode.height;
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
	/* Render a frame. */
	self->root->renderOneFrame ();
}

int liren_internal_screenshot (
	LIRenRender* self,
	const char*  path)
{
	self->render_window->writeContentsToFile (path);

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

	/* Update objects. */
	LIALG_U32DIC_FOREACH (iter1, self->objects)
	{
		object = (LIRenObject*) iter1.value;
		object->update (secs);
	}

	/* Update the backend. */
	Ogre::WindowEventUtilities::messagePump ();
	if (self->render_window->isClosed ())
		return 0;
	private_update_mode (self);

	/* Update the aspect ratio of the camera. */
	Ogre::Real w (self->viewport->getActualWidth ());
	Ogre::Real h (self->viewport->getActualHeight ());
	self->camera->setAspectRatio (w / h);

	/* Free unused resources. */
	/* Ogre internals seem to be sloppy with using resource pointers. At
	   least the terrain system will crash if we try to remove all unused
	   resources. We need to limit unloading to our own resource group. */
	/* Ogre seems to not have a function for removing unreferenced
	   resources from a specific group so we need to do it manually. */
	self->unload_timer += secs;
	if (self->unload_timer > 5.0f)
	{
		self->unload_timer = 0.0f;
		private_unload_unused_resources (self, Ogre::MeshManager::getSingleton ());
		private_unload_unused_resources (self, Ogre::SkeletonManager::getSingleton ());
		private_unload_unused_resources (self, Ogre::MaterialManager::getSingleton ());
		private_unload_unused_resources (self, Ogre::TextureManager::getSingleton ());
		private_garbage_collect_builders (self);
	}

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
		self->material_manager->setDefaultAnisotropy (value);
		if (value)
			self->material_manager->setDefaultTextureFiltering (Ogre::TFO_ANISOTROPIC);
		else
			self->material_manager->setDefaultTextureFiltering (Ogre::TFO_BILINEAR);
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
	self->camera->setFarClipDistance (value);
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
	self->camera->setNearClipDistance (value);
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
	self->camera->setPosition (value->position.x, value->position.y, value->position.z);
	self->camera->setOrientation (Ogre::Quaternion (value->rotation.w, value->rotation.x, value->rotation.y, value->rotation.z));
}

/**
 * \brief Sets the name of the preferred Ogre material scheme.
 * \param self Renderer.
 * \param value Material scheme name.
 */
void liren_internal_set_material_scheme (
	LIRenRender* self,
	const char*  value)
{
	self->viewport->setMaterialScheme (value);
}

void liren_internal_set_scene_ambient (
	LIRenRender* self,
	const float* value)
{
	self->scene_manager->setAmbientLight (Ogre::ColourValue (value[0], value[1], value[2]));
}

/**
 * \brief Sets the skybox material.
 * \param self Renderer.
 * \param value Material name.
 */
void liren_internal_set_skybox (
	LIRenRender* self,
	const char*  value)
{
	if (value != NULL)
	{
		try
		{
			self->scene_manager->setSkyBox (true, value, 10.0f, true);
			return;
		}
		catch (...)
		{
		}
	}
	self->scene_manager->setSkyBox (false, "");
}

void liren_internal_get_stats (
	LIRenRender* self,
	LIRenStats*  result)
{
	result->batch_count = self->viewport->_getNumRenderedBatches ();
	result->face_count = self->viewport->_getNumRenderedFaces ();
	result->material_count = private_count_resources (self, Ogre::MaterialManager::getSingleton ());
	result->material_count_loaded = private_count_resources_loaded (self, Ogre::MaterialManager::getSingleton ());
	result->mesh_count = private_count_resources (self, Ogre::MeshManager::getSingleton ());
	result->mesh_memory = (int) Ogre::MeshManager::getSingleton ().getMemoryUsage ();
	result->skeleton_count = private_count_resources (self, Ogre::SkeletonManager::getSingleton ());
	result->texture_count = private_count_resources (self, Ogre::TextureManager::getSingleton ());
	result->texture_count_loaded = private_count_resources_loaded (self, Ogre::TextureManager::getSingleton ());
	result->texture_memory = (int) Ogre::TextureManager::getSingleton ().getMemoryUsage ();

	/* Count entities. */
	result->entity_count = 0;
	Ogre::SceneManager::MovableObjectIterator iterator = self->scene_manager->getMovableObjectIterator ("Entity");
	while (iterator.hasMoreElements ())
	{
		result->entity_count++;
		iterator.getNext ();
	}

	/* Count attachments. */
	result->attachment_count = 0;
	LIAlgU32dicIter iter;
	LIALG_U32DIC_FOREACH (iter, self->objects)
	{
		LIRenObject* object = (LIRenObject*) iter.value;
		result->attachment_count += object->get_attachment_count ();
	}
}

void liren_internal_set_title (
	LIRenRender* self,
	const char*  value)
{
#ifdef HAVE_XLIB
	Display* display;
	Window window;
	self->render_window->getCustomAttribute ("DISPLAY", &display);
	self->render_window->getCustomAttribute ("WINDOW", &window);
	XStoreName (display, window, value);
#elif OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	HWND window;
	self->render_window->getCustomAttribute ("WINDOW", &window);
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
		self->render_window->setFullscreen (true, mode->width, mode->height);
	else
		self->render_window->setFullscreen (false, mode->width, mode->height);
	return 1;
}

int liren_internal_get_videomodes (
	LIRenRender*     self,
	LIRenVideomode** modes,
	int*             modes_num)
{
	/* Count modes. */
	const Ogre::StringVector& list = self->render_system->
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
			(*modes)[i].multisamples = self->mode.multisamples;
		}
	}

	return 1;
}

/*****************************************************************************/

static int private_count_resources (
	LIRenRender*           self,
	Ogre::ResourceManager& manager)
{
	int count;

	Ogre::ResourceManager::ResourceMapIterator iter = manager.getResourceIterator ();
	for (count = 0 ; iter.hasMoreElements () ; iter.moveNext ())
		count++;

	return count;
}

static int private_count_resources_loaded (
	LIRenRender*           self,
	Ogre::ResourceManager& manager)
{
	int count;

	Ogre::ResourceManager::ResourceMapIterator iter = manager.getResourceIterator ();
	for (count = 0 ; iter.hasMoreElements () ; iter.moveNext ())
	{
		if (iter.peekNextValue ()->isLoaded ())
			count++;
	}

	return count;
}

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
		self->root->loadPlugin (path + name);
	}
	catch (Ogre::InternalErrorException e)
	{
		try
		{
			self->root->loadPlugin (path + name + Ogre::String("_d"));
		}
		catch (...)
		{
			throw e;
		}
	}
}

static int private_check_plugin (
	LIRenRender* self,
	const char*  name)
{
	Ogre::Root::PluginInstanceList ip = self->root->getInstalledPlugins ();
	for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
	{
		if ((*k)->getName () == name)
			return 1;
	}

	return 0;
}

static void private_garbage_collect_builders (
	LIRenRender* self)
{
	LIAlgStrdicIter iter;

	Ogre::MeshManager& mgr = Ogre::MeshManager::getSingleton ();

	/* Ogre will happily leak our manual resource loaders. To avoid them being
	   permanently leaked, we have stored them a dictionary by mesh name. If
	   the mesh no longer exists, we free the associated builder. */
	LIALG_STRDIC_FOREACH (iter, self->mesh_builders)
	{
		Ogre::MeshPtr ptr = mgr.getByName (iter.key, LIREN_RESOURCES_TEMPORARY);
		if (ptr.isNull ())
		{
			lialg_strdic_remove (self->mesh_builders, iter.key);
			OGRE_DELETE ((LIRenMeshBuilder*) iter.value);
		}
	}
}

static void private_unload_unused_resources (
	LIRenRender*           self,
	Ogre::ResourceManager& manager)
{
	Ogre::ResourceManager::ResourceMapIterator iter = manager.getResourceIterator ();
	while (iter.hasMoreElements ())
	{
		/* Check if the resource is loaded. */
		/* Resources that are not loaded may be subject to removal if they
		   belong to the group of temporary resources. Resources in the
		   permanent group are never removed, nor are resources that are
		   currently being background loaded. */
		Ogre::ResourcePtr resource = iter.getNext ();
		if (resource->isLoading ())
			continue;
		if (!resource->isLoaded () && resource->getGroup () == LIREN_RESOURCES_PERMANENT)
			continue;

		/* Check if the resource is in use. */
		/* The resource pointer we create here adds one extra reference. */
		if (resource.useCount () > Ogre::ResourceGroupManager::RESOURCE_SYSTEM_NUM_REFERENCE_COUNTS + 1)
			continue;

		/* Remove or unload the resource. */
		/* Resources in the temporary group are completely removed since we
		   don't want to keep any records of garbage collected meshes and such.
		   Resources in the permanent group can be unloaded but their records
		   are kept so that predefined materials and scripts don't get deleted.
		   The rest are Ogre's internal resources that shouldn't be touched. */
		if (resource->getGroup () == LIREN_RESOURCES_TEMPORARY)
			manager.remove (resource->getHandle ());
		else if (resource->getGroup () == LIREN_RESOURCES_PERMANENT)
			manager.unload (resource->getHandle ());
	}
}

static void private_update_mode (
	LIRenRender*  self)
{
	unsigned int w, h, d;
	int x, y;

	self->render_window->getMetrics (w, h, d, x, y);
	self->mode.width = w;
	self->mode.height = h;
	self->mode.fullscreen = self->render_window->isFullScreen ();
}

/** @} */
/** @} */
/** @} */
