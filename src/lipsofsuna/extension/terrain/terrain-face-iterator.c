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

#include "terrain-face-iterator.h"

#define DEBUGPRINT(...)
//#define DEBUGPRINT printf

static void private_emit_polygon (
	LIMdlBuilder*      builder,
	LIMatPolygon2d*    polygon,
	int                material,
	float              uoffset,
	float              voffset,
	float              uscale,
	float              vscale,
	const LIMatVector* normal,
	const LIMatVector* bot0,
	const LIMatVector* bot1,
	const LIMatVector* top0,
	const LIMatVector* top1);

static void private_emit_triangle (
	LIMdlBuilder*        builder,
	int                  material,
	const LIMatVector*   normal,
	const LIMatVector*   v0,
	const LIMatVector*   v1,
	const LIMatVector*   v2,
	const LIMatVector2d* uv0,
	const LIMatVector2d* uv1,
	const LIMatVector2d* uv2);

/*****************************************************************************/

/**
 * \brief Initializes the face iterator.
 * \param self Face iterator.
 * \param stick Stick containing the face.
 * \param vx0 X index of the first vertex.
 * \param vz0 Z indes of the first vertex.
 * \param vx1 X index of the second vertex.
 * \param vz1 Z indes of the second vertex.
 */
void liext_terrain_face_iterator_init (
	LIExtTerrainFaceIterator* self,
	LIExtTerrainStick*        stick,
	int                       vx0,
	int                       vz0,
	int                       vx1,
	int                       vz1)
{
	self->vx0 = vx0;
	self->vz0 = vz0;
	self->vx1 = vx1;
	self->vz1 = vz1;
	self->stick_y = 0.0f;
	self->stick_y0 = 0.0f;
	self->stick_y1 = 0.0f;
	self->stick = stick;
}

/**
 * \brief Iterates while culling a neighbor stick.
 * \param self Face iterator.
 * \param builder Model builder.
 * \param texture Texture index.
 * \param uoffset Horizontal texture offset.
 * \param voffset Vertical texture offset.
 * \param uscale Horizontal texture scaling.
 * \param vscale Vertical texture scaling.
 * \param normal Face normal.
 * \param bot0 Coordinates of the first bottom vertex of the neighbor face.
 * \param bot1 Coordinates of the second bottom vertex of the neighbor face.
 * \param top0 Coordinates of the first top vertex of the neighbor face.
 * \param top1 Coordinates of the second top vertex of the neighbor face.
 */
void liext_terrain_face_iterator_emit (
	LIExtTerrainFaceIterator* self,
	LIMdlBuilder*             builder,
	int                       texture,
	float                     uoffset,
	float                     voffset,
	float                     uscale,
	float                     vscale,
	const LIMatVector*        normal,
	const LIMatVector*        bot0,
	const LIMatVector*        bot1,
	const LIMatVector*        top0,
	const LIMatVector*        top1)
{
	int i;
	float top_y;
	float top_y0;
	float top_y1;
	LIMatVector2d face[4];
	LIMatPolygonCuller* culler;

	/* FIXME */
	float valid_y = self->stick_y;
	float valid_y0 = self->stick_y0;
	float valid_y1 = self->stick_y1;
	LIExtTerrainStick* valid_stick = self->stick;

	/* Initialize the polygon culler. */
	if (normal->x == 0.0f)
	{
		face[0] = limat_vector2d_init (bot0->x, bot0->y);
		face[1] = limat_vector2d_init (bot1->x, bot1->y);
		face[2] = limat_vector2d_init (top1->x, top1->y);
		face[3] = limat_vector2d_init (top0->x, top0->y);
	}
	else
	{
		face[0] = limat_vector2d_init (bot0->z, bot0->y);
		face[1] = limat_vector2d_init (bot1->z, bot1->y);
		face[2] = limat_vector2d_init (top1->z, top1->y);
		face[3] = limat_vector2d_init (top0->z, top0->y);
	}
	culler = limat_polygon_culler_new (&limat_vtxops_v2, face, 4);
	if (culler == NULL)
		return;

	/* The stick pointer should never point above the bottom of the wall being
	   culled. Otherwise, there is an error in the iteration logic. */
	DEBUGPRINT ("\nSTART y=%f,%f face_bot=%f,%f face_top=%f,%f\n",
		self->stick_y0, self->stick_y1, bot0->y, bot1->y, top0->y, top1->y);
	lisys_assert (self->stick_y0 <= bot0->y);
	lisys_assert (self->stick_y1 <= bot1->y);

	/* Move the stick pointer to the first stick that intersects with the
	   wall. This may save significant amount of culling work if the
	   culling wall has high compelixity compared to the culled one. */
	while (self->stick != NULL)
	{
		top_y = self->stick_y + self->stick->height;
		top_y0 = top_y + self->stick->vertices[self->vx0][self->vz0].offset;
		top_y1 = top_y + self->stick->vertices[self->vx1][self->vz1].offset;
		if (top_y0 > bot0->y || top_y1 > bot1->y)
			break;
		DEBUGPRINT ("SKIP1 y=%f,%f top_y=%f,%f\n", self->stick_y0, self->stick_y1, top_y0, top_y1);
		self->stick = self->stick->next;
		self->stick_y = top_y;
		self->stick_y0 = top_y0;
		self->stick_y1 = top_y1;
	}

	/* Cull the face until it has been fully consumed or there are no more
	 * culling stick left in the column. */
	while (self->stick != NULL)
	{
		top_y = self->stick_y + self->stick->height;
		top_y0 = top_y + self->stick->vertices[self->vx0][self->vz0].offset;
		top_y1 = top_y + self->stick->vertices[self->vx1][self->vz1].offset;

		/* If the current stick is not empty, cull the polygon with it.
		   The polygon culler will take care of generating polygons for the
		   part that didn't get culled previously. */
		if (self->stick->material != 0)
		{
			face[0].y = self->stick_y0;
			face[1].y = self->stick_y1;
			face[2].y = top_y1;
			face[3].y = top_y0;
			DEBUGPRINT ("CULL1 face_bot=%f,%f face_top=%f,%f\n", face[0].y, face[1].y, face[3].y, face[2].y);
			limat_polygon_culler_subtract_quad (culler, face, face + 1, face + 3, face + 2);
			if (!culler->remainder->vertices.count)
				break;
		}

		/* Advance to the next culling stick. */
		self->stick = self->stick->next;
		self->stick_y = top_y;
		self->stick_y0 = top_y0;
		self->stick_y1 = top_y1;
	}

	/* Triangulate the generated polygons. */
	DEBUGPRINT ("RESULT remain=%d pieces=%d\n", culler->remainder->vertices.count, culler->pieces.count);
	if (culler->remainder->vertices.count)
	{
		private_emit_polygon (builder, culler->remainder, texture, uoffset, voffset,
			uscale, vscale, normal, bot0, bot1, top0, top1);
	}
	for (i = 0 ; i < culler->pieces.count ; i++)
	{
		private_emit_polygon (builder, culler->pieces.array[i], texture, uoffset, voffset,
			uscale, vscale, normal, bot0, bot1, top0, top1);
	}
	limat_polygon_culler_free (culler);

	/* FIXME */
	self->stick_y = valid_y;
	self->stick_y0 = valid_y0;
	self->stick_y1 = valid_y1;
	self->stick = valid_stick;
}

