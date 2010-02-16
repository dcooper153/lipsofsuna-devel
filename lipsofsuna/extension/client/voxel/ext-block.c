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
 * \addtogroup liextcliVoxel Voxel
 * @{
 */

#include <lipsofsuna/network.h>
#include "ext-module.h"

#define CULL_EPSILON 0.01f
#define LINE (LIVOX_TILES_PER_LINE + 2)

typedef struct _LIExtVoxel LIExtVoxel;
struct _LIExtVoxel
{
	int mask;
	LIEngModel* model;
	LIMatTransform transform;
};

static void
private_build_quickly (LIExtBlock*  self,
                       LIExtModule* module,
                       LIExtVoxel*  voxels);

static void
private_build_slowly (LIExtBlock*  self,
                       LIExtModule* module,
                       LIExtVoxel*  voxels);

static void
private_build_physics (LIExtBlock*  self,
                       LIExtModule* module,
                       LIExtVoxel*  voxels);

static void
private_merge_model (LIExtBlock* self,
                     LIExtVoxel* voxel,
                     LIMdlModel* model);

/*****************************************************************************/

LIExtBlock*
liext_block_new (LICliClient* client)
{
	LIExtBlock* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIExtBlock));
	if (self == NULL)
		return NULL;
	self->client = client;

	return self;
}

void
liext_block_free (LIExtBlock* self)
{
	if (self->group != NULL)
		liren_group_free (self->group);
	if (self->rmodel != NULL)
		liren_model_free (self->rmodel);
	if (self->mmodel != NULL)
		limdl_model_free (self->mmodel);
	if (self->physics != NULL)
		liphy_object_free (self->physics);
	lisys_free (self);
}

void
liext_block_clear (LIExtBlock* self)
{
	if (self->group != NULL)
	{
		liren_group_free (self->group);
		self->group = NULL;
	}
	if (self->rmodel != NULL)
	{
		liren_model_free (self->rmodel);
		self->rmodel = NULL;
	}
	if (self->mmodel != NULL)
	{
		limdl_model_free (self->mmodel);
		self->mmodel = NULL;
	}
	if (self->physics != NULL)
	{
		liphy_object_free (self->physics);
		self->physics = NULL;
	}
}

