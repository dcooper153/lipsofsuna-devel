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
 * \addtogroup liphy Physics
 * @{
 * \addtogroup LIPhyObject Object
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

#define PRIVATE_REALIZED 0x0200

static float
private_sweep_shape (const LIPhyObject* self,
                     const btTransform& start,
                     const btVector3&   sweep,
                     btVector3*         normal);

static int
private_sweep_sphere (const LIPhyObject* self,
                      btConvexShape*     shape,
                      const btTransform& start,
                      const btTransform& end,
                      LIPhyCollision*    result);

static void
private_update_state (LIPhyObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new physics object.
 *
 * \param physics Physics system.
 * \param id Object ID.
 * \param shape Collision shape or NULL.
 * \param control_mode Simulation mode.
 * \return New object or NULL.
 */
LIPhyObject*
liphy_object_new (LIPhyPhysics*    physics,
                  uint32_t         id,
                  LIPhyShape*      shape,
                  LIPhyControlMode control_mode)
{
	LIPhyObject* self;
	btVector3 position (0.0f, 0.0f, 0.0f);
	btQuaternion orientation (0.0f, 0.0f, 0.0f, 1.0f);

	self = (LIPhyObject*) lisys_calloc (1, sizeof (LIPhyObject));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->id = id;
	self->control_mode = control_mode;
	self->config.mass = 10.0f;
	self->config.speed = LIPHY_DEFAULT_SPEED;
	self->config.character_step = 0.35;
	self->config.collision_group = LIPHY_DEFAULT_COLLISION_GROUP;
	self->config.collision_mask = LIPHY_DEFAULT_COLLISION_MASK;
	try
	{
		self->shape = new btCompoundShape ();
		self->motion = new liphyMotionState (self, btTransform (orientation, position));
	}
	catch (...)
	{
		liphy_object_free (self);
		return NULL;
	}
	if (shape != NULL)
		liphy_object_insert_shape (self, shape, NULL);

	/* Add to dictionary. */
	if (self->id)
	{
		lisys_assert (!lialg_u32dic_find (physics->objects, id));
		if (!lialg_u32dic_insert (physics->objects, id, self))
		{
			liphy_object_free (self);
			return NULL;
		}
	}

	return self;
}

/**
 * \brief Frees the physics object.
 *
 * \param self Object.
 */
void
liphy_object_free (LIPhyObject* self)
{
	/* Cancel all contact callbacks involving us. */
	liphy_physics_clear_contacts (self->physics, self);

	/* Remove from dictionary. */
	if (self->id)
		lialg_u32dic_remove (self->physics->objects, self->id);

	/* Unrealize. */
	self->flags &= ~PRIVATE_REALIZED;
	private_update_state (self);

	/* Free shape. */
	if (self->shape != NULL)
		liphy_object_clear_shape (self);
	delete self->shape;

	/* Free self. */
	delete self->motion;
	lisys_free (self);
}

/**
 * \brief Clears the current shape of the object.
 *
 * \param self Object.
 */
void
liphy_object_clear_shape (LIPhyObject* self)
{
	LIPhyShape* shape;

	while (self->shape->getNumChildShapes ())
	{
		shape = (LIPhyShape*) self->shape->getChildShape (0)->getUserPointer ();
		self->shape->removeChildShapeByIndex (0);
		liphy_shape_free (shape);
	}
}

/**
 * \brief Modifies the velocity of the object with an impulse.
 *
 * \param self Object.
 * \param point Impulse point relative to the body.
 * \param impulse Impulse force.
 */
void
liphy_object_impulse (LIPhyObject*       self,
                      const LIMatVector* point,
                      const LIMatVector* impulse)
{
	btVector3 v0 (impulse->x, impulse->y, impulse->z);
	btVector3 v1 (point->x, point->y, point->z);

	if (self->control != NULL)
		self->control->apply_impulse (v0, v1);
}

/**
 * \brief Adds a collision shape to the object.
 *
 * The reference count of the collision shape is increased by one when calling
 * this so the caller should free the shape if it doesn't need it.
 *
 * \param self Object.
 * \param shape Collision shape.
 * \param transform Shape transformation or NULL for identity.
 */
int
liphy_object_insert_shape (LIPhyObject*          self,
                           LIPhyShape*           shape,
                           const LIMatTransform* transform)
{
	liphy_shape_ref (shape);
	if (transform != NULL)
	{
		btTransform btransform(
			btQuaternion (transform->rotation.x, transform->rotation.y,
			              transform->rotation.z, transform->rotation.w),
			btVector3 (transform->position.x, transform->position.y, transform->position.z));
		self->shape->addChildShape (btransform, shape->shape);
	}
	else
	{
		btTransform btransform (btQuaternion (0.0f, 0.0f, 0.0f, 1.0f));
		self->shape->addChildShape (btransform, shape->shape);
	}

	return 1;
}

/**
 * \brief Adds a new wheel to a vehicle object.
 *
 * \param self Object.
 * \param point Chassis connection point.
 * \param axle Axle direction vector.
 * \param radius Wheel radius.
 * \param susplen Length of suspension spring.
 * \param turning Nonzero if the wheel is controllable.
 */
void
liphy_object_insert_wheel (LIPhyObject*       self,
                           const LIMatVector* point,
                           const LIMatVector* axle,
                           float              radius,
                           float              susplen,
                           int                turning)
{
#if 0
	/* FIXME */
	if (self->flags & PRIVATE_ADDED_VEHICLE)
	{
		btVector3 bpoint (point->x, point->y, point->z);
		btVector3 bdir (0.0f, -1.0f, 0.0f);
		btVector3 baxle (axle->x, axle->y, axle->z);
		btWheelInfo& wheel = self->vehicle->addWheel (bpoint, bdir, baxle, susplen, radius, *self->vehicle_tuning, turning);
		/* FIXME: Should be configurable. */
		wheel.m_suspensionStiffness = 20.0f;
		wheel.m_wheelsDampingRelaxation = 2.3f;
		wheel.m_wheelsDampingCompression = 4.4f;
		wheel.m_frictionSlip = 1000.0f;
		wheel.m_rollInfluence = 0.1f;
	}
#endif
}

/**
 * \brief Causes the object to jump.
 *
 * Does the same as #liphy_object_impulse with origin as the position vector.
 *
 * \param self Object.
 * \param impulse Jump force.
 */
void
liphy_object_jump (LIPhyObject*       self,
                   const LIMatVector* impulse)
{
	LIMatVector o = { 0.0f, 0.0f, 0.0f };

	liphy_object_impulse (self, &o, impulse);
}

/**
 * \brief Performs a sweep test.
 *
 * Tests how far the object could move along the given sweep vector before
 * hitting an obstacle. Multiplying the sweep vector by the return value
 * yields a displacement vector that causes the object to move until it hits
 * an obstacle or the end of the sweep vector.
 *
 * \param self Object.
 * \param sweep Sweep vector.
 * \return Fraction swept before hitting an obstacle.
 */
float
liphy_object_sweep (const LIPhyObject* self,
                    const LIMatVector* sweep)
{
	btTransform transform;

	self->motion->getWorldTransform (transform);
	return private_sweep_shape (self, transform, btVector3 (sweep->x, sweep->y, sweep->z), NULL);
}

/**
 * \brief Performs a sweep test with a sphere.
 *
 * This function is a handy for things like melee attack checks since
 * you can place the sphere at the damaging point of the weapon and
 * project it to the direction of the attack.
 *
 * \param self Object.
 * \param relsrc Sweep start point, in object space.
 * \param reldst Sweep end point, in object space.
 * \param radius Radius of the swept sphere.
 * \param result Return location for collision information.
 * \return Nonzero if hit something.
 */
int
liphy_object_sweep_sphere (LIPhyObject*       self,
                           const LIMatVector* relsrc,
                           const LIMatVector* reldst,
                           float              radius,
                           LIPhyCollision*    result)
{
	int ret;
	btTransform transform;
	btTransform start (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (relsrc->x, relsrc->y, relsrc->z));
	btTransform end (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (reldst->x, reldst->y, reldst->z));
	btSphereShape shape (radius);

	self->motion->getWorldTransform (transform);
	start = transform * start;
	end = transform * end;

	btVector3 center = transform.getOrigin ();
	btVector3 origin0 = start.getOrigin ();
	btVector3 origin1 = end.getOrigin ();
	start.setOrigin (btVector3 (
		center[0] + (origin0[0] - center[0]),
		center[1] + (origin0[1] - center[1]),
		center[2] + (origin0[2] - center[2])));
	end.setOrigin (btVector3 (
		center[0] + (origin1[0] - center[0]),
		center[1] + (origin1[1] - center[1]),
		center[2] + (origin1[2] - center[2])));

	ret = private_sweep_sphere (self, &shape, start, end, result);
	return ret;
}

/**
 * \brief Gets the angular velocity of the object.
 *
 * \param self Object.
 * \param value Return location for the angular velocity vector.
 */
void
liphy_object_get_angular (const LIPhyObject* self,
                          LIMatVector*       value)
{
	btVector3 velocity;

	if (self->control != NULL)
	{
		self->control->get_angular (&velocity);
		value->x = velocity[0];
		value->y = velocity[1];
		value->z = velocity[2];
	}
	else
	{
		value->x = 0.0f;
		value->y = 0.0f;
		value->z = 0.0f;
	}
}

/**
 * \brief Sets the angular velocity of the object.
 *
 * \param self Object.
 * \param value Angular velocity vector.
 */
void
liphy_object_set_angular (LIPhyObject*       self,
                          const LIMatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.angular = *value;
	if (self->control != NULL)
		self->control->set_angular (velocity);
}

/**
 * \brief Gets the collision group of the object.
 *
 * \param self Object.
 * \return Group mask.
 */
int
liphy_object_get_collision_group (const LIPhyObject* self)
{
	return self->config.collision_group;
}

/**
 * \brief Sets the collision group of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision group mask.
 */
void
liphy_object_set_collision_group (LIPhyObject* self,
                                  int          mask)
{
	self->config.collision_group = mask;
	if (self->control != NULL)
		self->control->set_collision_group (mask);
}

/**
 * \brief Gets the collision mask of the object.
 *
 * \param self Object.
 * \return Collision mask.
 */
int
liphy_object_get_collision_mask (const LIPhyObject* self)
{
	return self->config.collision_mask;
}

/**
 * \brief Sets the collision mask of the object.
 *
 * Two objects can collide with each other if the mask of the first object has
 * common bits with the group of the second object and the mask of the second
 * object has common bits with the group of the first object.
 *
 * \param self Object.
 * \param mask Collision mask.
 */
void
liphy_object_set_collision_mask (LIPhyObject* self,
                                 int          mask)
{
	self->config.collision_mask = mask;
	if (self->control != NULL)
		self->control->set_collision_mask (mask);
}

/**
 * \brief Sets the contact handler callback of the object.
 *
 * The contact callback, when not NULL, is called every time the object
 * collides with something.
 *
 * \param self Object.
 * \param value Contact callback.
 */
void
liphy_object_set_contact_call (LIPhyObject*     self,
                               LIPhyContactCall value)
{
	self->config.contact_call = value;
	if (self->control != NULL)
		self->control->set_contacts (value != NULL);
}

/**
 * \brief Gets the simulation mode of the object.
 *
 * \param self Object.
 * \return Simulation mode.
 */
LIPhyControlMode
liphy_object_get_control_mode (const LIPhyObject* self)
{
	return self->control_mode;
}

/**
 * \brief Sets the simulation mode of the object.
 *
 * \param self Object.
 * \param value Simulation mode.
 */
void
liphy_object_set_control_mode (LIPhyObject*     self,
                               LIPhyControlMode value)
{
	if (self->control_mode == value)
		return;
	self->control_mode = value;
	private_update_state (self);
}

/**
 * \brief Gets the physics engine for which this object was created.
 * 
 * \param self Object.
 * \return Physics engine.
 */
LIPhyPhysics*
liphy_object_get_engine (LIPhyObject* self)
{
	return self->physics;
}

/**
 * \brief Returns the gravity acceleration vector of the object.
 *
 * \param self Object.
 * \param value Return location for the gravity vector.
 */
void
liphy_object_get_gravity (const LIPhyObject* self,
                          LIMatVector*       value)
{
	btVector3 gravity;

	if (self->control != NULL)
	{
		self->control->get_gravity (&gravity);
		*value = limat_vector_init (gravity[0], gravity[1], gravity[2]);
	}
	else
		*value = limat_vector_init (0.0f, 0.0f, 0.0f);
}

/**
 * \brief Sets the gravity acceleration vector of the object.
 *
 * \param self Object.
 * \param value Gravity vector.
 */
void
liphy_object_set_gravity (const LIPhyObject* self,
                          const LIMatVector* value)
{
	if (self->control != NULL)
		self->control->set_gravity (btVector3 (value->x, value->y, value->z));
}

/**
 * \brief Returns nonzero if the object is standing on ground.
 *
 * This only works for character objects. Other types always return zero.
 *
 * \param self Object.
 * \return Nonzero if standing on ground.
 */
int
liphy_object_get_ground (const LIPhyObject* self)
{
	if (self->control != NULL)
		return self->control->get_ground ();

	return 0;
}

/**
 * \brief Gets the inertia of the object.
 *
 * \param self Object.
 * \param result Return location for the inertia vector.
 */
void
liphy_object_get_inertia (LIPhyObject* self,
                          LIMatVector* result)
{
	btVector3 inertia;

	self->shape->calculateLocalInertia (self->config.mass, inertia);
	result->x = inertia[0];
	result->y = inertia[1];
	result->z = inertia[2];
}

/**
 * \brief Gets the mass of the object.
 *
 * \param self Object.
 * \return Mass.
 */
float
liphy_object_get_mass (const LIPhyObject* self)
{
	return self->config.mass;
}

/**
 * \brief Sets the mass of the object.
 *
 * \param self Object.
 * \param value Mass.
 */
void
liphy_object_set_mass (LIPhyObject* self,
                       float        value)
{
	LIMatVector v;
	btVector3 inertia(0.0, 0.0, 0.0);

	self->config.mass = value;
	if (self->control != NULL)
	{
		liphy_object_get_inertia (self, &v);
		inertia[0] = v.x;
		inertia[1] = v.y;
		inertia[2] = v.z;
		self->control->set_mass (value, inertia);
	}
}

/**
 * \brief Gets the movement force of the object.
 *
 * \param self Object.
 * \return Movement force.
 */
float
liphy_object_get_movement (const LIPhyObject* self)
{
	return self->config.movement;
}

/**
 * \brief Sets the movement force of the object.
 *
 * \param self Object.
 * \param value Movement force.
 */
void
liphy_object_set_movement (LIPhyObject* self,
                           float        value)
{
	int i;

	self->config.movement = value;
#if 0
	if (self->control_mode == LIPHY_CONTROL_MODE_VEHICLE)
	{
		for (i = 0 ; i < self->vehicle->getNumWheels () ; i++)
		{
			btWheelInfo& wheel = self->vehicle->getWheelInfo (i);
			wheel.m_engineForce = value;
		}
	}
#endif
}

/**
 * \brief Returns nonzero if the object is a part of the physics simulation.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int
liphy_object_get_realized (const LIPhyObject* self)
{
	return (self->flags & PRIVATE_REALIZED) != 0;
}

/**
 * \brief Realizes or unrealizes the object.
 *
 * \param self Object.
 * \param value Nonzero if should realize.
 * \return Nonzero if realized.
 */
int
liphy_object_set_realized (LIPhyObject* self,
                           int          value)
{
	if ((value != 0) == ((self->flags & PRIVATE_REALIZED) != 0))
		return 1;
	if (value)
		self->flags |= PRIVATE_REALIZED;
	else
		self->flags &= ~PRIVATE_REALIZED;
	private_update_state (self);

	return 1;
}

/**
 * \brief Sets the angular factor of the object.
 * 
 * \param self Object.
 * \param value Angular factor
 */
void
liphy_object_set_rotating (LIPhyObject* self,
                           float        value)
{
#if 0
	if (self->body)
	{
		if (self->control_mode != LIPHY_CONTROL_MODE_STATIC)
			self->body->setAngularFactor (value);
	}
#endif
}

/**
 * \brief Gets the movement speed of the object.
 *
 * \param self Object.
 * \return Movement speed.
 */
float
liphy_object_get_speed (const LIPhyObject* self)
{
	return self->config.speed;
}

/**
 * \brief Sets the movement speed of the object.
 *
 * \param self Object.
 * \param value Movement speed.
 */
void
liphy_object_set_speed (LIPhyObject* self,
                        float        value)
{
	self->config.speed = value;
}

/**
 * \brief Gets the strafing force of the object.
 *
 * \param self Object.
 * \return Strafing force.
 */
float
liphy_object_get_strafing (const LIPhyObject* self)
{
	return self->config.strafing;
}

/**
 * \brief Sets the strafing force of the object.
 *
 * \param self Object.
 * \param value Strafing force.
 */
void
liphy_object_set_strafing (LIPhyObject* self,
                           float        value)
{
	self->config.strafing = value;
}

/**
 * \brief Gets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void
liphy_object_get_transform (const LIPhyObject* self,
                            LIMatTransform*    value)
{
	btTransform trans;
	btQuaternion rotation;
	btVector3 position;

	self->motion->getWorldTransform (trans);
	rotation = trans.getRotation ();
	position = trans.getOrigin ();
	value->position.x = position[0];
	value->position.y = position[1];
	value->position.z = position[2];
	value->rotation.x = rotation[0];
	value->rotation.y = rotation[1];
	value->rotation.z = rotation[2];
	value->rotation.w = rotation[3];
}

/**
 * \brief Sets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Quaternion.
 */
void
liphy_object_set_transform (LIPhyObject*          self,
                            const LIMatTransform* value)
{
	btVector3 origin (value->position.x, value->position.y, value->position.z);
	btQuaternion rotation (value->rotation.x, value->rotation.y, value->rotation.z, value->rotation.w);
	btTransform transform (rotation, origin);

	self->motion->setWorldTransform (transform);
	if (self->control != NULL)
		self->control->transform (transform);
}

/**
 * \brief Gets the user pointer stored to the object.
 *
 * \param self Object.
 * \return User pointer.
 */
void*
liphy_object_get_userdata (LIPhyObject* self)
{
	return self->config.userdata;
}

/**
 * \brief Stores a user pointer to the object.
 *
 * \param self Object.
 * \param value User pointer.
 */
void
liphy_object_set_userdata (LIPhyObject* self,
                           void*        value)
{
	self->config.userdata = value;
}

/**
 * \brief Gets the current linear velocity of the object.
 *
 * \param self Object.
 * \param value Return location for the velocity.
 */
void
liphy_object_get_velocity (LIPhyObject* self,
                           LIMatVector* value)
{
	btVector3 velocity;

	if (self->control != NULL)
	{
		self->control->get_velocity (&velocity);
		*value = limat_vector_init (velocity[0], velocity[1], velocity[2]);
	}
	else
		*value = self->config.velocity;
}

/**
 * \brief Sets the linear velocity of the object.
 *
 * \param self Object.
 * \param value Linear velocity vector.
 */
void
liphy_object_set_velocity (LIPhyObject*       self,
                           const LIMatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.velocity = *value;
	if (self->control != NULL)
		self->control->set_velocity (velocity);
}

/*****************************************************************************/

class PrivateConvexTest : public btCollisionWorld::ClosestConvexResultCallback
{
public:
	PrivateConvexTest (btCollisionObject* self) :
		btCollisionWorld::ClosestConvexResultCallback (btVector3 (0.0, 0.0, 0.0), btVector3 (0.0, 0.0, 0.0))
	{
		this->self = self;
	}
	virtual btScalar addSingleResult (btCollisionWorld::LocalConvexResult& result, bool world)
	{
		if (result.m_hitCollisionObject == self)
			return 1.0;
		return ClosestConvexResultCallback::addSingleResult (result, world);
	}
protected:
	btCollisionObject* self;
};

static int
private_sweep_sphere (const LIPhyObject* self,
                      btConvexShape*     shape,
                      const btTransform& start,
                      const btTransform& end,
                      LIPhyCollision*    result)
{
	btTransform src;
	btTransform dst;
	btCollisionObject* object;
	btCollisionWorld* collision;

	/* Get own object. */
	if (self->control == NULL)
		return 0;
	object = self->control->get_object ();
	if (object == NULL)
		return 0;

	/* Initialize sweep. */
	PrivateConvexTest test (object);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = btBroadphaseProxy::DefaultFilter;
	test.m_collisionFilterMask = btBroadphaseProxy::AllFilter;

	/* Sweep the shape. */
	collision = self->physics->dynamics->getCollisionWorld ();
	collision->convexSweepTest (shape, start, end, test);
	result->fraction = test.m_closestHitFraction;
	result->normal.x = test.m_hitNormalWorld[0];
	result->normal.y = test.m_hitNormalWorld[1];
	result->normal.z = test.m_hitNormalWorld[2];
	result->point.x = test.m_hitPointWorld[0];
	result->point.y = test.m_hitPointWorld[1];
	result->point.z = test.m_hitPointWorld[2];
	if (test.m_hitCollisionObject != NULL)
		result->object = (LIPhyObject*) test.m_hitCollisionObject->getUserPointer ();
	else
		result->object = NULL;

	return result->fraction < 1.0f;
}

static float
private_sweep_shape (const LIPhyObject* self,
                     const btTransform& start,
                     const btVector3&   sweep,
                     btVector3*         normal)
{
	int i;
	btTransform src;
	btTransform dst;
	btCollisionObject* object;
	btCollisionWorld* collision;
	btConvexShape* shape;

	/* Get own object. */
	if (self->control == NULL)
		return 1.0f;
	object = self->control->get_object ();
	if (object == NULL)
		return 1.0f;
	if (self->shape->getNumChildShapes () == 0)
		return 1.0f;

	/* Initialize sweep. */
	float best = 1.0f;
	PrivateConvexTest test (object);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = self->config.collision_group;
	test.m_collisionFilterMask = self->config.collision_mask;

	/* Sweep the shape. */
	for (i = 0 ; i < self->shape->getNumChildShapes () ; i++)
	{
		collision = self->physics->dynamics->getCollisionWorld ();
		shape = (btConvexShape*) self->shape->getChildShape (i);
		src = start;
		dst = src;
		dst.setOrigin (dst.getOrigin () + sweep);
		collision->convexSweepTest (shape, src, dst, test);
		if (test.m_closestHitFraction <= best)
		{
			best = test.m_closestHitFraction;
			if (normal != NULL)
				*normal = test.m_hitNormalWorld;
		}
	}

	return best;
}

static void
private_update_state (LIPhyObject* self)
{
	btCollisionShape* shape;

	/* Remove all constraints involving us. */
	liphy_physics_clear_constraints (self->physics, self);

	/* Remove old controller. */
	if (self->control != NULL)
	{
		delete self->control;
		self->control = NULL;
	}

	/* Create new controller. */
	if (self->flags & PRIVATE_REALIZED)
	{
		shape = self->shape;
		switch (self->control_mode)
		{
			case LIPHY_CONTROL_MODE_NONE:
				break;
			case LIPHY_CONTROL_MODE_CHARACTER:
				self->control = new liphyCharacterControl (self, shape);
				break;
			case LIPHY_CONTROL_MODE_RIGID:
				self->control = new liphyRigidControl (self, shape);
				break;
			case LIPHY_CONTROL_MODE_STATIC:
				self->control = new liphyStaticControl (self, shape);
				break;
			case LIPHY_CONTROL_MODE_VEHICLE:
				self->control = new liphyVehicleControl (self, shape);
				break;
			default:
				lisys_assert (0);
				break;
		}
		if (self->control != NULL)
			self->control->set_contacts (self->config.contact_call != NULL);
	}
}

/** @} */
/** @} */
