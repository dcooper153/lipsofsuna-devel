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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "module.h"

typedef struct _LIExtColumnStick LIExtColumnStick;
struct _LIExtColumnStick
{
	float offset;
	int material;
	int vertex_x;
	int vertex_z;
	LIExtTerrainStick* stick;
};

typedef struct _LIExtColumnVertex LIExtColumnVertex;
struct _LIExtColumnVertex
{
	float splatting;
	LIMatVector coord;
	LIMatVector normal;
};

static void private_copy_slope (
	LIExtTerrainStick* stick,
	LIExtTerrainStick* src);

static int private_cull_wall (
	LIExtTerrainStick**      stick,
	float*                   stick_y,
	float*                   stick_y0,
	float*                   stick_y1,
	int                      vx0,
	int                      vz0,
	int                      vx1,
	int                      vz1,
	const LIExtColumnVertex* bot0,
	const LIExtColumnVertex* bot1,
	const LIExtColumnVertex* top0,
	const LIExtColumnVertex* top1);

static int private_find_closest_stick (
	LIExtTerrainStick* sticks,
	float              y,
	float              range_min,
	float              range_max,
	LIExtColumnStick*  result);

static void private_free_sticks (
	LIExtTerrainStick* sticks);

static void private_insert_quad (
	LIMdlBuilder* builder,
	LIMdlVertex*  quad);

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert);

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove);

static void private_reset_slope (
	LIExtTerrainStick* stick);

static void private_smoothen_stick (
	LIExtTerrainStick* sticks[4],
	const int          order[4],
	const int          vertex_x[4],
	const int          vertex_z[4],
	float              range_start,
	float              range_end);

static void private_validate (
	LIExtTerrainColumn* self);

/*****************************************************************************/