/*****************************************************************************/

static void private_emit_polygon (
	LIMdlBuilder*      builder,
	LIMatPolygon2d*    polygon,
	int                texture,
	float              uoffset,
	float              voffset,
	float              uscale,
	float              vscale,
	const LIMatVector* normal,
	const LIMatVector* bot0,
	const LIMatVector* bot1,
	const LIMatVector* top0,
	const LIMatVector* top1)
{
	int i;
	LIMatVector first;
	LIMatVector prev;
	LIMatVector curr;
	LIMatVector2d first_uv;
	LIMatVector2d prev_uv;
	LIMatVector2d curr_uv;
	LIMatVector2d* vertex;

	if (LIMAT_ABS (normal->x) == 0.0f)
	{
		vertex = limat_polygon2d_get_vertex (polygon, 0);
		first = limat_vector_init (vertex->x, vertex->y, bot0->z);
		first_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
		vertex = limat_polygon2d_get_vertex (polygon, 1);
		prev = limat_vector_init (vertex->x, vertex->y, bot0->z);
		prev_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
		for (i = 2 ; i < polygon->vertices.count ; i++)
		{
			vertex = limat_polygon2d_get_vertex (polygon, i);
			curr = limat_vector_init (vertex->x, vertex->y, bot0->z);
			curr_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
			private_emit_triangle(builder, texture, normal, &first, &prev, &curr, &first_uv, &prev_uv, &curr_uv);
			prev = curr;
			prev_uv = curr_uv;
		}
	}
	else
	{
		vertex = limat_polygon2d_get_vertex (polygon, 0);
		first = limat_vector_init (bot0->x, vertex->y, vertex->x);
		first_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
		vertex = limat_polygon2d_get_vertex (polygon, 1);
		prev = limat_vector_init (bot0->x, vertex->y, vertex->x);
		prev_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
		for (i = 2 ; i < polygon->vertices.count ; i++)
		{
			vertex = limat_polygon2d_get_vertex (polygon, i);
			curr = limat_vector_init (bot0->x, vertex->y, vertex->x);
			curr_uv = limat_vector2d_init (uscale * (uoffset + vertex->x), vscale * (voffset + vertex->y));
			private_emit_triangle(builder, texture, normal, &first, &prev, &curr, &first_uv, &prev_uv, &curr_uv);
			prev = curr;
			prev_uv = curr_uv;
		}
	}
}

static void private_emit_triangle (
	LIMdlBuilder*        builder,
	int                  texture,
	const LIMatVector*   normal,
	const LIMatVector*   v0,
	const LIMatVector*   v1,
	const LIMatVector*   v2,
	const LIMatVector2d* uv0,
	const LIMatVector2d* uv1,
	const LIMatVector2d* uv2)
{
	LIMdlVertex vertices[3];

	/* Initialize the vertices. */
	limdl_vertex_init (vertices + 0, v0, normal, uv0->x, uv0->y);
	limdl_vertex_init (vertices + 1, v1, normal, uv1->x, uv1->y);
	limdl_vertex_init (vertices + 2, v2, normal, uv2->x, uv2->y);

	/* Set the atlas index. */
	vertices[0].color[0] = texture;
	vertices[1].color[0] = texture;
	vertices[2].color[0] = texture;

	/* Generate the splatting colors. */
	/* FIXME: Not implemented. Should be interpolated by the polygon culler. */
	vertices[0].color[2] = 0; vertices[0].color[3] = 0;
	vertices[1].color[2] = 0; vertices[1].color[3] = 0;
	vertices[2].color[2] = 0; vertices[2].color[3] = 0;

	/* Insert the triangle into the builder. */
	limdl_builder_insert_face (builder, 0, 0, vertices, NULL);
}

/** @} */
/** @} */
