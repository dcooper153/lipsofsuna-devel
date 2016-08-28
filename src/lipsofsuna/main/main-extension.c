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

/**
 * \addtogroup LIMai Main
 * @{
 * \addtogroup LIMaiExtension Extension
 * @{
 */

#include <string.h>
#include "main-extension.h"

extern LIMaiExtensionInfo liext_animation_info;
extern LIMaiExtensionInfo liext_array_info;
extern LIMaiExtensionInfo liext_billboard_info;
extern LIMaiExtensionInfo liext_camera_info;
extern LIMaiExtensionInfo liext_clipboard_info;
extern LIMaiExtensionInfo liext_config_file_info;
extern LIMaiExtensionInfo liext_database_info;
extern LIMaiExtensionInfo liext_file_info;
extern LIMaiExtensionInfo liext_filter_info;
extern LIMaiExtensionInfo liext_graphics_info;
extern LIMaiExtensionInfo liext_heightmap_info;
extern LIMaiExtensionInfo liext_heightmap_physics_info;
extern LIMaiExtensionInfo liext_heightmap_render_info;
extern LIMaiExtensionInfo liext_image_info;
extern LIMaiExtensionInfo liext_image_merge_info;
extern LIMaiExtensionInfo liext_input_info;
extern LIMaiExtensionInfo liext_lobby_info;
extern LIMaiExtensionInfo liext_math_info;
extern LIMaiExtensionInfo liext_model_info;
extern LIMaiExtensionInfo liext_model_editing_info;
extern LIMaiExtensionInfo liext_model_merge_info;
extern LIMaiExtensionInfo liext_network_info;
extern LIMaiExtensionInfo liext_noise_info;
extern LIMaiExtensionInfo liext_object_info;
extern LIMaiExtensionInfo liext_object_physics_info;
extern LIMaiExtensionInfo liext_object_render_info;
extern LIMaiExtensionInfo liext_parallel_info;
extern LIMaiExtensionInfo liext_particle_info;
extern LIMaiExtensionInfo liext_password_info;
extern LIMaiExtensionInfo liext_physics_info;
extern LIMaiExtensionInfo liext_physics_debug_info;
extern LIMaiExtensionInfo liext_physics_terrain_info;
extern LIMaiExtensionInfo liext_reload_info;
extern LIMaiExtensionInfo liext_render_info;
extern LIMaiExtensionInfo liext_render_model_info;
extern LIMaiExtensionInfo liext_sectors_info;
extern LIMaiExtensionInfo liext_skeleton_info;
extern LIMaiExtensionInfo liext_sound_info;
extern LIMaiExtensionInfo liext_string_info;
extern LIMaiExtensionInfo liext_softbody_info;
extern LIMaiExtensionInfo liext_terrain_info;
extern LIMaiExtensionInfo liext_thread_info;
extern LIMaiExtensionInfo liext_time_info;
extern LIMaiExtensionInfo liext_vision_info;
extern LIMaiExtensionInfo liext_watchdog_info;
extern LIMaiExtensionInfo liext_widgets_info;

/**
 * \brief Gets a built-in extension by name.
 * \param name Mangled extension name.
 */
LIMaiExtensionInfo* limai_extension_get_builtin (
	const char* name)
{
	int i;
	static const struct
	{
		const char* name;
		LIMaiExtensionInfo* info;
	}
	info[] =
	{
		{ "animation", &liext_animation_info },
		{ "array", &liext_array_info },
		{ "config-file", &liext_config_file_info },
		{ "database", &liext_database_info },
		{ "file", &liext_file_info },
		{ "filter", &liext_filter_info },
		{ "heightmap", &liext_heightmap_info },
		{ "heightmap-physics", &liext_heightmap_physics_info },
		{ "image", &liext_image_info },
		{ "image-merge", &liext_image_merge_info },
		{ "lobby", &liext_lobby_info },
		{ "math", &liext_math_info },
		{ "model", &liext_model_info },
		{ "model-editing", &liext_model_editing_info },
		{ "model-merge", &liext_model_merge_info },
		{ "network", &liext_network_info },
		{ "noise", &liext_noise_info },
		{ "object", &liext_object_info },
		{ "parallel", &liext_parallel_info },
		{ "password", &liext_password_info },
		{ "physics", &liext_physics_info },
		{ "physics-object", &liext_object_physics_info },
		{ "physics-terrain", &liext_physics_terrain_info },
		{ "reload", &liext_reload_info },
		{ "sectors", &liext_sectors_info },
		{ "skeleton", &liext_skeleton_info },
		{ "sound", &liext_sound_info },
		{ "string", &liext_string_info },
		{ "terrain", &liext_terrain_info },
		{ "thread", &liext_thread_info },
		{ "time", &liext_time_info },
		{ "vision", &liext_vision_info },
		{ "watchdog", &liext_watchdog_info },
		{ "clipboard", &liext_clipboard_info },
#ifndef LI_DISABLE_GRAPHICS
		{ "billboard", &liext_billboard_info },
		{ "camera", &liext_camera_info },
		{ "graphics", &liext_graphics_info },
		{ "heightmap-render", &liext_heightmap_render_info },
		{ "input", &liext_input_info },
		{ "particle", &liext_particle_info },
		{ "physics-debug", &liext_physics_debug_info },
		{ "render", &liext_render_info },
		{ "render-object", &liext_object_render_info },
		{ "render-model", &liext_render_model_info },
		{ "softbody", &liext_softbody_info },
		{ "widgets", &liext_widgets_info },
#endif
	};

	for (i = 0 ; i < sizeof (info) / sizeof (*info) ; i++)
	{
		if (!strcmp (info[i].name, name))
			return info[i].info;
	}

	return NULL;
}

/** @} */
/** @} */
