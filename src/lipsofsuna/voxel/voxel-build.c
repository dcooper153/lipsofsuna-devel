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

/* The builder isn't completely thread-safe at the moment. The biggest issue is
   that material data is queried from the voxel manager without locking. Our
   current scripts don't edit the materials after initialization so it works,
   but it needs to be fixed in the future. */
#warning Thread-safety issues in terrain builder

typedef struct _LIVoxVoxelB LIVoxVoxelB;
struct _LIVoxVoxelB
{
	int index;
	int type;
	LIEngModel* model;
	LIMatVector position;
	LIVoxMaterial* material;
};

struct _LIVoxBuilder
{
	int model_wanted;
	int physics_wanted;
	int offset[3];
	int size[3];
	int step[3];
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

static int private_build (
	LIVoxBuilder* self);

static int private_merge_material (
	LIVoxBuilder*  self,
	LIMdlMaterial* material);

static void private_merge_triangle (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           group,
	LIMdlVertex*  verts);

static int private_merge_vertex (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	LIMdlVertex*  vertex);

static void private_merge_voxel (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz);

static int private_triangulate_cube (
	LIVoxBuilder* self,
	int           types[3][3][3],
	LIMatVector*  result);

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
	self->voxels = lisys_calloc (self->size[0] * self->size[1] * self->size[2], sizeof (LIVoxVoxel));
	self->voxelsb = lisys_calloc (self->size[0] * self->size[1] * self->size[2], sizeof (LIVoxVoxelB));
	if (self->voxels == NULL || self->voxelsb == NULL)
	{
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

	count = 0;
	self->model_wanted = (result_model != NULL);
	self->physics_wanted = (result_physics != NULL);

	/* Calculate area offset. */
	offset = limat_vector_init (self->offset[0] - 1.5f, self->offset[1] - 1.5f, self->offset[2] - 1.5f);
	offset = limat_vector_multiply (offset, self->tile_width);

	/* Precalculate useful information on voxels. */
	for (z = 0 ; z < self->size[2] ; z++)
	for (y = 0 ; y < self->size[1] ; y++)
	for (x = 0 ; x < self->size[0] ; x++)
	{
		i = x + y * self->step[1] + z * self->step[2];
		self->voxelsb[i].type = 0;

		/* Type check. */
		voxel = self->voxels + i;
		if (!voxel->type)
			continue;

		/* Material check. */
		material = livox_manager_find_material (self->manager, voxel->type);
		if (material == NULL)
			continue;
		self->voxelsb[i].material = material;
		self->voxelsb[i].type = voxel->type;
		count++;

		/* Cache position. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, self->tile_width);
		self->voxelsb[i].position = limat_vector_add (vector, offset);
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
		ret = private_build (self);
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

static int private_build (
	LIVoxBuilder* self)
{
	int x;
	int y;
	int z;
	int ret = 1;

	/* Allocate vertex cache. */
	if (self->model_wanted)
	{
		self->accel = lialg_memdic_new ();
		if (self->accel == NULL)
			return 0;
	}

	/* Build all voxels inside the area. */
	for (z = 1 ; z < self->size[2] - 1 ; z++)
	for (y = 1 ; y < self->size[1] - 1 ; y++)
	for (x = 1 ; x < self->size[0] - 1 ; x++)
		private_merge_voxel (self, x, y, z);

	/* Calculate bounds needed by frustum culling. */
	if (self->model != NULL)
		limdl_model_calculate_bounds (self->model);

	/* Free vertex cache. */
	if (self->model_wanted)
		lialg_memdic_free (self->accel);

	return ret;
}

static int private_merge_material (
	LIVoxBuilder*  self,
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

static void private_merge_triangle (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           group,
	LIMdlVertex*  verts)
{
	int k;
	uint32_t indices[3];
	LIMatTransform transform;
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
		transform = limat_transform_identity ();
		liphy_terrain_add_vertices (self->physics, voxel->index, coords, 3, &transform);
	}
}

static int private_merge_vertex (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	LIMdlVertex*  vertex)
{
	void* ptr;
	uint32_t index;

	ptr = lialg_memdic_find (self->accel, &vertex, sizeof (LIMdlVertex));
	if (ptr == NULL)
	{
		/* Add to model. */
		index = self->model->vertices.count;
		if (!limdl_model_insert_vertex (self->model, vertex))
			return -1;

		/* Add to lookup. */
		ptr = (void*)(intptr_t)(index + 1);
		if (!lialg_memdic_insert (self->accel, &vertex, sizeof (LIMdlVertex), ptr))
			return -1;
	}
	else
		index = ((int)(intptr_t) ptr) - 1;

	return index;
}

static void private_merge_voxel (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	int xr;
	int yr;
	int zr;
	int count;
	int group;
	int size[3];
	int types[3][3][3];
	float scale;
	float splat;
	float uv[2] = { 0.0f, 0.0f };
	LIMatVector coord[3];
	LIMatVector coords[64];
	LIMatVector normal[3];
	LIMdlVertex vertices[64];
	LIVoxVoxelB* voxel;
	const int regions[4] = { 0, 1, 1, 2 };

	/* Skip empty voxels. */
	voxel = self->voxelsb + vx + vy * self->step[1] + vz * self->step[2];
	if (!voxel->type)
		return;

	/* Get neighborhood. */
	for (z = -1 ; z <= 1 ; z++)
	for (y = -1 ; y <= 1 ; y++)
	for (x = -1 ; x <= 1 ; x++)
		types[x + 1][y + 1][z + 1] = self->voxelsb[(x + vx) + (y + vy) * self->step[1] + (z + vz) * self->step[2]].type;

	/* Generate triangles. */
	/* TODO: Support different kinds of triangulation schemes. */
	count = private_triangulate_cube (self, types, coords);
	if (!count)
		return;

	/* Generate vertices. */
	/* TODO: Smooth normals. */
	scale = 1.0f - voxel->material->material.emission;
	for (i = 0 ; i < count ; i += 3)
	{
		coord[0] = limat_vector_multiply (coords[i + 0], self->tile_width);
		coord[1] = limat_vector_multiply (coords[i + 1], self->tile_width);
		coord[2] = limat_vector_multiply (coords[i + 2], self->tile_width);
		coord[0] = limat_vector_add (coord[0], voxel->position);
		coord[1] = limat_vector_add (coord[1], voxel->position);
		coord[2] = limat_vector_add (coord[2], voxel->position);
		normal[0] = normal[1] = normal[2] =
			limat_vector_normalize (limat_vector_cross (
			limat_vector_subtract (coord[0], coord[1]),
			limat_vector_subtract (coord[1], coord[2])));
		for (j = 0 ; j < 3 ; j++)
		{
			/* Calculate texture splatting factor. */
			/* The factor is stored as the length of the normal. */
			if (self->model_wanted)
			{
				splat = 0.0f;
				xr = (int)(coords[i + j].x / 0.34f);
				yr = (int)(coords[i + j].y / 0.34f);
				zr = (int)(coords[i + j].z / 0.34f);
				for (z = regions[zr] ; z <= regions[zr + 1] ; z++)
				for (y = regions[yr] ; y <= regions[yr + 1] ; y++)
				for (x = regions[xr] ; x <= regions[xr + 1] ; x++)
				{
					if (types[x][y][z] && types[x][y][z] != types[1][1][1])
					{
						splat = 1.0f;
						z = y = x = 3;
					}
				}
				normal[j] = limat_vector_multiply (normal[j], 1.0f + splat);
			}

			/* Calculate texture coordinates. */
			if (self->model_wanted)
			{
				if (hypotf (coord[0].x - coord[1].x, coord[0].z - coord[1].z) >= LIMAT_EPSILON &&
					hypotf (coord[1].x - coord[2].x, coord[1].z - coord[2].z) >= LIMAT_EPSILON &&
					hypotf (coord[2].x - coord[0].x, coord[2].z - coord[0].z) >= LIMAT_EPSILON)
				{
					uv[0] = scale * coord[j].x;
					uv[1] = scale * coord[j].z;
				}
				else if (LIMAT_ABS (coord[0].x - coord[1].x) < LIMAT_EPSILON &&
						 LIMAT_ABS (coord[1].x - coord[2].x) < LIMAT_EPSILON &&
						 LIMAT_ABS (coord[2].x - coord[0].x) < LIMAT_EPSILON)
				{
					uv[0] = scale * coord[j].z;
					uv[1] = scale * coord[j].y;
				}
				else
				{
					uv[0] = scale * coord[j].x;
					uv[1] = scale * coord[j].y;
				}
			}

			/* Initialize a new vertex. */
			limdl_vertex_init (vertices + i + j, coord + j, normal + j, uv[0], uv[1]);
		}
	}

	/* Allocate outputs now that we know we're going to output something. */
	if (self->model_wanted && self->model == NULL)
		self->model = limdl_model_new ();
	if (self->physics_wanted && self->physics == NULL && self->physics_manager != NULL)
	{
		size[0] = self->size[0] - 2;
		size[1] = self->size[1] - 2;
		size[2] = self->size[2] - 2;
		self->physics = liphy_terrain_new (self->physics_manager, self->offset, size,
			LIPHY_GROUP_TILES, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
	}

	/* Find or create materials. */
	if (self->model != NULL)
	{
		group = private_merge_material (self, &voxel->material->material);
		if (group == -1)
			return;
	}
	else
		group = 0;

	/* Add triangles to the model. */
	for (i = 0 ; i < count ; i += 3)
		private_merge_triangle (self, voxel, group, vertices + i);
}

static int private_triangulate_cube (
	LIVoxBuilder* self,
	int           types[3][3][3],
	LIMatVector*  result)
{
	int count;
	const LIMatVector cubevert[2][2][2] = {
		{{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }},
		 {{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 1.0f }}},
		{{{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }},
		 {{ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f }}}};

	count = 0;
	if (!types[0][1][1])
	{
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[0][1][1];
		result[count++] = cubevert[0][1][0];
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[0][0][1];
		result[count++] = cubevert[0][1][1];
	}
	if (!types[2][1][1])
	{
		result[count++] = cubevert[1][0][0];
		result[count++] = cubevert[1][1][0];
		result[count++] = cubevert[1][1][1];
		result[count++] = cubevert[1][0][0];
		result[count++] = cubevert[1][1][1];
		result[count++] = cubevert[1][0][1];
	}
	if (!types[1][0][1])
	{
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[1][0][0];
		result[count++] = cubevert[1][0][1];
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[1][0][1];
		result[count++] = cubevert[0][0][1];
	}
	if (!types[1][2][1])
	{
		result[count++] = cubevert[0][1][0];
		result[count++] = cubevert[1][1][1];
		result[count++] = cubevert[1][1][0];
		result[count++] = cubevert[0][1][0];
		result[count++] = cubevert[0][1][1];
		result[count++] = cubevert[1][1][1];
	}
	if (!types[1][1][0])
	{
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[1][0][0];
		result[count++] = cubevert[1][1][0];
		result[count++] = cubevert[0][0][0];
		result[count++] = cubevert[1][1][0];
		result[count++] = cubevert[0][1][0];
	}
	if (!types[1][1][2])
	{
		result[count++] = cubevert[0][0][1];
		result[count++] = cubevert[1][1][1];
		result[count++] = cubevert[1][0][1];
		result[count++] = cubevert[0][0][1];
		result[count++] = cubevert[0][1][1];
		result[count++] = cubevert[1][1][1];
	}

	return count;
}

/** @} */
/** @} */
