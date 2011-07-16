/* Lips of Suna
 * Copyright© 2007-2011 Lips of Suna development team.
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
 * \addtogroup LIMdl Model
 * @{
 * \addtogroup LIMdlBuilder Builder
 * @{
 */

#include "lipsofsuna/algorithm.h"
#include "lipsofsuna/system.h"
#include "model.h"
#include "model-builder.h"

typedef struct _LIMdlEdge LIMdlEdge;
struct _LIMdlEdge
{
	uint32_t i1;
	uint32_t i2;
	int users;
	int locked;
	float cost;
	LIMdlEdge* next;
	LIMdlEdge* prev;
	LIMdlEdge* twin;
};

static int private_build_level (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge**      edges_left,
	int              target_index_count);

static float private_calculate_collapse_cost (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge*       edge);

static int private_collapse_edge (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge*        edge);

static LIMdlEdge* private_extract_edges (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	int              index_count,
	int*             edge_count);

static LIMdlEdge* private_extract_sliding_edges (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	int              index_count,
	LIMdlEdge*       edges,
	int              edge_count);

/*****************************************************************************/

/**
 * \brief Calculates a level-of-detail version of the model.
 * \param self Model.
 * \param factor Polygon reduction factor.
 * \return Nonzero on success.
 */
int limdl_builder_calculate_lod (
	LIMdlBuilder* self,
	int           levels,
	float         factor)
{
	int i;
	int target;
	int num_edges;
	int num_indices;
	LIMdlEdge* edges;
	LIMdlEdge* edges_left;
	LIMdlBuilderLod* lod;

	/* Calculate maximum memory use. */
	lod = self->lod.array + self->lod.count - 1;
	num_indices = 0;
	for (i = 0 ; i < lod->face_groups.count ; i++)
		num_indices += lod->face_groups.array[i].indices.count;
	if (num_indices < 3)
		return 1;

	/* Get a linked list of sliding edges. */
	edges = private_extract_edges (self, lod, num_indices, &num_edges);
	if (edges == NULL)
		return 0;
	edges_left = private_extract_sliding_edges (self, lod, num_indices, edges, num_edges);
	if (edges_left == NULL)
	{
		lisys_free (edges);
		return 1;
	}

	/* Calculate the levels of detail. */
	for (i = 0 ; i < levels ; i++)
	{
		if (!limdl_builder_add_detail_levels (self, 1))
		{
			lisys_free (edges);
			return 0;
		}
		lod = self->lod.array + self->lod.count - 1;
		target = (int)(num_indices - num_indices * factor * (i + 1) / levels);
		if (!private_build_level (self, lod, &edges_left, target))
		{
			lisys_free (edges);
			return 0;
		}
	}

	lisys_free (edges);

	return 1;
}

/*****************************************************************************/

static int private_build_level (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge**      edges_left,
	int              target_index_count)
{
	int num_indices;
	LIMdlEdge* edge;
	LIMdlEdge* best_edge;

	/* Calculate collapse costs. */
	/* TODO: Should be recalculated for each iteration but it's too slow
	   without an incremental recalculation algorithm. */
	for (edge = *edges_left ; edge != NULL ; edge = edge->next)
		edge->cost = private_calculate_collapse_cost (self, lod, edge);

	while (1)
	{
		/* Find the best edge to collapse. */
		/* TODO: This is slow, better sort first or use a priority queue. */
		best_edge = NULL;
		for (edge = *edges_left ; edge != NULL ; edge = edge->next)
		{
			if (best_edge == NULL || edge->cost < best_edge->cost)
				best_edge = edge;
		}
		if (best_edge == NULL)
			break;
		if (best_edge->cost >= 1000000.0f)
			break;

		/* Collapse the edge. */
		/* If the edge has a twin, collapse it too so that we can collapse
		   edges at UV seams without creating holes. */
		num_indices = private_collapse_edge (self, lod, best_edge);
		if (best_edge->twin)
			num_indices = private_collapse_edge (self, lod, best_edge->twin);
		if (num_indices <= target_index_count)
			break;

		/* Remove the edge from the linked list. */
		if (best_edge->prev != NULL)
			best_edge->prev->next = best_edge->next;
		else
			*edges_left = best_edge->next;
		if (best_edge->next != NULL)
			best_edge->next->prev = best_edge->prev;
	}

	return 1;
}

static float private_calculate_collapse_cost (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge*       edge)
{
	LIMdlVertex* v1 = self->model->vertices.array + edge->i1;
	LIMdlVertex* v2 = self->model->vertices.array + edge->i2;
	LIMatVector d_vtx = limat_vector_subtract (v1->coord, v2->coord);
	LIMatVector d_nml = limat_vector_subtract (v1->normal, v2->normal);
	float d_tex[2] = { v1->texcoord[0] - v2->texcoord[0], v1->texcoord[1] - v2->texcoord[1] };
	float d_col[4] = { v1->color[0] - v2->color[0], v1->color[1] - v2->color[1],
		v1->color[3] - v2->color[3], v1->color[4] - v2->color[4] };

	return limat_vector_dot (d_vtx, d_vtx) + limat_vector_dot (d_nml, d_nml) +
		d_tex[0] * d_tex[0] + d_tex[1] * d_tex[1] +
		d_col[0] * d_col[0] + d_col[1] * d_col[1] + d_col[2] * d_col[2] + d_col[3] * d_col[3];
}

