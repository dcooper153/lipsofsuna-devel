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
		if (this->object->physics->transform_callback != NULL)
			this->object->physics->transform_callback (this->object);
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

liphyContactController::liphyContactController (liphyObject* data)
{
	this->data = data;
	this->visited = lialg_ptrdic_new ();
}

liphyContactController::~liphyContactController ()
{
	lialg_ptrdic_free (this->visited);
}

void liphyContactController::updateAction (btCollisionWorld* world, btScalar delta)
{
	int i;
	int j;
	int p;
	int contacts;
	limatVector momentum0;
	limatVector momentum1;
	liphyObject* tmp;
	liphyContact contact;
	btManifoldArray manifolds;
	btBroadphasePair* pair;
	btCollisionObject* object;
	btPersistentManifold* manifold;
	btHashedOverlappingPairCache* cache = this->data->ghost->getOverlappingPairCache ();
	btBroadphasePairArray& pairarray = cache->getOverlappingPairArray ();
	btOverlappingPairCache* paircache = this->data->physics->dynamics->getPairCache ();

	lialg_ptrdic_clear (this->visited);

	/* Update ghost position. */
	tmp = this->data;
	if (tmp->body != NULL)
	{
		btTransform transform;
		tmp->motion->getWorldTransform (transform);
		tmp->ghost->setWorldTransform (transform);
	}

	/* Get own momentum. */
	liphy_object_get_velocity (tmp, &momentum0);
	momentum0 = limat_vector_multiply (momentum0, liphy_object_get_mass (tmp));

	/* Loop through collision pairs. */
	for (i = 0 ; i < pairarray.size () ; i++)
	{
		/* Get contact manifolds. */
		pair = paircache->findPair (pairarray[i].m_pProxy0, pairarray[i].m_pProxy1);
		if (pair == NULL || pair->m_algorithm == NULL)
			continue;
		manifolds.clear ();
		pair->m_algorithm->getAllContactManifolds (manifolds);

		/* Loop through all manifolds. */
		for (j = 0 ; j < manifolds.size () ; j++)
		{
			manifold = manifolds[j];
			contacts = manifold->getNumContacts ();
			if (!contacts)
				continue;

			/* Get pair object. */
			object = (btCollisionObject*) manifold->getBody0 ();
			if (object == tmp->ghost || object == tmp->body)
				object = (btCollisionObject*) manifold->getBody1 ();
			if (object == tmp->ghost || object == tmp->body)
				continue;
			contact.object = (liphyObject*) object->getUserPointer ();
			assert (contact.object == NULL || contact.object->physics == tmp->physics);

			/* Reject duplicates. */
			if (contact.object != NULL && lialg_ptrdic_find (this->visited, contact.object) != NULL)
				continue;
			lialg_ptrdic_insert (this->visited, contact.object, (void*) -1);

			/* Get pair momentum. */
			if (contact.object != NULL)
			{
				liphy_object_get_velocity (contact.object, &momentum1);
				momentum1 = limat_vector_multiply (momentum1, liphy_object_get_mass (contact.object));
			}
			else
				momentum1 = limat_vector_init (0.0f, 0.0f, 0.0f);

			/* Calculate total impulse. */
			/* FIXME: This is pretty sloppy. */
			contact.impulse = limat_vector_get_length (
				limat_vector_subtract (momentum0, momentum1));

			/* Just returning the first point should be good enough. */
			for (p = 0 ; p < contacts ; p++)
			{
				const btManifoldPoint& point = manifold->getContactPoint (p);
				if (point.getDistance () < 0.0f)
				{
					/* Invoke callback. */
					const btVector3& pt = point.getPositionWorldOnB ();
					const btVector3& nm = point.m_normalWorldOnB;
					contact.point = limat_vector_init (pt[0], pt[1], pt[2]);
					contact.normal = limat_vector_init (nm[0], nm[1], nm[2]);
					tmp->config.contact_call (tmp, &contact);

					/* Dangerous delete this condition here. */
					if (tmp->config.contact_call == NULL ||
					    tmp->contact_controller != this)
						return;
					break;
				}
			}
		}
	}
}

void liphyContactController::debugDraw (btIDebugDraw* debugDrawer)
{
}

/*****************************************************************************/

liphyCharacterController::liphyCharacterController (liphyObject* object, btConvexShape* shape) :
	btKinematicCharacterController (object->ghost, shape, object->config.character_step)
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

	dir = forward * this->object->config.movement * this->object->config.speed * delta;
	if (this->object->control_mode == LIPHY_CONTROL_MODE_CHARACTER)
	{
		/* v = v0+at = v0+tF/m */
		f = &this->object->config.character_force;
		v = &this->object->config.velocity;
		*v = limat_vector_add (this->object->config.velocity,
			limat_vector_multiply (*f, delta / this->object->config.mass));
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
