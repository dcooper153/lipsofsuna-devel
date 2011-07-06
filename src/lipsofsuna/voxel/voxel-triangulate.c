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
 * \addtogroup LIVox Voxel
 * @{
 * \addtogroup LIVoxTriangulate Triangulate
 * @{
 */

#include "voxel-material.h"
#include "voxel-triangulate.h"

#define ISFILLED(x,y,z) (types[x][y][z])
#define ISCUBE(x,y,z) (types[x][y][z] && (voxels[x][y][z]->material->type != LIVOX_MATERIAL_TYPE_SLOPED && voxels[x][y][z]->material->type != LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL))
#define ISEMPTY(x,y,z) (!types[x][y][z])
#define ISROUNDED(x,y,z) (types[x][y][z] && (voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL))
#define ISROUNDEDSLOPED(x,y,z) (types[x][y][z] && (\
	voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED ||\
	voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL ||\
	voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED ||\
	voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL))
#define ISSLOPED(x,y,z) (types[x][y][z] && (voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL))
#define ISSOLID(x,y,z) (types[x][y][z] && voxels[x][y][z]->material->type != LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUID(x,y,z) (types[x][y][z] && voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUIDEMPTY(x,y,z) (!types[x][y][z] || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_LIQUID)
#define ISLIQUIDEMPTYFRACTAL(x,y,z) (ISLIQUIDEMPTY(x,y,z) || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL)
#define ISLIQUIDEMPTYROUNDED(x,y,z) (ISLIQUIDEMPTY(x,y,z) || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL)
#define ISLIQUIDEMPTYSLOPED(x,y,z) (ISLIQUIDEMPTY(x,y,z) || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED || voxels[x][y][z]->material->type == LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL)

#define OCCLUDES(x,y,z) ((liquid && ISFILLED(x,y,z)) || (!liquid && ISSOLID(x,y,z)))
#define OCCLUDESXNEG(x,y,z) OCCLUDES(x,y,z)
#define OCCLUDESXPOS(x,y,z) OCCLUDES(x,y,z)
#define OCCLUDESYNEG(x,y,z) ((liquid && ISFILLED(x,y,z)) || (!liquid && ISSOLID(x,y,z)))
#define OCCLUDESYPOS(x,y,z) ((liquid && ISLIQUID(x,y,z)) || (!liquid && ISSOLID(x,y,z)))
#define OCCLUDESZNEG(x,y,z) OCCLUDES(x,y,z)
#define OCCLUDESZPOS(x,y,z) OCCLUDES(x,y,z)

static inline void private_noise_3d (
	LIMatVector* vector,
	LIMatVector* result);

static void private_triangulate_liquid (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3]);

static void private_triangulate_fractal (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3]);

static void private_triangulate_rounded (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3]);

static void private_triangulate_sloped (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3]);

/*****************************************************************************/

/**
 * \brief Triangulates a voxel.
 * \param self Voxel builder.
 * \param vx Index of the triangulated voxel.
 * \param vy Index of the triangulated voxel.
 * \param vz Index of the triangulated voxel.
 * \param result Return location for the vertices.
 * \param result_faces Return location for face indices of triangles.
 * \param result_types Return location for voxel types.
 * \return Number of vertices.
 */
