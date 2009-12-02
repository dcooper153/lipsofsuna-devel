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
#define LIPHY_BROADPHASE_DBVT

class liphyControl;
class liphyContactController;
class liphyCharacterController;
class liphyObjectShape;
class liphyMotionState;

struct _liphyPhysics
{
#ifdef LIPHY_BROADPHASE_DBVT
	btDbvtBroadphase* broadphase;
#else
	bt32BitAxisSweep3* broadphase;
#endif
	btDefaultCollisionConfiguration* configuration;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamics;
	btGhostPairCallback* ghostcallback;
	lialgList* constraints;
	lialgList* contacts;
	lialgList* contacts_iter;
	lialgList* controllers;
	liphyTransformCall transform_callback;
	void* userdata;
};

struct _liphyShape
{
	liphyPhysics* physics;
	const limdlModel* model;
	btConvexShape* shape;
};

struct _liphyObject
{
	liphyControlMode control_mode;
	liphyControl* control;
	liphyMotionState* motion;
	liphyPhysics* physics;
	btCompoundShape* shape;
	int flags;
	struct
	{
		int collision_group;
		int collision_mask;
		float mass;
		float movement;
		float speed;
		float strafing;
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

struct _liphyConstraint
{
	liphyPhysics* physics;
	liphyObject* object0;
	liphyObject* object1;
	btTypedConstraint* constraint;
};

struct liphyContactRecord
{
	float impulse;
	limatVector point;
	limatVector normal;
	liphyObject* object0;
	liphyObject* object1;
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

class liphyCharacterController : public btKinematicCharacterController
{
public:
	liphyCharacterController (liphyObject* object, btPairCachingGhostObject* ghost, btConvexShape* shape);
	virtual bool onGround () const;
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
public:
	liphyObject* object;
};

class liphyControl
{
public:
	liphyControl (liphyObject* object, btCollisionShape* shape);
	virtual ~liphyControl ();
public:
	virtual void apply_impulse (const btVector3& pos, const btVector3& imp);
	virtual void transform (const btTransform& value);
	virtual void update ();
	virtual void get_angular (btVector3* value);
	virtual void set_angular (const btVector3& value);
	virtual void set_collision_group (int mask);
	virtual void set_collision_mask (int mask);
	virtual void set_contacts (bool value);
	virtual void get_gravity (btVector3* value);
	virtual void set_gravity (const btVector3& value);
	virtual bool get_ground ();
	virtual void set_mass (float value, const btVector3& inertia);
	virtual btCollisionObject* get_object ();
	virtual void set_velocity (const btVector3& value);
public:
	liphyObject* object;
	liphyContactController* contact_controller;
};

class liphyCharacterControl : public liphyControl
{
public:
	liphyCharacterControl (liphyObject* object, btCollisionShape* shape);
	virtual ~liphyCharacterControl ();
public:
	virtual void transform (const btTransform& value);
	virtual void set_collision_group (int mask);
	virtual void set_collision_mask (int mask);
	virtual void set_contacts (bool value);
	virtual bool get_ground ();
	virtual btCollisionObject* get_object ();
public:
	btPairCachingGhostObject ghost;
	liphyCharacterController controller;
};

class liphyRigidControl : public liphyControl
{
public:
	liphyRigidControl (liphyObject* object, btCollisionShape* shape);
	virtual ~liphyRigidControl ();
public:
	virtual void apply_impulse (const btVector3& pos, const btVector3& imp);
	virtual void transform (const btTransform& value);
	virtual void update ();
	virtual void get_angular (btVector3* res);
	virtual void set_angular (const btVector3& value);
	virtual void set_collision_group (int mask);
	virtual void set_collision_mask (int mask);
	virtual void set_contacts (bool value);
	virtual void get_gravity (btVector3* value);
	virtual void set_gravity (const btVector3& value);
	virtual void set_mass (float value, const btVector3& inertia);
	virtual btCollisionObject* get_object ();
	virtual void set_velocity (const btVector3& value);
public:
	btRigidBody body;
};

class liphyStaticControl : public liphyControl
{
public:
	liphyStaticControl (liphyObject* object, btCollisionShape* shape);
	virtual ~liphyStaticControl ();
public:
	virtual void transform (const btTransform& value);
	virtual void set_contacts (bool value);
	virtual btCollisionObject* get_object ();
public:
	btRigidBody body;
};

class liphyVehicleControl : public liphyControl
{
public:
	liphyVehicleControl (liphyObject* object, btCollisionShape* shape);
	virtual ~liphyVehicleControl ();
public:
	virtual void transform (const btTransform& value);
	virtual void update ();
	virtual void set_collision_group (int mask);
	virtual void set_collision_mask (int mask);
	virtual void set_contacts (bool value);
	virtual void get_gravity (btVector3* value);
	virtual void set_gravity (const btVector3& value);
	virtual void set_mass (float value, const btVector3& inertia);
	virtual btCollisionObject* get_object ();
	virtual void set_velocity (const btVector3& value);
public:
	btRigidBody body;
	btRaycastVehicle* vehicle;
	btDefaultVehicleRaycaster caster;
	btRaycastVehicle::btVehicleTuning tuning;
};

#endif
