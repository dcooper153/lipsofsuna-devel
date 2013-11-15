/* Lips of Suna
 * Copyright© 2007-2013 Lips of Suna development team.
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
 * \addtogroup LIPhyPhysics Physics
 * @{
 */

#include "lipsofsuna/system.h"
#include "lipsofsuna/extension/physics-terrain/physics-terrain.hpp"
#include "physics.h"
#include "physics-collision-configuration.hpp"
#include "physics-constraint.h"
#include "physics-model.h"
#include "physics-object.h"
#include "physics-private.h"
#include "physics-raycast-hook.hpp"

#define MAXPROXIES 1000000

static bool private_contact_processed (
	btManifoldPoint& point,
	void*            body0,
	void*            body1);

static void private_internal_tick (
	btDynamicsWorld* dynamics,
	btScalar         step);

/*****************************************************************************/

/**
 * \brief Creates a new physics simulation.
 * \param callbacks Callback manager.
 * \return New physics simulation or NULL.
 */
LIPhyPhysics* liphy_physics_new (
	LICalCallbacks* callbacks)
{
	LIPhyPhysics* self;
	btVector3 vmin (-65535, -65535, -65535);
	btVector3 vmax ( 65535,  65535,  65535);

	/* Allocate self. */
	self = (LIPhyPhysics*) lisys_calloc (1, sizeof (LIPhyPhysics));
	if (self == NULL)
		return NULL;
	self->callbacks = callbacks;

	/* Allocation dictionaries. */
	self->models = lialg_u32dic_new ();
	if (self->models == NULL)
	{
		liphy_physics_free (self);
		return NULL;
	}
	self->objects = lialg_ptrdic_new ();
	if (self->objects == NULL)
	{
		liphy_physics_free (self);
		return NULL;
	}

	/* Create simulation. */
	try
	{
		self->ghostcallback = new btGhostPairCallback ();
#ifdef LIPHY_BROADPHASE_DBVT
		self->broadphase = new btDbvtBroadphase ();
#else
		self->broadphase = new bt32BitAxisSweep3 (vmin, vmax, MAXPROXIES);
#endif
		self->broadphase->getOverlappingPairCache ()->setInternalGhostPairCallback (self->ghostcallback);
		self->configuration = new LIPhyCollisionConfiguration ();
		self->dispatcher = new btCollisionDispatcher (self->configuration);
		self->solver = new btSequentialImpulseConstraintSolver ();
		self->dynamics = new LIPhyDynamicsWorld (self->dispatcher, self->broadphase, self->solver, self->configuration);
		self->dynamics->setInternalTickCallback (private_internal_tick, self);
	}
	catch (...)
	{
		liphy_physics_free (self);
		return NULL;
	}

	extern ContactProcessedCallback gContactProcessedCallback;
	gContactProcessedCallback = private_contact_processed;

	extern btScalar gDeactivationTime;
	gDeactivationTime = 1.0;

	return self;
}

/**
 * \brief Frees the physics simulation.
 * \param self Physics simulation.
 */
void liphy_physics_free (
	LIPhyPhysics* self)
{
	LIAlgPtrdicIter iter0;
	LIAlgU32dicIter iter1;

	lisys_assert (self->constraints == NULL);

	if (self->objects != NULL)
	{
		LIALG_PTRDIC_FOREACH (iter0, self->objects)
			liphy_object_free ((LIPhyObject*) iter0.value);
	}
	if (self->models != NULL)
	{
		LIALG_U32DIC_FOREACH (iter1, self->models)
			liphy_model_free ((LIPhyModel*) iter1.value);
	}

	delete self->dynamics;
	delete self->solver;
	delete self->configuration;
	delete self->dispatcher;
	delete self->broadphase;
	delete self->ghostcallback;
	lialg_u32dic_free (self->models);
	lialg_ptrdic_free (self->objects);
	lialg_list_free (self->controllers);
	lisys_free (self);
}

/**
 * \brief Adds a raycast hook.
 * \param self Physics.
 * \param hook Raycast hook object.
 */
