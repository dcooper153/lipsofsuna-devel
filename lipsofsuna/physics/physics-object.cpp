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
 * \addtogroup liphyObject Object
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

#define PRIVATE_ADDED_BODY 0x0001
#define PRIVATE_ADDED_GHOST 0x0002
#define PRIVATE_ADDED_VEHICLE 0x0004
#define PRIVATE_ADDED_CHARACTER_CONTROLLER 0x0008
#define PRIVATE_ADDED_CONTACT_CONTROLLER 0x0010
#define PRIVATE_ADDED_CUSTOM_CONTROLLER 0x0020
#define PRIVATE_CHANGED_CONTROL_MODE 0x0040
#define PRIVATE_CHANGED_SHAPE 0x0080
#define PRIVATE_CHANGED_USERCALL 0x0100
#define PRIVATE_REALIZED 0x0200
#define PRIVATE_CCD_MOTION_THRESHOLD 1.0f
#ifdef ENABLE_PHYSICS_DEBUG
#define PRIVATE_DEBUG_STATE(s, m) printf ("DEBUG: %p " m "\n", s)
#else
#define PRIVATE_DEBUG_STATE(s, m)
#endif

static btCollisionShape*
private_choose_shape (const liphyObject* self,
                      liphyShape*        shape,
                      liphyShapeMode     mode);

static float
private_sweep_shape (const liphyObject* self,
                     const btTransform& start,
                     const btVector3&   sweep,
                     btVector3*         normal);

static int
private_sweep_sphere (const liphyObject* self,
                      btConvexShape*     shape,
                      const btTransform& start,
                      const btTransform& end,
                      liphyCollision*    result);

static void
private_update_state (liphyObject* self);

/*****************************************************************************/

/**
 * \brief Creates a new physics object.
 *
 * \param physics Physics system.
 * \param shape Collision shape or NULL.
 * \param shape_mode Collision shape mode.
 * \param control_mode Simulation mode.
 * \return New object or NULL.
 */
liphyObject*
liphy_object_new (liphyPhysics*    physics,
                  liphyShape*      shape,
                  liphyShapeMode   shape_mode,
                  liphyControlMode control_mode)
{
	liphyObject* self;
	btVector3 position (0.0f, 0.0f, 0.0f);
	btQuaternion orientation (0.0f, 0.0f, 0.0f, 1.0f);

	self = (liphyObject*) calloc (1, sizeof (liphyObject));
	if (self == NULL)
		return NULL;
	self->physics = physics;
	self->control_mode = control_mode;
	self->config.mass = 10.0f;
	self->config.speed = LIPHY_DEFAULT_SPEED;
	self->config.character_step = 0.35;
	self->config.collision_group = LIPHY_DEFAULT_COLLISION_GROUP;
	self->config.collision_mask = LIPHY_DEFAULT_COLLISION_MASK;
	try
	{
		self->motion = new liphyMotionState (self, btTransform (orientation, position));
	}
	catch (...)
	{
		liphy_object_free (self);
		return NULL;
	}
	self->flags = PRIVATE_CHANGED_CONTROL_MODE | PRIVATE_CHANGED_SHAPE | PRIVATE_CHANGED_USERCALL;
	liphy_object_set_shape (self, shape, shape_mode);

	return self;
}

/**
 * \brief Frees the physics object.
 *
 * \param self Object.
 */
void
liphy_object_free (liphyObject* self)
{
	self->flags &= ~PRIVATE_REALIZED;
	private_update_state (self);
	delete self->motion;
	free (self);
}

/**
 * \brief Modifies the velocity of the object with an impulse.
 *
 * \param self Object.
 * \param point Impulse point relative to the body.
 * \param impulse Impulse force.
 */
