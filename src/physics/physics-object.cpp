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

#define LIPHY_OBJECT_VERSION 0

static btCollisionShape*
private_choose_shape (const liphyObject* self,
                      liphyShape*        shape,
                      liphyShapeMode     mode);

static void
private_mode_clear (liphyObject* self);

static void
private_mode_enable (liphyObject* self);

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
private_update_userdata (liphyObject* self);

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
	btScalar mass = 10.0f;
	btVector3 inertia (0.0f, 0.0f, 0.0f);
	btVector3 position (0.0f, 0.0f, 0.0f);
	btQuaternion orientation (0.0f, 0.0f, 0.0f, 1.0f);

	self = (liphyObject*) calloc (1, sizeof (liphyObject));
	if (self == NULL)
		return NULL;
	self->control_mode = control_mode;
	self->speed = LIPHY_DEFAULT_SPEED;
	try
	{
		self->collision.group = LIPHY_DEFAULT_COLLISION_GROUP;
		self->collision.mask = LIPHY_DEFAULT_COLLISION_MASK;
		self->mass = mass;
		self->physics = physics;
		self->motion = new liphyMotionState (self, btTransform (orientation, position));
	}
	catch (...)
	{
		liphy_object_free (self);
		return NULL;
	}
	private_mode_enable (self);
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
	private_mode_clear (self);
	delete self->controller;
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

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			self->character.force = limat_vector_add (self->character.force, *impulse);
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->applyImpulse (v0, v1);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			self->rigid.body->applyImpulse (v0, v1);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->applyImpulse (v0, v1);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
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
	if (self->control_mode != LIPHY_CONTROL_MODE_VEHICLE)
		return;
	btVector3 bpoint (point->x, point->y, point->z);
	btVector3 bdir (0.0f, -1.0f, 0.0f);
	btVector3 baxle (axle->x, axle->y, axle->z);
	btWheelInfo& wheel = self->vehicle.vehicle->addWheel (
		bpoint, bdir, baxle, susplen, radius, *self->vehicle.tuning, turning);
	/* FIXME: Should be configurable. */
	wheel.m_suspensionStiffness = 20.0f;
	wheel.m_wheelsDampingRelaxation = 2.3f;
	wheel.m_wheelsDampingCompression = 4.4f;
	wheel.m_frictionSlip = 1000.0f;
	wheel.m_rollInfluence = 0.1f;
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
		v = self->character.velocity;
		v = limat_vector_add (v, limat_vector_multiply (*impulse, 1.0f / self->mass));
		self->character.velocity = v;
	}
}

/**
 * \brief Deserializes the object from a stream.
 * 
 * \param self Object.
 * \param reader Reader.
 * \return Nonzero on success.
 */
