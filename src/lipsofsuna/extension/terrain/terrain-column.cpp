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
#include <btBulletCollisionCommon.h>
#include <BulletCollision/NarrowPhaseCollision/btGjkConvexCast.h>
#include <BulletCollision/NarrowPhaseCollision/btSubSimplexConvexCast.h>

static int private_convex_ray_cast (
	const btTransform&   rayFromTrans,
	const btTransform&   rayToTrans,
	const btConvexShape* convexShape,
	float*               resultFraction,
	btVector3*           resultNormal);

/*****************************************************************************/

/**
 * \brief Casts a ray against the column.
 * \param self Terrain column.
 * \param src Source point in column local units.
 * \param dst Destination point in column local units.
 * \param result_point Return location for the column local space hit point.
 * \param result_normal Return location for the normal vector.
 * \param result_fraction Return location for the ray fraction.
 * \return Nonzero if intersected.
 */
int liext_terrain_column_intersect_ray (
	LIExtTerrainColumn* self,
	const LIMatVector*  src,
	const LIMatVector*  dst,
	LIMatVector*        result_point,
	LIMatVector*        result_normal,
	float*              result_fraction)
{
	float min_y = LIMAT_MIN (src->y, dst->y);
	float max_y = LIMAT_MAX (src->y, dst->y);
	btVector3 p0 (src->x, src->y, src->z);
	btVector3 p1 (dst->x, dst->y, dst->z);
	btTransform t0 (btQuaternion (0.0f, 0.0f, 0.0f, 1.0f), p0);
	btTransform t1 (btQuaternion (0.0f, 0.0f, 0.0f, 1.0f), p1);

	/* Initialize the stick vertices. */
	btVector3 verts[8] =
	{
		btVector3(0.0f, 0.0f, 0.0f),
		btVector3(1.0f, 0.0f, 0.0f),
		btVector3(0.0f, 0.0f, 1.0f),
		btVector3(1.0f, 0.0f, 1.0f),
		btVector3(0.0f, 0.0f, 0.0f),
		btVector3(1.0f, 0.0f, 0.0f),
		btVector3(0.0f, 0.0f, 1.0f),
		btVector3(1.0f, 0.0f, 1.0f)
	};

	/* Collide against each stick. */
	float ys = 0.0f;
	float ys_min = 0.0f;
	float ys_max = 0.0f;
	LIExtTerrainStick* stick;
	for (stick = self->sticks ; stick != NULL ; stick = stick->next)
	{
		ys += stick->height;
		if (ys_min >= max_y)
			break;

		/* Set the top surface offset. */
		verts[4][1] = ys + stick->vertices[0][0].offset;
		verts[5][1] = ys + stick->vertices[1][0].offset;
		verts[6][1] = ys + stick->vertices[0][1].offset;
		verts[7][1] = ys + stick->vertices[1][1].offset;
		ys_max = LIMAT_MAX (ys_max, verts[4][1]);
		ys_max = LIMAT_MAX (ys_max, verts[5][1]);
		ys_max = LIMAT_MAX (ys_max, verts[6][1]);
		ys_max = LIMAT_MAX (ys_max, verts[7][1]);

		/* Cast the ray against a temporary convex shape. */
		if (ys_max >= min_y && stick->material != 0)
		{
			float fraction;
			btVector3 normal;
			btConvexHullShape shape ((btScalar*) verts, 8, sizeof (btVector3));
			if (private_convex_ray_cast (t0, t1, &shape, &fraction, &normal))
			{
				btVector3 p (p0 + (p1 - p0) * fraction);
				*result_point = limat_vector_init (p[0], p[1], p[2]);
				*result_normal = limat_vector_init (normal[0], normal[1], normal[2]);
				*result_fraction = fraction;
				return 1;
			}
		}

		/* Set the bottom surface offset. */
		verts[0] = verts[4];
		verts[1] = verts[5];
		verts[2] = verts[6];
		verts[3] = verts[7];
		ys_min = LIMAT_MIN (ys_max, verts[4][1]);
		ys_min = LIMAT_MIN (ys_max, verts[5][1]);
		ys_min = LIMAT_MIN (ys_max, verts[6][1]);
		ys_min = LIMAT_MIN (ys_max, verts[7][1]);
	}

	return 0;
}

/*****************************************************************************/

// Based on btCollisionWorld::rayTestSingle() from Bullet.
static int private_convex_ray_cast (
	const btTransform&   rayFromTrans,
	const btTransform&   rayToTrans,
	const btConvexShape* convexShape,
	float*               resultFraction,
	btVector3*           resultNormal)
{
	btSphereShape pointShape (btScalar(0.0));
	pointShape.setMargin(0.0f);
	const btConvexShape* castShape = &pointShape;

	btConvexCast::CastResult castResult;
	castResult.m_fraction = 1.0f;

	btVoronoiSimplexSolver simplexSolver;
	btSubsimplexConvexCast convexCaster(castShape,convexShape,&simplexSolver);

	btTransform worldTransform;
	worldTransform.setIdentity();

	if (convexCaster.calcTimeOfImpact(rayFromTrans, rayToTrans, worldTransform, worldTransform, castResult))
	{
		if (castResult.m_normal.length2() > btScalar(0.0001))
		{
			//rotate normal into worldspace
			*resultNormal = rayFromTrans.getBasis() * castResult.m_normal;
			resultNormal->normalize();
			*resultFraction = castResult.m_fraction;
			return 1;
		}
	}

	return 0;
}

/** @} */
/** @} */
