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

	this->current = transform;
	len0 = (this->current.getOrigin () - this->previous.getOrigin ()).length ();
	len1 = (this->current.getRotation () - this->previous.getRotation ()).length ();
	if (len0 > LIPHY_MOTION_TOLERANCE || len1 > LIPHY_ROTATION_TOLERANCE)
	{
		this->previous = this->current;
		if (this->object->physics->callbacks.transform != NULL)
			this->object->physics->callbacks.transform (this->object);
	}
}

/*****************************************************************************/

liphyCustomController::liphyCustomController (liphyCallback call, liphyObject* data)
{
	this->call = call;
	this->data = data;
}

void
liphyCustomController::updateAction (btCollisionWorld* world, btScalar delta)
{
	this->call (this->data, delta);
}

void
liphyCustomController::debugDraw (btIDebugDraw* debugDrawer)
{
}

/*****************************************************************************/

liphyCharacterController::liphyCharacterController (liphyObject* object, btConvexShape* shape) :
	btKinematicCharacterController (object->character.ghost, shape, object->character.step)
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
	limatVector* f;
	limatVector* v;
	btTransform transform = m_ghostObject->getWorldTransform ();
	btVector3 dir;
	btVector3 pos =
		transform * btVector3 (0.0f, 0.0f, 0.0f);
	btVector3 down =
		transform * btVector3 (0.0f, -1.0f, 0.0f) -
		transform * btVector3 (0.0f, 0.0f, 0.0f);
	btVector3 forward =
		transform * btVector3 (0.0f, 0.0f, 0.0f) -
		transform * btVector3 (0.0f, 0.0f, 1.0f);

	dir = forward * this->object->movement * this->object->speed * delta;
	if (this->object->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
	{
		/* v = v0+at = v0+tF/m */
		f = &this->object->character.force;
		v = &this->object->character.velocity;
		*v = limat_vector_add (this->object->character.velocity,
			limat_vector_multiply (*f, delta / this->object->mass));
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
