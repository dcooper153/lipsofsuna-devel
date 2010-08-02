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
 * \addtogroup livox Voxel
 * @{
 * \addtogroup LIVoxBuild Build
 * @{
 */

#include <lipsofsuna/model.h>
#include "voxel-build.h"
#include "voxel-material.h"

#define CULL_EPSILON 0.01f
#define MISSING_HEIGHT -10.0f

typedef struct _LIVoxVoxelB LIVoxVoxelB;
struct _LIVoxVoxelB
{
	int mask;
	float height;
	LIEngModel* model;
	LIMatTransform transform;
	LIVoxMaterial* material;
};

typedef struct _LIVoxBuilder LIVoxBuilder;
struct _LIVoxBuilder
{
	int xsize;
	int ysize;
	int zsize;
	int xstep;
	int ystep;
	int zstep;
	char* occlud;
	LIAlgMemdic* accel;
	LIEngEngine* engine;
	LIMdlModel* model;
	LIPhyObject* physics;
	LIPhyPhysics* physics_manager;
	LIVoxManager* manager;
	LIVoxVoxel* voxels;
	LIVoxVoxelB* voxelsb;
};

static int
private_build (LIVoxBuilder* self,
               int           model,
               int           physics);

static void
private_merge_height_model (LIVoxBuilder* self,
                            LIVoxVoxelB*  voxel,
                            LIMatVector   botco[3][3],
                            LIMatVector   topco[3][3]);

static void
private_merge_height_physics (LIVoxBuilder* self,
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

int livox_build_area (
	LIVoxManager* manager,
	LIEngEngine*  engine,
	LIPhyPhysics* physics,
	int           xstart,
	int           ystart,
	int           zstart,
	int           xsize,
	int           ysize,
	int           zsize,
	LIMdlModel**  result_model,
	LIPhyObject** result_physics)
{
	int i;
	int x;
	int y;
	int z;
	int count;
	int ret;
	LIEngModel* model;
	LIMatVector offset;
	LIMatVector vector;
	LIVoxMaterial* material;
	LIVoxVoxel* voxel;
	LIVoxBuilder self;

	/* Allocate temporary data. */
	count = 0;
	memset (&self, 0, sizeof (LIVoxBuilder));
	self.manager = manager;
	self.engine = engine;
	self.physics_manager = physics;
	self.xsize = xsize + 2;
	self.ysize = ysize + 2;
	self.zsize = zsize + 2;
	self.xstep = 1;
	self.ystep = self.xsize;
	self.zstep = self.xsize * self.ysize;
	self.occlud = lisys_calloc (self.xsize * self.ysize * self.zsize, sizeof (char));
	self.voxels = lisys_calloc (self.xsize * self.ysize * self.zsize, sizeof (LIVoxVoxel));
	self.voxelsb = lisys_calloc (self.xsize * self.ysize * self.zsize, sizeof (LIVoxVoxelB));
	if (self.occlud == NULL || self.voxels == NULL || self.voxelsb == NULL)
	{
		lisys_free (self.occlud);
		lisys_free (self.voxels);
		lisys_free (self.voxelsb);
		return 0;
	}

	/* Fetch surrounding voxel data. */
	livox_manager_copy_voxels (manager, xstart - 1, ystart - 1, zstart - 1,
		self.xsize, self.ysize, self.zsize, self.voxels);

	/* Calculate occlusion information. */
	i = livox_build_occlusion (manager, self.xsize, self.ysize, self.zsize, self.voxels, self.occlud);
	if (i == xsize * ysize * zsize)
	{
		if (result_model != NULL) *result_model = NULL;
		if (result_physics != NULL) *result_physics = NULL;
		return 1;
	}

	/* Calculate area offset. */
	offset = limat_vector_init (xstart - 1, ystart - 1, zstart - 1);
	offset = limat_vector_multiply (offset, LIVOX_TILE_WIDTH);

	/* Precalculate useful information on voxels. */
	for (z = 0 ; z < self.zsize ; z++)
	for (y = 0 ; y < self.ysize ; y++)
	for (x = 0 ; x < self.xsize ; x++)
	{
		i = x + y * self.ystep + z * self.zstep;
		self.voxelsb[i].height = MISSING_HEIGHT;

		/* Type check. */
		voxel = self.voxels + i;
		if (!voxel->type)
			continue;

		/* Material check. */
		material = livox_manager_find_material (manager, voxel->type);
		if (material == NULL)
			continue;
		self.voxelsb[i].material = material;

		/* Cache model. */
		if (material->type == LIVOX_MATERIAL_TYPE_TILE)
		{
			model = lieng_engine_find_model_by_name (engine, material->model);
			self.voxelsb[i].model = model;
			self.voxelsb[i].height = 1.0f;
		}

		/* Cache height. */
		if (material->type == LIVOX_MATERIAL_TYPE_HEIGHT)
			self.voxelsb[i].height = livox_voxel_get_height (voxel);

		/* Cache transformation. */
		vector = limat_vector_init (x + 0.5f, y + 0.5f, z + 0.5f);
		vector = limat_vector_multiply (vector, LIVOX_TILE_WIDTH);
		self.voxelsb[i].transform.position = limat_vector_add (vector, offset);
		livox_voxel_get_quaternion (voxel, &self.voxelsb[i].transform.rotation);

		/* Occlusion check. */
		if (!(self.occlud[i] & LIVOX_OCCLUDE_OCCLUDED))
			count++;
		self.voxelsb[i].mask = self.occlud[i];
	}

	/* Build mesh and/or physics. */
	ret = 1;
	if (count)
	{
		ret = private_build (&self, result_model != NULL, result_physics != NULL);
		if (self.physics != NULL)
		{
			liphy_object_set_collision_group (self.physics, LIPHY_GROUP_TILES);
			liphy_object_set_collision_mask (self.physics, LIPHY_DEFAULT_COLLISION_MASK & ~LIPHY_GROUP_TILES);
		}
		if (ret)
		{
			if (result_physics != NULL)
				*result_physics = self.physics;
			if (result_model != NULL)
				*result_model = self.model;
		}
		else
		{
			if (self.physics != NULL)
				liphy_object_free (self.physics);
			if (self.model != NULL)
				limdl_model_free (self.model);
		}
	}

	/* Free temporary data. */
	lisys_free (self.occlud);
	lisys_free (self.voxels);
	lisys_free (self.voxelsb);

	return ret;
}

int livox_build_block (
	LIVoxManager*         manager,
	LIEngEngine*          engine,
	LIPhyPhysics*         physics,
	const LIVoxBlockAddr* addr,
	LIMdlModel**          result_model,
	LIPhyObject**         result_physics)
{
	return livox_build_area (manager, engine, physics,
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[0] + addr->block[0]),
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[1] + addr->block[1]),
		LIVOX_TILES_PER_LINE * (LIVOX_BLOCKS_PER_LINE * addr->sector[2] + addr->block[2]),
		LIVOX_TILES_PER_LINE, LIVOX_TILES_PER_LINE, LIVOX_TILES_PER_LINE,
		result_model, result_physics);
}

/**
 * \brief Solves occlusion masks for a volume of voxels.
 *
 * \param manager Voxel manager.
 * \param xsize Number of voxels.
 * \param ysize Number of voxels.
 * \param zsize Number of voxels.
 * \param voxels Array of voxels in the volume.
 * \param result Buffer with room for xsize*ysize*zsize integers.
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
	for (z = 1 ; z < self->zsize - 1 ; z++)
	for (y = 1 ; y < self->ysize - 1 ; y++)
	for (x = 1 ; x < self->xsize - 1 ; x++)
	{
		voxel = self->voxelsb + x + y * self->ystep + z * self->zstep;
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
			self->physics = liphy_object_new (self->physics_manager, 0, NULL, LIPHY_CONTROL_MODE_STATIC);
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
			if (self->model != NULL)
				private_merge_height_model (self, voxel, botco, topco);
			if (self->physics != NULL)
				private_merge_height_physics (self, voxel, botco, topco);
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

static void
private_merge_height_model (LIVoxBuilder* self,
                            LIVoxVoxelB*  voxel,
                            LIMatVector   botco[3][3],
                            LIMatVector   topco[3][3])
{
	int g0;
	int g1;
	int i;
	int k;
	int x;
	int z;
	float ssurf;
	float sside;
	uint32_t indices[3];
	LIMatVector ex[2][3];
	LIMatVector ez[2][3];
	LIMatVector n[4];
	LIMatVector t[4];
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
		/* FIXME: Bad tangents. */
		t[0] = limat_vector_init (1.0f, 0.0f, 0.0f);
		t[1] = limat_vector_init (1.0f, 0.0f, 0.0f);
		limdl_vertex_init (&(top[x][z]), &(topco[x][z]), n + 0, t + 0, ssurf * uv[0].x, ssurf * uv[0].z);
		limdl_vertex_init (&(bot[x][z]), &(botco[x][z]), n + 1, t + 1, sside * uv[1].x, sside * uv[1].z);
	}

	/* Calculate smooth normals. */
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
		/* FIXME: Bad tangents. */
		t[0] = limat_vector_init (1.0f, 0.0f, 0.0f);
		t[1] = limat_vector_init (0.0f, 0.0f, 1.0f);
		t[2] = limat_vector_init (-1.0f, 0.0f, 0.0f);
		t[3] = limat_vector_init (0.0f, 0.0f, -1.0f);
		limdl_vertex_init (&(side[x][0]), &(botco[x][0]), n + 0, t + 0, sside * uv[0].x, sside * uv[0].y);
		limdl_vertex_init (&(side[x][1]), &(topco[x][0]), n + 0, t + 0, sside * uv[1].x, sside * uv[1].y);
		limdl_vertex_init (&(side[3 + x][0]), &(botco[2][x]), n + 1, t + 1, sside * uv[2].z, sside * uv[2].y);
		limdl_vertex_init (&(side[3 + x][1]), &(topco[2][x]), n + 1, t + 1, sside * uv[3].z, sside * uv[3].y);
		limdl_vertex_init (&(side[8 - x][0]), &(botco[2 - x][2]), n + 2, t + 2, sside * uv[4].x, sside * uv[4].y);
		limdl_vertex_init (&(side[8 - x][1]), &(topco[2 - x][2]), n + 2, t + 2, sside * uv[5].x, sside * uv[5].y);
		limdl_vertex_init (&(side[11 - x][0]), &(botco[0][2 - x]), n + 3, t + 3, sside * uv[6].z, sside * uv[6].y);
		limdl_vertex_init (&(side[11 - x][1]), &(topco[0][2 - x]), n + 3, t + 3, sside * uv[7].z, sside * uv[7].y);
	}

	/* Find or create materials. */
	g0 = private_merge_material (self, &voxel->material->mat_top);
	if (g0 == -1)
		return;
	g1 = private_merge_material (self, &voxel->material->mat_side);
	if (g1 == -1)
		return;

	/* Create top and bottom triangles. */
	for (z = 0 ; z < 2 ; z++)
	for (x = 0 ; x < 2 ; x++)
	for (i = 0 ; i < 2 ; i++)
	{
		/* Top surface. */
		for (k = 0 ; k < 3 ; k++)
			verts[k] = top[x + quadidx[z == x][2*k + 6*i]][z + quadidx[z == x][2*k + 6*i + 1]];
		if (!private_occlude_face (self, voxel, verts))
		{
			for (k = 0 ; k < 3 ; k++)
			{
				indices[k] = private_merge_vertex (self, voxel, verts + k);
				if (indices[k] == -1)
					break;
			}
			if (k == 3)
				limdl_model_insert_indices (self->model, g0, indices, 3);
		}

		/* Bottom surface. */
		for (k = 0 ; k < 3 ; k++)
			verts[k] = bot[x + quadidxrev[z == x][2*k + 6*i]][z + quadidxrev[z == x][2*k + 6*i + 1]];
		if (!private_occlude_face (self, voxel, verts))
		{
			for (k = 0 ; k < 3 ; k++)
			{
				indices[k] = private_merge_vertex (self, voxel, verts + k);
				if (indices[k] == -1)
					break;
			}
			if (k == 3)
				limdl_model_insert_indices (self->model, g1, indices, 3);
		}
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
		{
			for (k = 0 ; k < 3 ; k++)
			{
				indices[k] = private_merge_vertex (self, voxel, verts + k);
				if (indices[k] == -1)
					break;
			}
			if (k == 3)
				limdl_model_insert_indices (self->model, g1, indices, 3);
		}
	}
}

