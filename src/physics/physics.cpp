/* Lips of Suna
 * Copyright© 2007-2009 Lips of Suna development team.
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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup liphyPhysics Physics
 * @{
 */

#include "physics.h"
#include "physics-private.h"

#define MAXPROXIES 1024

static void
private_internal_tick (btDynamicsWorld* dynamics,
                       btScalar         step);

/*****************************************************************************/

/**
 * \brief Creates a new physics simulation.
 *
 * \return New physics simulation or NULL.
 */
liphyPhysics*
liphy_physics_new ()
{
	liphyPhysics* self;
	btVector3 min (-65535, -65535, -65535);
	btVector3 max ( 65535,  65535,  65535);

	self = (liphyPhysics*) calloc (1, sizeof (liphyPhysics));
	if (self == NULL)
		return NULL;
	try
	{
		self->broadphase = new bt32BitAxisSweep3 (min, max, MAXPROXIES);
		self->broadphase->getOverlappingPairCache ()->setInternalGhostPairCallback (new btGhostPairCallback ());
		self->configuration = new btDefaultCollisionConfiguration ();
		self->dispatcher = new btCollisionDispatcher (self->configuration);
		self->solver = new btSequentialImpulseConstraintSolver ();
		self->dynamics = new btDiscreteDynamicsWorld (self->dispatcher, self->broadphase, self->solver, self->configuration);
		self->dynamics->setGravity (btVector3 (0.0f, -10.0f, 0.0f));
		self->dynamics->setInternalTickCallback (private_internal_tick, self);
	}
	catch (...)
	{
		liphy_physics_free (self);
		return NULL;
	}

	return self;
}

/**
 * \brief Frees the physics simulation.
 *
 * \param self Physics simulation.
 */
void
liphy_physics_free (liphyPhysics* self)
{
	delete self->dynamics;
	delete self->solver;
	delete self->configuration;
	delete self->dispatcher;
	delete self->broadphase;
	lialg_list_free (self->controllers);
	free (self);
}

/**
 * \brief Casts a ray to the scene.
 *
 * If the ray hits no obstacle, result is set to the ray end point, normal is
 * set to ray direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Ray start point.
 * \param end Ray end point.
 * \param result Return location for collision point.
 * \param normal Return location for collision surface normal.
 * \return Nonzero if no collision occurred.
 */
int
liphy_physics_cast_ray (const liphyPhysics* self,
                        const limatVector*     start,
                        const limatVector*     end,
                        limatVector*           result,
                        limatVector*           normal)
{
	btVector3 src (start->x, start->y, start->z);
	btVector3 dst (end->x, end->y, end->z);
//	btBroadphaseProxy* proxy;
	btCollisionWorld* collision;
	btCollisionWorld::ClosestRayResultCallback test (src, dst);

	/* Cast the ray. */
//	proxy = self->body->getBroadphaseHandle ();
	collision = self->dynamics->getCollisionWorld ();
	test.m_closestHitFraction = 1.0f;
//	test.m_collisionFilterGroup = proxy->m_collisionFilterGroup;
//	test.m_collisionFilterMask = proxy->m_collisionFilterMask;
	collision->rayTest (src, dst, test);

	/* Inspect results. */
	if (test.m_closestHitFraction < 1.0f)
	{
		*result = limat_vector_init (test.m_hitPointWorld[0], test.m_hitPointWorld[1], test.m_hitPointWorld[2]);
		*normal = limat_vector_init (test.m_hitNormalWorld[0], test.m_hitNormalWorld[1], test.m_hitNormalWorld[2]);
		return 0;
	}
	else
	{
		*result = *end;
		*normal = limat_vector_normalize (limat_vector_subtract (*end, *start));
		return 1;
	}
}

/**
 * \brief Updates the physics simulation.
 *
 * \param self Physics simulation.
 * \param secs Tick length in seconds.
 */
void
liphy_physics_update (liphyPhysics* self,
                      float         secs)
{
	self->dynamics->stepSimulation (secs, 20);
}

/**
 * \brief Sets the object transformation callback.
 *
 * The callback will be called every time an object is moved.
 *
 * \param self Physics simulation.
 * \param func Transformation callback function.
 */
void
liphy_physics_set_transform_callback (liphyPhysics*      self,
                                      liphyTransformFunc func)
{
	self->callbacks.transform = func;
}

/*****************************************************************************/

static void
private_internal_tick (btDynamicsWorld* dynamics,
                       btScalar         step)
{
#if 0
	lialgList* ptr;
	liphyObject* object;
	liphyPhysics* self = (liphyPhysics*) dynamics->getWorldUserInfo ();

	for (ptr = self->controllers ; ptr != NULL ; ptr = ptr->next)
	{
		object = (liphyObject*) ptr->data;
		assert (object->control.call0 != NULL || object->control.call1 != NULL);
		if (object->control.call0 != NULL)
			object->control.call0 (object, step);
		if (object->control.call1 != NULL)
			object->control.call1 (object, step);
	}
#endif
}

/** @} */
/** @} */
