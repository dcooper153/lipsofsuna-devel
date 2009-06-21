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

#ifndef __PHYSICS_PRIVATE_H__
#define __PHYSICS_PRIVATE_H__

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <algorithm/lips-algorithm.h>
#include <math/lips-math.h>
#include <model/lips-model.h>
#include "physics-types.h"

#define LIPHY_MOTION_TOLERANCE 0.25f
#define LIPHY_ROTATION_TOLERANCE 0.1f

class liphyMotionState;
class liphyCustomController;
class liphyContactController;
class liphyCharacterController;

struct _liphyPhysics
{
	bt32BitAxisSweep3* broadphase;
	btDefaultCollisionConfiguration* configuration;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamics;
	lialgList* controllers;
	liphyTransformCall transform_callback;
	void* userdata;
};

struct _liphyShape
{
	liphyPhysics* physics;
	const limdlModel* model;
	btTriangleIndexVertexArray* mesh;
	struct
	{
		int count;
		int* array;
	} indices;
	struct
	{
		int count;
		btScalar* array;
	} vertices;
	struct
	{
		btConvexShape* box;
		btMultiSphereShape* capsule;
		btConvexShape* convex;
		btBvhTriangleMeshShape* concave;
	} shapes;
};

struct _liphyObject
{
	liphyPhysics* physics;
	liphyShape* shape;
	liphyShapeMode shape_mode;
	liphyControlMode control_mode;
	liphyMotionState* motion;
	liphyCustomController* custom_controller;
	liphyContactController* contact_controller;
	btKinematicCharacterController* character_controller;
	btPairCachingGhostObject* ghost;
	btRigidBody* body;
	btRaycastVehicle* vehicle;
	btVehicleRaycaster* vehicle_caster;
	btRaycastVehicle::btVehicleTuning* vehicle_tuning;
	int flags;
	struct
	{
		int collision_group;
		int collision_mask;
		float mass;
		float movement;
		float speed;
		float character_step;
		void* userdata;
		limatTransform transform;
		limatVector angular;
		limatVector velocity;
		limatVector character_force; /* FIXME */
		liphyCallback custom_call;
		liphyContactCall contact_call;
	} config;
};

class liphyMotionState : public btMotionState
{
public:
	liphyMotionState (liphyObject* object, const btTransform& transform);
	virtual void getWorldTransform (btTransform& transform) const;
	virtual void setWorldTransform (const btTransform& transform);
public:
	liphyObject* object;
	btTransform current;
	btTransform previous;
};

class liphyCustomController : public btActionInterface
{
public:
	liphyCustomController (liphyCallback call, liphyObject* data);
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
	virtual void debugDraw (btIDebugDraw* debugDrawer);
public:
	liphyCallback call;
	liphyObject* data;
};

class liphyContactController : public btActionInterface
{
public:
	liphyContactController (liphyObject* data);
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
	virtual void debugDraw (btIDebugDraw* debugDrawer);
public:
	liphyObject* data;
};

class liphyCharacterController : public btKinematicCharacterController
{
public:
	liphyCharacterController (liphyObject* object, btConvexShape* shape);
	virtual bool onGround () const;
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
public:
	liphyObject* object;
};

#endif
