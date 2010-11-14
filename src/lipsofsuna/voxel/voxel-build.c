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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxBuild Build
 * @{
 */

#include <lipsofsuna/model.h>
#include "voxel-build.h"
#include "voxel-material.h"

#define CULL_EPSILON 0.01f
#define MISSING_HEIGHT -10.0f

/* The builder isn't completely thread-safe at the moment. The biggest issue is
   that material data is queried from the voxel manager without locking. Our
   current scripts don't edit the materials after initialization so it works,
   but it needs to be fixed in the future. */
#warning Thread-safety issues in terrain builder

typedef struct _LIVoxVoxelB LIVoxVoxelB;
struct _LIVoxVoxelB
{
	int mask;
	int index;
	float height;
	LIEngModel* model;
	LIMatTransform transform;
	LIVoxMaterial* material;
};

struct _LIVoxBuilder
{
	int offset[3];
	int size[3];
	int step[3];
	char* occlud;
	float tile_width;
	float vertex_scale;
	LIAlgMemdic* accel;
	LIEngEngine* engine;
	LIMdlModel* model;
	LIPhyTerrain* physics;
	LIPhyPhysics* physics_manager;
	LIVoxManager* manager;
	LIVoxVoxel* voxels;
	LIVoxVoxelB* voxelsb;
};

static int
private_build (LIVoxBuilder* self,
               int           model,
               int           physics);

static void private_merge_height (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	LIMatVector   botco[3][3],
	LIMatVector   topco[3][3]);

static int
private_merge_tile_model (LIVoxBuilder* self,
                          LIVoxVoxelB*  voxel);

static int
private_merge_tile_physics (LIVoxBuilder* self,
                            LIVoxVoxelB*  voxel);

static int
private_merge_material (LIVoxBuilder*  self,
                        LIMdlMaterial* material);

static int
private_merge_vertex (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMdlVertex*  vertex);

static void private_merge_triangle (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           group,
	LIMdlVertex*  verts);

static int
private_occlude_face (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMdlVertex*  verts);

static inline void
private_solve_coords (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMatVector   bot[3][3],
                      LIMatVector   top[3][3]);

static inline float
private_solve_height_2 (LIVoxBuilder* self,
                        float         main,
                        float*        h0);

static inline float
private_solve_height_4 (LIVoxBuilder* self,
                        float         main,
                        float*        h0,
                        float*        h1,
                        float*        h2);

/*****************************************************************************/

LIVoxBuilder* livox_builder_new (
	LIVoxManager* manager,
	LIEngEngine*  engine,
	LIPhyPhysics* physics,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize)
{
	LIVoxBuilder* self;

	/* Allocate self. */
	self = lisys_calloc (1, sizeof (LIVoxBuilder));
	if (self == NULL)
		return NULL;
	self->manager = manager;
	self->engine = engine;
	self->physics_manager = physics;
	self->tile_width = manager->tile_width;
	self->vertex_scale = manager->tile_width * 0.5f;
	self->offset[0] = xstart;
	self->offset[1] = ystart;
	self->offset[2] = zstart;
	self->size[0] = xsize + 2;
	self->size[1] = ysize + 2;
	self->size[2] = zsize + 2;
	self->step[0] = 1;
	self->step[1] = self->size[0];
	self->step[2] = self->size[0] * self->size[1];
	self->occlud = lisys_calloc (self->size[0] * self->size[1] * self->size[2], sizeof (char));
	self->voxels = lisys_calloc (self->size[0] * self->size[1] * self->size[2], sizeof (LIVoxVoxel));
	self->voxelsb = lisys_calloc (self->size[0] * self->size[1] * self->size[2], sizeof (LIVoxVoxelB));
	if (self->occlud == NULL || self->voxels == NULL || self->voxelsb == NULL)
	{
		lisys_free (self->occlud);
		lisys_free (self->voxels);
		lisys_free (self->voxelsb);
		lisys_free (self);
		return NULL;
	}

	/* Fetch terrain data. */
	/* This is done in the initializer so that the builder doesn't need to
	   reference to external data. This helps with thread-safety. */
	livox_manager_copy_voxels (manager, xstart - 1, ystart - 1, zstart - 1,
		self->size[0], self->size[1], self->size[2], self->voxels);

	return self;
}

void livox_builder_free (
	LIVoxBuilder* self)
{
	lisys_free (self->occlud);
	lisys_free (self->voxels);
	lisys_free (self->voxelsb);
	lisys_free (self);
}

int livox_builder_build (
	LIVoxBuilder*  self,
	LIMdlModel**   result_model,
	LIPhyTerrain** result_physics)
{
	int i;
	int x;
	int y;
	int z;
	int count;
	int ret;
	LIMatVector offset;
	LIMatVector vector;
	LIVoxMaterial* material;
	LIVoxVoxel* voxel;

	/* Allocate temporary data. */
	count = 0;

	/* Calculate occlusion information. */
	i = livox_build_occlusion (self->manager, self->size[0], self->size[1], self->size[2], self->voxels, self->occlud);
	if (i == (self->size[0] - 2) * (self->size[1] - 2) * (self->size[2] - 2))
	{
		if (result_model != NULL) *result_model = NULL;
		if (result_physics != NULL) *result_physics = NULL;
		return 1;
	}

	/* Calculate area offset. */
	offset = limat_vector_init (self->offset[0] - 1, self->offset[1] - 1, self->offset[2] - 1);
	offset = limat_vector_multiply (offset, self->tile_width);

	/* Precalculate useful information on voxels. */
	for (z = 0 ; z < self->size[2] ; z++)
	for (y = 0 ; y < self->size[1] ; y++)
	for (x = 0 ; x < self->size[0] ; x++)
	{
		i = x + y * self->step[1] + z * self->step[2];
		self->voxelsb[i].height = MISSING_HEIGHT;

		/* Type check. */
		voxel = self->voxels + i;
		if (!voxel->type)
			continue;

		/* Material check. */
		material = livox_manager_find_material (self->manager, voxel->type);
		if (material == NULL)
			continue;
		self->voxelsb[i].material = material;

		/* Cache model. */
		if (material->type == LIVOX_MATERIAL_TYPE_TILE)
		{
			self->voxelsb[i].model = material->model;
			self->voxelsb[i].height = 1.0f;
		}

		/* Cache height. */
		if (material->type == LIVOX_MATERIAL_TYPE_HEIGHT)
			self->voxelsb[i].height = livox_voxel_get_height (voxel);

		/* Cache transformation. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, self->tile_width);
		self->voxelsb[i].transform.position = limat_vector_add (vector, offset);
		livox_voxel_get_quaternion (voxel, &self->voxelsb[i].transform.rotation);

		/* Occlusion check. */
		if (!(self->occlud[i] & LIVOX_OCCLUDE_OCCLUDED))
			count++;
		self->voxelsb[i].mask = self->occlud[i];
	}

	/* Store voxel coordinates so that we can save them to collision
	   objects and then later use them to determine the exact tile hit. */
	for (z = 0 ; z < self->size[2] - 2 ; z++)
	for (y = 0 ; y < self->size[1] - 2 ; y++)
	for (x = 0 ; x < self->size[0] - 2 ; x++)
	{
		i = (x + 1) + (y + 1) * self->step[1] + (z + 1) * self->step[2];
		self->voxelsb[i].index = x + y * (self->size[0] - 2) + z * (self->size[0] - 2) * (self->size[1] - 2);
	}

	/* Build mesh and/or physics. */
	ret = 1;
	if (count)
	{
		ret = private_build (self, result_model != NULL, result_physics != NULL);
		if (ret)
		{
			if (result_physics != NULL)
				*result_physics = self->physics;
			if (result_model != NULL)
				*result_model = self->model;
		}
		else
		{
			if (self->physics != NULL)
				liphy_terrain_free (self->physics);
			if (self->model != NULL)
				limdl_model_free (self->model);
		}
	}

	return ret;
}

/**
 * \brief Solves occlusion masks for a volume of voxels.
 *
 * \param manager Voxel manager.
 * \param size[0] Number of voxels.
 * \param size[1] Number of voxels.
 * \param size[2] Number of voxels.
 * \param voxels Array of voxels in the volume.
 * \param result Buffer with room for size[0]*size[1]*size[2] integers.
 * \return Number of completely occluded voxels.
 */
int livox_build_occlusion (
	LIVoxManager* manager,
	int           xsize,
	int           ysize,
	int           zsize,
	LIVoxVoxel*   voxels,
	char*         result)
{
	int i;
	int x;
	int y;
	int z;
	int count = 0;

	/* Solve occluders. */
	for (i = 0 ; i < xsize * ysize * zsize ; i++)
	{
		if (livox_manager_check_occluder (manager, voxels + i))
			result[i] = LIVOX_OCCLUDE_OCCLUDER;
		else
			result[i] = 0;
	}

	/* Solve occlusion. */
	for (z = 1 ; z < zsize - 1 ; z++)
	for (y = 1 ; y < ysize - 1 ; y++)
	for (x = 1 ; x < xsize - 1 ; x++)
	{
		/* Check for occlusion from each side. */
		i = x + y * xsize + z * xsize * ysize;
		if (result[i - 1] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_XNEG;
		if (result[i + 1] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_XPOS;
		if (result[i - xsize] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_YNEG;
		if (result[i + xsize] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_YPOS;
		if (result[i - xsize * ysize] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_ZNEG;
		if (result[i + xsize * ysize] & LIVOX_OCCLUDE_OCCLUDER)
			result[i] |= LIVOX_OCCLUDE_ZPOS;
		if ((result[i] & LIVOX_OCCLUDE_ALL) == LIVOX_OCCLUDE_ALL)
		{
			result[i] |= LIVOX_OCCLUDE_OCCLUDED;
			count++;
		}
	}

	return count;
}

/*****************************************************************************/

static int
private_build (LIVoxBuilder* self,
               int           model,
               int           physics)
{
	int x;
	int y;
	int z;
	int ret = 1;
	int size[3];
	LIMatVector botco[3][3];
	LIMatVector topco[3][3];
	LIVoxVoxelB* voxel;

	/* Allocate vertex cache. */
	if (model)
	{
		self->accel = lialg_memdic_new ();
		if (self->accel == NULL)
			return 0;
	}

	/* Build all voxels inside the area. */
	for (z = 1 ; z < self->size[2] - 1 ; z++)
	for (y = 1 ; y < self->size[1] - 1 ; y++)
	for (x = 1 ; x < self->size[0] - 1 ; x++)
	{
		voxel = self->voxelsb + x + y * self->step[1] + z * self->step[2];
		if (voxel->material == NULL)
			continue;

		/* Create outputs. */
		if (model && self->model == NULL)
		{
			self->model = limdl_model_new ();
			if (self->model == NULL)
				ret = 0;
		}
		if (physics && self->physics == NULL && self->physics_manager != NULL)
		{
			size[0] = self->size[0] - 2;
			size[1] = self->size[1] - 2;
			size[2] = self->size[2] - 2;
			self->physics = liphy_terrain_new (self->physics_manager, self->offset, size,
				LIPHY_GROUP_TILES, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
			if (self->physics == NULL)
				ret = 0;
		}

		/* Merge tile model. */
		if (voxel->model != NULL)
		{
			if (self->model != NULL)
				private_merge_tile_model (self, voxel);
			if (self->physics != NULL)
				private_merge_tile_physics (self, voxel);
		}

		/* Merge heightmap. */
		if (voxel->material->type == LIVOX_MATERIAL_TYPE_HEIGHT)
		{
			private_solve_coords (self, voxel, botco, topco);
			private_merge_height (self, voxel, botco, topco);
		}
	}

	/* Calculate bounds needed by frustum culling. */
	if (self->model != NULL)
		limdl_model_calculate_bounds (self->model);

	/* Free vertex cache. */
	if (model)
		lialg_memdic_free (self->accel);

	return ret;
}

static void private_merge_height (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	LIMatVector   botco[3][3],
	LIMatVector   topco[3][3])
{
	int g0 = 0;
	int g1 = 0;
	int i;
	int k;
	int x;
	int z;
	float ssurf;
	float sside;
	LIMatVector ex[2][3];
	LIMatVector ez[2][3];
	LIMatVector n[4];
	LIMatVector uv[8];
	LIMdlVertex bot[3][3];
	LIMdlVertex top[3][3];
	LIMdlVertex side[12][2];
	LIMdlVertex verts[3];
	static const int quadidx[2][12] =
	{
		{ 1, 0, 0, 0, 0, 1,   1, 1, 1, 0, 0, 1 },
		{ 1, 0, 0, 0, 1, 1,   1, 1, 0, 0, 0, 1 }
	};
	static const int quadidxrev[2][12] =
	{
		{ 0, 0, 1, 0, 0, 1,   1, 0, 1, 1, 0, 1 },
		{ 0, 0, 1, 0, 1, 1,   0, 0, 1, 1, 0, 1 }
	};

	/* Get texture scale factors. */
	ssurf = 1.0f - voxel->material->mat_top.emission;
	sside = 1.0f - voxel->material->mat_side.emission;

	/* Calculate edges for top surface. */
	for (x = 0 ; x < 3 ; x++)
	{
		ex[0][x] = limat_vector_normalize (limat_vector_subtract (topco[1][x], topco[0][x]));
		ex[1][x] = limat_vector_normalize (limat_vector_subtract (topco[2][x], topco[1][x]));
		ez[0][x] = limat_vector_normalize (limat_vector_subtract (topco[x][0], topco[x][1]));
		ez[1][x] = limat_vector_normalize (limat_vector_subtract (topco[x][1], topco[x][2]));
	}

	/* Build top and bottom vertices. */
	for (z = 0 ; z < 3 ; z++)
	for (x = 0 ; x < 3 ; x++)
	{
		n[0] = limat_vector_init (0.0f, 1.0f, 0.0f);
		n[1] = limat_vector_init (0.0f, -1.0f, 0.0f);
		uv[0] = limat_transform_transform (voxel->transform, topco[x][z]);
		uv[1] = limat_transform_transform (voxel->transform, botco[x][z]);
		limdl_vertex_init (&(top[x][z]), &(topco[x][z]), n + 0, ssurf * uv[0].x, ssurf * uv[0].z);
		limdl_vertex_init (&(bot[x][z]), &(botco[x][z]), n + 1, sside * uv[1].x, sside * uv[1].z);
	}

	/* Calculate smooth normals. */
	if (self->model != NULL)
	{
		top[0][0].normal = limat_vector_normalize (
			limat_vector_cross (ex[0][0], ez[0][0]));
		top[1][0].normal = limat_vector_normalize (
			limat_vector_add (
			limat_vector_cross (ex[0][0], ez[0][1]),
			limat_vector_cross (ex[1][0], ez[0][1])));
		top[2][0].normal = limat_vector_normalize (
			limat_vector_cross (ex[1][0], ez[0][2]));
		top[0][1].normal = limat_vector_normalize (
			limat_vector_add (
			limat_vector_cross (ex[0][1], ez[0][0]),
			limat_vector_cross (ex[0][1], ez[1][0])));
		top[1][1].normal = limat_vector_normalize (
			limat_vector_add (limat_vector_add (limat_vector_add (
			limat_vector_cross (ex[0][1], ez[0][1]),
			limat_vector_cross (ex[0][1], ez[1][1])),
			limat_vector_cross (ex[1][1], ez[0][1])),
			limat_vector_cross (ex[1][1], ez[1][1])));
		top[2][1].normal = limat_vector_normalize (
			limat_vector_add (
			limat_vector_cross (ex[1][1], ez[0][2]),
			limat_vector_cross (ex[1][1], ez[1][2])));
		top[0][2].normal = limat_vector_normalize (
			limat_vector_cross (ex[0][2], ez[1][0]));
		top[1][2].normal = limat_vector_normalize (
			limat_vector_add (
			limat_vector_cross (ex[0][2], ez[1][1]),
			limat_vector_cross (ex[1][2], ez[1][1])));
		top[2][2].normal = limat_vector_normalize (
			limat_vector_cross (ex[1][2], ez[1][2]));
	}

	/* Build side vertices. */
	for (x = 0 ; x < 3 ; x++)
	{
		n[0] = limat_vector_init (0.0f, 0.0f, -1.0f);
		n[1] = limat_vector_init (1.0f, 0.0f, 0.0f);
		n[2] = limat_vector_init (0.0f, 0.0f, 1.0f);
		n[3] = limat_vector_init (-1.0f, 0.0f, 0.0f);
		uv[0] = limat_transform_transform (voxel->transform, botco[x][0]);
		uv[1] = limat_transform_transform (voxel->transform, topco[x][0]);
		uv[2] = limat_transform_transform (voxel->transform, botco[2][x]);
		uv[3] = limat_transform_transform (voxel->transform, topco[2][x]);
		uv[4] = limat_transform_transform (voxel->transform, botco[2 - x][2]);
		uv[5] = limat_transform_transform (voxel->transform, topco[2 - x][2]);
		uv[6] = limat_transform_transform (voxel->transform, botco[0][2 - x]);
		uv[7] = limat_transform_transform (voxel->transform, topco[0][2 - x]);
		limdl_vertex_init (&(side[x][0]), &(botco[x][0]), n + 0, sside * uv[0].x, sside * uv[0].y);
		limdl_vertex_init (&(side[x][1]), &(topco[x][0]), n + 0, sside * uv[1].x, sside * uv[1].y);
		limdl_vertex_init (&(side[3 + x][0]), &(botco[2][x]), n + 1, sside * uv[2].z, sside * uv[2].y);
		limdl_vertex_init (&(side[3 + x][1]), &(topco[2][x]), n + 1, sside * uv[3].z, sside * uv[3].y);
		limdl_vertex_init (&(side[8 - x][0]), &(botco[2 - x][2]), n + 2, sside * uv[4].x, sside * uv[4].y);
		limdl_vertex_init (&(side[8 - x][1]), &(topco[2 - x][2]), n + 2, sside * uv[5].x, sside * uv[5].y);
		limdl_vertex_init (&(side[11 - x][0]), &(botco[0][2 - x]), n + 3, sside * uv[6].z, sside * uv[6].y);
		limdl_vertex_init (&(side[11 - x][1]), &(topco[0][2 - x]), n + 3, sside * uv[7].z, sside * uv[7].y);
	}

	/* Find or create materials. */
	if (self->model != NULL)
	{
		g0 = private_merge_material (self, &voxel->material->mat_top);
		if (g0 == -1)
			return;
		g1 = private_merge_material (self, &voxel->material->mat_side);
		if (g1 == -1)
			return;
	}

	/* Create top and bottom triangles. */
	for (z = 0 ; z < 2 ; z++)
	for (x = 0 ; x < 2 ; x++)
	for (i = 0 ; i < 2 ; i++)
	{
		/* Top surface. */
		for (k = 0 ; k < 3 ; k++)
			verts[k] = top[x + quadidx[z == x][2*k + 6*i]][z + quadidx[z == x][2*k + 6*i + 1]];
		if (!private_occlude_face (self, voxel, verts))
			private_merge_triangle (self, voxel, g0, verts);

		/* Bottom surface. */
		for (k = 0 ; k < 3 ; k++)
			verts[k] = bot[x + quadidxrev[z == x][2*k + 6*i]][z + quadidxrev[z == x][2*k + 6*i + 1]];
		if (!private_occlude_face (self, voxel, verts))
			private_merge_triangle (self, voxel, g1, verts);
	}

	/* Create side triangles. */
	for (z = 0 ; z < 4 ; z++)
	for (x = 0 ; x < 2 ; x++)
	for (i = 0 ; i < 2 ; i++)
	{
		for (k = 0 ; k < 3 ; k++)
		{
			if (z == 2 || z == 3)
				verts[k] = side[3 * z + x + quadidxrev[0][2*k + 6*i]][quadidxrev[0][2*k + 6*i + 1]];
			else
				verts[k] = side[3 * z + x + quadidx[0][2*k + 6*i]][quadidx[0][2*k + 6*i + 1]];
		}
		if (!private_occlude_face (self, voxel, verts))
			private_merge_triangle (self, voxel, g1, verts);
	}
}

static int private_merge_tile_model (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel)
{
	int g;
	int i;
	int j;
	int k;
	float scale;
	uint32_t indices[3];
	LIMatTransform transform;
	LIMdlFaces* faces;
	LIMdlMaterial* material;
	LIMdlNode* node;
	LIMdlVertex verts[3];
	LIMdlModel* model;

	model = voxel->model->model;

	/* Add each light source. */
	for (i = 0 ; i < model->nodes.count ; i++)
	{
		for (j = 0 ; j < model->nodes.array[i]->nodes.count ; j++)
		{
			node = model->nodes.array[i]->nodes.array[j];
			if (node->type == LIMDL_NODE_LIGHT)
			{
				if (limdl_model_insert_node (self->model, node))
				{
					node = self->model->nodes.array[self->model->nodes.count - 1];
					limdl_node_get_world_transform (node, &scale, &transform);
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

		/* Find or create face group. */
		g = private_merge_material (self, material);
		if (g == -1)
			continue;

		/* Add each face. */
		for (j = 0 ; j < faces->indices.count ; j += 3)
		{
			/* Get vertices. */
			for (k = 0 ; k < 3 ; k++)
				verts[k] = model->vertices.array[faces->indices.array[j + k]];

			/* Occlusion check. */
			if (private_occlude_face (self, voxel, verts))
				continue;

			/* Insert vertices to model. */
			for (k = 0 ; k < 3 ; k++)
			{
				verts[k].coord = limat_vector_multiply (verts[k].coord, self->vertex_scale);
				indices[k] = private_merge_vertex (self, voxel, verts + k);
				if (indices[k] == -1)
					break;
			}

			/* Insert indices to model. */
			if (k == 3)
				limdl_model_insert_indices (self->model, g, indices, 3);
		}
	}

	return 1;
}

static int private_merge_tile_physics (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel)
{
	LIPhyModel* model;

	model = liphy_physics_find_model (self->physics_manager, voxel->model->id);
	if (model == NULL)
		return 0;
	return liphy_terrain_add_model (self->physics, voxel->index,
		model, &voxel->transform, self->vertex_scale);
}

static int
private_merge_material (LIVoxBuilder*  self,
                        LIMdlMaterial* material)
{
	int g;
	int m;

	/* Find or create material. */
	m = limdl_model_find_material (self->model, material);
	if (m == -1)
	{
		m = self->model->materials.count;
		if (!limdl_model_insert_material (self->model, material))
			return -1;
	}

	/* Find or create face group. */
	g = limdl_model_find_facegroup (self->model, m);
	if (g == -1)
	{
		g = self->model->facegroups.count;
		if (!limdl_model_insert_facegroup (self->model, m))
			return -1;
	}

	return g;
}

static int
private_merge_vertex (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMdlVertex*  vertex)
{
	void* ptr;
	uint32_t index;
	LIMdlVertex vert;

	vert = *vertex;
	vert.coord = limat_transform_transform (voxel->transform, vertex->coord);
	vert.normal = limat_quaternion_transform (voxel->transform.rotation, vertex->normal);

	ptr = lialg_memdic_find (self->accel, &vert, sizeof (LIMdlVertex));
	if (ptr == NULL)
	{
		/* Add to model. */
		index = self->model->vertices.count;
		if (!limdl_model_insert_vertex (self->model, &vert))
			return -1;

		/* Add to lookup. */
		ptr = (void*)(intptr_t)(index + 1);
		if (!lialg_memdic_insert (self->accel, &vert, sizeof (LIMdlVertex), ptr))
			return -1;
	}
	else
		index = ((int)(intptr_t) ptr) - 1;

	return index;
}

static int
private_occlude_face (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMdlVertex*  verts)
{
	int i;
	LIMatVector coords[3];

	/* Get rotated vertices. */
	for (i = 0 ; i < 3 ; i++)
		coords[i] = limat_quaternion_transform (voxel->transform.rotation, verts[i].coord);

	/* Occlusion check. */
	if ((voxel->mask & LIVOX_OCCLUDE_XNEG) &&
		LIMAT_ABS (coords[0].x + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].x + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].x + self->tile_width / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_XPOS) &&
		LIMAT_ABS (coords[0].x - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].x - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].x - self->tile_width / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_YNEG) &&
		LIMAT_ABS (coords[0].y + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].y + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].y + self->tile_width / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_YPOS) &&
		LIMAT_ABS (coords[0].y - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].y - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].y - self->tile_width / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_ZNEG) &&
		LIMAT_ABS (coords[0].z + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].z + self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].z + self->tile_width / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_ZPOS) &&
		LIMAT_ABS (coords[0].z - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].z - self->tile_width / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].z - self->tile_width / 2) < CULL_EPSILON)
		return 1;

	return 0;
}

