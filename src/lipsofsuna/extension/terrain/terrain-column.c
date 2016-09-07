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
 * \addtogroup LIExt Extension
 * @{
 * \addtogroup LIExtTerrain Terrain
 * @{
 */

#include "lipsofsuna/extension/noise/ext-module.h"
#include "terrain-column.h"
#include "terrain-face-iterator.h"
#include "terrain-types.h"

#if 0
#define DEBUGPRINT printf
#else
#define DEBUGPRINT(...)
#endif

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

static int private_find_closest_stick (
	LIExtTerrainStick* sticks,
	float              y,
	float              range_min,
	float              range_max,
	LIExtColumnStick*  result);

static void private_free_sticks (
	LIExtTerrainStick* sticks);

static void private_generate_grass (
	LIExtTerrainColumn*         self,
	LIMdlBuilder*               builder,
	const LIExtTerrainMaterial* material,
	LIExtColumnVertex           top[2][2],
	const LIMatVector*          offset);

static void private_insert_quad (
	LIMdlBuilder* builder,
	int           material,
	LIMdlVertex*  quad);

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert);

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove);

static void private_smoothen_stick (
	LIExtTerrainStick* sticks[4],
	const int          order[4],
	const int          vertex_x[4],
	const int          vertex_z[4],
	float              range_start,
	float              range_end);

/*****************************************************************************/