void
liphy_object_impulse (liphyObject*       self,
                      const limatVector* point,
                      const limatVector* impulse)
{
	btVector3 v0 (impulse->x, impulse->y, impulse->z);
	btVector3 v1 (point->x, point->y, point->z);

	if (self->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
		self->config.character_force = limat_vector_add (self->config.character_force, *impulse);
	if (self->body != NULL)
		self->body->applyImpulse (v0, v1);
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
liphy_object_insert_wheel (liphyObject*       self,
                           const limatVector* point,
                           const limatVector* axle,
                           float              radius,
                           float              susplen,
                           int                turning)
{
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
}

/**
 * \brief Causes the object to jump.
 *
 * Adds the walking velocity vector of the character to its rigid body
 * velocity and then does the same as #liphy_object_impulse. This causes
 * the character to automatically jump to its walking direction.
 *
 * If the object is not a character, the character specific operations are
 * skipped and the function behaves the same way as #liphy_object_impulse
 * with point of impulse set to origin in body space.
 *
 * \param self Object.
 * \param impulse Jump force.
 */
void
liphy_object_jump (liphyObject*       self,
                   const limatVector* impulse)
{
	limatVector v;

	if (self->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
	{
		v = self->config.velocity;
		v = limat_vector_add (v, limat_vector_multiply (*impulse, 1.0f / self->config.mass));
		self->config.velocity = v;
	}
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
liphy_object_sweep (const liphyObject* self,
                    const limatVector* sweep)
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
liphy_object_sweep_sphere (liphyObject*       self,
                           const limatVector* relsrc,
                           const limatVector* reldst,
                           float              radius,
                           liphyCollision*    result)
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
liphy_object_get_angular (const liphyObject* self,
                          limatVector*       value)
{
	btVector3 velocity;

	if (self->body != NULL)
	{
		velocity = self->body->getAngularVelocity ();
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
liphy_object_set_angular (liphyObject*       self,
                          const limatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.angular = *value;
	if (self->body != NULL)
	{
		if (self->control_mode != LIPHY_CONTROL_MODE_STATIC)
			self->body->setAngularVelocity (velocity);
	}
}

/**
 * \brief Gets the collision group of the object.
 *
 * \param self Object.
 * \return Group mask.
 */
int
liphy_object_get_collision_group (const liphyObject* self)
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
liphy_object_set_collision_group (liphyObject* self,
                                  int          mask)
{
	btBroadphaseProxy* proxy;

	self->config.collision_group = mask;
	if (self->body != NULL)
	{
		proxy = self->body->getBroadphaseHandle ();
		if (proxy != NULL)
			proxy->m_collisionFilterGroup = mask;
	}
	if (self->ghost != NULL)
	{
		proxy = self->ghost->getBroadphaseHandle ();
		if (proxy != NULL)
			proxy->m_collisionFilterGroup = mask;
	}
}

/**
 * \brief Gets the collision mask of the object.
 *
 * \param self Object.
 * \return Collision mask.
 */
int
liphy_object_get_collision_mask (const liphyObject* self)
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
liphy_object_set_collision_mask (liphyObject* self,
                                 int          mask)
{
	btBroadphaseProxy* proxy;

	self->config.collision_mask = mask;
	if (self->body != NULL)
	{
		proxy = self->body->getBroadphaseHandle ();
		if (proxy != NULL)
			proxy->m_collisionFilterMask = mask;
	}
	if (self->ghost != NULL)
	{
		proxy = self->ghost->getBroadphaseHandle ();
		if (proxy != NULL)
			proxy->m_collisionFilterMask = mask;
	}
}

/**
 * \brief Sets the contact handler callback of the object.
 *
 * The contact callback, when not NULL, is called every time the object
 * collides with something.
 *
 * \param self Object.
 * \return Simulation mode.
 */
void
liphy_object_set_contact_call (liphyObject*     self,
                               liphyContactCall value)
{
	self->config.contact_call = value;
	private_update_state (self);
}

/**
 * \brief Gets the simulation mode of the object.
 *
 * \param self Object.
 * \return Simulation mode.
 */
liphyControlMode
liphy_object_get_control_mode (const liphyObject* self)
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
liphy_object_set_control_mode (liphyObject*     self,
                               liphyControlMode value)
{
	if (self->control_mode == value)
		return;
	self->control_mode = value;
	self->flags |= PRIVATE_CHANGED_CONTROL_MODE;
	private_update_state (self);
}

/**
 * \brief Gets the physics engine for which this object was created.
 * 
 * \param self Object.
 * \return Physics engine.
 */
liphyPhysics*
liphy_object_get_engine (liphyObject* self)
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
liphy_object_get_gravity (const liphyObject* self,
                          limatVector*       value)
{
	btVector3 gravity;

	if (self->body != NULL)
	{
		gravity = self->body->getGravity ();
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
liphy_object_set_gravity (const liphyObject* self,
                          const limatVector* value)
{
	if (self->body != NULL)
		self->body->setGravity (btVector3 (value->x, value->y, value->z));
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
liphy_object_get_ground (const liphyObject* self)
{
	if (self->control_mode != LIPHY_CONTROL_MODE_CHARACTER)
		return 0;
	if (self->character_controller == NULL)
		return 0;
	return self->character_controller->onGround ();
}

/**
 * \brief Gets the inertia of the object.
 *
 * \param self Object.
 * \param result Return location for the inertia vector.
 */
void
liphy_object_get_inertia (liphyObject* self,
                          limatVector* result)
{
	if (self->shape != NULL)
		liphy_shape_get_inertia (self->shape, self->config.mass, result);
	else
		*result = limat_vector_init (0.0f, 0.0f, 0.0f);
}

/**
 * \brief Gets the mass of the object.
 *
 * \param self Object.
 * \return Mass.
 */
float
liphy_object_get_mass (const liphyObject* self)
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
liphy_object_set_mass (liphyObject* self,
                       float        value)
{
	limatVector v;
	btVector3 inertia(0.0, 0.0, 0.0);

	if (self->shape != NULL)
	{
		liphy_shape_get_inertia (self->shape, value, &v);
		inertia[0] = v.x;
		inertia[1] = v.y;
		inertia[2] = v.z;
	}
	if (self->body != NULL)
	{
		if (self->control_mode != LIPHY_CONTROL_MODE_STATIC)
			self->body->setMassProps (value, inertia);
	}
	self->config.mass = value;
}

/**
 * \brief Gets the movement force of the object.
 *
 * \param self Object.
 * \return Movement force.
 */
float
liphy_object_get_movement (const liphyObject* self)
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
liphy_object_set_movement (liphyObject* self,
                           float        value)
{
	int i;

	self->config.movement = value;
	if (self->control_mode == LIPHY_CONTROL_MODE_VEHICLE)
	{
		for (i = 0 ; i < self->vehicle->getNumWheels () ; i++)
		{
			btWheelInfo& wheel = self->vehicle->getWheelInfo (i);
			wheel.m_engineForce = value;
		}
	}
}

/**
 * \brief Returns nonzero if the object is a part of the physics simulation.
 *
 * \param self Object.
 * \return Nonzero if realized.
 */
int
liphy_object_get_realized (const liphyObject* self)
{
	return (self->flags & PRIVATE_ADDED_BODY) ||
	       (self->flags & PRIVATE_ADDED_GHOST) ||
	       (self->flags & PRIVATE_ADDED_VEHICLE);
}

/**
 * \brief Realizes or unrealizes the object.
 *
 * \param self Object.
 * \param value Nonzero if should realize.
 * \return Nonzero if realized.
 */
int
liphy_object_set_realized (liphyObject* self,
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
liphy_object_set_rotating (liphyObject* self,
                           float        value)
{
	if (self->body)
	{
		if (self->control_mode != LIPHY_CONTROL_MODE_STATIC)
			self->body->setAngularFactor (value);
	}
}

/**
 * \brief Gets the movement speed of the object.
 *
 * \param self Object.
 * \return Movement speed.
 */
float
liphy_object_get_speed (const liphyObject* self)
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
liphy_object_set_speed (liphyObject* self,
                        float        value)
{
	self->config.speed = value;
}

/**
 * \brief Gets the collision shape of the object.
 *
 * \param self Object.
 * \return Collision shape or NULL.
 */
const liphyShape*
liphy_object_get_shape (const liphyObject* self)
{
	return self->shape;
}

/**
 * \brief Sets the collision shape of the object.
 *
 * \param self Object.
 * \param shape Collision shape or NULL.
 * \param mode Collision shape mode.
 */
void
liphy_object_set_shape (liphyObject*   self,
                        liphyShape*    shape,
                        liphyShapeMode mode)
{
	if (self->shape == shape && self->shape_mode == mode)
		return;
	self->shape = shape;
	self->shape_mode = mode;
	self->flags |= PRIVATE_CHANGED_SHAPE;
	private_update_state (self);
}

/**
 * \brief Gets the collision shape mode of the object.
 *
 * \param self Object.
 * \return Shape mode.
 */
liphyShapeMode
liphy_object_get_shape_mode (const liphyObject* self)
{
	return self->shape_mode;
}

/**
 * \brief Sets the collision shape mode of the object.
 *
 * \param self Object.
 * \param value Shape mode.
 */
void
liphy_object_set_shape_mode (liphyObject*   self,
                             liphyShapeMode value)
{
	liphy_object_set_shape (self, self->shape, value);
}

/**
 * \brief Gets the world space transformation of the object.
 *
 * \param self Object.
 * \param value Return location for the transformation.
 */
void
liphy_object_get_transform (const liphyObject* self,
                            limatTransform*    value)
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
liphy_object_set_transform (liphyObject*          self,
                            const limatTransform* value)
{
	int i;
	btVector3 origin (value->position.x, value->position.y, value->position.z);
	btQuaternion rotation (value->rotation.x, value->rotation.y, value->rotation.z, value->rotation.w);
	btTransform transform (rotation, origin);

	self->motion->setWorldTransform (transform);
	if (self->flags & PRIVATE_ADDED_GHOST)
	{
		assert (self->ghost != NULL);
		self->ghost->setWorldTransform (transform);
	}
	if (self->flags & PRIVATE_ADDED_BODY)
	{
		assert (self->body != NULL);
		if (self->control_mode == LIPHY_CONTROL_MODE_STATIC)
		{
			self->physics->dynamics->removeRigidBody (self->body);
			self->body->setCenterOfMassTransform (transform);
			self->physics->dynamics->addRigidBody (self->body, self->config.collision_group, self->config.collision_mask);
		}
		else
			self->body->setCenterOfMassTransform (transform);
	}
	if (self->flags & PRIVATE_ADDED_VEHICLE)
	{
		assert (self->vehicle != NULL);
		self->body->setCenterOfMassTransform (transform);
		self->vehicle->resetSuspension ();
		for (i = 0 ; i < self->vehicle->getNumWheels () ; i++)
			self->vehicle->updateWheelTransform (i, true);
	}
}

/**
 * \brief Gets the pointer to external control function.
 *
 * \param self Object.
 * \return External control function or NULL.
 */
liphyCallback
liphy_object_get_usercall (liphyObject* self)
{
	if (self->custom_controller != NULL)
		return self->custom_controller->call;
	return NULL;
}

/**
 * \brief Sets or unsets the external control function of the object.
 *
 * The control function is called every internal tick and can be used for
 * implementing custom control schemes or tracking the state of the object.
 *
 * \param self Object.
 * \param value External control function or NULL.
 */
void
liphy_object_set_usercall (liphyObject*  self,
                           liphyCallback value)
{
	self->config.custom_call = value;
	self->flags |= PRIVATE_CHANGED_USERCALL;
	private_update_state (self);
}

/**
 * \brief Gets the user pointer stored to the object.
 *
 * \param self Object.
 * \return User pointer.
 */
void*
liphy_object_get_userdata (liphyObject* self)
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
liphy_object_set_userdata (liphyObject* self,
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
liphy_object_get_velocity (liphyObject* self,
                           limatVector* value)
{
	if (self->body != NULL)
	{
		const btVector3& velocity = self->body->getLinearVelocity ();
		self->config.velocity = limat_vector_init (velocity[0], velocity[1], velocity[2]);
	}
	*value = self->config.velocity;
}

/**
 * \brief Sets the linear velocity of the object.
 *
 * \param self Object.
 * \param value Linear velocity vector.
 */
void
liphy_object_set_velocity (liphyObject*       self,
                           const limatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);

	self->config.velocity = *value;
	if (self->body != NULL)
		self->body->setLinearVelocity (velocity);
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

static btCollisionShape*
private_choose_shape (const liphyObject* self,
                      liphyShape*        shape,
                      liphyShapeMode     mode)
{
	int fallback;
	btCollisionShape* btshape = NULL;

	if (shape != NULL)
	{
		fallback = 0;
		if (mode == LIPHY_SHAPE_MODE_CONCAVE)
		{
			btshape = shape->shapes.concave;
			fallback = (btshape == NULL);
		}
		if (fallback || mode == LIPHY_SHAPE_MODE_CONVEX)
		{
			btshape = shape->shapes.convex;
			fallback = (btshape == NULL);
		}
		if (fallback || mode == LIPHY_SHAPE_MODE_CAPSULE)
		{
			btshape = shape->shapes.capsule;
			fallback = (btshape == NULL);
		}
		if (fallback || mode == LIPHY_SHAPE_MODE_BOX)
		{
			btshape = shape->shapes.box;
			fallback = (btshape == NULL);
		}
	}

	return btshape;
}

static int
private_sweep_sphere (const liphyObject* self,
                      btConvexShape*     shape,
                      const btTransform& start,
                      const btTransform& end,
                      liphyCollision*    result)
{
	btTransform src;
	btTransform dst;
	btCollisionObject* object;
	btCollisionWorld* collision;

	/* Get own object. */
	if (self->body != NULL)
		object = self->body;
	else if (self->ghost != NULL)
		object = self->ghost;
	else
		return 0;
	if (self->shape == NULL)
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
		result->object = (liphyObject*) test.m_hitCollisionObject->getUserPointer ();
	else
		result->object = NULL;

	return result->fraction < 1.0f;
}

static float
private_sweep_shape (const liphyObject* self,
                     const btTransform& start,
                     const btVector3&   sweep,
                     btVector3*         normal)
{
	btTransform src;
	btTransform dst;
	btBroadphaseProxy* proxy;
	btCollisionObject* object;
	btCollisionWorld* collision;
	btConvexShape* shape;

	/* Get own object. */
	if (self->body != NULL)
	{
		object = self->body;
		proxy = self->body->getBroadphaseHandle ();
	}
	else if (self->ghost != NULL)
	{
		object = self->ghost;
		proxy = self->ghost->getBroadphaseHandle ();
	}
	else
		return 1.0f;
	if (proxy == NULL)
		return 1.0f;
	if (self->shape == NULL)
		return 1.0f;

	/* Initialize sweep. */
	PrivateConvexTest test (object);
	test.m_closestHitFraction = 1.0f;
	test.m_collisionFilterGroup = proxy->m_collisionFilterGroup;
	test.m_collisionFilterMask = proxy->m_collisionFilterMask;

	/* Sweep the shape. */
	collision = self->physics->dynamics->getCollisionWorld ();
	if (self->shape_mode != LIPHY_SHAPE_MODE_CONCAVE)
		shape = (btConvexShape*) private_choose_shape (self, self->shape, self->shape_mode);
	else
		shape = (btConvexShape*) private_choose_shape (self, self->shape, LIPHY_SHAPE_MODE_CONVEX);
	src = start;
	dst = src;
	dst.setOrigin (dst.getOrigin () + sweep);
	collision->convexSweepTest (shape, src, dst, test);
	if (normal != NULL)
		*normal = test.m_hitNormalWorld;

	return test.m_closestHitFraction;
}

static void
private_update_state (liphyObject* self)
{
	limatVector v;
	btCollisionShape* shape;

	if (self->control_mode != LIPHY_CONTROL_MODE_STATIC && self->shape_mode == LIPHY_SHAPE_MODE_CONCAVE)
		shape = private_choose_shape (self, self->shape, LIPHY_SHAPE_MODE_CONVEX);
	else
		shape = private_choose_shape (self, self->shape, self->shape_mode);

	/* Remove invalid components. */
	if ((self->flags & PRIVATE_CHANGED_CONTROL_MODE) ||
	    (self->flags & PRIVATE_CHANGED_SHAPE) ||
	    (self->flags & PRIVATE_CHANGED_USERCALL) ||
	   !(self->flags & PRIVATE_REALIZED) || !(self->shape != NULL))
	{
		self->flags &= ~(PRIVATE_CHANGED_CONTROL_MODE | PRIVATE_CHANGED_SHAPE | PRIVATE_CHANGED_USERCALL);
		if (self->flags & PRIVATE_ADDED_GHOST)
		{
			assert (self->ghost != NULL);
			self->physics->dynamics->removeCollisionObject (self->ghost);
			delete self->ghost;
			self->ghost = NULL;
			self->flags &= ~PRIVATE_ADDED_GHOST;
			PRIVATE_DEBUG_STATE (self, "ghost removed");
		}
		if (self->flags & PRIVATE_ADDED_CHARACTER_CONTROLLER)
		{
			assert (self->character_controller != NULL);
			self->physics->dynamics->removeAction (self->character_controller);
			delete self->character_controller;
			self->character_controller = NULL;
			self->flags &= ~PRIVATE_ADDED_CHARACTER_CONTROLLER;
			PRIVATE_DEBUG_STATE (self, "character controller removed");
		}
		if (self->flags & PRIVATE_ADDED_CONTACT_CONTROLLER)
		{
			assert (self->contact_controller != NULL);
			self->physics->dynamics->removeAction (self->contact_controller);
			delete self->contact_controller;
			self->contact_controller = NULL;
			self->flags &= ~PRIVATE_ADDED_CONTACT_CONTROLLER;
			PRIVATE_DEBUG_STATE (self, "contact controller removed");
		}
		if (self->flags & PRIVATE_ADDED_CUSTOM_CONTROLLER)
		{
			assert (self->custom_controller != NULL);
			self->physics->dynamics->removeAction (self->custom_controller);
			delete self->custom_controller;
			self->custom_controller = NULL;
			self->flags &= ~PRIVATE_ADDED_CUSTOM_CONTROLLER;
			PRIVATE_DEBUG_STATE (self, "custom controller removed");
		}
		if (self->flags & PRIVATE_ADDED_BODY)
		{
			assert (self->body != NULL);
			self->physics->dynamics->removeRigidBody (self->body);
			delete self->body;
			self->body = NULL;
			self->flags &= ~PRIVATE_ADDED_BODY;
			PRIVATE_DEBUG_STATE (self, "body removed");
		}
		if (self->flags & PRIVATE_ADDED_VEHICLE)
		{
			assert (self->vehicle != NULL);
			self->physics->dynamics->removeVehicle (self->vehicle);
			delete self->body;
			delete self->vehicle;
			delete self->vehicle_tuning;
			delete self->vehicle_caster;
			self->body = NULL;
			self->vehicle = NULL;
			self->vehicle_tuning = NULL;
			self->vehicle_caster = NULL;
			self->flags &= ~PRIVATE_ADDED_VEHICLE;
			PRIVATE_DEBUG_STATE (self, "vehicle removed");
		}
	}

	/* Create required components. */
	if ((self->flags & PRIVATE_REALIZED) && (self->shape != NULL))
	{
		if (!(self->flags & PRIVATE_ADDED_GHOST))
		{
			assert (self->ghost == NULL);
			if (self->config.contact_call != NULL || self->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
			{
				self->ghost = new btPairCachingGhostObject ();
				self->ghost->setUserPointer (self);
				self->ghost->setWorldTransform (self->motion->current);
				self->ghost->setCollisionShape (shape);
				self->physics->dynamics->addCollisionObject (self->ghost, self->config.collision_group, self->config.collision_mask);
				self->flags |= PRIVATE_ADDED_GHOST;
				PRIVATE_DEBUG_STATE (self, "ghost added");
			}
		}
		if (!(self->flags & PRIVATE_ADDED_CUSTOM_CONTROLLER))
		{
			assert (self->custom_controller == NULL);
			if (self->config.custom_call != NULL)
			{
				self->custom_controller = new liphyCustomController (self->config.custom_call, self);
				self->physics->dynamics->addAction (self->custom_controller);
				self->flags |= PRIVATE_ADDED_CUSTOM_CONTROLLER;
				PRIVATE_DEBUG_STATE (self, "custom controller added");
			}
		}
		if (!(self->flags & PRIVATE_ADDED_CONTACT_CONTROLLER))
		{
			assert (self->contact_controller == NULL);
			if (self->config.contact_call != NULL)
			{
				assert (self->ghost != NULL);
				self->contact_controller = new liphyContactController (self);
				self->physics->dynamics->addAction (self->contact_controller);
				self->flags |= PRIVATE_ADDED_CONTACT_CONTROLLER;
				PRIVATE_DEBUG_STATE (self, "contact controller added");
			}
		}
		if (!(self->flags & PRIVATE_ADDED_CHARACTER_CONTROLLER))
		{
			assert (self->character_controller == NULL);
			if (self->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
			{
				self->character_controller = new liphyCharacterController (self, (btConvexShape*) shape);
				self->physics->dynamics->addAction (self->character_controller);
				self->flags |= PRIVATE_ADDED_CHARACTER_CONTROLLER;
				PRIVATE_DEBUG_STATE (self, "character controller added");
			}
		}
		if (!(self->flags & PRIVATE_ADDED_BODY))
		{
			if (self->control_mode == LIPHY_CONTROL_MODE_STATIC)
			{
				assert (self->body == NULL);
				self->body = new btRigidBody (0.0, self->motion, shape, btVector3 (0.0, 0.0, 0.0));
				self->body->setUserPointer (self);
				self->physics->dynamics->addRigidBody (self->body, self->config.collision_group, self->config.collision_mask);
				self->flags |= PRIVATE_ADDED_BODY;
				PRIVATE_DEBUG_STATE (self, "static body added");
			}
			else if (self->control_mode == LIPHY_CONTROL_MODE_RIGID)
			{
				btVector3 angular (self->config.angular.x, self->config.angular.y, self->config.angular.z);
				btVector3 velocity (self->config.velocity.x, self->config.velocity.y, self->config.velocity.z);
				assert (self->body == NULL);
				liphy_object_get_inertia (self, &v);
				self->body = new btRigidBody (self->config.mass, self->motion, shape, btVector3 (v.x, v.y, v.z));
				self->body->setUserPointer (self);
				self->body->setLinearVelocity (velocity);
				self->body->setAngularVelocity (angular);
				self->body->setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
				self->physics->dynamics->addRigidBody (self->body, self->config.collision_group, self->config.collision_mask);
				self->flags |= PRIVATE_ADDED_BODY;
				PRIVATE_DEBUG_STATE (self, "rigid body added");
			}
		}
		if (!(self->flags & PRIVATE_ADDED_VEHICLE))
		{
			if (self->control_mode == LIPHY_CONTROL_MODE_VEHICLE)
			{
				btVector3 angular (self->config.angular.x, self->config.angular.y, self->config.angular.z);
				btVector3 velocity (self->config.velocity.x, self->config.velocity.y, self->config.velocity.z);
				assert (self->body == NULL);
				liphy_object_get_inertia (self, &v);
				self->body = new btRigidBody (self->config.mass, self->motion, shape, btVector3 (v.x, v.y, v.z));
				self->body->setUserPointer (self);
				self->body->setLinearVelocity (velocity);
				self->body->setAngularVelocity (angular);
				self->body->setActivationState (DISABLE_DEACTIVATION);
				self->body->setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
				self->vehicle_tuning = new btRaycastVehicle::btVehicleTuning ();
				self->vehicle_caster = new btDefaultVehicleRaycaster (self->physics->dynamics);
				self->vehicle = new btRaycastVehicle (*self->vehicle_tuning, self->body, self->vehicle_caster);
				self->vehicle->setCoordinateSystem (0, 1, 2);
				self->physics->dynamics->addVehicle (self->vehicle);
				self->flags |= PRIVATE_ADDED_VEHICLE;
				PRIVATE_DEBUG_STATE (self, "vehicle added");
			}
		}
	}

	/* Set ghost collision mode. */
	if (self->flags & PRIVATE_ADDED_GHOST)
	{
		if (self->flags & PRIVATE_ADDED_CHARACTER_CONTROLLER)
			self->ghost->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
		else
			self->ghost->setCollisionFlags (btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
}

/** @} */
/** @} */
