/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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

#ifndef __RENDER_INTERNAL_RENDER_HPP__
#define __RENDER_INTERNAL_RENDER_HPP__

#include "lipsofsuna/paths.h"
#include "render-unique-id.hpp"
#include "../render-types.h"
#include <OgreRoot.h>
#include <OgreCamera.h>
#include <OgreMaterialManager.h>
#include <OgreRenderWindow.h>
#include <OgreRenderSystem.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreTextureManager.h>
#include <OgreViewport.h>
#include <OgreLogManager.h>

#define LIREN_RESOURCES_PERMANENT "permanent"
#define LIREN_RESOURCES_TEMPORARY "temporary"

class LIRenMaterialUtils;
class LIRenModel;
class LIRenOverlayManager;
class LIRenResourceLoadingListener;

class LIRenRender
{
public:
	int init (
		LIRenVideomode* mode);

	void deinit ();

	void add_compositor (
		const char* name);

	void remove_compositor (
		const char* name);

	LIRenModel* find_model (
		int         id);

	int layout_text (
		const char* font,
		const char* text,
		int         width_limit,
		int**       result_glyphs,
		int*        result_glyphs_num);

	int load_font (
		const char* name,
		const char* file,
		int         size);

	void load_resources ();

	int measure_text (
		const char* font,
		const char* text,
		int         width_limit,
		int*        result_width,
		int*        result_height);

	void project (
		const LIMatVector* world,
		LIMatVector*       screen);

	void reload (
		int pass);

	void render ();

	int screenshot (
		const char* path);

	int update (
		float secs);

	int get_anisotropy () const;

	void set_anisotropy (
		int value);

	void set_camera_far (
		float        value);

	void set_camera_near (
		float value);

	void set_camera_transform (
		const LIMatTransform* value);

	float get_opengl_version () const;

	void set_material_scheme (
		const char* value);

	void set_scene_ambient (
		const float* value);

	void set_skybox (
		const char* value);

	void get_stats (
		LIRenStats* result) const;

	void set_title (
		const char* value);

	int set_videomode (
		LIRenVideomode* mode);

	int get_videomodes (
		LIRenVideomode** modes,
		int*             modes_num) const;

private:
	bool init_window (
		LIRenVideomode*                  mode,
		Ogre::RenderSystemCapabilities*& retry);

	void deinit_window ();

	int count_resources (
		const Ogre::ResourceManager& manager) const;

	int count_resources_loaded (
		const Ogre::ResourceManager& manager) const;

	void load_plugin (
		const char* name);

	int check_plugin (
		const char* name);

	void garbage_collect_builders ();

	void unload_unused_resources (
		Ogre::ResourceManager& manager);

	void update_mode ();

// FIXME
public:
	int anisotropy;
	LIAlgRandom random;
	LIAlgStrdic* fonts;
	LIAlgU32dic* lights;
	LIAlgU32dic* models;
	LIAlgU32dic* objects;
	LIPthPaths* paths;
	LIRenVideomode mode;

	float unload_timer;
	Ogre::Root* root;
	Ogre::Camera* camera;
	Ogre::MaterialManager* material_manager;
	LIRenOverlayManager* overlay_mgr;
	Ogre::RenderWindow* render_window;
	Ogre::RenderSystem* render_system;
	Ogre::SceneManager* scene_manager;
	Ogre::SceneNode* scene_root;
	Ogre::TextureManager* texture_manager;
	Ogre::Viewport* viewport;
	Ogre::LogManager* log;
	LIRenMaterialUtils* material_utils;
	LIRenResourceLoadingListener* resource_loading_listener;
	LIRenUniqueId id;
	LIAlgStrdic* mesh_builders;
};

#endif