/**
 * \brief Draws a stick at the given Y offset.
 * \param self Terrain column.
 * \param world_y Y offset of the stick in world units.
 * \param world_h Y height of the stick in world units.
 * \param slope_bot Array of 4 floats, denoting the vertex offsets of the bottom slope.
 * \param slope_top Array of 4 floats, denoting the vertex offsets of the top slope.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick (
	LIExtTerrainColumn*     self,
	float                   world_y,
	float                   world_h,
	const float*            slope_bot,
	const float*            slope_top,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	float y;
	float dy;
	float dh;
	float length;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick1;
	LIExtTerrainStick* stick2;
	LIExtTerrainStick* stick_prev;

	/* Make sure that the material is valid. */
	if (material >= LIEXT_TERRAIN_MATERIAL_MAX)
		material = LIEXT_TERRAIN_MATERIAL_MAX - 1;

	y = 0.0f;
	stick = self->sticks;
	stick_prev = NULL;
	self->stamp++;

	/* Subtract the stick from the column. */
	DEBUGPRINT ("\nAdd stick: y=%.2f   h=%.2f   m=%d\n", world_y, world_h, material);
	while (stick != NULL)
	{
		DEBUGPRINT (" Stick: y=%.2f   h=%.2f   m=%d\n", y, stick->height, stick->material);
		dy = world_y - y;
		dh = world_h;

		/* Skip filtered sticks. */
		if (filter_func != NULL && !filter_func (stick, filter_data))
		{
			y += stick->height;
			stick_prev = stick;
			stick = stick->next;
			continue;
		}

		/* Modify intersecting sticks. */
		switch (liext_terrain_stick_get_intersection_type (stick, dy, dh))
		{
			/* Out of memory? */
			case 0:
				return 0;

			/* A) Is the stick completely below us?
			 * 
			 * .....SSSSSSSSSS.....
			 * XXX??...............
			 * 
			 * Iteration is ended as this and all the remaining sticks are
			 * not affected.
			 */
			case 1:
				DEBUGPRINT ("  1-below: y=%.2f..%.2f\n", y, y + stick->height);
				return 1;

			/* B) Is the stick completely above us?
			 * 
			 * .....SSSSSSSSSS.....
			 * ...............??XXX
			 * 
			 * The top surface of the current stick is adjusted when necessary.
			 */
			case 2:
				DEBUGPRINT ("  2-above: y=%.2f..%.2f\n", y, y + stick->height);
				liext_terrain_stick_set_vertices_min (stick, slope_bot, dy - stick->height);
				liext_terrain_stick_clamp_vertices_bottom (stick, stick_prev);
				y += stick->height;
				stick_prev = stick;
				stick = stick->next;
				break;

			/* C) Does the stick replace us completely?
			 * 
			 * .....SSSSSSSSSS.....
			 * ..???XXXXXXXXXX???..
			 * 
			 * The material of the current stick is changed and the top surface
			 * adjusted. If the previous stick uses the same material, the
			 * stick is merged with it. Likewise, if the next stick uses the
			 * same material, the stick is merged with it.
			 * 
			 * As a special case, if the replaced stick is the last one in the
			 * column and the replacement material is zero, the stick is
			 * removed and interation ended. This case needs to be handled
			 * separately to avoid creating trailing empty sticks.
			 */
			case 3:
				DEBUGPRINT ("  3-full: y=%.2f..%.2f\n", y, y + stick->height);
				if (stick->material == material)
				{
					DEBUGPRINT ("   Same material.\n");
					/* Update the top surface. */
					liext_terrain_stick_set_vertices_max (stick, slope_top, dy + dh - stick->height);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* Advance to the next stick. */
					y += stick->height;
					stick_prev = stick;
					stick = stick->next;
				}
				else if (stick_prev  != NULL && stick_prev ->material == material &&
				         stick->next != NULL && stick->next->material == material)
				{
					DEBUGPRINT ("   Join up and down.\n");
					/* Extend the previous stick. */
					liext_terrain_stick_copy_vertices (stick_prev, stick->next);
					length = stick->height + stick->next->height;
					stick_prev->height += length;
					liext_terrain_stick_clamp_vertices (stick_prev, stick->next->next);
					/* Delete the current and next sticks. */
					private_remove_stick (self, stick, stick->next);
					private_remove_stick (self, stick_prev, stick);
					/* Advance to the next stick. */
					y += length;
					stick = stick_prev->next;
				}
				else if (stick_prev != NULL && stick_prev->material == material)
				{
					DEBUGPRINT ("   Join down.\n");
					/* Extend the previous stick. */
					length = stick->height;
					stick_prev->height += length;
					liext_terrain_stick_copy_vertices (stick_prev, stick);
					liext_terrain_stick_clamp_vertices (stick_prev, stick->next);
					/* Delete the current stick. */
					private_remove_stick (self, stick_prev, stick);
					/* Advance to the next stick. */
					y += length;
					stick = stick_prev->next;
				}
				else if (stick->next != NULL && stick->next->material == material)
				{
					DEBUGPRINT ("   Join up.\n");
					/* Extend the current stick. */
					stick->height += stick->next->height;
					/* Delete the next stick. */
					private_remove_stick (self, stick, stick->next);
				}
				else if (stick->next == NULL && material == 0)
				{
					DEBUGPRINT ("   Delete trailing.\n");
					/* Delete the trailing empty stick. */
					private_remove_stick (self, stick_prev, stick);
					/* End iteration. */
					return 1;
				}
				else
				{
					DEBUGPRINT ("   Replace normally.\n");
					/* Replace the current stick. */
					stick->material = material;
					/* Update the top surface. */
					liext_terrain_stick_set_vertices_max (stick, slope_top, dy + dh - stick->height);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* Advance to the next stick. */
					y += stick->height;
					stick_prev = stick;
					stick = stick->next;
				}
				break;

			/* D) Does the stick replace part of the bottom?
			 * 
			 * .....SSSSSSSSSS.....
			 * ..???XXXXX..........
			 * 
			 * The current stick is shortened by the length of the intersection
			 * and a new stick is created before it. The newly created stick
			 * may be merged with the previous stick.
			 */
			case 4:
				DEBUGPRINT ("  4-bottom: y=%.2f..%.2f\n", y, y + stick->height);
				if (stick->material == material)
				{
					DEBUGPRINT ("   Same material.\n");
					/* Update the top surface. */
					liext_terrain_stick_set_vertices_max (stick, slope_top, dy + dh - stick->height);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* Advance to the next stick. */
					y += stick->height;
					stick_prev = stick;
					stick = stick->next;
				}
				else if (stick_prev != NULL && stick_prev->material == material)
				{
					DEBUGPRINT ("   Join down\n");
					/* Extend the previous stick. */
					stick_prev->height += dy + dh;
					liext_terrain_stick_move_vertices (stick_prev, -dy - dh);
					liext_terrain_stick_set_vertices_max (stick_prev, slope_top, 0.0f);
					/* Shorten the current stick. */
					stick->height -= dy + dh;
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					liext_terrain_stick_clamp_vertices (stick_prev, stick);
					/* Advance to the next stick. */
					y += dy + dh + stick->height;
					stick_prev = stick;
					stick = stick->next;
				}
				else
				{
					DEBUGPRINT ("   Insert normally.\n");
					/* Create the new stick. */
					stick1 = liext_terrain_stick_new (material, dy + dh);
					if (stick1 == NULL)
						return 0;
					liext_terrain_stick_set_vertices (stick1, slope_top, 0.0f);
					private_insert_stick (self, stick_prev, stick1);
					/* Shorten the current stick. */
					stick->height -= dy + dh;
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					liext_terrain_stick_clamp_vertices (stick1, stick);
					/* Advance to the next stick. */
					y += stick->height;
					stick_prev = stick;
					stick = stick1;
				}
				break;

			/* E) Does the stick replace part of the top?
			 * 
			 * .....SSSSSSSSSS.....
			 * ..........XXXXX???..
			 *
			 * The current stick is shortened by the length of the intersection
			 * and a new stick is created after it. The newly created stick
			 * may be merged with the next stick.
			 * 
			 * As a special case, if the shortened stick is the last one in the
			 * column and the replacement material is zero, no new stick is
			 * created and iteration ends. This case needs to be handled
			 * separately to avoid creating trailing empty sticks.
			 */
			case 5:
				DEBUGPRINT ("  5-top: y=%.2f..%.2f\n", y, y + stick->height);
				if (stick->material == material)
				{
					DEBUGPRINT ("   Same material.\n");
					/* Update the top surface. */
					liext_terrain_stick_set_vertices_max (stick, slope_top, dy + dh - stick->height);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* Advance to the next stick. */
					y += stick->height;
					stick_prev = stick;
					stick = stick->next;
				}
				else if (stick->next == NULL && material == 0)
				{
					DEBUGPRINT ("   Shorten last.\n");
					/* Shorten the current stick. */
					liext_terrain_stick_move_vertices (stick, stick->height - dy);
					liext_terrain_stick_set_vertices_min (stick, slope_bot, 0.0f);
					stick->height = dy;
					liext_terrain_stick_clamp_vertices_bottom (stick, stick_prev);
					/* End iteration. */
					return 1;
				}
				else if (stick->next != NULL && stick->next->material == material)
				{
					DEBUGPRINT ("   Join up.\n");
					/* Extend the next stick. */
					liext_terrain_stick_set_vertices_max (stick->next, slope_top, dy + dh - stick->height - stick->next->height);
					stick->next->height += stick->height - dy;
					/* Shorten the current stick. */
					liext_terrain_stick_move_vertices (stick, stick->height - dy);
					liext_terrain_stick_set_vertices_min (stick, slope_bot, 0.0f);
					stick->height = dy;
					liext_terrain_stick_clamp_vertices_bottom (stick, stick_prev);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					liext_terrain_stick_clamp_vertices (stick->next->next, stick->next->next);
					/* Advance to the next stick. */
					y += dy;
					stick_prev = stick;
					stick = stick->next;
				}
				else
				{
					DEBUGPRINT ("   Insert normally.\n");
					/* Create the new stick. */
					stick1 = liext_terrain_stick_new (material, stick->height - dy);
					if (stick == NULL)
						return 0;
					liext_terrain_stick_set_vertices (stick1, slope_top, dy + dh - stick->height);
					private_insert_stick (self, stick, stick1);
					/* Shorten the current stick. */
					liext_terrain_stick_move_vertices (stick, dy - stick->height);
					liext_terrain_stick_set_vertices_max (stick, slope_bot, 0.0f);
					stick->height = dy;
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* Advance to the next stick. */
					y += dy;
					stick_prev = stick;
					stick = stick1;
				}
				break;

			/* F) Does the stick replace part of the middle?
			 * 
			 * .....SSSSSSSSSS.....
			 * ........XXXX........
			 *
			 * The stick is split in three parts and the part in the middle is
			 * given the requested material. If the material is the same as
			 * the original, no splitting is done. Iteration always ends after
			 * this since no other sticks will ever intersect.
			 */
			case 6:
				DEBUGPRINT ("  6-middle: y=%.2f..%.2f\n", y, y + stick->height);
				if (stick->material == material)
				{
					DEBUGPRINT ("   Same material.\n");
					/* Update the top surface. */
					liext_terrain_stick_set_vertices_max (stick, slope_top, dy + dh - stick->height);
					liext_terrain_stick_clamp_vertices (stick, stick->next);
					/* End iteration. */
					return 1;
				}
				else
				{
					DEBUGPRINT ("   Insert normally.\n");
					/* Create the middle stick. */
					stick1 = liext_terrain_stick_new (material, dh);
					if (stick1 == NULL)
						return 0;
					liext_terrain_stick_set_vertices (stick1, slope_top, 0.0f);
					private_insert_stick (self, stick, stick1);
					/* Create the end stick. */
					stick2 = liext_terrain_stick_new (stick->material, stick->height - dy - dh);
					if (stick2 == NULL)
						return 0;
					liext_terrain_stick_copy_vertices (stick2, stick);
					liext_terrain_stick_set_vertices_max (stick2, slope_top, dy + dh - stick->height);
					private_insert_stick (self, stick1, stick2);
					/* Adjust the start stick. */
					liext_terrain_stick_move_vertices (stick, stick->height - dy);
					liext_terrain_stick_set_vertices_min (stick, slope_bot, 0.0f);
					liext_terrain_stick_clamp_vertices_bottom (stick, stick_prev);
					stick->height = dy;
					liext_terrain_stick_clamp_vertices (stick2, stick2->next);
					liext_terrain_stick_clamp_vertices (stick1, stick2);
					liext_terrain_stick_clamp_vertices (stick, stick1);
					/* End iteration. */
					return 1;
				}
				break;

			/* Should not happen. */
			default:
				lisys_assert (0);
				return 0;
		}
	}

	/* Calculate the length of the remaining stick. */
	if (world_y >= y)
		dh = world_h;
	else
		dh = world_y + world_h - y;

	/* Append the rest of the stick to the end of the column. */
	if (dh > 0.0f)
	{
		/* Append after the last stick. */
		DEBUGPRINT (" Append h=%.2f!\n", dh);
		if (material == 0)
		{
			DEBUGPRINT ("  Append empty: y=%.2f\n", y);
			return 1;
		}
		if (world_y - y > 0.0f)
		{
			DEBUGPRINT ("  Create padding stick: y=%.2f h=%.2f\n", y, world_y - y);
			stick = liext_terrain_stick_new (0, world_y - y);
			if (stick == NULL)
				return 0;
			liext_terrain_stick_set_vertices (stick, slope_bot, 0.0f);
			private_insert_stick (self, stick_prev, stick);
			stick_prev = stick;
			y += stick->height;
		}
		if (stick_prev != NULL && stick_prev->material == material)
		{
			DEBUGPRINT ("  Extend last stick: y=%.2f h=%.2f\n", y, dh);
			stick_prev->height += dh;
			liext_terrain_stick_move_vertices (stick_prev, -world_h);
			liext_terrain_stick_set_vertices_max (stick_prev, slope_top, 0.0f);
		}
		else
		{
			DEBUGPRINT ("  Append new stick: y=%.2f h=%.2f\n", y, dh);
			stick = liext_terrain_stick_new (material, dh);
			if (stick == NULL)
				return 0;
			liext_terrain_stick_set_vertices (stick, slope_top, 0.0f);
			private_insert_stick (self, stick_prev, stick);
		}
	}
	else
		DEBUGPRINT (" No append.\n");

	return 1;
}