void liphy_physics_add_raycast_hook (
	LIPhyPhysics* self,
	void*         hook)
{
	LIPhyRaycastHook* h = (LIPhyRaycastHook*) hook;

	h->next = self->dynamics->raycast_hooks;
	self->dynamics->raycast_hooks = h;
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
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignored objects.
 * \param result Return location for collision data or NULL.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_ray (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyContact*       result)
{
	btVector3 src (start->x, start->y, start->z);
	btVector3 dst (end->x, end->y, end->z);

	/* Cast the ray. */
	LIPhyRayResultCallback test (ignore_array, ignore_count, src, dst);
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;
	self->dynamics->rayTest (src, dst, test);
	if (test.m_closestHitFraction >= 1.0f)
		return 0;

	/* Copy the results. */
	if (result != NULL)
		*result = test.result;

	return 1;
}

/**
 * \brief Casts a shape to the scene.
 *
 * If the shape hits no obstacle, result is set to the cast end point, normal is
 * set to cast direction, and zero is returned. Otherwise, nonzero is returned
 * and result and normal hold information on the collision point.
 *
 * \param self Physics simulation.
 * \param start Cast start transform.
 * \param end Cast end transform.
 * \param shape Cast shape.
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignore objects.
 * \param result Return location for collision data.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_shape (
	const LIPhyPhysics*   self,
	const LIMatTransform* start,
	const LIMatTransform* end,
	const LIPhyShape*     shape,
	int                   group,
	int                   mask,
	LIPhyObject**         ignore_array,
	int                   ignore_count,
	LIPhyContact*         result)
{
	int i;
	float best;
	btConvexShape* btshape;
	LIPhyConvexResultCallback test (ignore_array, ignore_count);
	btTransform btstart (
		btQuaternion (start->rotation.x, start->rotation.y, start->rotation.z, start->rotation.w),
		btVector3 (start->position.x, start->position.y, start->position.z));
	btTransform btend (
		btQuaternion (end->rotation.x, end->rotation.y, end->rotation.z, end->rotation.w),
		btVector3 (end->position.x, end->position.y, end->position.z));

	/* Initialize sweep. */
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;
	result->fraction = best = 1.0f;

	/* Sweep the shape. */
	for (i = 0 ; i < shape->shape->getNumChildShapes () ; i++)
	{
		btshape = (btConvexShape*) shape->shape->getChildShape (i);
		self->dynamics->convexSweepTest (btshape, btstart, btend, test);
		if (test.m_closestHitFraction <= best && test.m_hitCollisionObject != NULL)
		{
			best = test.m_closestHitFraction;
			*result = test.result;
		}
	}

	return result->fraction < 1.0f;
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
 * \param group Collision group.
 * \param mask Collision mask.
 * \param ignore_array Array of ignored objects.
 * \param ignore_count Number of ignore objects.
 * \param result Return location for collision data.
 * \return Nonzero if a collision occurred.
 */
int liphy_physics_cast_sphere (
	const LIPhyPhysics* self,
	const LIMatVector*  start,
	const LIMatVector*  end,
	float               radius,
	int                 group,
	int                 mask,
	LIPhyObject**       ignore_array,
	int                 ignore_count,
	LIPhyContact*       result)
{
	btTransform btstart (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (start->x, start->y, start->z));
	btTransform btend (btQuaternion (0.0, 0.0, 0.0, 1.0), btVector3 (end->x, end->y, end->z));

	/* Initialize sweep. */
	LIPhyConvexResultCallback test (ignore_array, ignore_count);
	test.m_collisionFilterGroup = group;
	test.m_collisionFilterMask = mask;

	/* Sweep the shape. */
	self->dynamics->sphereSweepTest (radius, btstart, btend, test);
	if (test.m_hitCollisionObject != NULL && test.m_closestHitFraction < 1.0f)
	{
		*result = test.result;
		return 1;
	}

	return 0;
}

/**
 * \brief Clears all constraints affecting the object.
 *
 * Used by the object class to remove invalid constraints.
 *
 * \param self Physics.
 * \param object Object.
 */
void liphy_physics_clear_constraints (
		LIPhyPhysics* self,
		LIPhyObject*  object)
{
	LIAlgList* ptr;
	LIAlgList* next;
	LIPhyConstraint* constraint;

	for (ptr = self->constraints ; ptr != NULL ; ptr = next)
	{
		next = ptr->next;
		constraint = (LIPhyConstraint*) ptr->data;
		if (constraint->object0 == object || constraint->object1 == object)
		{
			lialg_list_remove (&self->constraints, ptr);
			liphy_constraint_free (constraint);
		}
	}
}

/**
 * \brief Finds a physics model by ID.
 * \param self Physics simulation.
 * \param id Model ID.
 * \return Model or NULL.
 */
LIPhyModel* liphy_physics_find_model (
	LIPhyPhysics* self,
	uint32_t      id)
{
	return (LIPhyModel*) lialg_u32dic_find (self->models, id);
}

/**
 * \brief Removes the model from all objects.
 * \param self Physics simulation.
 * \param model Physics model.
 */
void liphy_physics_remove_model (
	LIPhyPhysics* self,
	LIPhyModel*   model)
{
	LIAlgPtrdicIter iter;
	LIPhyObject* object;

	LIALG_PTRDIC_FOREACH (iter, self->objects)
	{
		object = (LIPhyObject*) iter.value;
		if (object->model == model)
			liphy_object_set_model (object, NULL);
	}
}

/**
 * \brief Removes a raycast hook.
 * \param self Physics.
 * \param hook Raycast hook object.
 */
void liphy_physics_remove_raycast_hook (
	LIPhyPhysics* self,
	void*         hook)
{
	LIPhyRaycastHook* h = (LIPhyRaycastHook*) hook;
	LIPhyRaycastHook* ptr;

	if (h == self->dynamics->raycast_hooks)
	{
		self->dynamics->raycast_hooks = h->next;
		return;
	}
	for (ptr = self->dynamics->raycast_hooks ; ptr != NULL ; ptr = ptr->next)
	{
		if (ptr->next == h)
		{
			ptr->next = h->next;
			return;
		}
	}
	lisys_assert (0);
}

/**
 * \brief Updates the physics simulation.
 * \param self Physics simulation.
 * \param secs Tick length in seconds.
 */
void liphy_physics_update (
	LIPhyPhysics* self,
	float         secs)
{
	/* Step simulation. */
	self->updating = 1;
	self->dynamics->stepSimulation (secs, 20);
	self->updating = 0;
}

/**
 * \brief Gets the userdata of the physics engine.
 * \param self Physics simulation.
 * \return Userdata.
 */
void* liphy_physics_get_userdata (
	LIPhyPhysics* self)
{
	return self->userdata;
}

/**
 * \brief Sets the userdata of the physics engine.
 * \param self Physics simulation.
 * \param data Userdata.
 */
void liphy_physics_set_userdata (
	LIPhyPhysics* self,
	void*         data)
{
	self->userdata = data;
}

/**
 * \brief Gets the physics engine version.
 * \param self Physics simulation.
 * \return Version number.
 */
float liphy_physics_get_version (
	LIPhyPhysics* self)
{
	return BT_BULLET_VERSION / 100.0f + 0.005;
}

/*****************************************************************************/

static bool private_contact_processed (
	btManifoldPoint& point,
	void*            body0,
	void*            body1)
{
	int tmp;
	LIMatVector momentum0;
	LIMatVector momentum1;
	LIPhyObject* object0 = NULL;
	LIPhyObject* object1 = NULL;
	LIPhyPhysics* physics = NULL;
	LIPhyPointer* pointer0;
	LIPhyPointer* pointer1;
	LIPhyContact contact;

	/* Get the userdata pointers. */
	/* These contain information on the types of the bodies. */
	pointer0 = (LIPhyPointer*)((btCollisionObject*) body0)->getUserPointer ();
	pointer1 = (LIPhyPointer*)((btCollisionObject*) body1)->getUserPointer ();

	/* Make sure that the contact involved at least one object with a contact
	   callback. Otherwise, we don't need to do anything abou this contact. */
	if ((pointer0 == NULL || pointer0->type != LIPHY_POINTER_TYPE_OBJECT || !((LIPhyObject*) pointer0->pointer)->config.contact_events) &&
	    (pointer1 == NULL || pointer1->type != LIPHY_POINTER_TYPE_OBJECT || !((LIPhyObject*) pointer1->pointer)->config.contact_events))
		return false;

	/* Get contact information. */
	/* If the userdata is NULL, the body was a heightmap. Otherwise, was
	   either an object or terrain depending on which one is set in the
	   struct of the pointer. */
	liphy_contact_init (&contact);
	if (pointer0 != NULL)
	{
		if (pointer0->type == LIPHY_POINTER_TYPE_OBJECT)
		{
			object0 = (LIPhyObject*) pointer0->pointer;
			contact.object_id = liphy_object_get_external_id (object0);
			physics = object0->physics;
		}
		else
		{
			lisys_assert (pointer0->type == LIPHY_POINTER_TYPE_TERRAIN);
			contact.terrain_id = pointer0->id;
			liext_physics_terrain_get_column_by_object (
				(LIExtPhysicsTerrain*) pointer0->pointer,
				body0,
				point.m_index0,
				contact.terrain_tile);
		}
	}
	if (pointer1 != NULL)
	{
		if (pointer1->type == LIPHY_POINTER_TYPE_OBJECT)
		{
			if (object0 != NULL)
			{
				object1 = (LIPhyObject*) pointer1->pointer;
				contact.object1_id = liphy_object_get_external_id (object1);
				physics = object1->physics;
			}
			else
			{
				object0 = (LIPhyObject*) pointer1->pointer;
				contact.object_id = liphy_object_get_external_id (object0);
				physics = object0->physics;
			}
		}
		else
		{
			lisys_assert (pointer1->type == LIPHY_POINTER_TYPE_TERRAIN);
			contact.terrain_id = pointer1->id;
			liext_physics_terrain_get_column_by_object (
				(LIExtPhysicsTerrain*) pointer1->pointer,
				body1,
				point.m_index1,
				contact.terrain_tile);
		}
	}

	/* Get collision point. */
	const btVector3& pt = point.getPositionWorldOnB ();
	const btVector3& nm = point.m_normalWorldOnB;
	contact.point = limat_vector_init (pt[0], pt[1], pt[2]);
	contact.normal = limat_vector_init (nm[0], nm[1], nm[2]);

	/* Calculate impulse. */
	/* FIXME: This is sloppy. */
	if (object0 != NULL)
	{
		liphy_object_get_velocity (object0, &momentum0);
		momentum0 = limat_vector_multiply (momentum0, liphy_object_get_mass (object0));
	}
	else
		momentum1 = limat_vector_init (0.0f, 0.0f, 0.0f);
	if (object1 != NULL)
	{
		liphy_object_get_velocity (object1, &momentum1);
		momentum1 = limat_vector_multiply (momentum1, liphy_object_get_mass (object1));
	}
	else
		momentum1 = limat_vector_init (0.0f, 0.0f, 0.0f);
	contact.impulse = limat_vector_get_length (limat_vector_subtract (momentum0, momentum1));

	/* Invoke callbacks. */
	if (object0 != NULL && object0->config.contact_events)
		lical_callbacks_call (physics->callbacks, "object-contact", lical_marshal_DATA_PTR, &contact);
	if (object1 != NULL && object1->config.contact_events)
	{
		tmp = contact.object_id;
		contact.object_id = contact.object1_id;
		contact.object1_id = tmp;
		lical_callbacks_call (physics->callbacks, "object-contact", lical_marshal_DATA_PTR, &contact);
	}

	return false;
}

static void private_internal_tick (
	btDynamicsWorld* dynamics,
	btScalar         step)
{
}

/** @} */
/** @} */
