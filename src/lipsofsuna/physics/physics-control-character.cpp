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
 * \addtogroup LIPhy Physics
 * @{
 * \addtogroup LIPhyPrivate Private
 * @{
 */

#include "physics-object.h"
#include "physics-private.h"

#define LIPHY_CHARACTER_FLIGHT_FACTOR 0.3f
#define LIPHY_CHARACTER_RISING_LIMIT 5.0f 
#define LIPHY_CHARACTER_GROUND_DAMPING 1.0f

LIPhyCharacterAction::LIPhyCharacterAction (
	LIPhyObject* object)
{
	this->object = object;
	this->timer = 0.0f;
	this->ground = 0;
}

void LIPhyCharacterAction::updateAction (
	btCollisionWorld* world, btScalar delta)
{
	int ground;
	float damp0;
	float damp1;
	float speed;
	btCollisionObject* object = this->object->control->get_object ();
	btTransform transform = object->getWorldTransform ();
	btVector3 pos = transform * btVector3 (0.0f, 0.0f, 0.0f);
	btVector3 down = transform * btVector3 (0.0f, -1.0f, 0.0f) - pos;
	btVector3 right = transform * btVector3 (1.0f, 0.0f, 0.0f) - pos;
	btVector3 forward = transform * btVector3 (0.0f, 0.0f, -1.0f) - pos;

	/* Check for ground. */
	this->timer += delta;
	if (this->timer >= 0.2f)
	{
		LIMatVector check0 = { pos[0], pos[1], pos[2] };
		LIMatVector check1 = { pos[0], pos[1] - 0.3f, pos[2] };
		ground = liphy_physics_cast_ray (this->object->physics, &check0, &check1,
			this->object->config.collision_group, this->object->config.collision_mask,
			&this->object, 1, NULL, NULL) < 1.0f;
		this->ground = ground;
		this->timer = 0.0f;
	}
	else
		ground = this->ground;

	/* Get velocity components. */
	btVector3 vel = ((btRigidBody*) object)->getLinearVelocity ();
	float dotx = vel.dot (right);
	float doty = vel.dot (-down);
	float dotz = vel.dot (-forward);
	btVector3 velx = dotx * right;
	btVector3 vely = doty * -down;
	btVector3 velz = dotz * -forward;

	/* Damp when moving upwards too fast. */
	/* Without this the player would shoot upwards from any slopes. */
	/* FIXME: Doesn't work for non-vertical gravity. */
	damp0 = 1.0f - LIMAT_CLAMP (vel[1], 0.0f, LIPHY_CHARACTER_RISING_LIMIT) /
		LIPHY_CHARACTER_RISING_LIMIT;

	/* Damp when not moving. */
	/* Without this the character would slide a lot after releasing controls. */
	damp1 = (1.0f - LIPHY_CHARACTER_GROUND_DAMPING) * delta;

	/* Walking. */
	speed = this->object->config.movement * this->object->config.speed;
	if (speed != 0.0f)
		velz = forward * speed * damp0;
	else if (ground)
		velz *= damp1;

	/* Strafing. */
	speed = this->object->config.strafing * this->object->config.speed;
	if (speed != 0.0f)
		velx = right * speed * damp0;
	else if (ground)
		velx *= damp1;

	/* Sum modified component velocities. */
	((btRigidBody*) object)->setLinearVelocity (velx + vely + velz);
}

void LIPhyCharacterAction::debugDraw (
	btIDebugDraw* debug)
{
}

LIPhyCharacterControl::LIPhyCharacterControl (LIPhyObject* object, btCollisionShape* shape) :
	LIPhyControlRigid (object, shape), action (object)
{
	object->physics->dynamics->addAction (&this->action);
	this->body.setActivationState (DISABLE_DEACTIVATION);
	this->body.setAngularFactor (0.0f);
}

LIPhyCharacterControl::~LIPhyCharacterControl ()
{
	object->physics->dynamics->removeAction (&this->action);
	this->body.setAngularFactor (1.0f);
}

bool LIPhyCharacterControl::get_ground ()
{
	return this->action.ground;
}

/** @} */
/** @} */