int
liext_block_build (LIExtBlock*     self,
                   LIExtModule*    module,
                   LIVoxBlock*     block,
                   LIVoxBlockAddr* addr)
{
	int i;
	int x;
	int y;
	int z;
	int index;
	int count;
	int occlusion[LINE * LINE * LINE];
	LIEngModel* model;
	LIMatTransform transform;
	LIMatVector vector;
	LIMatVector offset;
	LIVoxMaterial* material;
	LIVoxVoxel* voxel;
	LIVoxVoxel voxels[LINE * LINE * LINE];
	LIExtVoxel info[LIVOX_TILES_PER_BLOCK];

	/* Free old objects. */
	liext_block_clear (self);

	/* Fetch voxel data. */
	livox_manager_copy_voxels (module->voxels,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[0] + addr->block[0]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[1] + addr->block[1]) - 1,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[2] + addr->block[2]) - 1,
		LINE, LINE, LINE, voxels);

	/* Calculate occlusion. */
	i = livox_manager_solve_occlusion (module->voxels, LINE, LINE, LINE, voxels, occlusion);
	if (i == LIVOX_TILES_PER_BLOCK)
		return 1;

	/* Calculate offset. */
	vector = limat_vector_init (addr->sector[0], addr->sector[1], addr->sector[2]);
	vector = limat_vector_multiply (vector, LIVOX_SECTOR_WIDTH);
	offset = limat_vector_init (addr->block[0], addr->block[1], addr->block[2]);
	offset = limat_vector_multiply (offset, LIVOX_BLOCK_WIDTH);
	offset = limat_vector_add (offset, vector);
	transform = limat_transform_identity ();

	/* Clear object data. */
	memset (info, 0, sizeof (info));
	i = 0;
	count = 0;

	/* Prebuild object data. */
	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++, i++)
	{
		/* Type check. */
		index = (x + 1) + (y + 1) * LINE + (z + 1) * LINE * LINE;
		voxel = voxels + index;
		if (!voxel->type)
			continue;

		/* Occlusion check. */
		if (occlusion[index] & LIVOX_OCCLUDE_OCCLUDED)
			continue;

		/* Engine model. */
		material = livox_manager_find_material (module->voxels, voxel->type);
		if (material == NULL)
			continue;
		model = lieng_engine_find_model_by_name (module->client->engine, material->model);
		if (model == NULL)
			continue;
		info[i].model = model;

		/* Transformation. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		info[i].transform.position = limat_vector_add (vector, offset);
		livox_voxel_get_quaternion (voxel, &info[i].transform.rotation);
		count++;
	}
	if (!count)
		return 1;

	/* Build new objects. */
	private_build_physics (self, module, info);
	//private_build_quickly (self, module, info);
	private_build_slowly (self, module, info);

	/* Create render model if not empty. */
	if (self->mmodel != NULL)
	{
		self->rmodel = liren_model_new (self->client->render, self->mmodel, NULL);
		if (self->rmodel != NULL)
		{
			self->group = liren_group_new (self->client->scene);
			if (self->group != NULL)
				liren_group_insert_model (self->group, self->rmodel, &transform);
		}
	}

	/* Realize if not empty. */
	if (self->group != NULL)
		liren_group_set_realized (self->group, 1);
	if (self->physics != NULL)
	{
		liphy_object_set_collision_group (self->physics, LIPHY_GROUP_TILES);
		liphy_object_set_collision_mask (self->physics, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		liphy_object_set_realized (self->physics, 1);
	}

	return 1;
}

/*****************************************************************************/

static void
private_build_quickly (LIExtBlock*  self,
                       LIExtModule* module,
                       LIExtVoxel*  voxels)
{
	int i = 0;
	int x;
	int y;
	int z;
	LIRenModel* model;

	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++, i++)
	{
		/* Get model data. */
		if (voxels[i].model == NULL)
			continue;

		/* Get render model. */
		model = liren_render_find_model (module->client->render, voxels[i].model->name);
		if (model == NULL)
		{
			liren_render_load_model (module->client->render, voxels[i].model->name, voxels[i].model->model);
			model = liren_render_find_model (module->client->render, voxels[i].model->name);
			if (model == NULL)
				continue;
		}

		/* Add to render group. */
		if (self->group == NULL)
			self->group = liren_group_new (self->client->scene);
		if (self->group != NULL)
			liren_group_insert_model (self->group, model, &voxels[i].transform);
	}
}

static void
private_build_slowly (LIExtBlock*  self,
                      LIExtModule* module,
                      LIExtVoxel*  voxels)
{
	int i = 0;
	int x;
	int y;
	int z;
	LIMdlModel* model;

	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++, i++)
	{
		/* Get model data. */
		if (voxels[i].model == NULL)
			continue;
		model = voxels[i].model->model;

		/* Add to render group. */
		if (self->mmodel == NULL)
			self->mmodel = limdl_model_new ();
		if (self->mmodel != NULL)
			private_merge_model (self, voxels + i, model);
	}

	/* Needed by frustum culling. */
	if (self->mmodel != NULL)
		limdl_model_calculate_bounds (self->mmodel);
}

static void
private_build_physics (LIExtBlock*  self,
                       LIExtModule* module,
                       LIExtVoxel*  voxels)
{
	int i = 0;
	int x;
	int y;
	int z;

	for (z = 0 ; z < LIVOX_TILES_PER_LINE ; z++)
	for (y = 0 ; y < LIVOX_TILES_PER_LINE ; y++)
	for (x = 0 ; x < LIVOX_TILES_PER_LINE ; x++, i++)
	{
		/* Get model data. */
		if (voxels[i].model == NULL)
			continue;

		/* Add to physics object. */
		if (self->physics == NULL)
			self->physics = liphy_object_new (self->client->engine->physics, 0, NULL, LIPHY_CONTROL_MODE_STATIC);
		if (self->physics != NULL)
			liphy_object_insert_shape (self->physics, voxels[i].model->physics, &voxels[i].transform);
	}
}