int livox_triangulate_voxel (
	LIVoxBuilder* self,
	int           vx,
	int           vy,
	int           vz,
	LIMatVector*  result,
	int*          result_faces,
	int           result_types[3][3][3])
{
#define types result_types
	int x;
	int y;
	int z;
	int count;
	int count_faces;
	int liquid = 0;
	LIVoxVoxelB* voxel;
	LIVoxVoxelB* voxels[3][3][3];
	LIMatVector v[3][3][3] = {
		{{{ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.5f }, { 0.0f, 0.0f, 1.0f }},
		 {{ 0.0f, 0.5f, 0.0f }, { 0.0f, 0.5f, 0.5f }, { 0.0f, 0.5f, 1.0f }},
		 {{ 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.5f }, { 0.0f, 1.0f, 1.0f }}},
		{{{ 0.5f, 0.0f, 0.0f }, { 0.5f, 0.0f, 0.5f }, { 0.5f, 0.0f, 1.0f }},
		 {{ 0.5f, 0.5f, 0.0f }, { 0.5f, 0.5f, 0.5f }, { 0.5f, 0.5f, 1.0f }},
		 {{ 0.5f, 1.0f, 0.0f }, { 0.5f, 1.0f, 0.5f }, { 0.5f, 1.0f, 1.0f }}},
		{{{ 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.5f }, { 1.0f, 0.0f, 1.0f }},
		 {{ 1.0f, 0.5f, 0.0f }, { 1.0f, 0.5f, 0.5f }, { 1.0f, 0.5f, 1.0f }},
		 {{ 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 0.5f }, { 1.0f, 1.0f, 1.0f }}}};

	/* Get the neighborhood. */
	/* If the neighborhood is full of solid tiles, the centermost one isn't
	   visible and we can exit early. Liquid tiles don't count towards this. */
	count = 0;
	for (z = -1 ; z <= 1 ; z++)
	for (y = -1 ; y <= 1 ; y++)
	for (x = -1 ; x <= 1 ; x++)
	{
		voxel = self->voxelsb + (x + vx) + (y + vy) * self->step[1] + (z + vz) * self->step[2];
		voxels[x + 1][y + 1][z + 1] = voxel;
		result_types[x + 1][y + 1][z + 1] = voxel->type;
		count += ISSOLID(x + 1, y + 1, z + 1);
	}
	if (count == 27)
		return 0;

	/* Deform the voxel cube. */
	switch (voxels[1][1][1]->material->type)
	{
		case LIVOX_MATERIAL_TYPE_LIQUID:
			liquid = 1;
			private_triangulate_liquid (self, voxels, result_types, v);
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED:
			private_triangulate_rounded (self, voxels, result_types, v);
			break;
		case LIVOX_MATERIAL_TYPE_ROUNDED_FRACTAL:
			private_triangulate_rounded (self, voxels, result_types, v);
			private_triangulate_fractal (self, voxels, result_types, v);
			break;
		case LIVOX_MATERIAL_TYPE_SLOPED:
			private_triangulate_sloped (self, voxels, result_types, v);
			break;
		case LIVOX_MATERIAL_TYPE_SLOPED_FRACTAL:
			private_triangulate_sloped (self, voxels, result_types, v);
			private_triangulate_fractal (self, voxels, result_types, v);
			break;
	}

	/* Triangulate the deformed cube. */
	count = 0;
	count_faces = 0;
	if (!OCCLUDESXNEG (0, 1, 1))
	{
		for (y = 0 ; y < 2 ; y++)
		for (z = 0 ; z < 2 ; z++)
		{
			result[count++] = v[0][y+0][z+0];
			result[count++] = v[0][y+1][z+1];
			result[count++] = v[0][y+1][z+0];
			result[count++] = v[0][y+0][z+0];
			result[count++] = v[0][y+0][z+1];
			result[count++] = v[0][y+1][z+1];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_X;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_X;
		}
	}
	if (!OCCLUDESXPOS (2, 1, 1))
	{
		for (y = 0 ; y < 2 ; y++)
		for (z = 0 ; z < 2 ; z++)
		{
			result[count++] = v[2][y+0][z+0];
			result[count++] = v[2][y+1][z+0];
			result[count++] = v[2][y+1][z+1];
			result[count++] = v[2][y+0][z+0];
			result[count++] = v[2][y+1][z+1];
			result[count++] = v[2][y+0][z+1];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_X;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_X;
		}
	}
	if (!OCCLUDESYNEG (1, 0, 1))
	{
		for (x = 0 ; x < 2 ; x++)
		for (z = 0 ; z < 2 ; z++)
		{
			result[count++] = v[x+0][0][z+0];
			result[count++] = v[x+1][0][z+0];
			result[count++] = v[x+1][0][z+1];
			result[count++] = v[x+0][0][z+0];
			result[count++] = v[x+1][0][z+1];
			result[count++] = v[x+0][0][z+1];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_Y;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_Y;
		}
	}
	if (!OCCLUDESYPOS (1, 2, 1))
	{
		for (x = 0 ; x < 2 ; x++)
		for (z = 0 ; z < 2 ; z++)
		{
			result[count++] = v[x+0][2][z+0];
			result[count++] = v[x+1][2][z+1];
			result[count++] = v[x+1][2][z+0];
			result[count++] = v[x+0][2][z+0];
			result[count++] = v[x+0][2][z+1];
			result[count++] = v[x+1][2][z+1];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_Y;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_Y;
		}
	}
	if (!OCCLUDESZNEG (1, 1, 0))
	{
		for (x = 0 ; x < 2 ; x++)
		for (y = 0 ; y < 2 ; y++)
		{
			result[count++] = v[x+0][y+0][0];
			result[count++] = v[x+1][y+1][0];
			result[count++] = v[x+1][y+0][0];
			result[count++] = v[x+0][y+0][0];
			result[count++] = v[x+0][y+1][0];
			result[count++] = v[x+1][y+1][0];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_Z;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_NEGATIVE_Z;
		}
	}
	if (!OCCLUDESZPOS (1, 1, 2))
	{
		for (x = 0 ; x < 2 ; x++)
		for (y = 0 ; y < 2 ; y++)
		{
			result[count++] = v[x+0][y+0][2];
			result[count++] = v[x+1][y+0][2];
			result[count++] = v[x+1][y+1][2];
			result[count++] = v[x+0][y+0][2];
			result[count++] = v[x+1][y+1][2];
			result[count++] = v[x+0][y+1][2];
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_Z;
			result_faces[count_faces++] = LIVOX_TRIANGULATE_POSITIVE_Z;
		}
	}
#undef types

	return count;
}

