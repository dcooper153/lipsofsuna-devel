/* Lips of Suna
 * Copyright© 2007-2010 Lips of Suna development team.
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
 * \addtogroup liext Extension
 * @{
 * \addtogroup liextcli Client
 * @{
 * \addtogroup liextcliCamera Camera
 * @{
 */

#include "ext-module.h"

static int
private_tick (LIExtModule* self,
              float        secs);

static void
private_camera_clip (LIExtModule* self);

/*****************************************************************************/

LIMaiExtensionInfo liext_info =
{
	LIMAI_EXTENSION_VERSION, "Camera",
	liext_module_new,
	liext_module_free
};

LIExtModule*
liext_module_new (LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->client = limai_program_find_component (program, "client");

	/* Register callbacks. */
	if (!lical_callbacks_insert (program->callbacks, program->engine, "tick", 0, private_tick, self, self->calls + 0))
	{
		liext_module_free (self);
		return NULL;
	}

	/* Register classes. */
	liscr_script_create_class (program->script, "Camera", liext_script_camera, self);

	return self;
}

void
liext_module_free (LIExtModule* self)
{
	lical_handle_releasev (self->calls, sizeof (self->calls) / sizeof (LICalHandle));
	lisys_free (self);
}

/*****************************************************************************/

static int
private_tick (LIExtModule* self,
              float        secs)
{
	lialg_camera_move (self->client->camera, secs * self->move);
	lialg_camera_tilt (self->client->camera, secs * self->tilt);
	lialg_camera_turn (self->client->camera, secs * self->turn);
	lialg_camera_zoom (self->client->camera, secs * self->zoom);

	lialg_camera_update (self->client->camera, secs);
	if (lialg_camera_get_driver (self->client->camera) != LIALG_CAMERA_FIRSTPERSON)
		private_camera_clip (self);

	return 1;
}

static void
private_camera_clip (LIExtModule* self)
{
	int hit;
	float frac;
	LIMatAabb aabb;
	LIMatTransform start;
	LIMatTransform end;
	LIMatVector diff;
	LIPhyCollision tmp;
	LIPhyShape* shape;

	/* Create sweep shape. */
	/* FIXME: Could use a more accurate shape. */
	lialg_camera_get_bounds (self->client->camera, &aabb);
	shape = liphy_shape_new_aabb (self->client->engine->physics, &aabb);
	if (shape == NULL)
		return;

	/* Sweep the shape. */
	lialg_camera_get_center (self->client->camera, &start);
	lialg_camera_get_transform (self->client->camera, &end);
	diff = limat_vector_subtract (end.position, start.position);
	hit = liphy_physics_cast_shape (self->client->engine->physics, &start, &end, shape,
		LICLI_PHYSICS_GROUP_CAMERA, LIPHY_GROUP_STATICS | LIPHY_GROUP_TILES, NULL, 0, &tmp);
	liphy_shape_free (shape);

	/* Clip the camera. */
	if (hit)
	{
		frac = tmp.fraction * limat_vector_get_length (diff);
		lialg_camera_clip (self->client->camera, frac);
	}
}

/** @} */
/** @} */
/** @} */