static void
private_merge_height_physics (LIVoxBuilder* self,
                              LIVoxVoxelB*  voxel,
                              LIMatVector   botco[3][3],
                              LIMatVector   topco[3][3])
{
	int i;
	int x;
	int z;
	LIMatVector verts[18];
	LIPhyShape* shape;

	/* Collect vertices. */
	for (i = z = 0 ; z < 3 ; z++)
	for (x = 0 ; x < 3 ; x++)
	{
		verts[i++] = botco[x][z];
		verts[i++] = topco[x][z];
	}

	/* Insert convex shape. */
	shape = liphy_shape_new_convex (self->physics_manager, verts, 18);
	if (shape != NULL)
	{
		liphy_object_insert_shape (self->physics, shape, &voxel->transform);
		liphy_shape_free (shape);
	}
}

static int
private_merge_tile_model (LIVoxBuilder* self,
                          LIVoxVoxelB*  voxel)
{
	int g;
	int i;
	int j;
	int k;
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

static int
private_merge_tile_physics (LIVoxBuilder* self,
                            LIVoxVoxelB*  voxel)
{
	return liphy_object_insert_shape (self->physics, voxel->model->physics, &voxel->transform);
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
		if (!limdl_model_insert_vertex (self->model, &vert, NULL))
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
		LIMAT_ABS (coords[0].x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].x + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_XPOS) &&
		LIMAT_ABS (coords[0].x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].x - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_YNEG) &&
		LIMAT_ABS (coords[0].y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].y + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_YPOS) &&
		LIMAT_ABS (coords[0].y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].y - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_ZNEG) &&
		LIMAT_ABS (coords[0].z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].z + LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;
	if ((voxel->mask & LIVOX_OCCLUDE_ZPOS) &&
		LIMAT_ABS (coords[0].z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[1].z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON &&
		LIMAT_ABS (coords[2].z - LIVOX_TILE_WIDTH / 2) < CULL_EPSILON)
		return 1;

	return 0;
}

