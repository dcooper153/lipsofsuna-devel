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
#include "voxel-triangulate.h"

#define CULL_EPSILON 0.01f
#define LIVOX_ISEMPTY(m)

/* The builder isn't completely thread-safe at the moment. The biggest issue is
   that material data is queried from the voxel manager without locking. Our
   current scripts don't edit the materials after initialization so it works,
   but it needs to be fixed in the future. */
#warning Thread-safety issues in terrain builder

static int private_build (
	LIVoxBuilder* self);

static int private_merge_material (
	LIVoxBuilder*  self,
	LIMdlMaterial* material);

static int private_merge_triangles_model (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           types[3][3][3],
	LIMatVector*  coords,
	int*          faces,
	int           count);

static void private_merge_voxel (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz);

/*****************************************************************************/

LIVoxBuilder* livox_builder_new (
	LIVoxManager* manager,
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
	LIMdlModel**   result)
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
	lisys_assert (result != NULL);

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
			if (self->model_builder != NULL)
				*result = self->model_builder->model;
			else
				*result = NULL;
		}
		if (self->model_builder != NULL)
			limdl_builder_free (self->model_builder);
	}

	return ret;
}

/**
 * \brief Solves occlusion masks for a volume of voxels.
 * \param manager Voxel manager.
 * \param xsize Number of voxels.
 * \param ysize Number of voxels.
 * \param zsize Number of voxels.
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

	/* Build all voxels inside the area. */
	for (z = 1 ; z < self->size[2] - 1 ; z++)
	for (y = 1 ; y < self->size[1] - 1 ; y++)
	for (x = 1 ; x < self->size[0] - 1 ; x++)
		private_merge_voxel (self, x, y, z);

	/* Calculate bounds and tangents. */
	if (self->model_builder != NULL)
		limdl_builder_finish (self->model_builder);

	return ret;
}

static int private_merge_material (
	LIVoxBuilder*  self,
	LIMdlMaterial* material)
{
	int g;
	int m;

	/* Find or create material. */
	m = limdl_model_find_material (self->model_builder->model, material);
	if (m == -1)
	{
		m = self->model_builder->model->materials.count;
		if (!limdl_builder_insert_material (self->model_builder, material))
			return -1;
	}

	/* Find or create face group. */
	g = limdl_model_find_facegroup (self->model_builder->model, m);
	if (g == -1)
	{
		g = self->model_builder->model->facegroups.count;
		if (!limdl_builder_insert_facegroup (self->model_builder, m))
			return -1;
	}

	return g;
}

static int private_merge_triangles_model (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxel,
	int           types[3][3][3],
	LIMatVector*  coords,
	int*          faces,
	int           count)
{
	int i;
	int j;
	int x;
	int y;
	int z;
	int xr;
	int yr;
	int zr;
	int group;
	float scale;
	float splat;
	float uv[2] = { 0.0f, 0.0f };
	LIMatVector coord[3];
	LIMatVector normal[3];
	LIMdlVertex vertices[3];
	const int regions[4] = { 0, 1, 1, 2 };

	/* Find or create material. */
	group = private_merge_material (self, &voxel->material->material);
	if (group == -1)
		return 0;

	/* Generate normals and texture coordinates. */
	/* TODO: Smooth normals. */
	scale = voxel->material->texture_scale;
	for (i = 0 ; i < count ; i += 3)
	{
		/* Calculate world coordinates and the triangle normal. */
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

		/* Calculate texture coordinates and splatting. */
		for (j = 0 ; j < 3 ; j++)
		{
			/* Calculate texture splatting factor. */
			/* The factor is stored as the length of the normal. */
			splat = 0.0f;
			xr = LIMAT_CLAMP ((int)(coords[i + j].x / 0.34f), 0, 2);
			yr = LIMAT_CLAMP ((int)(coords[i + j].y / 0.34f), 0, 2);
			zr = LIMAT_CLAMP ((int)(coords[i + j].z / 0.34f), 0, 2);
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

			/* Calculate texture coordinates. */
			switch (faces[i / 3])
			{
				case LIVOX_TRIANGULATE_NEGATIVE_X:
					uv[0] = scale * coord[j].z;
					uv[1] = scale * coord[j].y;
					break;
				case LIVOX_TRIANGULATE_POSITIVE_X:
					uv[0] = scale * coord[j].z;
					uv[1] = scale * coord[j].y;
					break;
				case LIVOX_TRIANGULATE_NEGATIVE_Y:
					uv[0] = scale * coord[j].x;
					uv[1] = scale * coord[j].z;
					break;
				case LIVOX_TRIANGULATE_POSITIVE_Y:
					uv[0] = -scale * coord[j].x;
					uv[1] = scale * coord[j].z;
					break;
				case LIVOX_TRIANGULATE_NEGATIVE_Z:
					uv[0] = scale * coord[j].x;
					uv[1] = scale * coord[j].y;
					break;
				case LIVOX_TRIANGULATE_POSITIVE_Z:
				default:
					uv[0] = scale * coord[j].x;
					uv[1] = scale * coord[j].y;
					break;
			}

			/* Initialize the vertex. */
			limdl_vertex_init (vertices + j, coord + j, normal + j, uv[0], uv[1]);
		}

		/* Merge vertices. */
		if (!limdl_builder_insert_face (self->model_builder, group, vertices, NULL))
			return 0;
	}

	return 1;
}

static void private_merge_voxel (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz)
{
	int count;
	int types[3][3][3];
	int faces[200];
	LIMatVector coords[600];
	LIVoxVoxelB* voxel;

	/* Skip empty voxels. */
	voxel = self->voxelsb + vx + vy * self->step[1] + vz * self->step[2];
	if (!voxel->type)
		return;

	/* Generate triangles. */
	count = livox_triangulate_voxel (self, vx, vy, vz, coords, faces, types);
	if (!count)
		return;

	/* Add triangles to the model. */
	if (self->model_builder == NULL)
		self->model_builder = limdl_builder_new (NULL);
	if (self->model_builder != NULL)
		private_merge_triangles_model (self, voxel, types, coords, faces, count);
}

/** @} */
/** @} */
