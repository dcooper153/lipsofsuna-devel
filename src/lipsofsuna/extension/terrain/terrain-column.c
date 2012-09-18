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

static void private_move_slope (
	LIExtTerrainStick* stick,
	float              diff);

static void private_remove_stick (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  prev,
	LIExtTerrainStick*  remove);

static void private_set_slope (
	LIExtTerrainStick* stick,
	const float*       slope);

static void private_set_slope_max (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  stick,
	const float*        slope,
	float               offset);

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
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick (
	LIExtTerrainColumn* self,
	float               world_y,
	float               world_h,
	const float*        slope_bot,
	const float*        slope_top,
	int                 material)
{
	int ret;
	float h;
	float y;
	LIExtTerrainStick* stick;
	LIExtTerrainStick* stick_prev;
	LIExtTerrainStick* stick_after_isect;
	LIExtTerrainStick* stick_before_isect;

	y = 0.0f;
	stick = self->sticks;
	stick_prev = NULL;
	stick_before_isect = NULL;
	self->stamp++;

	/* Subtract the stick from the column. */
	DEBUGPRINT ("Add stick: y=%.2f h=%.2f m=%d\n", world_y, world_h, material);
	while (stick != NULL)
	{
		DEBUGPRINT (" Stick: y=%.2f h=%.2f\n", y, stick->height);
		h = stick->height;
		ret = liext_terrain_stick_subtract (stick, world_y - y, world_h,
			slope_bot[0], slope_bot[1], slope_bot[2], slope_bot[3],
			slope_top[0], slope_top[1], slope_top[2], slope_top[3]);
		switch (ret)
		{
			/* Out of memory? */
			case 0:
				return 0;
			/* A) Is the stick completely below us? */
			case 1:
				DEBUGPRINT ("  1-below : y=%.2f..%.2f\n", y, y + h);
				y += h;
				stick_prev = stick;
				stick = stick->next;
				break;
			/* B) Is the stick completely above us? */
			case 2:
				DEBUGPRINT ("  2-above : y=%.2f..%.2f\n", y, y + h);
				y += h;
				stick_before_isect = stick;
				stick_prev = stick;
				stick = stick->next;
				break;
			/* C) Does the stick replace us completely? */
			case 3:
				DEBUGPRINT ("  3-full : y=%.2f..%.2f\n", y, y + h);
				y += h;
				private_remove_stick (self, stick_prev, stick);
				if (stick_prev != NULL)
					stick = stick_prev->next;
				else
					stick = self->sticks;
				break;
			/* D) Does the stick replace part of the bottom? */
			case 4:
				DEBUGPRINT ("  4-bottom: y=%.2f..%.2f\n", y, y + h);
				y += h;
				stick_prev = stick;
				stick = stick->next;
				break;
			/* E) Does the stick replace part of the top? */
			case 5:
				DEBUGPRINT ("  5-top   : y=%.2f..%.2f\n", y, y + h);
				y += h;
				stick_before_isect = stick;
				stick_prev = stick;
				stick = stick->next;
				break;
			/* F) Does the stick replace part of the middle? */
			case 6:
				DEBUGPRINT ("  6-middle: y=%.2f..%.2f\n", y, y + stick->height);
				y += stick->height + world_h;
				stick_before_isect = stick;
				stick_prev = stick;
				stick = stick->next;
				break;
			/* Should not happen. */
			default:
				lisys_assert (0);
				return 0;
		}
	}

	/* Insert the stick to the column. */
	if (world_y >= y)
	{
		/* Append after the last stick. */
		DEBUGPRINT (" Append!\n");
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
			private_set_slope (stick, slope_bot);
			private_insert_stick (self, stick_prev, stick);
			stick_prev = stick;
			y += stick->height;
		}
		if (stick_prev != NULL && stick_prev->material == material)
		{
			DEBUGPRINT ("  Extend last stick: y=%.2f h=%.2f\n", y, world_h);
			stick_prev->height += world_h;
			private_move_slope (stick_prev, -world_h);
			private_set_slope_max (self, stick_prev, slope_top, 0.0f);
		}
		else
		{
			DEBUGPRINT ("  Append new stick: y=%.2f h=%.2f\n", y, world_h);
			stick = liext_terrain_stick_new (material, world_h);
			if (stick == NULL)
				return 0;
			private_set_slope (stick, slope_top);
			private_insert_stick (self, stick_prev, stick);
		}
	}
	else
	{
		/* Insert to the stick. */
		DEBUGPRINT (" Insert!\n");
		if (stick_before_isect != NULL)
			stick_after_isect = stick_before_isect->next;
		else
			stick_after_isect = NULL;
		if (stick_before_isect != NULL && stick_before_isect->material == material &&
		     stick_after_isect != NULL &&  stick_after_isect->material == material)
		{
			/*
			 *   11111.....11111
			 * +      11111
			 */
			DEBUGPRINT (" Join up and down: y=%.2f h=%.2f\n", y_before_isect, world_h);
			stick = stick_after_isect;
			stick_before_isect->height += world_h + stick->height;
			private_move_slope (stick_before_isect, -world_h - stick->height);
			private_set_slope_max (self, stick_before_isect, slope_top, -stick->height);
			stick_before_isect->next = stick->next;
			liext_terrain_stick_free (stick);
		}
		else if (stick_before_isect != NULL && stick_before_isect->material == material)
		{
			/*
			 *   11111.....22222
			 * +      11111
			 */
			DEBUGPRINT (" Join down: y=%.2f h=%.2f\n", y_before_isect, world_h);
			stick_before_isect->height += world_h;
			private_move_slope (stick_before_isect, -world_h);
			private_set_slope_max (self, stick_before_isect, slope_top, 0.0f);
		}
		else if (stick_after_isect != NULL && stick_after_isect->material == material)
		{
			/*
			 *   22222.....11111
			 * +      11111
			 */
			DEBUGPRINT (" Join up: y=%.2f h=%.2f\n", y_before_isect, world_h);
			stick_after_isect->height += world_h;
			private_set_slope_max (self, stick_after_isect, slope_top, -world_h);
		}
		else if (stick_before_isect != NULL || material != 0 ||
				(self->sticks != NULL && self->sticks->height > world_y))
		{
			/*
			 *   22222.....22222
			 * +      11111
			 */
			DEBUGPRINT (" Create new stick: y=%.2f h=%.2f\n", y, world_h);
			stick = liext_terrain_stick_new (material, world_h);
			if (stick == NULL)
				return 0;
			private_set_slope (stick, slope_top);
			private_insert_stick (self, stick_before_isect, stick);
		}
		else
		{
			/*
			 *   22222.....
			 * +      00000
			 */
			DEBUGPRINT (" Replace by empty stick: y=%.2f h=%.2f\n", y, world_h);
		}
	}

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
 * \return Nonzero on success, zero if allocating memory failed.
 */
