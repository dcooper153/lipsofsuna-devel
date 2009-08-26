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
 * \addtogroup liphyPrivate Private
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

#define PRIVATE_CCD_MOTION_THRESHOLD 1.0f

/*****************************************************************************/

liphyControl::liphyControl (liphyObject* object, btCollisionShape* shape) :
	object (object),
	contact_controller (NULL)
{
}

liphyControl::~liphyControl ()
{
}

void
liphyControl::apply_impulse (const btVector3& pos, const btVector3& imp)
{
}

void
liphyControl::transform (const btTransform& value)
{
}

void
liphyControl::update ()
{
}

void
liphyControl::get_angular (btVector3* value)
{
	*value = btVector3 (0.0, 0.0, 0.0);
}

void
liphyControl::set_angular (const btVector3& value)
{
}

void
liphyControl::set_collision_group (int mask)
{
}

void
liphyControl::set_collision_mask (int mask)
{
}

void
liphyControl::set_contacts (bool value)
{
}

void
liphyControl::get_gravity (btVector3* value)
{
	*value = btVector3 (0.0, 0.0, 0.0);
}

void
liphyControl::set_gravity (const btVector3& value)
{
}

bool
liphyControl::get_ground ()
{
	return false;
}

void
liphyControl::set_mass (float value, const btVector3& inertia)
{
}

btCollisionObject*
liphyControl::get_object ()
{
	return NULL;
}

void
liphyControl::set_velocity (const btVector3& value)
{
}

/*****************************************************************************/

liphyCharacterControl::liphyCharacterControl (liphyObject* object, btCollisionShape* shape) :
	liphyControl (object, shape),
	controller (object, &ghost, (btConvexShape*) shape)
{
	this->ghost.setUserPointer (object);
	this->ghost.setWorldTransform (this->object->motion->current);
	this->ghost.setCollisionShape (shape);
	this->ghost.setCollisionFlags (btCollisionObject::CF_CHARACTER_OBJECT);
	this->object->physics->dynamics->addCollisionObject (&this->ghost,
		this->object->config.collision_group,
		this->object->config.collision_mask);
	this->object->physics->dynamics->addAction (&this->controller);
}

liphyCharacterControl::~liphyCharacterControl ()
{
	this->object->physics->dynamics->removeAction (&this->controller);
	this->object->physics->dynamics->removeCollisionObject (&this->ghost);
}

void
liphyCharacterControl::transform (const btTransform& value)
{
	this->ghost.setWorldTransform (value);
}

void
liphyCharacterControl::set_collision_group (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->ghost.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
}

void
liphyCharacterControl::set_collision_mask (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->ghost.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
}

