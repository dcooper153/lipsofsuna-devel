/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup lirnd Render
 * @{
 * \addtogroup lirndApi Api
 * @{
 */

#include "render-api.h"

/**
 * \brief Rendering function list.
 *
 * This variable exists to make it easier to load the renderer at run-time.
 */
lirndApi lirnd_render_api =
{
	lirnd_buffer_init,
	lirnd_buffer_free,
	lirnd_material_new,
	lirnd_material_new_from_model,
	lirnd_material_free,
	lirnd_model_new,
	lirnd_model_free,
	lirnd_object_new,
	lirnd_object_free,
	lirnd_object_deform,
	lirnd_object_update,
	lirnd_object_set_model,
	lirnd_object_set_realized,
	lirnd_object_set_transform,
	lirnd_render_new,
	lirnd_render_free,
	lirnd_render_update,
	lirnd_render_find_image,
	lirnd_render_load_image,
	lirnd_scene_new,
	lirnd_scene_free,
	lirnd_scene_update
};

/** @} */
/** @} */