/**
 * \brief Draws a stick with the given vertex offsets.
 * \param self Terrain column.
 * \param bot00 Bottom vertex Y coordinate, in world units.
 * \param bot10 Bottom vertex Y coordinate, in world units.
 * \param bot01 Bottom vertex Y coordinate, in world units.
 * \param bot11 Bottom vertex Y coordinate, in world units.
 * \param top00 Top vertex Y coordinate, in world units.
 * \param top10 Top vertex Y coordinate, in world units.
 * \param top01 Top vertex Y coordinate, in world units.
 * \param top11 Top vertex Y coordinate, in world units.
 * \param material Terrain material ID.
 * \param filter_func Filter function for choosing what sticks to modify.
 * \param filter_data Userdata to be passed to the filter function.
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick_corners (
	LIExtTerrainColumn*     self,
	float                   bot00,
	float                   bot10,
	float                   bot01,
	float                   bot11,
	float                   top00,
	float                   top10,
	float                   top01,
	float                   top11,
	int                     material,
	LIExtTerrainStickFilter filter_func,
	void*                   filter_data)
{
	float bot;
	float top;
	float slope_bot[4];
	float slope_top[4];

	/* Make sure that the material is valid. */
	if (material >= LIEXT_TERRAIN_MATERIAL_MAX)
		material = LIEXT_TERRAIN_MATERIAL_MAX - 1;

	/* Calculate the stick offset and height. */
	/* We estimate the offset and height of the stick by taking the average
	   of the bottom and top surface offsets. The vertices are then offset
	   against the end points of the stick. */
	bot = (bot00 + bot10 + bot01 + bot11) / 4.0f;
	top = (top00 + top10 + top01 + top11) / 4.0f;
	if (bot >= top)
		return 0;
	slope_bot[0] = LIMAT_MIN (bot00, top) - bot;
	slope_bot[1] = LIMAT_MIN (bot10, top) - bot;
	slope_bot[2] = LIMAT_MIN (bot01, top) - bot;
	slope_bot[3] = LIMAT_MIN (bot11, top) - bot;
	slope_top[0] = LIMAT_MAX (top00, bot + slope_bot[0]) - top;
	slope_top[1] = LIMAT_MAX (top10, bot + slope_bot[1]) - top;
	slope_top[2] = LIMAT_MAX (top01, bot + slope_bot[2]) - top;
	slope_top[3] = LIMAT_MAX (top11, bot + slope_bot[3]) - top;

	/* Create the stick. */
	return liext_terrain_column_add_stick (self, bot, top - bot, slope_bot, slope_top, material, filter_func, filter_data);
}