static void private_merge_triangle (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           group,
	LIMdlVertex*  verts)
{
	int k;
	uint32_t indices[3];
	LIMatVector coords[3];

	/* Merge to display model. */
	if (self->model != NULL)
	{
		for (k = 0 ; k < 3 ; k++)
		{
			indices[k] = private_merge_vertex (self, voxel, verts + k);
			if (indices[k] == -1)
				break;
		}
		if (k == 3)
			limdl_model_insert_indices (self->model, group, indices, 3);
	}

	/* Merge to physics model. */
	if (self->physics != NULL)
	{
		coords[0] = verts[0].coord;
		coords[1] = verts[1].coord;
		coords[2] = verts[2].coord;
		liphy_terrain_add_vertices (self->physics, voxel->index, coords, 3, &voxel->transform);
	}
}

static inline void private_solve_coords (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	LIMatVector   bot[3][3],
	LIMatVector   top[3][3])
{
	int x;
	int y;
	int z;
	float h[3][3][3];
	float size = 0.5f * self->tile_width;

	/* Cache tile heights. */
	for (z = -1 ; z <= 1 ; z++)
	for (y = -1 ; y <= 1 ; y++)
	for (x = -1 ; x <= 1 ; x++)
		h[x + 1][z + 1][y + 1] = voxel[x + y * self->step[1] + z * self->step[2]].height;

	/* Construct X and Z coordinates. */
	for (z = 0 ; z < 3 ; z++)
	for (x = 0 ; x < 3 ; x++)
	{
		/* Top. */
		top[x][z] = limat_vector_init (
			(-1.0f + x) * size, size,
			(-1.0f + z) * size);

		/* Bottom. */
		bot[x][z] = limat_vector_init (
			(-1.0f + x) * size, -size,
			(-1.0f + z) * size);
	}

	/* Construct Y coordinate. */
	if (voxel[self->step[1]].material == NULL ||
	    voxel[self->step[1]].material->type != LIVOX_MATERIAL_TYPE_HEIGHT)
	{
		top[0][0].y = size * private_solve_height_4 (self, h[1][1][1], h[0][0], h[0][1], h[1][0]);
		top[1][0].y = size * private_solve_height_2 (self, h[1][1][1], h[1][0]);
		top[2][0].y = size * private_solve_height_4 (self, h[1][1][1], h[1][0], h[2][0], h[2][1]);
		top[0][1].y = size * private_solve_height_2 (self, h[1][1][1], h[0][1]);
		top[1][1].y = size * h[1][1][1];
		top[2][1].y = size * private_solve_height_2 (self, h[1][1][1], h[2][1]);
		top[0][2].y = size * private_solve_height_4 (self, h[1][1][1], h[0][1], h[0][2], h[1][2]);
		top[1][2].y = size * private_solve_height_2 (self, h[1][1][1], h[1][2]);
		top[2][2].y = size * private_solve_height_4 (self, h[1][1][1], h[2][1], h[1][2], h[2][2]);
	}
}