/*****************************************************************************/

static inline void private_noise_3d (
	LIMatVector* vector,
	LIMatVector* result)
{
	uint32_t seed;
	LIAlgRandom random;

	seed = ((((uint32_t)(0.8f * vector->x + 234023.3f)) % 0x3FF) << 0) |
	       ((((uint32_t)(0.7f * vector->y + 8353.7f)) % 0x3FF) << 10) |
	       ((((uint32_t)(0.9f * vector->z + 27345.11f)) % 0x3FF) << 20);
	lialg_random_init (&random, 1 + seed);
	result->x = lialg_random_float (&random);
	lialg_random_init (&random, 2 + seed);
	result->y = lialg_random_float (&random);
	lialg_random_init (&random, 3 + seed);
	result->z = lialg_random_float (&random);
	*result = limat_vector_subtract (*result, limat_vector_init (0.5f, 0.5f, 0.5f));
	*result = limat_vector_multiply (*result, 0.3f);
}

static void private_triangulate_fractal (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3])
{
	int a;
	int b;
	int c;
	int a1;
	int b1;
	int c1;
	int skip;
	const int range[3][2] = {{0, 1}, {1, 1}, {1, 2}};
	LIMatVector vtx;
	LIMatVector rnd;

	for(a = 0 ; a < 3 ; a++)
	for(b = 0 ; b < 3 ; b++)
	for(c = 0 ; c < 3 ; c++)
	{
		/* Don't fractalize if connected to a non-fractal tile. */
		/* Seams between different triangulation schemes need special
		   handling or else holes appear where two schemes meet. */
		skip = 0;
		for (a1 = range[a][0] ; a1 <= range[a][1] ; a1++)
		for (b1 = range[b][0] ; b1 <= range[b][1] ; b1++)
		for (c1 = range[c][0] ; c1 <= range[c][1] ; c1++)
		{
			if (!ISLIQUIDEMPTYFRACTAL (a1, b1, c1))
			{
				skip = 1;
				a1 = 3;
				b1 = 3;
				break;
			}
		}
		if (skip)
			continue;

		/* Randomize the vertex coordinates. */
		vtx = limat_vector_multiply (v[a][b][c], self->tile_width);
		vtx = limat_vector_add (vtx, voxels[1][1][1]->position);
		private_noise_3d (&vtx, &rnd);
		v[a][b][c] = limat_vector_add (v[a][b][c], rnd);
	}
}

