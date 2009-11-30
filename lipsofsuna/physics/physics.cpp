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

#include <system/lips-system.h>
#include "physics.h"
#include "physics-object.h"
#include "physics-private.h"

#define MAXPROXIES 1000000

class ConvexTestIgnore : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	ConvexTestIgnore (liphyObject** ignore_array, int ignore_count) :
		btCollisionWorld::ClosestConvexResultCallback (btVector3 (0.0, 0.0, 0.0), btVector3 (0.0, 0.0, 0.0))
	{
		this->ignore_count = ignore_count;
		this->ignore_array = ignore_array;
	}
	virtual btScalar addSingleResult (btCollisionWorld::LocalConvexResult& result, bool world)
	{
		int i;
		liphyObject* hit = (liphyObject*) result.m_hitCollisionObject->getUserPointer ();
		for (i = 0 ; i < this->ignore_count ; i++)
		{
			if (hit == this->ignore_array[i])
				return 1.0;
		}
		return ClosestConvexResultCallback::addSingleResult (result, world);
	}
protected:
	int ignore_count;
	liphyObject** ignore_array;
};

static bool
private_contact_processed (btManifoldPoint& cp,
                           void*            body0,
                           void*            body1);

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

	self = (liphyPhysics*) lisys_calloc (1, sizeof (liphyPhysics));
	if (self == NULL)
		return NULL;
	try
	{
		self->ghostcallback = new btGhostPairCallback ();
#ifdef LIPHY_BROADPHASE_DBVT
		self->broadphase = new btDbvtBroadphase ();
#else
		self->broadphase = new bt32BitAxisSweep3 (min, max, MAXPROXIES);
#endif
		self->broadphase->getOverlappingPairCache ()->setInternalGhostPairCallback (self->ghostcallback);
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

	extern ContactProcessedCallback gContactProcessedCallback;
	gContactProcessedCallback = private_contact_processed;

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
	delete self->ghostcallback;
	lialg_list_free (self->controllers);
	lisys_free (self);
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
                        const limatVector*  start,
                        const limatVector*  end,
                        limatVector*        result,
                        limatVector*        normal)
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
 * \brief Casts a sphere to the scene.
 *
 * If the sphere hits no obstacle, result is set to the cast end point, normal is
 * set to cast direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Cast start point.
 * \param end Cast end point.
 * \param radius Sphere radius.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignore objects.
 * \param result Return location for collision data.
 * \return Nonzero if a collision occurred.
 */
int
liphy_physics_cast_sphere (const liphyPhysics* self,
                           const limatVector*  start,
                           const limatVector*  end,
                           float               radius,
                           liphyObject**       ignore_array,
                           int                 ignore_count,
                           liphyCollision*     result)
{
	btTransform src;
	btTransform dst;
	btCollisionWorld* collision;
	btTransform btstart (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (start->x, start->y, start->z));
	btTransform btend (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (end->x, end->y, end->z));
	btSphereShape shape (radius);

	/* Initialize sweep. */
	ConvexTestIgnore test (ignore_array, ignore_count);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
	test.m_collisionFilterMask = btBroadphaseProxy::AllFilter;

	/* Sweep the shape. */
	collision = self->dynamics->getCollisionWorld ();
	collision->convexSweepTest (&shape, btstart, btend, test);
	result->fraction = test.m_closestHitFraction;
	result->normal.x = test.m_hitNormalWorld[0];
	result->normal.y = test.m_hitNormalWorld[1];
	result->normal.z = test.m_hitNormalWorld[2];
	result->point.x = test.m_hitPointWorld[0];
	result->point.y = test.m_hitPointWorld[1];
	result->point.z = test.m_hitPointWorld[2];
	if (test.m_hitCollisionObject != NULL)
		result->object = (liphyObject*) test.m_hitCollisionObject->getUserPointer ();
	else
		result->object = NULL;

	return result->fraction < 1.0f;
}

/**
 * \brief Clear all pending contact callbacks for the object.
 *
 * This function can be safely called from inside the contact processing loop.
 * It's used by object cleanup code to clear references that are going invalid.
 *
 * \param self Physics.
 * \param object Object.
 */