int liext_terrain_column_add_stick_corners (
	LIExtTerrainColumn* self,
	float               bot00,
	float               bot10,
	float               bot01,
	float               bot11,
	float               top00,
	float               top10,
	float               top01,
	float               top11,
	int                 material)
{
	float bot;
	float top;
	float slope_bot[4];
	float slope_top[4];

	/* Calculate the stick offset and height. */
	/* We estimate the offset and height of the stick by taking the average
	   of the bottom and top surface offsets. The vertices are then offset
	   against the end points of the stick. */
	bot = (bot00 + bot10 + bot01 + bot11) / 4.0f;
	top = (top00 + top10 + top01 + top11) / 4.0f;
	if (bot >= top)
		return 0;
	slope_bot[0] = bot00 - bot;
	slope_bot[1] = bot10 - bot;
	slope_bot[2] = bot01 - bot;
	slope_bot[3] = bot11 - bot;
	slope_top[0] = top00 - top;
	slope_top[1] = top10 - top;
	slope_top[2] = top01 - top;
	slope_top[3] = top11 - top;

	/* Create the stick. */
	return liext_terrain_column_add_stick (self, bot, top - bot, slope_bot, slope_top, material);
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

static void private_move_slope (
	LIExtTerrainStick* stick,
	float              diff)
{
	if (stick == NULL)
		return;
	stick->vertices[0][0].offset += diff;
	stick->vertices[1][0].offset += diff;
	stick->vertices[0][1].offset += diff;
	stick->vertices[1][1].offset += diff;
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

static void private_set_slope (
	LIExtTerrainStick* stick,
	const float*       slope)
{
	if (stick == NULL)
		return;
	liext_terrain_stick_reset_vertices (stick);
	stick->vertices[0][0].offset = slope[0];
	stick->vertices[1][0].offset = slope[1];
	stick->vertices[0][1].offset = slope[2];
	stick->vertices[1][1].offset = slope[3];
}

static void private_set_slope_max (
	LIExtTerrainColumn* self,
	LIExtTerrainStick*  stick,
	const float*        slope,
	float               offset)
{
	if (stick == NULL)
		return;
	stick->vertices[0][0].offset = LIMAT_MAX (stick->vertices[0][0].offset + offset, slope[0]);
	stick->vertices[1][0].offset = LIMAT_MAX (stick->vertices[1][0].offset + offset, slope[1]);
	stick->vertices[0][1].offset = LIMAT_MAX (stick->vertices[0][1].offset + offset, slope[2]);
	stick->vertices[1][1].offset = LIMAT_MAX (stick->vertices[1][1].offset + offset, slope[3]);
	liext_terrain_stick_fix_vertices_upwards (stick);
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