static void
private_merge_model (LIExtBlock* self,
                     LIExtVoxel* voxel,
                     LIMdlModel* model)
{
	int g;
	int i;
	int j;
	int k;
	int m;
	void* ptr;
	uint32_t indices[3];
	LIAlgMemdic* accel;
	LIMatTransform transform;
	LIMdlFaces* faces;
	LIMdlMaterial* material;
	LIMdlNode* node;
	LIMdlVertex verts[3];

	accel = lialg_memdic_new ();
	if (accel == NULL)
		return;

	/* Add each light source. */
	for (i = 0 ; i < model->nodes.count ; i++)
	{
		for (j = 0 ; j < model->nodes.array[i]->nodes.count ; j++)
		{
			node = model->nodes.array[i]->nodes.array[j];
			if (node->type == LIMDL_NODE_LIGHT)
			{
				if (limdl_model_insert_node (self->mmodel, node))
				{
					node = self->mmodel->nodes.array[self->mmodel->nodes.count - 1];
					limdl_node_get_world_transform (node, &transform);
					transform = limat_transform_multiply (transform, voxel->transform);
					node->transform.rest = limat_transform_identity ();
					node->transform.local = transform;
					node->transform.global = transform;
				}
			}
		}
	}

	/* Add each face group. */
	for (i = 0 ; i < model->facegroups.count ; i++)
	{
		faces = model->facegroups.array + i;
		material = model->materials.array + faces->material;

		/* Find or create material. */
		m = limdl_model_find_material (self->mmodel, material);
		if (m == -1)
		{
			m = self->mmodel->materials.count;
			if (!limdl_model_insert_material (self->mmodel, material))
				continue;
		}

		/* Find or create face group. */
		g = limdl_model_find_facegroup (self->mmodel, m);
		if (g == -1)
		{
			g = self->mmodel->facegroups.count;
			if (!limdl_model_insert_facegroup (self->mmodel, m))
				continue;
		}

		/* Add each face. */
		for (j = 0 ; j < faces->indices.count ; j += 3)
		{
			/* Get rotated vertices. */
			for (k = 0 ; k < 3 ; k++)
			{
				verts[k] = model->vertices.array[faces->indices.array[j + k]];
				verts[k].coord = limat_quaternion_transform (voxel->transform.rotation, verts[k].coord);
			}

			/* Occlusion check. */
			if ((voxel->mask & 0x01) &&
				LIMAT_ABS (verts[0].coord.x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;
			if ((voxel->mask & 0x02) &&
				LIMAT_ABS (verts[0].coord.x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;
			if ((voxel->mask & 0x04) &&
				LIMAT_ABS (verts[0].coord.y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;
			if ((voxel->mask & 0x08) &&
				LIMAT_ABS (verts[0].coord.y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;
			if ((voxel->mask & 0x10) &&
				LIMAT_ABS (verts[0].coord.z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;
			if ((voxel->mask & 0x20) &&
				LIMAT_ABS (verts[0].coord.z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[1].coord.z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
				LIMAT_ABS (verts[2].coord.z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
				continue;

			/* Get vertices in world space. */
			for (k = 0 ; k < 3 ; k++)
			{
				verts[k] = model->vertices.array[faces->indices.array[j + k]];
				verts[k].coord = limat_transform_transform (voxel->transform, verts[k].coord);
				verts[k].normal = limat_quaternion_transform (voxel->transform.rotation, verts[k].normal);
			}

			/* Insert vertices to model. */
			for (k = 0 ; k < 3 ; k++)
			{
				ptr = lialg_memdic_find (accel, verts + k, sizeof (LIMdlVertex));
				if (ptr == NULL)
				{
					/* Add to model. */
					indices[k] = self->mmodel->vertices.count;
					if (!limdl_model_insert_vertex (self->mmodel, verts + k, NULL))
						continue;

					/* Add to lookup. */
					ptr = (void*)(intptr_t) indices[k];
					if (!lialg_memdic_insert (accel, verts + k, sizeof (LIMdlVertex), ptr));
						continue;
				}
				else
					indices[k] = (int)(intptr_t) ptr;
			}

			/* Insert indices to model. */
			limdl_model_insert_indices (self->mmodel, g, indices, 3);
		}
	}

	lialg_memdic_free (accel);
}

/** @} */
/** @} */
/** @} */