/**
 * \brief Draws a stick at the given Y offset.
 * \param self Terrain column.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param material Terrain material ID.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick (
	LIExtTerrainColumn* self,
	float               world_y,
	float               world_h,
	int                 material)
{
	float length;
	float padding;
	float isect_start_y;
	float isect_end_y;
	LIExtTerrainStick* stick1;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_next;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainStick* isect_first;
	LIExtTerrainStick* isect_last;

	/*************************************************************************/
	/* Intersection range calculation. */

	/* Find the first intersecting stick. */
	isect_start_y = 0.0f;
	stick_prev = NULL;
	for (isect_first = self->sticks ; isect_first != NULL ; isect_first = isect_first->next)
	{
		if (isect_start_y + isect_first->height > world_y)
			break;
		isect_start_y += isect_first->height;
		stick_prev = isect_first;
	}

	/* Find the last intersecting stick. */
	/* This stick is the one that extends past the end point of the added
	   stick. If the column ends before that, it is the last stick of the
	   column. */
	if (isect_first != NULL)
	{
		isect_end_y = isect_start_y;
		for (isect_last = isect_first ; 1 ; isect_last = isect_last->next)
		{
			isect_end_y += isect_last->height;
			if (isect_last->next == NULL)
				break;
			if (isect_end_y >= world_y + world_h)
				break;
		}
	}

	/*************************************************************************/
	/* Special cases for appending. */

	/* Do not add empty sticks to the end of the list. */
	/* The section past the end of the list is automatically considered
	   empty so creating an empty stick at the end would be redundant.
	   Some of the algorithms also expect there to be no empty sticks at
	   the end so we are obliged to avoid that. */
	if (isect_first == NULL && material == 0)
		return 1;

	/* Create a new stick if adding to the end of the list. */
	/* When the stick is appended to the end of the list, we can take a
	   shortcut since no intersections can occur. However, we might have
	   to add an extra padding stick before it if there's a gap between
	   it and the previous stick. */
	if (isect_first == NULL)
	{
		/* Fill the potential gap with an empty stick. */
		padding = world_y - isect_start_y;
		if (padding > LIEXT_STICK_EPSILON)
		{
			stick = liext_terrain_stick_new (0, padding);
			if (stick == NULL)
				return 0;
			if (stick_prev != NULL)
			{
				lisys_assert (stick_prev->material != 0);
				stick_prev->next = stick;
			}
			else
				self->sticks = stick;
			stick_prev = stick;
		}

		/* Append the stick. */
		if (stick_prev != NULL && stick_prev->material == material)
		{
			/* Extend an existing stick. */
			stick_prev->height += world_h;
			private_reset_slope (stick_prev);
		}
		else
		{
			/* Create the new stick. */
			stick = liext_terrain_stick_new (material, world_h);
			if (stick == NULL)
				return 0;
			private_insert_stick (self, stick_prev, stick);
		}

		/* Mark the column as changed. */
		self->stamp++;

		private_validate (self);

		return 1;
	}

	/*************************************************************************/
	/* Intersection range snapping. */

	/* Snap the start and end points to the end point of a stick if very close. */
	/* This will reduce the complexity of the column since pointlessly
	   short sticks will become subject to removal in the later steps.
	   The actual length of the added stick may change slightly to offset
	   for the snapping, but the difference is visually insignificant. */
	padding = world_y - isect_start_y;
	if (padding < LIEXT_STICK_EPSILON)
	{
		/*
		 *   0011111111 ---\  0011111111
		 *      2222222 ---/    22222222
		 */
		world_y = isect_start_y;
		world_h += padding;
	}
	padding = isect_end_y - world_y - world_h;
	if (padding > 0.0f && padding < LIEXT_STICK_EPSILON)
	{
		/*
		 *   1111111100 ---\  1111111100
		 *   2222222    ---/  22222222
		 */
		world_h += padding;
	}

	/*************************************************************************/
	/* Special insertion cases. */

	/* Handle a single stick being overwritten with the same material.
	 *
	 * This is an easy no operation case so skipping it here is easy
	 * and simplifies the rest of the algorithm.
	 *
	 *     1111     1111     1111     1111
	 *   + 1111   + 11     +  11    +   11
	 *   = 1111   = 1111   = 1111   = 1111
	 */
	if (isect_first == isect_last && material == isect_first->material)
		return 1;

	/* Handle the intersection being inside a single stick.
	 *
	 * This necessarily requires special handling since it is the only
	 * case in which an existing stick needs to be split in two.
	 *
	 *     1111
	 *   +  22
	 *   = 1221
	 */
	if (isect_first == isect_last && world_y > isect_start_y && world_y + world_h < isect_end_y)
	{
		stick = liext_terrain_stick_new (material, world_h);
		if (stick == NULL)
			return 0;
		stick1 = liext_terrain_stick_new (isect_first->material, isect_end_y - world_y - world_h);
		if (stick1 == NULL)
		{
			liext_terrain_stick_free (stick);
			return 0;
		}
		stick1->next = isect_first->next;
		stick->next = stick1;
		isect_first->next = stick;
		isect_first->height = world_y - isect_start_y;
		private_copy_slope (stick1, isect_first);
		private_reset_slope (isect_first);

		/* Mark the column as changed. */
		self->stamp++;

		private_validate (self);

		return 1;
	}

	/* Handle a single stick exactly matching the full intersection.
	 *
	 * This case follows the same logic as the regular insertion case at
	 * the bottom of this function. Handling this separately eliminates
	 * memory allocations since it only requires replacements and removals.
	 * 
	 * Note that the next stick may not exist when overwriting the last stick.
	 * If that case, the imaginary next stick consists of the empty material.
	 * This leads to the special case of inserting the empty material leading
	 * to the removal of the last stick.
	 */
	if (isect_first == isect_last && world_y == isect_start_y && world_y + world_h == isect_end_y)
	{
		stick_next = isect_first->next;
		if (stick_next == NULL || stick_next->material != material)
		{
			if (stick_next == NULL && material == 0)
			{
				/*
				 *     33111100
				 *   +   0000
				 *   = 33000000
				 */
				private_remove_stick (self, stick_prev, isect_first);
			}
			else if (stick_prev == NULL || stick_prev->material != material)
			{
				/*
				 *     331111XX
				 *   +   2222
				 *   = 332222XX
				 */
				isect_first->material = material;
				private_reset_slope (isect_first);
			}
			else
			{
				/*
				 *     221111XX
				 *   +   2222
				 *   = 222222XX
				 */
				stick_prev->height += isect_first->height;
				private_reset_slope (stick_prev);
				private_remove_stick (self, stick_prev, isect_first);
			}
		}
		else
		{
			if (stick_prev == NULL || stick_prev->material != material)
			{
				/*
				 *     33111122
				 *   +   2222
				 *   = 33222222
				 */
				stick_next->height += isect_first->height;
				private_copy_slope (stick_prev, stick_next);
				private_remove_stick (self, stick_prev, isect_first);
			}
			else
			{
				/*
				 *     22333322
				 *   +   2222
				 *   = 22222222
				 */
				stick_prev->height += isect_first->height + stick_next->height;
				private_copy_slope (stick_prev, stick_next);
				private_remove_stick (self, stick_prev, isect_first);
				private_remove_stick (self, stick_prev, stick_next);
			}
		}

		/* Mark the column as changed. */
		self->stamp++;

		private_validate (self);

		return 1;
	}

	/*************************************************************************/
	/* Removing intersecting sticks. */

	/* Clip the first intersecting stick. */
	/* The first intersecting stick is often only partially clipped, but
	   it can be fully removed if it intersects for its entire length. */
	padding = world_y - isect_start_y;
	if (padding == 0 && world_h >= isect_first->height)
	{
		/*
		 * 111111
		 * XXXXXX
		 */
		private_remove_stick (self, stick_prev, isect_first);
	}
	else if (padding == 0)
	{
		/*
		 * 111111
		 * XXXX
		 */
		isect_first->height -= world_h;
		lisys_assert (isect_first == isect_last);
		lisys_assert (isect_first->height >= LIEXT_STICK_EPSILON);
	}
	else
	{
		/*
		 * 111111
		 *   XXXX
		 */
		stick_prev = isect_first;
		isect_first->height -= isect_start_y + isect_first->height - world_y;
		lisys_assert (isect_first->height >= LIEXT_STICK_EPSILON);
	}

	/* Remove the intersecting sticks between the first and the last. */
	/* These sticks are always removed since they always intersect fully.
	   However, if only one stick intersected, the removal is already done,
	   and the contents of the stick pointers involved may be invalid. */
	if (isect_first != isect_last)
	{
		if (stick_prev != NULL)
			stick = stick_prev->next;
		else
			stick = self->sticks;
		for ( ; stick != isect_last ; stick = stick_next)
		{
			/*
			 * 111111
			 * XXXXXX
			 */
			stick_next = stick->next;
			private_remove_stick (self, stick_prev, stick);
		}
	}

	/* Clip the last intersecting stick. */
	/* The last intersecting stick may be either fully or partially clipped.
	   However, if only one stick intersected, the clipping is already done,
	   and the contents of the stick pointers involved may be invalid. */
	if (isect_first != isect_last)
	{
		length = isect_end_y - world_y - world_h;
		if (length > 0)
		{
			/*
			 * 111111
			 * XXXX
			 */
			lisys_assert (length >= LIEXT_STICK_EPSILON);
			isect_last->height = length;
		}
		else
		{
			/*
			 * 111111
			 * XXXXXX
			 */
			private_remove_stick (self, stick_prev, isect_last);
		}
	}

	/*************************************************************************/
	/* Inserting the stick into the created gap. */

	/* Create the new stick.
	 *
	 * Due to the special cases being handled above, the following cases
	 * remain to be handled here. These cases require insertion, deleteion
	 * and replacement operations.
	 */
	if (stick_prev != NULL)
		stick_next = stick_prev->next;
	else
		stick_next = self->sticks;
	if (stick_next == NULL || stick_next->material != material)
	{
		if (stick_next == NULL && material == 0)
		{
			/*
			 *     33....00
			 *   +   0000
			 *   = 33000000
			 */
		}
		else if (stick_prev == NULL || stick_prev->material != material)
		{
			/*
			 *     33....XX
			 *   +   2222
			 *   = 332222XX
			 */
			stick = liext_terrain_stick_new (material, world_h);
			if (stick == NULL)
				return 0;
			private_reset_slope (stick_prev);
			private_insert_stick (self, stick_prev, stick);
		}
		else
		{
			/*
			 *     22....XX
			 *   +   2222
			 *   = 222222XX
			 */
			stick_prev->height += world_h;
			private_reset_slope (stick_prev);
		}
	}
	else
	{
		if (stick_prev == NULL || stick_prev->material != material)
		{
			/*
			 *     33....22
			 *   +   2222
			 *   = 33222222
			 */
			stick_next->height += world_h;
		}
		else
		{
			/*
			 *     22....22
			 *   +   2222
			 *   = 22222222
			 */
			stick_prev->height += world_h + stick_next->height;
			private_copy_slope (stick_prev, stick_next);
			private_remove_stick (self, stick_prev, stick_next);
		}
	}

	/* Mark the column as changed. */
	self->stamp++;

	private_validate (self);

	return 1;
}