void
liphyCharacterControl::set_contacts (bool value)
{
	if (value)
	{
		this->ghost.setCollisionFlags (this->ghost.getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else
	{
		this->ghost.setCollisionFlags (this->ghost.getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

bool
liphyCharacterControl::get_ground ()
{
	return this->controller.onGround ();
}

btCollisionObject*
liphyCharacterControl::get_object ()
{
	return &this->ghost;
}

/*****************************************************************************/

liphyRigidControl::liphyRigidControl (liphyObject* object, btCollisionShape* shape) :
	liphyControl (object, shape),
	body (0.0, object->motion, shape, btVector3 (0.0, 0.0, 0.0))
{
	limatVector v;
	btVector3 angular (object->config.angular.x, object->config.angular.y, object->config.angular.z);
	btVector3 velocity (object->config.velocity.x, object->config.velocity.y, object->config.velocity.z);

	liphy_object_get_inertia (object, &v);
	this->body.setMassProps (object->config.mass, btVector3 (v.x, v.y, v.z));
	this->body.setUserPointer (object);
	this->body.setLinearVelocity (velocity);
	this->body.setAngularVelocity (angular);
	this->body.setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
	this->object->physics->dynamics->addRigidBody (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
}

liphyRigidControl::~liphyRigidControl ()
{
	this->object->physics->dynamics->removeRigidBody (&this->body);
}

void
liphyRigidControl::apply_impulse (const btVector3& pos, const btVector3& imp)
{
	this->body.applyImpulse (pos, imp);
}

void
liphyRigidControl::transform (const btTransform& value)
{
	this->body.setCenterOfMassTransform (value);
}

void
liphyRigidControl::update ()
{
	const btVector3& velocity = this->body.getLinearVelocity ();

	this->object->config.velocity = limat_vector_init (velocity[0], velocity[1], velocity[2]);
}

void
liphyRigidControl::get_angular (btVector3* value)
{
	*value = this->body.getAngularVelocity ();
}

void
liphyRigidControl::set_angular (const btVector3& value)
{
	this->body.setAngularVelocity (value);
}

void
liphyRigidControl::set_collision_group (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
}

void
liphyRigidControl::set_collision_mask (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
}

void
liphyRigidControl::set_contacts (bool value)
{
	if (value)
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

void
liphyRigidControl::get_gravity (btVector3* value)
{
	*value = this->body.getGravity ();
}

void
liphyRigidControl::set_gravity (const btVector3& value)
{
	this->body.setGravity (value);
}

void
liphyRigidControl::set_mass (float value, const btVector3& inertia)
{
	this->body.setMassProps (value, inertia);
}

btCollisionObject*
liphyRigidControl::get_object ()
{
	return &this->body;
}

void
liphyRigidControl::set_velocity (const btVector3& value)
{
	this->body.setLinearVelocity (value);
}

/*****************************************************************************/

liphyStaticControl::liphyStaticControl (liphyObject* object, btCollisionShape* shape) :
	liphyControl (object, shape),
	body (0.0, object->motion, shape, btVector3 (0.0, 0.0, 0.0))
{
	this->body.setUserPointer (object);
	this->object->physics->dynamics->addCollisionObject (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
}

liphyStaticControl::~liphyStaticControl ()
{
	this->object->physics->dynamics->removeCollisionObject (&this->body);
}

void
liphyStaticControl::transform (const btTransform& value)
{
	this->object->physics->dynamics->removeRigidBody (&this->body);
	this->body.setCenterOfMassTransform (value);
	this->object->physics->dynamics->addRigidBody (&this->body,
		this->object->config.collision_group,
		this->object->config.collision_mask);
}

void
liphyStaticControl::set_contacts (bool value)
{
	if (value)
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

btCollisionObject*
liphyStaticControl::get_object ()
{
	return &this->body;
}

/*****************************************************************************/

liphyVehicleControl::liphyVehicleControl (liphyObject* object, btCollisionShape* shape) :
	liphyControl (object, shape),
	body (0.0, object->motion, shape, btVector3 (0.0, 0.0, 0.0)),
	caster (object->physics->dynamics)
{
	limatVector v;
	btVector3 angular (object->config.angular.x, object->config.angular.y, object->config.angular.z);
	btVector3 velocity (object->config.velocity.x, object->config.velocity.y, object->config.velocity.z);

	liphy_object_get_inertia (object, &v);
	this->body.setMassProps (object->config.mass, btVector3 (v.x, v.y, v.z));
	this->body.setUserPointer (object);
	this->body.setLinearVelocity (velocity);
	this->body.setAngularVelocity (angular);
	this->body.setActivationState (DISABLE_DEACTIVATION);
	this->body.setCcdMotionThreshold (PRIVATE_CCD_MOTION_THRESHOLD);
	this->vehicle = new btRaycastVehicle (this->tuning, &this->body, &this->caster);
	this->vehicle->setCoordinateSystem (0, 1, 2);
	this->object->physics->dynamics->addVehicle (this->vehicle);
}

liphyVehicleControl::~liphyVehicleControl ()
{
	this->object->physics->dynamics->removeVehicle (this->vehicle);
	delete this->vehicle;
}

void
liphyVehicleControl::transform (const btTransform& value)
{
	int i;

	this->body.setCenterOfMassTransform (value);
	this->vehicle->resetSuspension ();
	for (i = 0 ; i < this->vehicle->getNumWheels () ; i++)
		this->vehicle->updateWheelTransform (i, true);
}

void
liphyVehicleControl::update ()
{
	const btVector3& velocity = this->body.getLinearVelocity ();

	this->object->config.velocity = limat_vector_init (velocity[0], velocity[1], velocity[2]);
}

void
liphyVehicleControl::set_collision_group (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterGroup = mask;
}

void
liphyVehicleControl::set_collision_mask (int mask)
{
	btBroadphaseProxy* proxy;

	proxy = this->body.getBroadphaseHandle ();
	if (proxy != NULL)
		proxy->m_collisionFilterMask = mask;
}

void
liphyVehicleControl::set_contacts (bool value)
{
	if (value)
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
	else
	{
		this->body.setCollisionFlags (this->body.getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

void
liphyVehicleControl::get_gravity (btVector3* value)
{
	*value = this->body.getGravity ();
}

void
liphyVehicleControl::set_gravity (const btVector3& value)
{
	this->body.setGravity (value);
}

void
liphyVehicleControl::set_mass (float value, const btVector3& inertia)
{
	this->body.setMassProps (value, inertia);
}

btCollisionObject*
liphyVehicleControl::get_object ()
{
	return &this->body;
}

void
liphyVehicleControl::set_velocity (const btVector3& value)
{
	this->body.setLinearVelocity (value);
}

/** @} */
/** @} */