static inline void
private_solve_coords (LIVoxBuilder* self,
                      LIVoxVoxelB*  voxel,
                      LIMatVector   bot[3][3],
                      LIMatVector   top[3][3])
{
	int x;
	int y;
	int z;
	float h[3][3][3];
	float size = 0.5f * LIVOX_TILE_WIDTH;

	/* Cache tile heights. */
	for (z = -1 ; z <= 1 ; z++)
	for (y = -1 ; y <= 1 ; y++)
	for (x = -1 ; x <= 1 ; x++)
		h[x + 1][z + 1][y + 1] = voxel[x + y * self->ystep + z * self->zstep].height;

	/* Construct X and Z coordinates. */
	for (z = 0 ; z < 3 ; z++)
	for (x = 0 ; x < 3 ; x++)
	{
		/* Top. */
		top[x][z] = limat_vector_init (
			(-0.5f + x / 2.0f) * LIVOX_TILE_WIDTH, 0.5f * LIVOX_TILE_WIDTH,
			(-0.5f + z / 2.0f) * LIVOX_TILE_WIDTH);

		/* Bottom. */
		bot[x][z] = limat_vector_init (
			(-0.5f + x / 2.0f) * LIVOX_TILE_WIDTH, -0.5f * LIVOX_TILE_WIDTH,
			(-0.5f + z / 2.0f) * LIVOX_TILE_WIDTH);
	}

	/* Construct Y coordinate. */
	if (voxel[self->ystep].material == NULL ||
	    voxel[self->ystep].material->type != LIVOX_MATERIAL_TYPE_HEIGHT)
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