static void private_triangulate_liquid (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3])
{
	int y;
	float sign;
	int slope[2][2] = { { 1, 1 }, { 1, 1 } };

	/* Determine the sloping states of corners. */
	if (ISEMPTY(1,0,1) + ISEMPTY(1,2,1) != 1)
		return;
	if (!ISEMPTY(0,1,1)) slope[0][0] = slope[0][1] = 0;
	if (!ISEMPTY(2,1,1)) slope[1][0] = slope[1][1] = 0;
	if (!ISEMPTY(1,1,0)) slope[0][0] = slope[1][0] = 0;
	if (!ISEMPTY(1,1,2)) slope[0][1] = slope[1][1] = 0;

	/* Decide whether to slope the bottom or the top side. */
	if (!ISEMPTY(1,0,1))
	{
		sign = -1.0f;
		y = 2;
	}
	else
	{
		sign = 1.0f;
		y = 0;
	}

	/* Create the slopes. */
	if (slope[0][0])
	{
		v[0][1][0].y += sign * 0.5f;
		v[0][y][0].y += sign * 1.0f;
		v[1][1][0].y += sign * 0.25f;
		v[1][y][0].y += sign * 0.5f;
		v[0][1][1].y += sign * 0.25f;
		v[0][y][1].y += sign * 0.5f;
	}
	if (slope[1][0])
	{
		v[2][1][0].y += sign * 0.5f;
		v[2][y][0].y += sign * 1.0f;
		v[1][1][0].y += sign * 0.25f;
		v[1][y][0].y += sign * 0.5f;
		v[2][1][1].y += sign * 0.25f;
		v[2][y][1].y += sign * 0.5f;
	}
	if (slope[0][1])
	{
		v[0][1][2].y += sign * 0.5f;
		v[0][y][2].y += sign * 1.0f;
		v[1][1][2].y += sign * 0.25f;
		v[1][y][2].y += sign * 0.5f;
		v[0][1][1].y += sign * 0.25f;
		v[0][y][1].y += sign * 0.5f;
	}
	if (slope[1][1])
	{
		v[2][1][2].y += sign * 0.5f;
		v[2][y][2].y += sign * 1.0f;
		v[1][1][2].y += sign * 0.25f;
		v[1][y][2].y += sign * 0.5f;
		v[2][1][1].y += sign * 0.25f;
		v[2][y][1].y += sign * 0.5f;
	}
	v[1][y][1].y = 0.25f * (v[0][y][1].y + v[2][y][1].y + v[1][y][0].y + v[1][y][2].y);

	/* Resolve degenerate cases. */
	if (slope[0][0] && slope[1][0] && slope[0][1] && slope[1][1])
	{
		v[1][y][1].y -= sign * 0.5f;
		if (ISEMPTY(1,0,0) && ISEMPTY(1,0,2))
		{
			v[1][y][0].y -= sign * 0.5f;
			v[1][y][2].y -= sign * 0.5f;
		}
		else if (ISEMPTY(0,0,1) && ISEMPTY(2,0,1))
		{
			v[0][y][1].y -= sign * 0.5f;
			v[2][y][1].y -= sign * 0.5f;
		}
	}
}

/**
 * \brief Rounds the edges and corners of a voxel cube.
 * \param self Voxel builder.
 * \param voxels Voxels in the neighborhood.
 * \param types Types of voxels in the neighborhood.
 * \param types Voxel neighborhood.
 * \param v Array of vertices to modify.
 */