static int private_collapse_edge (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	LIMdlEdge*       edge)
{
	int i;
	int j;
	int dst;
	int count;
	uint32_t face[3];
	LIMdlBuilderFaces* group;

	for (count = i = 0 ; i < lod->face_groups.count ; i++)
	{
		group = lod->face_groups.array + i;
		for (dst = j = 0 ; j < group->indices.count ; j += 3)
		{
			face[0] = group->indices.array[j];
			face[1] = group->indices.array[j + 1];
			face[2] = group->indices.array[j + 2];
			if (face[0] == edge->i1)
				group->indices.array[dst] = edge->i2;
			else
				group->indices.array[dst] = face[0];
			if (face[1] == edge->i1)
				group->indices.array[dst + 1] = edge->i2;
			else
				group->indices.array[dst + 1] = face[1];
			if (face[2] == edge->i1)
				group->indices.array[dst + 2] = edge->i2;
			else
				group->indices.array[dst + 2] = face[2];
			if ((face[0] == edge->i2) +
				(face[1] == edge->i2) +
				(face[2] == edge->i2) <= 1)
			{
				count += 3;
				dst += 3;
			}
		}
		group->indices.count = dst;
	}

	return count;
}

static LIMdlEdge* private_extract_edges (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	int              index_count,
	int*             edge_count)
{
	int i;
	int j;
	int k;
	uint32_t i1;
	uint32_t i2;
	uint32_t index;
	uint32_t face[4];
	LIAlgU32dic* dict;
	LIMdlEdge* edge;
	LIMdlEdge* edges;
	LIMdlBuilderFaces* group;

	/* Allocate the edge list. */
	edges = lisys_calloc (index_count, sizeof (LIMdlEdge));
	if (edges == NULL)
		return NULL;
	*edge_count = 0;

	/* Allocate a temporary lookup table. */
	dict = lialg_u32dic_new ();
	if (dict == NULL)
	{
		lisys_free (edges);
		return NULL;
	}

	/* Extract the edges using the index lists. */
	for (i = 0 ; i < lod->face_groups.count ; i++)
	{
		group = lod->face_groups.array + i;
		for (j = 0 ; j < group->indices.count ; j += 3)
		{
			face[0] = group->indices.array[j];
			face[1] = group->indices.array[j + 1];
			face[2] = group->indices.array[j + 2];
			face[3] = face[0];
			for (k = 0 ; k < 3 ; k++)
			{
				i1 = LIMAT_MIN (face[k], face[k + 1]);
				i2 = LIMAT_MAX (face[k], face[k + 1]);
				index = i1 + i2 * index_count;
				edge = lialg_u32dic_find (dict, index);
				if (edge == NULL)
				{
					edge = edges + *edge_count;
					edge->i1 = i1;
					edge->i2 = i2;
					edge->users = 1;
					lialg_u32dic_insert (dict, index, edge);
					*edge_count += 1;
				}
				else
					edge->users++;
			}
		}
	}

	/* Free the lookup table. */
	lialg_u32dic_free (dict);

	return edges;
}

static LIMdlEdge* private_extract_sliding_edges (
	LIMdlBuilder*    self,
	LIMdlBuilderLod* lod,
	int              index_count,
	LIMdlEdge*       edges,
	int              edge_count)
{
	int i;
	int j;
	char* locked_indices;
	uint32_t swap;
	LIMdlEdge* edge;
	LIMdlEdge* twin;
	LIMdlEdge* root = NULL;
	LIMdlIndex index;
	LIMdlVertex* verts;

	/* Allocate a temporary lookup table. */
	locked_indices = lisys_calloc (self->model->vertices.count, sizeof (char));
	if (locked_indices == NULL)
		return NULL;

	/* Mark non-manifold vertices as locked. */
	for (i = 0 ; i < edge_count ; i++)
	{
		edge = edges + i;
		if (edge->users == 1)
		{
			locked_indices[edge->i1] = 1;
			locked_indices[edge->i2] = 1;
		}
	}

	/* Find twin edges of non-manifold edges. */
	verts = self->model->vertices.array;
	for (i = 0 ; i < edge_count ; i++)
	{
		edge = edges + i;
		if (locked_indices[edge->i1] && locked_indices[edge->i2])
		{
			for (j = 0 ; j < edge_count ; j++)
			{
				twin = edges + j;
				if ((edge->i1 == twin->i1 && edge->i2 == twin->i2) ||
				    (edge->i1 == twin->i2 && edge->i2 == twin->i1))
					continue;
				if (limat_vector_compare (verts[edge->i1].coord, verts[twin->i1].coord, 0.001f) &&
				    limat_vector_compare (verts[edge->i2].coord, verts[twin->i2].coord, 0.001f))
				{
					edge->twin = twin;
					twin->twin = edge;
				}
				if (limat_vector_compare (verts[edge->i1].coord, verts[twin->i2].coord, 0.001f) &&
				    limat_vector_compare (verts[edge->i2].coord, verts[twin->i1].coord, 0.001f))
				{
					index = twin->i1;
					twin->i1 = twin->i2;
					twin->i2 = index;
					edge->twin = twin;
					twin->twin = edge;
				}
			}
		}
	}

	/* Create a linked list of slideable edges. */
	/* Edges with two locked vertices and no twin are rejected. */
	/* Edges with one locked vertex are reordered so that the free index slides. */
	for (i = 0 ; i < edge_count ; i++)
	{
		edge = edges + i;
		if (edge->twin == NULL && locked_indices[edge->i1] && locked_indices[edge->i2])
			continue;
		if (edge->twin == NULL && locked_indices[edge->i1])
		{
			swap = edge->i1;
			edge->i1 = edge->i2;
			edge->i2 = swap;
		}
		if (root)
		{
			root->prev = edge;
			edge->next = root;
		}
		root = edge;
	}

	/* Free the lookup table. */
	lisys_free (locked_indices);

	return root;
}

/** @} */
/** @} */