static inline float
private_solve_height_2 (LIVoxBuilder* self,
                        float         main,
                        float*        h0)
{
	float tmp0;

	if (h0[2] != MISSING_HEIGHT)
		return 1.0f;
	if (h0[1] == MISSING_HEIGHT)
		return -1.0f;
	tmp0 = (h0[2] != MISSING_HEIGHT)? h0[2] : LIMAT_CLAMP (h0[1], -1.0f, 1.0f);
	return 0.5f * (main + tmp0);
}

static inline float
private_solve_height_4 (LIVoxBuilder* self,
                        float         main,
                        float*        h0,
                        float*        h1,
                        float*        h2)
{
	float tmp0;
	float tmp1;
	float tmp2;

	if (h0[2] != MISSING_HEIGHT || h1[2] != MISSING_HEIGHT || h2[2] != MISSING_HEIGHT)
		return 1.0f;
	if (h0[1] == MISSING_HEIGHT || h1[1] == MISSING_HEIGHT || h2[1] == MISSING_HEIGHT)
		return -1.0f;
	tmp0 = (h0[2] != MISSING_HEIGHT)? h0[2] : LIMAT_CLAMP (h0[1], -1.0f, 1.0f);
	tmp1 = (h1[2] != MISSING_HEIGHT)? h1[2] : LIMAT_CLAMP (h1[1], -1.0f, 1.0f);
	tmp2 = (h2[2] != MISSING_HEIGHT)? h2[2] : LIMAT_CLAMP (h2[1], -1.0f, 1.0f);
	return 0.25f * (main + tmp0 + tmp1 + tmp2);
}

/** @} */
/** @} */