static void private_triangulate_rounded (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3])
{
	int x;
	int y;
	int z;
	int t;
	float f;

	/* Deform the cube. */
#define SMOOTH_CORNER(vx,vy,vz, ox,oy,oz, ix,iy,iz)\
	if (ISLIQUIDEMPTY(ox,oy,oz) &&\
	    ISLIQUIDEMPTYROUNDED(ix, oy, oz) && ISLIQUIDEMPTYROUNDED(ox, iy, oz) &&\
	    ISLIQUIDEMPTYROUNDED(ix, iy, oz) && ISLIQUIDEMPTYROUNDED(ox, oy, iz) &&\
	    ISLIQUIDEMPTYROUNDED(ix, oy, iz) && ISLIQUIDEMPTYROUNDED(ox, iy, iz))\
	{\
		if (ISLIQUIDEMPTY(ox,iy,oz) && ISLIQUIDEMPTY(ox,oy,iz) && ISLIQUIDEMPTY(ox,iy,iz))\
		{\
			t = !types[ix][oy][oz] + !types[ix][iy][oz] + !types[ix][oy][iz];\
			f = (t == 3)? 0.25f : (t == 2)? 0.15f : 0.0f;\
			v[vx][vy][vz].x += (ox < ix)? f : -f;\
		}\
		if (ISLIQUIDEMPTY(ix,oy,oz) && ISLIQUIDEMPTY(ox,oy,iz) && ISLIQUIDEMPTY(ix,oy,iz))\
		{\
			t = !types[ox][iy][oz] + !types[ix][iy][oz] + !types[ox][iy][iz];\
			f = (t == 3)? 0.25f : (t == 2)? 0.15f : 0.0f;\
			v[vx][vy][vz].y += (oy < iy)? f : -f;\
		}\
		if (ISLIQUIDEMPTY(ix,oy,oz) && ISLIQUIDEMPTY(ox,iy,oz) && ISLIQUIDEMPTY(ix,iy,oz))\
		{\
			t = !types[ox][oy][iz] + !types[ix][oy][iz] + !types[ox][iy][iz];\
			f = (t == 3)? 0.25f : (t == 2)? 0.15f : 0.0f;\
			v[vx][vy][vz].z += (oz < iz)? f : -f;\
		}\
	}
#define SMOOTH_EDGEX(vx,vy,vz, ox,oy,oz, ix,iy,iz)\
	if (ISLIQUIDEMPTY(ox,oy,oz) && ISLIQUIDEMPTY(ox,iy,oz) && ISLIQUIDEMPTY(ox,oy,iz))\
	{\
		v[vx][vy][vz].z += (oz < iz)? 0.15f : -0.15f;\
		v[vx][vy][vz].y += (oy < iy)? 0.15f : -0.15f;\
	}
#define SMOOTH_EDGEY(vx,vy,vz, ox,oy,oz, ix,iy,iz)\
	if (ISLIQUIDEMPTY(ox,oy,oz) && ISLIQUIDEMPTY(ix,oy,oz) && ISLIQUIDEMPTY(ox,oy,iz))\
	{\
		v[vx][vy][vz].z += (oz < iz)? 0.15f : -0.15f;\
		v[vx][vy][vz].x += (ox < ix)? 0.15f : -0.15f;\
	}
#define SMOOTH_EDGEZ(vx,vy,vz, ox,oy,oz, ix,iy,iz)\
	if (ISLIQUIDEMPTY(ox,oy,oz) && ISLIQUIDEMPTY(ix,oy,oz) && ISLIQUIDEMPTY(ox,iy,oz))\
	{\
		v[vx][vy][vz].y += (oy < iy)? 0.15f : -0.15f;\
		v[vx][vy][vz].x += (ox < ix)? 0.15f : -0.15f;\
	}
#define SMOOTH_FACE(vx,vy,vz, x0,y0,z0, x1,y1,z1, A, max)\
	v[vx][vy][vz].z = 0.5f;\
	for (x = x0 ; x <= x1 ; x++)\
	for (y = y0 ; y <= y1 ; y++)\
	for (z = z0 ; z <= z1 ; z++)\
	{\
		if (vx != x || vy != y || vz != z)\
		{\
			if (max)\
				v[vx][vy][vz].A = LIMAT_MAX (v[vx][vy][vz].A, v[x][y][z].A);\
			else\
				v[vx][vy][vz].A = LIMAT_MIN (v[vx][vy][vz].A, v[x][y][z].A);\
		}\
	}
	SMOOTH_CORNER(0,0,0, 0,0,0, 1,1,1);
	SMOOTH_CORNER(2,0,0, 2,0,0, 1,1,1);
	SMOOTH_CORNER(0,2,0, 0,2,0, 1,1,1);
	SMOOTH_CORNER(2,2,0, 2,2,0, 1,1,1);
	SMOOTH_CORNER(0,0,2, 0,0,2, 1,1,1);
	SMOOTH_CORNER(2,0,2, 2,0,2, 1,1,1);
	SMOOTH_CORNER(0,2,2, 0,2,2, 1,1,1);
	SMOOTH_CORNER(2,2,2, 2,2,2, 1,1,1);
	SMOOTH_EDGEX(1,0,0, 1,0,0, 1,1,1);
	SMOOTH_EDGEX(1,2,0, 1,2,0, 1,1,1);
	SMOOTH_EDGEX(1,0,2, 1,0,2, 1,1,1);
	SMOOTH_EDGEX(1,2,2, 1,2,2, 1,1,1);
	SMOOTH_EDGEY(0,1,0, 0,1,0, 1,1,1);
	SMOOTH_EDGEY(2,1,0, 2,1,0, 1,1,1);
	SMOOTH_EDGEY(0,1,2, 0,1,2, 1,1,1);
	SMOOTH_EDGEY(2,1,2, 2,1,2, 1,1,1);
	SMOOTH_EDGEZ(0,0,1, 0,0,1, 1,1,1);
	SMOOTH_EDGEZ(2,0,1, 2,0,1, 1,1,1);
	SMOOTH_EDGEZ(0,2,1, 0,2,1, 1,1,1);
	SMOOTH_EDGEZ(2,2,1, 2,2,1, 1,1,1);
	SMOOTH_FACE(0,1,1, 0,0,0, 0,2,2, x, 0);
	SMOOTH_FACE(2,1,1, 2,0,0, 2,2,2, x, 1);
	SMOOTH_FACE(1,0,1, 0,0,0, 2,0,2, y, 0);
	SMOOTH_FACE(1,2,1, 0,2,0, 2,2,2, y, 1);
	SMOOTH_FACE(1,1,0, 0,0,0, 2,2,0, z, 0);
	SMOOTH_FACE(1,1,2, 0,0,2, 2,2,2, z, 1);
#undef SMOOTH_CORNER
#undef SMOOTH_EDGEX
#undef SMOOTH_EDGEY
#undef SMOOTH_EDGEZ
#undef SMOOTH_FACE
}