int
liphy_object_read (liphyObject* self,
                   liReader*    reader)
{
	float mass;
	float movement;
	float speed;
	float step;
	uint8_t version;
	uint8_t realized;
	uint32_t collision_group;
	uint32_t collision_mask; 
	uint32_t control_mode;
	uint32_t shape_mode;
	limatTransform transform;
	limatVector angular;
	limatVector velocity;

	if (!li_reader_get_uint8 (reader, &version))
		return 0;
	if (version != LIPHY_OBJECT_VERSION)
	{
		lisys_error_set (EINVAL, "incorrect physics object version");
		return 0;
	}
	if (!li_reader_get_uint8 (reader, &realized) ||
	    !li_reader_get_uint32 (reader, &collision_group) ||
	    !li_reader_get_uint32 (reader, &collision_mask) ||
	    !li_reader_get_uint32 (reader, &control_mode) ||
	    !li_reader_get_uint32 (reader, &shape_mode) ||
	    !li_reader_get_float (reader, &mass) ||
	    !li_reader_get_float (reader, &movement) ||
	    !li_reader_get_float (reader, &speed) ||
	    !li_reader_get_float (reader, &transform.position.x) ||
	    !li_reader_get_float (reader, &transform.position.y) ||
	    !li_reader_get_float (reader, &transform.position.z) ||
	    !li_reader_get_float (reader, &transform.rotation.x) ||
	    !li_reader_get_float (reader, &transform.rotation.y) ||
	    !li_reader_get_float (reader, &transform.rotation.z) ||
	    !li_reader_get_float (reader, &transform.rotation.w) ||
	    !li_reader_get_float (reader, &velocity.x) ||
	    !li_reader_get_float (reader, &velocity.y) ||
	    !li_reader_get_float (reader, &velocity.z) ||
	    !li_reader_get_float (reader, &angular.x) ||
	    !li_reader_get_float (reader, &angular.y) ||
	    !li_reader_get_float (reader, &angular.z) ||
	    !li_reader_get_float (reader, &step))
		return 0;
	if (control_mode > LIPHY_CONTROL_MODE_MAX)
	{
		lisys_error_set (EINVAL, "invalid control mode");
		return 0;
	}
	if (shape_mode > LIPHY_SHAPE_MODE_MAX)
	{
		lisys_error_set (EINVAL, "invalid shape mode");
		return 0;
	}
	if (mass < 0.0f)
	{
		lisys_error_set (EINVAL, "invalid mass");
		return 0;
	}
	if (speed < 0.0f)
	{
		lisys_error_set (EINVAL, "invalid speed");
		return 0;
	}
	if (step < 0.0f)
	{
		lisys_error_set (EINVAL, "invalid character step");
		return 0;
	}
	liphy_object_set_mass (self, mass);
	liphy_object_set_collision_mask (self, collision_mask);
	liphy_object_set_collision_group (self, collision_group);
	liphy_object_set_control_mode (self, (liphyControlMode) control_mode);
	liphy_object_set_shape_mode (self, (liphyShapeMode) shape_mode);
	liphy_object_set_movement (self, movement);
	liphy_object_set_speed (self, speed);
	liphy_object_set_transform (self, &transform);
	liphy_object_set_velocity (self, &velocity);
	liphy_object_set_angular_momentum (self, &angular);
	if (control_mode == LIPHY_CONTROL_MODE_CHARACTER)
		self->character.step = step;
	/* FIXME: Can we do this safely? */
	liphy_object_set_realized (self, realized);

	return 1;
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
 * \brief Serializes the object to a stream.
 * 
 * \param self Object.
 * \param writer Writer.
 * \return Nonzero on success.
 */
int
liphy_object_write (liphyObject* self,
                    liarcWriter* writer)
{
	limatTransform transform;
	limatVector vector;

	liarc_writer_append_uint8 (writer, LIPHY_OBJECT_VERSION);
	liarc_writer_append_uint8 (writer, self->realized);
	liarc_writer_append_uint32 (writer, self->collision.group);
	liarc_writer_append_uint32 (writer, self->collision.mask);
	liarc_writer_append_uint32 (writer, self->control_mode);
	liarc_writer_append_uint32 (writer, self->shape_mode);
	liarc_writer_append_float (writer, self->mass);
	liarc_writer_append_float (writer, self->movement);
	liarc_writer_append_float (writer, self->speed);
	liphy_object_get_transform (self, &transform);
	liarc_writer_append_float (writer, transform.position.x);
	liarc_writer_append_float (writer, transform.position.y);
	liarc_writer_append_float (writer, transform.position.z);
	liarc_writer_append_float (writer, transform.rotation.x);
	liarc_writer_append_float (writer, transform.rotation.y);
	liarc_writer_append_float (writer, transform.rotation.z);
	liarc_writer_append_float (writer, transform.rotation.w);
	liphy_object_get_velocity (self, &vector);
	liarc_writer_append_float (writer, vector.x);
	liarc_writer_append_float (writer, vector.y);
	liarc_writer_append_float (writer, vector.z);
	liphy_object_get_angular_momentum (self, &vector);
	liarc_writer_append_float (writer, vector.x);
	liarc_writer_append_float (writer, vector.y);
	liarc_writer_append_float (writer, vector.z);
	if (self->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
		liarc_writer_append_float (writer, self->character.step);
	else
		liarc_writer_append_float (writer, 0.0f);

	return !writer->error;
}

/**
 * \brief Gets the angular momentum of the object.
 *
 * \param self Object.
 * \param value Return location for the angular momentum vector.
 */
void
liphy_object_get_angular_momentum (const liphyObject* self,
                                   limatVector*       value)
{
	btVector3 velocity;
	
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			velocity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			velocity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			velocity = self->rigid.body->getAngularVelocity ();
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			velocity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			velocity = self->vehicle.body->getAngularVelocity ();
			break;
		default:
			velocity = btVector3 (0.0, 0.0, 0.0);
			assert (0);
			break;
	}
	value->x = velocity[0];
	value->y = velocity[1];
	value->z = velocity[2];
}

/**
 * \brief Sets the angular momentum of the object.
 *
 * \param self Object.
 * \param value Angular momentum vector.
 */
void
liphy_object_set_angular_momentum (liphyObject*       self,
                                   const limatVector* value)
{
	btVector3 velocity (value->x, value->y, value->z);
	
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->setAngularVelocity (velocity);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setAngularVelocity (velocity);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
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
	return self->collision.group;
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

	self->collision.group = mask;
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			proxy = NULL;
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			proxy = self->character.ghost->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			proxy = self->rigid.body->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			proxy = self->vehicle.body->getBroadphaseHandle ();
			break;
		default:
			proxy = NULL;
			assert (0);
			break;
	}
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
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
	return self->collision.mask;
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

	self->collision.mask = mask;
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			proxy = NULL;
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			proxy = self->character.ghost->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			proxy = self->rigid.body->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			proxy = self->vehicle.body->getBroadphaseHandle ();
			break;
		default:
			proxy = NULL;
			assert (0);
			break;
	}
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
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
	private_mode_clear (self);
	self->control_mode = value;
	private_mode_enable (self);
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

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			gravity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			gravity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			gravity = self->rigid.body->getGravity ();
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			gravity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			gravity = self->vehicle.body->getGravity ();
			break;
		default:
			gravity = btVector3 (0.0, 0.0, 0.0);
			assert (0);
			break;
	}
	*value = limat_vector_init (gravity[0], gravity[1], gravity[2]);
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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->setGravity (btVector3 (value->x, value->y, value->z));
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setGravity (btVector3 (value->x, value->y, value->z));
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
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
	if (self->character.controller == NULL)
		return 0;
	return self->character.controller->onGround ();
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
		liphy_shape_get_inertia (self->shape, self->mass, result);
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
	return self->mass;
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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->setMassProps (value, inertia);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setMassProps (value, inertia);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
	self->mass = value;
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
	return self->movement;
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

	self->movement = value;
	if (self->control_mode == LIPHY_CONTROL_MODE_VEHICLE)
	{
		for (i = 0 ; i < self->vehicle.vehicle->getNumWheels () ; i++)
		{
			btWheelInfo& wheel = self->vehicle.vehicle->getWheelInfo (i);
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
	return self->realized;
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
	if (self->realized == value)
		return 1;
	if (value)
	{
		switch (self->control_mode)
		{
			case LIPHY_CONTROL_MODE_NONE:
				break;
			case LIPHY_CONTROL_MODE_CHARACTER:
				if (self->character.ghost->getCollisionShape () != NULL)
				{
					self->physics->dynamics->addCollisionObject (self->character.ghost, self->collision.group, self->collision.mask);
					self->character.controller = new liphyCharacterController (self,
						(btConvexShape*) private_choose_shape (self, self->shape, self->shape_mode));
					self->physics->dynamics->addAction (self->character.controller);
				}
				break;
			case LIPHY_CONTROL_MODE_RIGID:
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
				break;
			case LIPHY_CONTROL_MODE_STATIC:
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
				break;
			case LIPHY_CONTROL_MODE_VEHICLE:
				self->physics->dynamics->addVehicle (self->vehicle.vehicle);
				break;
			case LIPHY_CONTROL_MODE_MAX:
				assert (0);
				return 0;
		}
	}
	else
	{
		switch (self->control_mode)
		{
			case LIPHY_CONTROL_MODE_NONE:
				break;
			case LIPHY_CONTROL_MODE_CHARACTER:
				if (self->character.ghost != NULL)
					self->physics->dynamics->removeCollisionObject (self->character.ghost);
				if (self->character.controller != NULL)
				{
					self->physics->dynamics->removeAction (self->character.controller);
					delete self->character.controller;
					self->character.controller = NULL;
				}
				break;
			case LIPHY_CONTROL_MODE_RIGID:
			case LIPHY_CONTROL_MODE_STATIC:
				self->physics->dynamics->removeRigidBody (self->rigid.body);
				break;
			case LIPHY_CONTROL_MODE_VEHICLE:
				self->physics->dynamics->removeVehicle (self->vehicle.vehicle);
				break;
			case LIPHY_CONTROL_MODE_MAX:
				assert (0);
				return 0;
		}
	}
	self->realized = value;

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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->setAngularFactor (value);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setAngularFactor (value);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
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
	return self->speed;
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
	self->speed = value;
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
	int added;
	limatVector inertia;
	btCollisionShape* btshape;

	/* Choose collision shape. */
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_CHARACTER:
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_VEHICLE:
			if (mode == LIPHY_SHAPE_MODE_CONCAVE)
				mode = LIPHY_SHAPE_MODE_CONVEX;
			break;
		case LIPHY_CONTROL_MODE_NONE:
		case LIPHY_CONTROL_MODE_STATIC:
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
	btshape = private_choose_shape (self, shape, mode);

	/* Apply the shape to the object. */
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			if (self->character.controller != NULL)
			{
				self->physics->dynamics->removeAction (self->character.controller);
				delete self->character.controller;
				self->character.controller = NULL;
			}
			if (btshape != NULL)
			{
				added = (self->character.ghost->getCollisionShape () != NULL);
				self->character.ghost->setCollisionShape (btshape);
				if (self->realized)
				{
					if (!added)
					{
						self->physics->dynamics->addCollisionObject (
							self->character.ghost, self->collision.group, self->collision.mask);
					}
					self->character.controller = new liphyCharacterController (self,
						(btConvexShape*) private_choose_shape (self, shape, mode));
					self->physics->dynamics->addAction (self->character.controller);
				}
			}
			else
				self->character.ghost->setCollisionShape (NULL);
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			if (btshape != NULL)
			{
				liphy_shape_get_inertia (shape, self->mass, &inertia);
				self->rigid.body->setCollisionShape (btshape);
				self->rigid.body->setMassProps (self->mass, btVector3 (inertia.x, inertia.y, inertia.z));
			}
			else
			{
				self->rigid.body->setCollisionShape (NULL);
				self->rigid.body->setMassProps (self->mass, btVector3 (0.0f, 0.0f, 0.0f));
			}
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			if (self->realized)
				self->physics->dynamics->removeRigidBody (self->rigid.body);
			if (btshape != NULL)
				self->rigid.body->setCollisionShape (btshape);
			else
				self->rigid.body->setCollisionShape (NULL);
			if (self->realized)
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			if (btshape != NULL)
			{
				liphy_shape_get_inertia (shape, self->mass, &inertia);
				self->vehicle.body->setCollisionShape (btshape);
				self->vehicle.body->setMassProps (self->mass, btVector3 (inertia.x, inertia.y, inertia.z));
			}
			else
			{
				self->vehicle.body->setCollisionShape (NULL);
				self->vehicle.body->setMassProps (self->mass, btVector3 (0.0f, 0.0f, 0.0f));
			}
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
	self->shape = shape;
	self->shape_mode = mode;
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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			self->character.ghost->setWorldTransform (transform);
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			self->rigid.body->setCenterOfMassTransform (transform);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			if (self->realized)
				self->physics->dynamics->removeRigidBody (self->rigid.body);
			self->rigid.body->setCenterOfMassTransform (transform);
			if (self->realized)
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setCenterOfMassTransform (transform);
			self->vehicle.vehicle->resetSuspension ();
			for (i = 0 ; i < self->vehicle.vehicle->getNumWheels () ; i++)
				self->vehicle.vehicle->updateWheelTransform (i, true);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
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
	if (self->controller != NULL)
		return self->controller->call;
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
	if (self->controller != NULL)
	{
		self->physics->dynamics->removeAction (self->controller);
		delete self->controller;
		self->controller = NULL;
	}
	if (value != NULL)
	{
		self->controller = new liphyCustomController (value, self);
		self->physics->dynamics->addAction (self->controller);
	}
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
	return self->userdata;
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
	self->userdata = value;
	private_update_userdata (self);
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
	btVector3 velocity;

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			velocity = btVector3 (0.0, 0.0, 0.0);
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			*value = self->character.velocity;
			return;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			velocity = self->rigid.body->getLinearVelocity ();
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			velocity = self->vehicle.body->getLinearVelocity ();
			break;
		default:
			velocity = btVector3 (0.0, 0.0, 0.0);
			assert (0);
			break;
	}
	value->x = velocity[0];
	value->y = velocity[1];
	value->z = velocity[2];
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

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			self->character.velocity = *value;
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			self->rigid.body->setLinearVelocity (velocity);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			self->vehicle.body->setLinearVelocity (velocity);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
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

static void
private_mode_clear (liphyObject* self)
{
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			if (self->realized)
			{
				self->physics->dynamics->removeCollisionObject (self->character.ghost);
				if (self->character.controller != NULL)
					self->physics->dynamics->removeAction (self->character.controller);
			}
			delete self->character.ghost;
			delete self->character.controller;
			self->character.ghost = NULL;
			self->character.controller = NULL;
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			if (self->realized)
				self->physics->dynamics->removeRigidBody (self->rigid.body);
			delete self->rigid.body;
			self->rigid.body = NULL;
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			/* FIXME */
			if (self->realized)
				self->physics->dynamics->removeVehicle (self->vehicle.vehicle);
			delete self->vehicle.body;
			delete self->vehicle.vehicle;
			delete self->vehicle.caster;
			delete self->vehicle.tuning;
			self->vehicle.vehicle = NULL;
			self->vehicle.caster = NULL;
			self->vehicle.tuning = NULL;
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
}

static void
private_mode_enable (liphyObject* self)
{
	limatVector v;
	btCollisionShape* shape;

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			break;
		case LIPHY_CONTROL_MODE_CHARACTER:
			shape = private_choose_shape (self, self->shape, self->shape_mode);
			self->character.step = 0.35;
			self->character.ghost = new btPairCachingGhostObject ();
			self->character.ghost->setWorldTransform (self->motion->current);
			self->character.ghost->setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
			self->character.ghost->setCollisionShape (shape);
			self->character.controller = NULL;
			if (self->realized)
			{
				self->physics->dynamics->addCollisionObject (self->character.ghost, self->collision.group, self->collision.mask);
				if (shape != NULL)
				{
					self->character.controller = new liphyCharacterController (self,
						(btConvexShape*) private_choose_shape (self, self->shape, self->shape_mode));
					self->physics->dynamics->addAction (self->character.controller);
				}
			}
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			liphy_object_get_inertia (self, &v);
			self->rigid.body = new btRigidBody (self->mass, self->motion, NULL, btVector3 (v.x, v.y, v.z));
			if (self->realized)
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			self->rigid.body = new btRigidBody (0.0, self->motion, NULL, btVector3 (0.0, 0.0, 0.0));
			if (self->realized)
				self->physics->dynamics->addRigidBody (self->rigid.body, self->collision.group, self->collision.mask);
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			liphy_object_get_inertia (self, &v);
			self->vehicle.body = new btRigidBody (self->mass, self->motion, NULL, btVector3 (v.x, v.y, v.z));
			self->vehicle.body->setActivationState (DISABLE_DEACTIVATION);
			self->vehicle.tuning = new btRaycastVehicle::btVehicleTuning ();
			self->vehicle.caster = new btDefaultVehicleRaycaster (self->physics->dynamics);
			self->vehicle.vehicle = new btRaycastVehicle (*self->vehicle.tuning, self->vehicle.body, self->vehicle.caster);
			self->vehicle.vehicle->setCoordinateSystem (0, 1, 2);
#if 0
			/* FIXME! */
			limatVector a0 = { -1.0f, 0.0f, 0.0f };
			limatVector a1 = { 1.0f, 0.0f, 0.0f };
			limatVector p0 = { -2.0f, 0.0f, -2.0f };
			limatVector p1 = { 2.0f, 0.0f, -2.0f };
			limatVector p2 = { -2.0f, 0.0f, 2.0f };
			limatVector p3 = { 2.0f, 0.0f, 2.0f };
			liphy_object_insert_wheel (self, &p0, &a0, 0.5f, 0.6f, 0);
			liphy_object_insert_wheel (self, &p1, &a1, 0.5f, 0.6f, 0);
			liphy_object_insert_wheel (self, &p2, &a0, 0.5f, 0.6f, 1);
			liphy_object_insert_wheel (self, &p3, &a1, 0.5f, 0.6f, 1);
#endif
			if (self->realized)
				self->physics->dynamics->addVehicle (self->vehicle.vehicle);
			break;
		case LIPHY_CONTROL_MODE_MAX:
			assert (0);
			return;
	}
	private_update_userdata (self);
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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			return 1.0f;
		case LIPHY_CONTROL_MODE_CHARACTER:
			object = self->character.ghost;
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			object = self->rigid.body;
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			object = self->vehicle.body;
			break;
		default:
			assert (0);
			object = NULL;
			break;
	}
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
		result->object = test.m_hitCollisionObject->getUserPointer ();
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
	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			return 1.0f;
		case LIPHY_CONTROL_MODE_CHARACTER:
			object = self->character.ghost;
			proxy = self->character.ghost->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_RIGID:
			object = self->rigid.body;
			proxy = self->rigid.body->getBroadphaseHandle ();
			break;
		case LIPHY_CONTROL_MODE_STATIC:
			return 0.0f;
		case LIPHY_CONTROL_MODE_VEHICLE:
			object = self->vehicle.body;
			proxy = self->vehicle.body->getBroadphaseHandle ();
			break;
		default:
			assert (0);
			object = NULL;
			proxy = NULL;
			break;
	}
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
private_update_userdata (liphyObject* self)
{
	btCollisionObject* object;

	switch (self->control_mode)
	{
		case LIPHY_CONTROL_MODE_NONE:
			return;
		case LIPHY_CONTROL_MODE_CHARACTER:
			object = self->character.ghost;
			break;
		case LIPHY_CONTROL_MODE_RIGID:
		case LIPHY_CONTROL_MODE_STATIC:
			object = self->rigid.body;
			break;
		case LIPHY_CONTROL_MODE_VEHICLE:
			object = self->vehicle.body;
			break;
		default:
			assert (0);
			return;
	}
	if (object != NULL)
		object->setUserPointer (self->userdata);
}

/** @} */
/** @} */