/**
 * \brief Builds the model of the column.
 * \param self Terrain column.
 * \param materials Terrain materials.
 * \param sticks_back Neighbour sticks used for culling.
 * \param sticks_front Neighbour sticks used for culling.
 * \param sticks_left Neighbour sticks used for culling.
 * \param sticks_right Neighbour sticks used for culling.
 * \param grid_size Grid size.
 * \param offset Offset of the column, in world units.
 * \return Nonzero on success, zero on failure.
 */
int liext_terrain_column_build_model (
	LIExtTerrainColumn*         self,
	const LIExtTerrainMaterial* materials,
	LIExtTerrainStick*          sticks_back,
	LIExtTerrainStick*          sticks_front,
	LIExtTerrainStick*          sticks_left,
	LIExtTerrainStick*          sticks_right,
	float                       grid_size,
	const LIMatVector*          offset)
{
	float s;
	float y;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainFaceIterator left;
	LIExtTerrainFaceIterator right;
	LIExtTerrainFaceIterator front;
	LIExtTerrainFaceIterator back;
	const LIExtTerrainMaterial* material;
	LIExtColumnVertex bot[2][2];
	LIExtColumnVertex top[2][2];
	LIMatVector normal;
	LIMdlBuilder* builder;
	LIMdlMaterial material1;
	LIMdlMaterial material2;
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
	if (!limdl_material_init (&material1))
		return 0;
	if (!limdl_material_set_material (&material1, "stickterrain1"))
	{
		limdl_material_free (&material1);
		return 0;
	}

	/* Allocate the two-side material. */
	if (!limdl_material_init (&material2))
	{
		limdl_material_free (&material1);
		return 0;
	}
	if (!limdl_material_set_material (&material2, "stickterraintwoside1"))
	{
		limdl_material_free (&material1);
		limdl_material_free (&material2);
		return 0;
	}

	/* Allocate the model builder. */
	builder = limdl_builder_new (self->model);
	if (builder == NULL)
	{
		limdl_material_free (&material1);
		limdl_material_free (&material2);
		return 0;
	}
	if (!limdl_builder_insert_material (builder, &material1) ||
	    !limdl_builder_insert_material (builder, &material2))
	{
		limdl_material_free (&material1);
		limdl_material_free (&material2);
		limdl_builder_free (builder);
		return 0;
	}
	limdl_material_free (&material1);
	limdl_material_free (&material2);

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

	/* Initialize the neighbor face iterators. */
	liext_terrain_face_iterator_init (&left, sticks_left, 1, 0, 1, 1);
	liext_terrain_face_iterator_init (&right, sticks_right, 0, 1, 0, 0);
	liext_terrain_face_iterator_init (&front, sticks_front, 1, 1, 0, 1);
	liext_terrain_face_iterator_init (&back, sticks_back, 0, 0, 1, 0);

	/* Add the sticks to the builder. */
	for (stick = self->sticks ; stick != NULL ; stick_prev = stick, stick = stick->next)
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
			material = materials + stick->material;
			s = material->texture_scaling;

			/* Left face. */
			normal = limat_vector_init (-1.0f, 0.0f, 0.0f);
			liext_terrain_face_iterator_emit (&left, builder, material->texture_side,
				 offset->z, 0.0f, s, s, &normal,
				&bot[0][0].coord, &bot[0][1].coord, &top[0][0].coord, &top[0][1].coord);

			/* Right face. */
			normal = limat_vector_init (1.0f, 0.0f, 0.0f);
			liext_terrain_face_iterator_emit (&right, builder, material->texture_side,
				 offset->z, 0.0f, s, s, &normal,
				&bot[1][1].coord, &bot[1][0].coord, &top[1][1].coord, &top[1][0].coord);

			/* Front face. */
			normal = limat_vector_init (0.0f, 0.0f, -1.0f);
			liext_terrain_face_iterator_emit (&front, builder, material->texture_side,
				 offset->x, 0.0f, s, s, &normal,
				&bot[1][0].coord, &bot[0][0].coord, &top[1][0].coord, &top[0][0].coord);

			/* Back face. */
			normal = limat_vector_init (0.0f, 0.0f, 1.0f);
			liext_terrain_face_iterator_emit (&back, builder, material->texture_side,
				 offset->x, 0.0f, s, s, &normal,
				&bot[0][1].coord, &bot[1][1].coord, &top[0][1].coord, &top[1][1].coord);

			/* Bottom face. */
			if (stick_prev != NULL && stick_prev->material == 0)
			{
				/*Choose corners to best match the underlying convex hull model used for physics.*/
				size_t q0 = (bot[0][0].coord.y + bot[1][1].coord.y <= bot[1][0].coord.y + bot[0][1].coord.y) ? 0 : 1;
				size_t q1 = q0 + 1;
				size_t q2 = q0 + 2;
				size_t q3 = (q0 + 3) & 0x3;
				limdl_vertex_init (quad + q0, &bot[0][0].coord, &bot[0][0].normal, s * (offset->x + bot[0][0].coord.x), s * (offset->z + bot[0][0].coord.z));
				limdl_vertex_init (quad + q1, &bot[1][0].coord, &bot[1][0].normal, s * (offset->x + bot[1][0].coord.x), s * (offset->z + bot[1][0].coord.z));
				limdl_vertex_init (quad + q2, &bot[1][1].coord, &bot[1][1].normal, s * (offset->x + bot[1][1].coord.x), s * (offset->z + bot[1][1].coord.z));
				limdl_vertex_init (quad + q3, &bot[0][1].coord, &bot[0][1].normal, s * (offset->x + bot[0][1].coord.x), s * (offset->z + bot[0][1].coord.z));
				quad[q0].color[0] = material->texture_bottom;
				quad[q1].color[0] = material->texture_bottom;
				quad[q2].color[0] = material->texture_bottom;
				quad[q3].color[0] = material->texture_bottom;
				quad[q0].color[1] = 255 * (int)(1.0f - bot[0][0].splatting);
				quad[q1].color[1] = 255 * (int)(1.0f - bot[1][0].splatting);
				quad[q2].color[1] = 255 * (int)(1.0f - bot[1][1].splatting);
				quad[q3].color[1] = 255 * (int)(1.0f - bot[0][1].splatting);
				private_insert_quad (builder, 0, quad);
			}

			/* Top face. */
			if (stick->next == NULL || stick->next->material == 0)
			{
				/* Face. */
				/*Choose corners to best match the underlying convex hull model used for physics.*/
				size_t q0 = (top[0][0].coord.y + top[1][1].coord.y >= top[1][0].coord.y + top[0][1].coord.y) ? 0 : 1;
				size_t q1 = q0 + 1;
				size_t q2 = q0 + 2;
				size_t q3 = (q0 + 3) & 0x3;
				limdl_vertex_init (quad + q0, &top[0][0].coord, &top[0][0].normal, s * (offset->x + top[0][0].coord.x), s * (offset->z + top[0][0].coord.z));
				limdl_vertex_init (quad + q1, &top[0][1].coord, &top[0][1].normal, s * (offset->x + top[0][1].coord.x), s * (offset->z + top[0][1].coord.z));
				limdl_vertex_init (quad + q2, &top[1][1].coord, &top[1][1].normal, s * (offset->x + top[1][1].coord.x), s * (offset->z + top[1][1].coord.z));
				limdl_vertex_init (quad + q3, &top[1][0].coord, &top[1][0].normal, s * (offset->x + top[1][0].coord.x), s * (offset->z + top[1][0].coord.z));
				quad[q0].color[0] = material->texture_top;
				quad[q1].color[0] = material->texture_top;
				quad[q2].color[0] = material->texture_top;
				quad[q3].color[0] = material->texture_top;
				quad[q0].color[1] = 255 * (int)(1.0f - stick->vertices[0][0].splatting);
				quad[q1].color[1] = 255 * (int)(1.0f - stick->vertices[0][1].splatting);
				quad[q2].color[1] = 255 * (int)(1.0f - stick->vertices[1][1].splatting);
				quad[q3].color[1] = 255 * (int)(1.0f - stick->vertices[1][0].splatting);
				private_insert_quad (builder, 0, quad);

				/* Grass. */
				if (material->decoration_type == LIEXT_TERRAIN_DECORATION_TYPE_GRASS)
					private_generate_grass (self, builder, material, top, offset);
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
	limdl_builder_finish (builder, 0);
	limdl_builder_free (builder);
	self->stamp_model = self->stamp;

	return 1;
}

/**
 * \brief Calculates smooth normals for the vertex at the intersection point of the four columns.
 * \param self Terrain column (0,0).
 * \param c10 Terrain column (1,0).
 * \param c01 Terrain column (0,1).
 * \param c11 Terrain column (1,1).
 */
void liext_terrain_column_calculate_smooth_normals (
	LIExtTerrainColumn* self,
	LIExtTerrainColumn* c10,
	LIExtTerrainColumn* c01,
	LIExtTerrainColumn* c11)
{
	const int x[4] = { 1, 0, 1, 0 };
	const int z[4] = { 1, 1, 0, 0 };
	float y[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float yv[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	float splatting;
	float tmp;
	int handled[4];
	int i;
	int u;
	int found;
	LIExtTerrainStick* sticks[4];
	LIMatVector normal;
	LIMatVector normal1;

	/* Calculate the first Y offsets. */
	sticks[0] = self->sticks;
	sticks[1] = c10->sticks;
	sticks[2] = c01->sticks;
	sticks[3] = c11->sticks;
	handled[0] = (sticks[0] == NULL);
	handled[1] = (sticks[1] == NULL);
	handled[2] = (sticks[2] == NULL);
	handled[3] = (sticks[3] == NULL);
	if (!handled[0])
	{
		y[0] = sticks[0]->height;
		yv[0] = y[0] + sticks[0]->vertices[x[0]][z[0]].offset;
	}
	if (!handled[1])
	{
		y[1] = sticks[1]->height;
		yv[1] = y[1] + sticks[1]->vertices[x[1]][z[1]].offset;
	}
	if (!handled[2])
	{
		y[2] = sticks[2]->height;
		yv[2] = y[2] + sticks[2]->vertices[x[2]][z[2]].offset;
	}
	if (!handled[3])
	{
		y[3] = sticks[3]->height;
		yv[3] = y[3] + sticks[3]->vertices[x[3]][z[3]].offset;
	}

	/* Iterate through the vertices of sticks. */
	while (1)
	{
		/* Choose the lowest vertex for the update. */
		if (!handled[0] &&
				(handled[1] || yv[0] <= yv[1]) &&
				(handled[2] || yv[0] <= yv[2]) &&
				(handled[3] || yv[0] <= yv[3]))
		{
			u = 0;
		}
		else if (!handled[1] &&
				(handled[2] || yv[1] <= yv[2]) &&
				(handled[3] || yv[1] <= yv[3]))
		{
			u = 1;
		}
		else if (!handled[2] &&
			(handled[3] || yv[2] <= yv[3]))
		{
			u = 2;
		}
		else if (!handled[3])
		{
			u = 3;
		}
		else
			break;

		/* Calculate the splatting factor. */
		splatting = 0.0f;
		for (i = 0 ; i < 4 ; i++)
		{
			if (sticks[i] != NULL && LIMAT_ABS (yv[i] - yv[u]) <= LIEXT_TERRAIN_SMOOTHING_LIMIT)
			{
				if (sticks[i]->material != sticks[u]->material)
				{
					splatting = 1.0f;
					break;
				}
			}
		}

		/* Calculate the new vertex normal. */
		normal = limat_vector_init (0.0f, 0.0f, 0.0f);
		for (i = 0 ; i < 4 ; i++)
		{
			if (sticks[i] != NULL && LIMAT_ABS (yv[i] - yv[u]) <= LIEXT_TERRAIN_SMOOTHING_LIMIT)
			{
				liext_terrain_stick_get_normal (sticks[i], &normal1);
				normal = limat_vector_add (normal, normal1);
			}
		}
		normal = limat_vector_normalize (normal);

		/* Apply the new values to the connected vertices. */
		for (i = 0 ; i < 4 ; i++)
		{
			if (sticks[i] != NULL && LIMAT_ABS (yv[i] - yv[u]) <= LIEXT_TERRAIN_SMOOTHING_LIMIT)
			{
				sticks[i]->vertices[x[i]][z[i]].splatting = splatting;
				sticks[i]->vertices[x[i]][z[i]].normal = normal;
			}
		}

		/* Advance degenerate vertices. */
		/* If there exist any vertices above the current one that have the
		   exact same height, then only those vertices are advanced and the
		   rest not. This is done to avoid areas where slopes of different
		   materials meet to have correct splatting. */
		found = 0;
		for (i = 0 ; i < 4 ; i++)
		{
			if (!handled[i] && LIMAT_ABS (yv[i] - yv[u]) <= LIEXT_TERRAIN_SMOOTHING_LIMIT)
			{
				if (sticks[i]->next != NULL)
				{
					tmp = y[i] + sticks[i]->next->height + sticks[i]->next->vertices[x[i]][z[i]].offset;
					if (tmp - yv[i] <= LIEXT_STICK_EPSILON)
					{
						found = 1;
						sticks[i] = sticks[i]->next;
						y[i] += sticks[i]->height;
						/* yv[i] does not change. */
					}
				}
			}
		}
		if (found)
			continue;

		/* Advance all handled vertices. */
		/* It is necessary to advance all the affected vertices here because
		   otherwise their splatting values and normals would be overwritten
		   in subsequent rounds. */
		for (i = 0 ; i < 4 ; i++)
		{
			if (!handled[i] && LIMAT_ABS (yv[i] - yv[u]) <= LIEXT_TERRAIN_SMOOTHING_LIMIT)
			{
				handled[i] = (sticks[i]->next == NULL);
				if (!handled[i])
				{
					sticks[i] = sticks[i]->next;
					y[i] += sticks[i]->height;
					yv[i] = y[i] + sticks[i]->vertices[x[i]][z[i]].offset;
				}
			}
		}
	}
}

/**
 * \brief Casts a sphere against the column and returns the hit fraction.
 * \param self Terrain column.
 * \param sphere_rel_cast_start Cast start position of the sphere, in grid units relative to the column origin.
 * \param sphere_rel_cast_end Cast end position of the sphere, in grid units relative to the column origin.
 * \param sphere_radius Sphere radius, in grid units.
 * \param result Return location for the hit fraction.
 * \return Nonzero if hit. Zero otherwise.
 */
int liext_terrain_column_cast_sphere (
	const LIExtTerrainColumn* self,
	const LIMatVector*        sphere_rel_cast_start,
	const LIMatVector*        sphere_rel_cast_end,
	float                     sphere_radius,
	LIExtTerrainCollision*    result)
{
	float y;
	float min_y;
	float max_y;
	float bot[4];
	float top[4];
	LIExtTerrainStick* stick;
	LIExtTerrainCollision best;
	LIExtTerrainCollision frac;

	best.fraction = LIMAT_INFINITE;
	y = 0.0f;
	bot[0] = 0.0f;
	bot[1] = 0.0f;
	bot[2] = 0.0f;
	bot[3] = 0.0f;
	min_y = LIMAT_MIN (sphere_rel_cast_start->y, sphere_rel_cast_end->y) - sphere_radius;
	max_y = LIMAT_MAX (sphere_rel_cast_start->y, sphere_rel_cast_end->y) + sphere_radius;

	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		if (bot[0] > max_y && bot[1] > max_y && bot[2] > max_y && bot[3] > max_y)
			break;
		y += stick->height;
		top[0] = y + stick->vertices[0][0].offset;
		top[1] = y + stick->vertices[1][0].offset;
		top[2] = y + stick->vertices[0][1].offset;
		top[3] = y + stick->vertices[1][1].offset;
		if (stick->material && (top[0] >= min_y || top[1] >= min_y || top[2] >= min_y || top[3] >= min_y))
		{
			if (liext_terrain_stick_cast_sphere (stick,
					bot[0], bot[1], bot[2], bot[3], top[0], top[1], top[2], top[3],
					sphere_rel_cast_start, sphere_rel_cast_end, sphere_radius, &frac))
			{
				if (frac.fraction >= 0.0f && frac.fraction < best.fraction)
					best = frac;
			}
		}
		bot[0] = top[0];
		bot[1] = top[1];
		bot[2] = top[2];
		bot[3] = top[3];
	}

	if (best.fraction > 1.0f)
		return 0;
	*result = best;
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
 * \brief Frees the model of the column.
 * \param self Terrain column.
 */
void liext_terrain_column_clear_model (
	LIExtTerrainColumn* self)
{
	if (self->model != NULL)
	{
		limdl_model_free (self->model);
		self->model = NULL;
		self->stamp_model = self->stamp - 1;
	}
}

/**
 * \brief Finds the stick nearest to the given Y offset.
 * \param self Terrain column.
 * \param y Y offset.
 * \param min_height Minimum stick height.
 * \return Y offset.
 */
float liext_terrain_column_find_nearest_empty_stick (
	LIExtTerrainColumn* self,
	float               y,
	float               min_height)
{
	float dist;
	float best_dist;
	float best_y;
	float curr_y;
	LIExtTerrainStick* stick;

	best_dist = 1000000000000000.0f;
	best_y = 0.0f;
	curr_y = 0.0f;

	/* Try each stick. */
	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		if (!stick->material && stick->height >= min_height)
		{
			dist = LIMAT_ABS (curr_y - y);
			if (dist < best_dist)
			{
				best_y = curr_y;
				best_dist = dist;
			}
		}
		curr_y += stick->height;
	}

	/* Try the remainder. */
	dist = LIMAT_ABS (curr_y - y);
	if (dist < best_dist)
	{
		best_y = curr_y;
		best_dist = dist;
	}

	return best_y;
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
	float offset;
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
		/* We check that the vertex offsets of the previous stick do not
		   extend above the vertices of this stick. */
		if (sticks_last != NULL)
		{
			for (z = 0 ; z < 2 ; z++)
			{
				for (x = 0 ; x < 2 ; x++)
				{
					offset = stick->height + sticks_last->vertices[x][z].offset;
					if (sticks_last->vertices[x][z].offset > offset)
						sticks_last->vertices[x][z].offset = offset;
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

/**
 * \brief Gets the estimate of how much memory the chunk uses.
 * \param self Terrain column.
 * \return Memory consumption in bytes.
 */
int liext_terrain_column_get_memory_used (
	const LIExtTerrainColumn* self)
{
	int total;
	const LIExtTerrainStick* stick;

	total = sizeof (*self);
	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
		total += sizeof (*stick);
	if (self->model)
		total += limdl_model_get_memory (self->model);

	return total;
}

/*****************************************************************************/

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

static void private_generate_grass (
	LIExtTerrainColumn*         self,
	LIMdlBuilder*               builder,
	const LIExtTerrainMaterial* material,
	LIExtColumnVertex           top[2][2],
	const LIMatVector*          offset)
{
	int i;
	int j;
	float scale;
	float anim[2][2];
	float grass[2][2];
	LIMatVector c;
	LIMatVector normal;
	LIMdlVertex quad[4];

	/* Generate the corner heights. */
	for (j = 0 ; j < 2 ; ++j)
	{
		for (i = 0 ; i < 2 ; ++i)
		{
			c = limat_vector_add (top[i][j].coord, *offset);
			grass[i][j] = 0.7f + 0.7f * liext_noise_simplex_noise_2d(0.7 * c.x, 0.7 * c.z);
			anim[i][j] = grass[i][j] * 0.3f * liext_noise_simplex_noise_2d(0.1 * c.x, 0.1 * c.z);
		}
	}

	/* Diagonal. */
	scale = 0.3f;
	normal = limat_vector_init (0.707f, 0.0f, -0.707f);
	limdl_vertex_init (quad + 0, &top[0][0].coord, &normal, scale * (offset->x + top[0][0].coord.x), 1.0f);
	limdl_vertex_init (quad + 1, &top[1][1].coord, &normal, scale * (offset->x + top[1][1].coord.x), 1.0f);
	limdl_vertex_init (quad + 2, &top[1][1].coord, &normal, scale * (offset->x + top[1][1].coord.x), 0.0f);
	limdl_vertex_init (quad + 3, &top[0][0].coord, &normal, scale * (offset->x + top[0][0].coord.x), 0.0f);
	quad[2].coord.y += grass[1][1];
	quad[3].coord.y += grass[0][0];
	quad[2].tangent.x = anim[1][1];
	quad[3].tangent.x = anim[0][0];
	quad[0].color[0] = material->texture_decoration;
	quad[1].color[0] = material->texture_decoration;
	quad[2].color[0] = material->texture_decoration;
	quad[3].color[0] = material->texture_decoration;
	private_insert_quad (builder, 1, quad);

	/* Diagonal. */
	normal = limat_vector_init (0.707f, 0.0f, 0.707f);
	limdl_vertex_init (quad + 0, &top[0][1].coord, &normal, scale * (offset->x + top[0][1].coord.z), 1.0f);
	limdl_vertex_init (quad + 1, &top[1][0].coord, &normal, scale * (offset->x + top[1][0].coord.z), 1.0f);
	limdl_vertex_init (quad + 2, &top[1][0].coord, &normal, scale * (offset->x + top[1][0].coord.z), 0.0f);
	limdl_vertex_init (quad + 3, &top[0][1].coord, &normal, scale * (offset->x + top[0][1].coord.z), 0.0f);
	quad[2].coord.y += grass[1][0];
	quad[3].coord.y += grass[0][1];
	quad[2].tangent.x = anim[1][0];
	quad[3].tangent.x = anim[0][1];
	quad[0].color[0] = material->texture_decoration;
	quad[1].color[0] = material->texture_decoration;
	quad[2].color[0] = material->texture_decoration;
	quad[3].color[0] = material->texture_decoration;
	private_insert_quad (builder, 1, quad);

	/* Front. */
	normal = limat_vector_init (0.0f, 0.0f, 1.0f);
	limdl_vertex_init (quad + 0, &top[0][0].coord, &normal, scale * (offset->x + top[0][0].coord.x), 1.0f);
	limdl_vertex_init (quad + 1, &top[1][0].coord, &normal, scale * (offset->x + top[1][0].coord.x), 1.0f);
	limdl_vertex_init (quad + 2, &top[1][0].coord, &normal, scale * (offset->x + top[1][0].coord.x), 0.0f);
	limdl_vertex_init (quad + 3, &top[0][0].coord, &normal, scale * (offset->x + top[0][0].coord.x), 0.0f);
	quad[2].coord.y += grass[1][0];
	quad[3].coord.y += grass[0][0];
	quad[2].tangent.x = -anim[1][0];
	quad[3].tangent.x = -anim[0][0];
	quad[0].color[0] = material->texture_decoration;
	quad[1].color[0] = material->texture_decoration;
	quad[2].color[0] = material->texture_decoration;
	quad[3].color[0] = material->texture_decoration;
	private_insert_quad (builder, 1, quad);

	/* Right. */
	normal = limat_vector_init (1.0f, 0.0f, 0.0f);
	limdl_vertex_init (quad + 0, &top[1][0].coord, &normal, scale * (offset->z + top[1][0].coord.z), 1.0f);
	limdl_vertex_init (quad + 1, &top[1][1].coord, &normal, scale * (offset->z + top[1][1].coord.z), 1.0f);
	limdl_vertex_init (quad + 2, &top[1][1].coord, &normal, scale * (offset->z + top[1][1].coord.z), 0.0f);
	limdl_vertex_init (quad + 3, &top[1][0].coord, &normal, scale * (offset->z + top[1][0].coord.z), 0.0f);
	quad[2].coord.y += grass[1][1];
	quad[3].coord.y += grass[1][0];
	quad[2].tangent.x = -anim[1][1];
	quad[3].tangent.x = -anim[1][0];
	quad[0].color[0] = material->texture_decoration;
	quad[1].color[0] = material->texture_decoration;
	quad[2].color[0] = material->texture_decoration;
	quad[3].color[0] = material->texture_decoration;
	private_insert_quad (builder, 1, quad);
}

static void private_insert_quad (
	LIMdlBuilder* builder,
	int           material,
	LIMdlVertex*  quad)
{
	LIMdlVertex vertices[3];

	quad[0].color[2] =   0; quad[0].color[3] = 0;
	quad[1].color[2] = 255; quad[1].color[3] = 0;
	quad[2].color[2] = 255; quad[2].color[3] = 255;
	quad[3].color[2] =   0; quad[3].color[3] = 255;
	vertices[0] = quad[0];
	vertices[1] = quad[1];
	vertices[2] = quad[2];
	limdl_builder_insert_face (builder, 0, material, vertices, NULL);
	vertices[0] = quad[0];
	vertices[1] = quad[2];
	vertices[2] = quad[3];
	limdl_builder_insert_face (builder, 0, material, vertices, NULL);
}

static void private_insert_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  insert)
{
	if (prev == NULL)
	{
		liext_terrain_stick_clamp_vertices (insert, self->sticks);
		insert->next = self->sticks;
		self->sticks = insert;
	}
	else
	{
		liext_terrain_stick_clamp_vertices (insert, prev->next);
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

/** @} */
/** @} */