static void private_triangulate_sloped (
	LIVoxBuilder* self,
	LIVoxVoxelB*  voxels[3][3][3],
	int           types[3][3][3],
	LIMatVector   v[3][3][3])
{
	int y;
	int y1;
	float sign;
	int slope[2][2] = { { 1, 1 }, { 1, 1 } };

	/* Decide whether to slope the bottom or the top side. */
	if (ISLIQUIDEMPTY(1,0,1) + ISLIQUIDEMPTY(1,2,1) != 1)
		return;
	if (!ISLIQUIDEMPTY(1,0,1))
	{
		sign = -1.0f;
		y = 2;
		y1 = 0;
	}
	else
	{
		sign = 1.0f;
		y = 0;
		y1 = 2;
	}

	/* Determine the sloping states of corners. */
	/* Other tiles prevent slope creation by default. */
	if (!ISLIQUIDEMPTY(0,1,1)) slope[0][0] = slope[0][1] = 0;
	if (!ISLIQUIDEMPTY(2,1,1)) slope[1][0] = slope[1][1] = 0;
	if (!ISLIQUIDEMPTY(1,1,0)) slope[0][0] = slope[1][0] = 0;
	if (!ISLIQUIDEMPTY(1,1,2)) slope[0][1] = slope[1][1] = 0;
	/* Adjacent slopes are possible when facing empty tiles. */
	/* Facing negative X (west). */
	if (ISLIQUIDEMPTY(0,1,0) && ISLIQUIDEMPTY(0,1,1) && ISSLOPED(1,1,0) &&
	    ISLIQUIDEMPTY(1,y,0) && !ISLIQUIDEMPTY(1,y1,0) && 
	   !ISLIQUIDEMPTY(2,1,0) && !ISLIQUIDEMPTY(2,1,1)) slope[0][0] = 1;
	if (ISLIQUIDEMPTY(0,1,2) && ISLIQUIDEMPTY(0,1,1) && ISSLOPED(1,1,2) &&
	    ISLIQUIDEMPTY(1,y,2) && !ISLIQUIDEMPTY(1,y1,2) && 
	   !ISLIQUIDEMPTY(2,1,2) && !ISLIQUIDEMPTY(2,1,1)) slope[0][1] = 1;
	/* Facing positive X (east). */
	if (ISLIQUIDEMPTY(2,1,0) && ISLIQUIDEMPTY(2,1,1) && ISSLOPED(1,1,0) &&
	    ISLIQUIDEMPTY(1,y,0) && !ISLIQUIDEMPTY(1,y1,0) && 
	   !ISLIQUIDEMPTY(0,1,0) && !ISLIQUIDEMPTY(0,1,1)) slope[1][0] = 1;
	if (ISLIQUIDEMPTY(2,1,2) && ISLIQUIDEMPTY(2,1,1) && ISSLOPED(1,1,2) &&
	    ISLIQUIDEMPTY(1,y,2) && !ISLIQUIDEMPTY(1,y1,2) && 
	   !ISLIQUIDEMPTY(0,1,2) && !ISLIQUIDEMPTY(0,1,1)) slope[1][1] = 1;
	/* Facing negative Z (north). */
	if (ISLIQUIDEMPTY(1,1,0) && ISLIQUIDEMPTY(0,1,0) && ISSLOPED(0,1,1) &&
	    ISLIQUIDEMPTY(0,y,1) && !ISLIQUIDEMPTY(0,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,2) && !ISLIQUIDEMPTY(0,1,2)) slope[0][0] = 1;
	if (ISLIQUIDEMPTY(1,1,0) && ISLIQUIDEMPTY(2,1,0) && ISSLOPED(2,1,1) &&
	    ISLIQUIDEMPTY(2,y,1) && !ISLIQUIDEMPTY(2,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,2) && !ISLIQUIDEMPTY(2,1,2)) slope[1][0] = 1;
	/* Facing positive Z (south). */
	if (ISLIQUIDEMPTY(1,1,2) && ISLIQUIDEMPTY(0,1,2) && ISSLOPED(0,1,1) &&
	    ISLIQUIDEMPTY(0,y,1) && !ISLIQUIDEMPTY(0,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,0) && !ISLIQUIDEMPTY(0,1,0)) slope[0][1] = 1;
	if (ISLIQUIDEMPTY(1,1,2) && ISLIQUIDEMPTY(2,1,2) && ISSLOPED(2,1,1) &&
	    ISLIQUIDEMPTY(2,y,1) && !ISLIQUIDEMPTY(2,y1,1) && 
	   !ISLIQUIDEMPTY(1,1,0) && !ISLIQUIDEMPTY(2,1,0)) slope[1][1] = 1;

	/* Create the slopes. */
	if (slope[0][0])
	{
		v[0][1][0].y += sign * 0.5f;
		v[0][y][0].y += sign * 1.0f;
		v[1][1][0].y += sign * 0.25f;
		v[1][y][0].y += sign * 0.5f;
		v[0][1][1].y += sign * 0.25f;
		v[0][y][1].y += sign * 0.5f;
	}
	if (slope[1][0])
	{
		v[2][1][0].y += sign * 0.5f;
		v[2][y][0].y += sign * 1.0f;
		v[1][1][0].y += sign * 0.25f;
		v[1][y][0].y += sign * 0.5f;
		v[2][1][1].y += sign * 0.25f;
		v[2][y][1].y += sign * 0.5f;
	}
	if (slope[0][1])
	{
		v[0][1][2].y += sign * 0.5f;
		v[0][y][2].y += sign * 1.0f;
		v[1][1][2].y += sign * 0.25f;
		v[1][y][2].y += sign * 0.5f;
		v[0][1][1].y += sign * 0.25f;
		v[0][y][1].y += sign * 0.5f;
	}
	if (slope[1][1])
	{
		v[2][1][2].y += sign * 0.5f;
		v[2][y][2].y += sign * 1.0f;
		v[1][1][2].y += sign * 0.25f;
		v[1][y][2].y += sign * 0.5f;
		v[2][1][1].y += sign * 0.25f;
		v[2][y][1].y += sign * 0.5f;
	}
	v[1][y][1].y = 0.25f * (v[0][y][1].y + v[2][y][1].y + v[1][y][0].y + v[1][y][2].y);

	/* Resolve degenerate cases. */
	if (slope[0][0] && slope[1][0] && slope[0][1] && slope[1][1])
	{
		v[1][y][1].y -= sign * 0.5f;
		if (ISLIQUIDEMPTY(1,0,0) && ISLIQUIDEMPTY(1,0,2))
		{
			v[1][y][0].y -= sign * 0.5f;
			v[1][y][2].y -= sign * 0.5f;
		}
		else if (ISLIQUIDEMPTY(0,0,1) && ISLIQUIDEMPTY(2,0,1))
		{
			v[0][y][1].y -= sign * 0.5f;
			v[2][y][1].y -= sign * 0.5f;
		}
	}
}

/** @} */
/** @} */