void
liphy_physics_clear_contacts (liphyPhysics* self,
                              liphyObject*  object)
{
	lialgList* ptr;
	lialgList* next;
	liphyContactRecord* record;

	for (ptr = self->contacts ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		record = (liphyContactRecord*) ptr->data;
		if (record->object0 == object || record->object1 == object)
		{
			if (self->contacts_iter == ptr)
				self->contacts_iter = ptr->next;
			lialg_list_remove (&self->contacts, ptr);
			lisys_free (record);
		}
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
	lialgList* ptr;
	liphyContact contact;
	liphyContactRecord* record;

	/* Step simulation. */
	self->dynamics->stepSimulation (secs, 20);

	/* Process contacts. */
	for (ptr = self->contacts ; ptr != NULL ; ptr = self->contacts_iter)
	{
		self->contacts_iter = ptr->next;
		record = (liphyContactRecord*) ptr->data;
		lialg_list_remove (&self->contacts, ptr);
		contact.impulse = record->impulse;
		contact.point = record->point;
		contact.normal = record->normal;
		if (record->object0->config.contact_call != NULL)
		{
			contact.object = record->object1;
			record->object0->config.contact_call (record->object0, &contact);
		}
		if (record->object1->config.contact_call != NULL)
		{
			contact.object = record->object0;
			record->object1->config.contact_call (record->object1, &contact);
		}
		lisys_free (record);
	}
	self->contacts_iter = NULL;
}

/**
 * \brief Sets the object transformation callback.
 *
 * The callback will be called every time an object is moved.
 *
 * \param self Physics simulation.
 * \param value Transformation callback function.
 */
void
liphy_physics_set_transform_callback (liphyPhysics*      self,
                                      liphyTransformCall value)
{
	self->transform_callback = value;
}

/**
 * \brief Gets the userdata of the physics engine.
 *
 * \param self Physics simulation.
 * \return Userdata.
 */
void*
liphy_physics_get_userdata (liphyPhysics* self)
{
	return self->userdata;
}

/**
 * \brief Sets the userdata of the physics engine.
 *
 * \param self Physics simulation.
 * \param data Userdata.
 */
void
liphy_physics_set_userdata (liphyPhysics* self,
                            void*         data)
{
	self->userdata = data;
}

/*****************************************************************************/

static bool
private_contact_processed (btManifoldPoint& point,
                           void*            body0,
                           void*            body1)
{
	limatVector momentum0;
	limatVector momentum1;
	liphyObject* object;
	liphyContactRecord contact;
	liphyContactRecord* tmp;

	/* Get objects. */
	contact.object0 = (liphyObject*)((btCollisionObject*) body0)->getUserPointer ();
	contact.object1 = (liphyObject*)((btCollisionObject*) body1)->getUserPointer ();
	if (contact.object0 != NULL) object = contact.object0;
	else if (contact.object1 == NULL) object = contact.object1;
	else return false;
	if (contact.object0->config.contact_call == NULL &&
	    contact.object1->config.contact_call == NULL)
		return false;

	/* Get collision point. */
	const btVector3& pt = point.getPositionWorldOnB ();
	const btVector3& nm = point.m_normalWorldOnB;
	contact.point = limat_vector_init (pt[0], pt[1], pt[2]);
	contact.normal = limat_vector_init (nm[0], nm[1], nm[2]);

	/* Calculate impulse. */
	/* FIXME: This is sloppy. */
	liphy_object_get_velocity (contact.object0, &momentum0);
	liphy_object_get_velocity (contact.object1, &momentum1);
	momentum0 = limat_vector_multiply (momentum0, liphy_object_get_mass (contact.object0));
	momentum1 = limat_vector_multiply (momentum1, liphy_object_get_mass (contact.object1));
	contact.impulse = limat_vector_get_length (limat_vector_subtract (momentum0, momentum1));

	/* Manipulating the physics state in the contact callback is dangerous
	   so let's store the contact to a list and process it later. */
	tmp = (liphyContactRecord*) lisys_calloc (1, sizeof (liphyContactRecord));
	if (tmp == NULL)
		return false;
	*tmp = contact;
	if (!lialg_list_prepend (&object->physics->contacts, tmp))
		lisys_free (tmp);

	return false;
}

static void
private_internal_tick (btDynamicsWorld* dynamics,
                       btScalar         step)
{
}

/** @} */
/** @} */
