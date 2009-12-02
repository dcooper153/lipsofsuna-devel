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

liphyMotionState::liphyMotionState (liphyObject*       object,
                                    const btTransform& transform)
{
	this->object = object;
	this->current = transform;
	this->previous = transform;
}

void
liphyMotionState::getWorldTransform (btTransform& transform) const
{
	transform = this->current;
}

void
liphyMotionState::setWorldTransform (const btTransform& transform)
{
	float len0;
	float len1;

	assert (!isnan (transform.getOrigin ()[0]));
	assert (!isnan (transform.getOrigin ()[1]));
	assert (!isnan (transform.getOrigin ()[2]));

	this->current = transform;
	len0 = (this->current.getOrigin () - this->previous.getOrigin ()).length ();
	len1 = (this->current.getRotation () - this->previous.getRotation ()).length ();
	if (len0 > LIPHY_MOTION_TOLERANCE || len1 > LIPHY_ROTATION_TOLERANCE)
	{
		this->previous = this->current;
		if (this->object->control != NULL)
			this->object->control->update ();
		if (this->object->physics->transform_callback != NULL)
			this->object->physics->transform_callback (this->object);
	}
}

/*****************************************************************************/

liphyCharacterController::liphyCharacterController (liphyObject* object, btPairCachingGhostObject* ghost, btConvexShape* shape) :
	btKinematicCharacterController (ghost, shape, object->config.character_step)
{
	this->object = object;
}

bool
liphyCharacterController::onGround () const
{
	limatVector down = { 0.0f, -0.1f, 0.0f };

	return liphy_object_sweep (this->object, &down) < 1.0f;
}

void
liphyCharacterController::updateAction (btCollisionWorld* world, btScalar delta)
{
	float s;
	limatVector* f;
	limatVector* v;
	btTransform transform = m_ghostObject->getWorldTransform ();
	btVector3 dir;
	btVector3 pos = transform * btVector3 (0.0f, 0.0f, 0.0f);
	btVector3 down = transform * btVector3 (0.0f, -1.0f, 0.0f) - pos;
	btVector3 right = transform * btVector3 (1.0f, 0.0f, 0.0f) - pos;
	btVector3 forward = transform * btVector3 (0.0f, 0.0f, -1.0f) - pos;

	dir = forward * this->object->config.movement * this->object->config.speed * delta;
	dir += right * this->object->config.strafing * this->object->config.speed * delta;
	if (this->object->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
	{
		/* v = v0+at = v0+tF/m */
		s = delta / LI_MAX (0.1f, this->object->config.mass);
		f = &this->object->config.character_force;
		v = &this->object->config.velocity;
		*v = limat_vector_add (this->object->config.velocity, limat_vector_multiply (*f, s));
		/* x = vt */
		dir += delta * btVector3 (v->x, v->y, v->z);
		/* FIXME: Hardcoded decay. */
		*v = limat_vector_multiply (*v, 0.95f);
		*f = limat_vector_multiply (*f, 0.95f);
	}

	//dir = dir + btVector3 (0.0, 40.0 * delta, 0.0);
	setWalkDirection (dir);

	btKinematicCharacterController::updateAction (world, delta);

	transform = m_ghostObject->getWorldTransform ();
	this->object->motion->setWorldTransform (transform);
}

/** @} */
/** @} */