/**
 * \brief Builds the model of the column.
 * \param self Terrain column.
 * \param sticks_back Neighbour sticks used for culling.
 * \param sticks_front Neighbour sticks used for culling.
 * \param sticks_left Neighbour sticks used for culling.
 * \param sticks_right Neighbour sticks used for culling.
 * \param grid_size Grid size.
 * \return Nonzero on success, zero on failure.
 */
int liext_terrain_column_build_model (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  sticks_back,
	LIExtTerrainStick*  sticks_front,
	LIExtTerrainStick*  sticks_left,
	LIExtTerrainStick*  sticks_right,
	float               grid_size)
{
	float u;
	float v;
	float y;
	float y_back;
	float y_back0;
	float y_back1;
	float y_front;
	float y_front0;
	float y_front1;
	float y_left;
	float y_left0;
	float y_left1;
	float y_right;
	float y_right0;
	float y_right1;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainStick* stick_back;
	LIExtTerrainStick* stick_front;
	LIExtTerrainStick* stick_left;
	LIExtTerrainStick* stick_right;
	LIExtColumnVertex bot[2][2];
	LIExtColumnVertex top[2][2];
	LIMatVector normal;
	LIMdlBuilder* builder;
	LIMdlMaterial material;
	LIMdlVertex quad[4];

	/* Check if changes are needed. */
	if (self->stamp == self->stamp_model)
		return 1;

	/* Allocate the model. */
	if (self->model == NULL)
	{
		self->model = limdl_model_new ();
		if (self->model == NULL)
			return 0;
	}
	else
		limdl_model_clear (self->model);

	/* Allocate the material. */
	if (!limdl_material_init (&material))
		return 0;
	if (!limdl_material_set_material (&material, "stickterrain1"))
	{
		limdl_material_free (&material);
		return 0;
	}

	/* Allocate the model builder. */
	builder = limdl_builder_new (self->model);
	if (builder == NULL)
	{
		limdl_material_free (&material);
		return 0;
	}
	if (!limdl_builder_insert_material (builder, &material))
	{
		limdl_material_free (&material);
		limdl_builder_free (builder);
		return 0;
	}
	limdl_material_free (&material);

	/* Initialize the bottom surface data. */
	y = 0.0f;
	stick_prev = NULL;
	normal = limat_vector_init (0.0f, -1.0f, 0.0f);
	bot[0][0].splatting = 0.0f;
	bot[1][0].splatting = 0.0f;
	bot[0][1].splatting = 0.0f;
	bot[1][1].splatting = 0.0f;
	bot[0][0].coord = limat_vector_init (0.0f, 0.0f, 0.0f);
	bot[1][0].coord = limat_vector_init (grid_size, 0.0f, 0.0f);
	bot[0][1].coord = limat_vector_init (0.0f, 0.0f, grid_size);
	bot[1][1].coord = limat_vector_init (grid_size, 0.0f, grid_size);
	bot[0][0].normal = normal;
	bot[1][0].normal = normal;
	bot[0][1].normal = normal;
	bot[1][1].normal = normal;

	/* Initialize the neighbor iteration. */
	y_back = 0.0f;
	y_back0 = 0.0f;
	y_back1 = 0.0f;
	y_front = 0.0f;
	y_front0 = 0.0f;
	y_front1 = 0.0f;
	y_left = 0.0f;
	y_left0 = 0.0f;
	y_left1 = 0.0f;
	y_right = 0.0f;
	y_right0 = 0.0f;
	y_right1 = 0.0f;
	stick_back = sticks_back;
	stick_front = sticks_front;
	stick_left = sticks_left;
	stick_right = sticks_right;

	/* Add the sticks to the builder. */
	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		/* Calculate the top surface data. */
		y += stick->height;
		top[0][0].splatting = stick->vertices[0][0].splatting;
		top[1][0].splatting = stick->vertices[1][0].splatting;
		top[0][1].splatting = stick->vertices[0][1].splatting;
		top[1][1].splatting = stick->vertices[1][1].splatting;
		top[0][0].coord = limat_vector_init (     0.0f, y + stick->vertices[0][0].offset, 0.0f);
		top[1][0].coord = limat_vector_init (grid_size, y + stick->vertices[1][0].offset, 0.0f);
		top[0][1].coord = limat_vector_init (     0.0f, y + stick->vertices[0][1].offset, grid_size);
		top[1][1].coord = limat_vector_init (grid_size, y + stick->vertices[1][1].offset, grid_size);
		top[0][0].normal = stick->vertices[0][0].normal;
		top[1][0].normal = stick->vertices[1][0].normal;
		top[0][1].normal = stick->vertices[0][1].normal;
		top[1][1].normal = stick->vertices[1][1].normal;

		/* Generate vertices for non-empty sticks. */
		if (stick->material != 0)
		{
			u = (stick->material - 1) / 255.0f;

			/* Left face. */
			if (!private_cull_wall (&stick_left, &y_left, &y_left0, &y_left1,
						1, 0, 1, 1, &bot[0][0], &bot[0][1], &top[0][0], &top[0][1]))
			{
				v = 0.0f / 5.0f;
				normal = limat_vector_init (-1.0f, 0.0f, 0.0f);
				limdl_vertex_init (quad + 0, &bot[0][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 1, &bot[0][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 2, &top[0][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 3, &top[0][0].coord, &normal, u, v);
				private_insert_quad (builder, quad);
			}

			/* Right face. */
			if (!private_cull_wall (&stick_right, &y_right, &y_right0, &y_right1,
						0, 1, 0, 0, &bot[1][1], &bot[1][0], &top[1][1], &top[1][0]))
			{
				v = 1.0f / 5.0f;
				normal = limat_vector_init (1.0f, 0.0f, 0.0f);
				limdl_vertex_init (quad + 0, &bot[1][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 1, &bot[1][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 2, &top[1][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 3, &top[1][1].coord, &normal, u, v);
				private_insert_quad (builder, quad);
			}

			/* Front face. */
			if (!private_cull_wall (&stick_front, &y_front, &y_front0, &y_front1,
						0, 1, 1, 1, &bot[0][0], &bot[1][0], &top[0][0], &top[1][0]))
			{
				v = 2.0f / 5.0f;
				normal = limat_vector_init (0.0f, 0.0f, -1.0f);
				limdl_vertex_init (quad + 0, &bot[0][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 1, &top[0][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 2, &top[1][0].coord, &normal, u, v);
				limdl_vertex_init (quad + 3, &bot[1][0].coord, &normal, u, v);
				private_insert_quad (builder, quad);
			}

			/* Back face. */
			if (!private_cull_wall (&stick_back, &y_back, &y_back0, &y_back1,
						1, 0, 0, 0, &bot[1][1], &bot[0][1], &top[1][1], &top[0][1]))
			{
				v = 3.0f / 5.0f;
				normal = limat_vector_init (0.0f, 0.0f, 1.0f);
				limdl_vertex_init (quad + 0, &bot[1][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 1, &top[1][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 2, &top[0][1].coord, &normal, u, v);
				limdl_vertex_init (quad + 3, &bot[0][1].coord, &normal, u, v);
				private_insert_quad (builder, quad);
			}

			/* Bottom face. */
			if (stick_prev != NULL && stick_prev->material == 0)
			{
				v = 4.0f / 5.0f;
				limdl_vertex_init (quad + 0, &bot[0][0].coord, &bot[0][0].normal, u, v);
				limdl_vertex_init (quad + 1, &bot[1][0].coord, &bot[1][0].normal, u, v);
				limdl_vertex_init (quad + 2, &bot[1][1].coord, &bot[1][1].normal, u, v);
				limdl_vertex_init (quad + 3, &bot[0][1].coord, &bot[0][1].normal, u, v);
				quad[0].color[0] = 255 * (int)(1.0f - bot[0][0].splatting);
				quad[1].color[0] = 255 * (int)(1.0f - bot[1][0].splatting);
				quad[2].color[0] = 255 * (int)(1.0f - bot[1][1].splatting);
				quad[3].color[0] = 255 * (int)(1.0f - bot[0][1].splatting);
				private_insert_quad (builder, quad);
			}

			/* Top face. */
			if (stick->next == NULL || stick->next->material == 0)
			{
				v = 5.0f / 5.0f;
				limdl_vertex_init (quad + 0, &top[0][0].coord, &top[0][0].normal, u, v);
				limdl_vertex_init (quad + 1, &top[0][1].coord, &top[0][1].normal, u, v);
				limdl_vertex_init (quad + 2, &top[1][1].coord, &top[1][1].normal, u, v);
				limdl_vertex_init (quad + 3, &top[1][0].coord, &top[1][0].normal, u, v);
				quad[0].color[0] = 255 * (int)(1.0f - stick->vertices[0][0].splatting);
				quad[1].color[0] = 255 * (int)(1.0f - stick->vertices[0][1].splatting);
				quad[2].color[0] = 255 * (int)(1.0f - stick->vertices[1][1].splatting);
				quad[3].color[0] = 255 * (int)(1.0f - stick->vertices[1][0].splatting);
				private_insert_quad (builder, quad);
			}
		}

		/* Calculate the new bottom surface data. */
		bot[0][0] = top[0][0];
		bot[1][0] = top[1][0];
		bot[0][1] = top[0][1];
		bot[1][1] = top[1][1];
		bot[0][0].normal = limat_vector_multiply (top[0][0].normal, -1.0f);
		bot[1][0].normal = limat_vector_multiply (top[1][0].normal, -1.0f);
		bot[0][1].normal = limat_vector_multiply (top[0][1].normal, -1.0f);
		bot[1][1].normal = limat_vector_multiply (top[1][1].normal, -1.0f);
	}

	/* Finish the build. */
	limdl_builder_finish (builder);
	limdl_builder_free (builder);
	self->stamp_model = self->stamp;

	return 1;
}

/**
 * \brief Frees all the sticks of the column.
 * \param self Terrain column.
 */
void liext_terrain_column_clear (
	LIExtTerrainColumn* self)
{
	private_free_sticks (self->sticks);
	self->sticks = NULL;
	if (self->model != NULL)
	{
		limdl_model_free (self->model);
		self->model = NULL;
	}
}

/**
 * \brief Smoothens the vertices of the four columns in the given Y range.
 * \param self Terrain column (0,0).
 * \param c10 Terrain column (1,0).
 * \param c01 Terrain column (0,1).
 * \param c11 Terrain column (1,1).
 * \param y Y range start.
 * \param h Y range height.
 */
void liext_terrain_column_smoothen (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11,
	float               y,
	float               h)
{
	const int order1[4] = { 0, 1, 2, 3 };
	const int order2[4] = { 1, 0, 2, 3 };
	const int order3[4] = { 2, 0, 1, 3 };
	const int order4[4] = { 3, 0, 1, 2 };
	const int vertex_x[4] = { 1, 0, 1, 0 };
	const int vertex_z[4] = { 1, 1, 0, 0 };
	LIExtTerrainStick* sticks[4];

	/* Find the first sticks that can affect the range. */
	/* TODO: This would be a big optimization. */
	/* TODO: When done, the Y offset of the stick needs to be passed to smoothen_stick. */
	sticks[0] = self->sticks;
	sticks[1] = c10->sticks;
	sticks[2] = c01->sticks;
	sticks[3] = c11->sticks;

	/* Smoothing is done for one vertex chain at a time. The smoothed
	 * vertices are at the intersection of four columns so we need to
	 * iterate through them all when we smooth.
	 *
	 *   + - - - + - - - +
	 * 
	 *   |  0,1  |  1,1  |     X,Z = column
	 * 
	 *   + - - - # - - - +      +  = vertex
	 * 
	 *   |  0,0  |  1,0  |      #  = smoothed vertex
	 * 
	 *   + - - - + - - - +
	 *
	 * Smoothing is done by averaging the Y offsets of the sticks of the
	 * four columns. In other words, the stick heights are the only
	 * variable that affects the vertex positions.
	 * 
	 *       0,1   1,1         X,Z = column
	 *          \ /
	 *           #             \ / = affects
	 *          / \
	 *       0,0   1,0          #  = smoothed vertex
	 */

	private_smoothen_stick (sticks, order1, vertex_x, vertex_z, y, y + h);
	private_smoothen_stick (sticks, order2, vertex_x, vertex_z, y, y + h);
	private_smoothen_stick (sticks, order3, vertex_x, vertex_z, y, y + h);
	private_smoothen_stick (sticks, order4, vertex_x, vertex_z, y, y + h);
}

/**
 * \brief Gets the serialized contents of the column.
 * \param self Terrain column.
 * \param writer Archive write.
 * \return Nonzero on success.
 */
int liext_terrain_column_get_data (
	LIExtTerrainColumn* self,
	LIArcWriter*        writer)
{
	LIExtTerrainStick* stick;

	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		if (!liarc_writer_append_uint8 (writer, 1))
			return 0;
		if (!liext_terrain_stick_get_data (stick, writer))
			return 0;
	}
	if (!liarc_writer_append_uint8 (writer, 0))
		return 0;

	return 1;
}

/**
 * \brief Sets the contents of the column from serialized data.
 * \param self Terrain column.
 * \param reader Archive reader.
 * \return Nonzero on success.
 */
int liext_terrain_column_set_data (
	LIExtTerrainColumn* self,
	LIArcReader*        reader)
{
	int x;
	int z;
	uint8_t tmp;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* sticks_first;
	LIExtTerrainStick* sticks_last;

	/* Read the sticks. */
	sticks_first = NULL;
	sticks_last = NULL;
	while (1)
	{
		/* Check for more data. */
		if (!liarc_reader_get_uint8 (reader, &tmp))
		{
			private_free_sticks (sticks_first);
			return 0;
		}
		if (!tmp)
			break;

		/* Allocate a new stick. */
		stick = liext_terrain_stick_new (0, 1.0f);
		if (stick == NULL)
		{
			private_free_sticks (sticks_first);
			return 0;
		}

		/* Read the contents of the stick. */
		if (!liext_terrain_stick_set_data (stick, reader))
		{
			private_free_sticks (sticks_first);
			return 0;
		}

		/* Validate the vertex offsets. */
		/* The above call validates the stick data internally but not against
		   the previous stick. Here, we finish validation by checking that the
		   vertex offsets of the previous stick do not extend above the bottom
		   of this stick. */
		if (sticks_last != NULL)
		{
			for (z = 0 ; z < 2 ; z++)
			{
				for (x = 0 ; x < 2 ; x++)
				{
					if (sticks_last->vertices[x][z].offset > stick->height)
						sticks_last->vertices[x][z].offset = stick->height;
				}
			}
		}

		/* Append the stick to the linked list. */
		if (sticks_last != NULL)
		{
			sticks_last->next = stick;
			sticks_last = stick;
		}
		else
		{
			sticks_first = stick;
			sticks_last = stick;
		}
	}

	/* Use the new sticks. */
	liext_terrain_column_clear (self);
	self->sticks = sticks_first;
	self->stamp++;

	return 1;
}

/*****************************************************************************/

static void private_copy_slope (
	LIExtTerrainStick* stick,
	LIExtTerrainStick* src)
{
	if (stick == NULL)
		return;
	if (src != NULL)
		liext_terrain_stick_copy_vertices (stick, src);
	else
		liext_terrain_stick_reset_vertices (stick);
}

static int private_cull_wall (
	LIExtTerrainStick**      stick,
	float*                   stick_y,
	float*                   stick_y0,
	float*                   stick_y1,
	int                      vx0,
	int                      vz0,
	int                      vx1,
	int                      vz1,
	const LIExtColumnVertex* bot0,
	const LIExtColumnVertex* bot1,
	const LIExtColumnVertex* top0,
	const LIExtColumnVertex* top1)
{
	float stick_y_top;
	float stick_y0_top;
	float stick_y1_top;
	LIExtTerrainStick* s;

	/* Check that there are neighbor sticks left. */
	/* If the wall starts above all the neighbor sticks or there are no
	   neighbors at all, then no culling can be done. */
	if (*stick == NULL)
		return 0;

	/* Check for the bottom culling offset. */
	/* If an empty stick caused the stick pointer to start above the bottom
	   of the culled wall, then no culling can be done. */
	if (*stick_y0 > bot0->coord.y || *stick_y1 > bot1->coord.y)
		return 0;

	/* Find the bottom of the neighbor wall. */
	/* The culled wall may start well above the current stick pointer.
	   To simplify things and reduce future iteration, the stick pointer
	   and the Y offsets are rewound to the first neighbor stick that
	   starts below the wall. */
	stick_y_top = *stick_y;
	stick_y0_top = *stick_y0;
	stick_y1_top = *stick_y1;
	for ( ; *stick != NULL ; *stick = (*stick)->next)
	{
		stick_y_top += (*stick)->height;
		stick_y0_top = stick_y_top + (*stick)->vertices[vx0][vz0].offset;
		stick_y1_top = stick_y_top + (*stick)->vertices[vx1][vz1].offset;
		if (stick_y0_top > bot0->coord.y || stick_y1_top > bot1->coord.y)
			break;
		*stick_y = stick_y_top;
		*stick_y0 = stick_y0_top;
		*stick_y1 = stick_y1_top;
	}
	if (*stick == NULL)
		return 0;
	lisys_assert(*stick_y0 <= bot0->coord.y);
	lisys_assert(*stick_y1 <= bot1->coord.y);

	/* Skip empty sticks. */
	/* The stick pointer is currently at the bottommost stick that is
	   still below the bottom edge of the wall. If it is an empty stick,
	   the culling fails as the bottom of the wall is not occluded. */
	if ((*stick)->material == 0)
	{
		*stick_y += (*stick)->height;
		*stick_y0 = *stick_y + (*stick)->vertices[vx0][vz0].offset;
		*stick_y1 = *stick_y + (*stick)->vertices[vx1][vz1].offset;
		*stick = (*stick)->next;
		return 0;
	}

	/* Find the top of the neighbor wall. */
	/* If the neighbor wall extends past the culled wall, then culling
	   should be done. If an empty stick or the end of the column occur
	   before that, no culling can be done. */
	stick_y_top = *stick_y;
	stick_y0_top = *stick_y0;
	stick_y1_top = *stick_y1;
	for (s = *stick ; s != NULL && s->material != 0 ; s = s->next)
	{
		stick_y_top += s->height;
		stick_y0_top = stick_y_top + s->vertices[vx0][vz0].offset;
		stick_y1_top = stick_y_top + s->vertices[vx1][vz1].offset;
		if (stick_y0_top >= top0->coord.y && stick_y1_top >= top1->coord.y)
			return 1;
	}

	return 0;
}

static int private_find_closest_stick (
	LIExtTerrainStick* sticks,
	float              y,
	float              range_min,
	float              range_max,
	LIExtColumnStick*  result)
{
	float ys = 0.0f;
	float dist;
	float dist_best = 0.0f;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_best = NULL;

	for (stick = sticks ; stick != NULL ; stick = stick->next)
	{
		ys += stick->height;
		if (ys < range_min)
			continue;
		if (ys >= range_max)
			break;
		dist = LIMAT_ABS (ys - y);
		if (stick_best == NULL || dist_best >= dist)
		{
			result->offset = ys;
			result->material = stick->material;
			result->stick = stick;
			dist_best = dist;
			stick_best = stick;
		}
	}

	return stick_best != NULL;
}

static void private_free_sticks (
	LIExtTerrainStick* sticks)
{
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_next;

	for (stick = sticks ; stick != NULL ; stick = stick_next)
	{
		stick_next = stick->next;
		liext_terrain_stick_free (stick);
	}
}

static void private_insert_quad (
	LIMdlBuilder* builder,
	LIMdlVertex*  quad)
{
	LIMdlVertex vertices[3];

	quad[0].color[1] =   0; quad[0].color[2] = 0;
	quad[1].color[1] = 255; quad[1].color[2] = 0;
	quad[2].color[1] = 255; quad[2].color[2] = 255;
	quad[3].color[1] =   0; quad[3].color[2] = 255;
	vertices[0] = quad[0];
	vertices[1] = quad[1];
	vertices[2] = quad[2];
	limdl_builder_insert_face (builder, 0, 0, vertices, NULL);
	vertices[0] = quad[0];
	vertices[1] = quad[2];
	vertices[2] = quad[3];
	limdl_builder_insert_face (builder, 0, 0, vertices, NULL);
}

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert)
{
	if (prev == NULL)
	{
		insert->next = self->sticks;
		self->sticks = insert;
	}
	else
	{
		insert->next = prev->next;
		prev->next = insert;
	}
}

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove)
{
	if (prev == NULL)
	{
		lisys_assert (self->sticks == remove);
		self->sticks = remove->next;
	}
	else
	{
		lisys_assert (prev->next == remove);
		prev->next = remove->next;
	}
	liext_terrain_stick_free (remove);
}

static void private_reset_slope (
	LIExtTerrainStick* stick)
{
	if (stick == NULL)
		return;
	liext_terrain_stick_reset_vertices (stick);
}

static void private_smoothen_stick (
	LIExtTerrainStick* sticks[4],
	const int          order[4],
	const int          vertex_x[4],
	const int          vertex_z[4],
	float              range_start,
	float              range_end)
{
	int i;
	int count;
	int dst_vertex_x;
	int dst_vertex_z;
	int material;
	float ys;
	float offset;
	float splatting;
	float range_min;
	float range_max;
	LIExtTerrainStick* stick;
	LIExtColumnStick tmp[4];
	LIMatVector normal;
	LIMatVector normal1;

	dst_vertex_x = vertex_x[order[0]];
	dst_vertex_z = vertex_z[order[0]];

	for (ys = 0.0f, stick = sticks[order[0]] ; stick != NULL; stick = stick->next)
	{
		/* Limit to the allowed range. */
		ys += stick->height;
		if (ys < range_start)
			continue;
		if (ys >= range_end)
			break;

		/* Find the connected sticks. */
		tmp[0].offset = ys;
		tmp[0].material = stick->material;
		tmp[0].vertex_x = dst_vertex_x;
		tmp[0].vertex_z = dst_vertex_z;
		tmp[0].stick = stick;
		count = 1;
		for (i = 1 ; i < 4 ; i++)
		{
			if (private_find_closest_stick (sticks[order[i]], ys, range_start, range_end, tmp + count))
			{
				tmp[count].vertex_x = vertex_x[order[i]];
				tmp[count].vertex_z = vertex_z[order[i]];
				count++;
			}
		}

		/* Calculate the smoothed position. */
		offset = 0.0f;
		for (i = 0 ; i < count ; i++)
			offset += tmp[i].offset;
		offset = offset / count;

		/* Clamp the position for each stick. */
		range_min = range_start;
		range_max = range_end;
		for (i = 0 ; i < count ; i++)
		{
			range_min = LIMAT_MAX (range_min, tmp[i].offset - tmp[i].stick->height);
			range_max = LIMAT_MIN (range_max, tmp[i].offset + 10.0f);
			if (tmp[i].stick->next != NULL)
				range_max = LIMAT_MIN (range_max, tmp[i].offset + tmp[i].stick->next->height);
		}

		/* Set the position for each stick. */
		if (range_min <= range_max)
		{
			offset = LIMAT_MAX (range_min, offset);
			offset = LIMAT_MIN (range_max, offset);
			for (i = 0 ; i < count ; i++)
			{
				tmp[i].stick->vertices[tmp[i].vertex_x][tmp[i].vertex_z].offset =
					offset - tmp[i].offset;
			}
		}

		/* Calculate the splatting factor. */
		splatting = 0.0f;
		material = tmp[0].material;
		for (i = 1 ; i < count ; i++)
		{
			if (tmp[i].material != material)
			{
				if (material != 0)
				{
					splatting = 1.0f;
					break;
				}
				else
					material = tmp[i].material;
			}
		}
		stick->vertices[dst_vertex_x][dst_vertex_z].splatting = splatting;

		/* Calculate the new vertex normal. */
		normal = limat_vector_init (0.0f, 0.0f, 0.0f);
		for (i = 0 ; i < count ; i++)
		{
			liext_terrain_stick_get_normal (tmp[i].stick, &normal1);
			normal = limat_vector_add (normal, normal1);
		}
		normal = limat_vector_normalize (normal);
		for (i = 0 ; i < count ; i++)
		{
			tmp[i].stick->vertices[tmp[i].vertex_x][tmp[i].vertex_z].normal = normal;
		}
	}
}

static void private_validate (
	LIExtTerrainColumn* self)
{
#ifndef NDEBUG
	int material = -1;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick1;

	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		if (stick->material == material || stick->height <= 0.0f)
		{
			printf ("FATAL: Invalid terrain column detected!\n");
			for (stick1 = self->sticks ; stick1 != NULL ; stick1 = stick1->next)
			{
				printf ("  material=%d, height=%f", stick1->material, stick1->height);
				if (stick1 == stick)
					printf (" <- HERE!\n");
				else
					printf ("\n");
			}
			lisys_assert (0 && "Terrain management logic error");
		}
		material = stick->material;
	}
#endif
}

/** @} */
/** @} */
