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

static void private_camera_clip (
	LIExtModule* self,
	LIAlgCamera* camera);

/*****************************************************************************/

LIMaiExtensionInfo liext_camera_info =
{
	LIMAI_EXTENSION_VERSION, "Camera",
	liext_cameras_new,
	liext_cameras_free
};

LIExtModule* liext_cameras_new (
	LIMaiProgram* program)
{
	LIExtModule* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtModule));
	if (self == NULL)
		return NULL;
	self->program = program;
	self->client = limai_program_find_component (program, "client");

	/* Register classes. */
	liscr_script_create_class (program->script, "Camera", liext_script_camera, self);

	return self;
}

void liext_cameras_free (
	LIExtModule* self)
{
	lisys_free (self);
}

void liext_cameras_update (
	LIExtModule* self,
	LIAlgCamera* camera,
	float        secs)
{
	lialg_camera_update (camera, secs);
	if (lialg_camera_get_driver (camera) != LIALG_CAMERA_FIRSTPERSON)
		private_camera_clip (self, camera);
}

/*****************************************************************************/

static void private_camera_clip (
	LIExtModule* self,
	LIAlgCamera* camera)
{
	int hit;
	float frac;
	LIMatAabb aabb;
	LIMatTransform start;
	LIMatTransform end;
	LIMatVector diff;
	LIPhyCollision tmp;
	LIPhyPhysics* physics;
	LIPhyShape* shape;

	/* Find the physics manager. */
	physics = limai_program_find_component (self->program, "physics");
	if (physics == NULL)
		return;

	/* Create sweep shape. */
	/* FIXME: Could use a more accurate shape. */
	lialg_camera_get_bounds (camera, &aabb);
	shape = liphy_shape_new (physics);
	if (shape == NULL)
		return;
	liphy_shape_add_aabb (shape, &aabb, NULL);

	/* Sweep the shape. */
	lialg_camera_get_center (camera, &start);
	lialg_camera_get_transform (camera, &end);
	diff = limat_vector_subtract (end.position, start.position);
	hit = liphy_physics_cast_shape (physics, &start, &end, shape,
		LICLI_PHYSICS_GROUP_CAMERA, LIPHY_GROUP_STATICS | LIPHY_GROUP_TILES, NULL, 0, &tmp);
	liphy_shape_free (shape);

	/* Clip the camera. */
	if (hit)
	{
		frac = tmp.fraction * limat_vector_get_length (diff);
		lialg_camera_clip (camera, frac);
	}
}

/** @} */
/** @} */
/** @} */
