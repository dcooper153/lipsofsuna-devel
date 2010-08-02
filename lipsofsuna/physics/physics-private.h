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

#ifndef __PHYSICS_PRIVATE_H__
#define __PHYSICS_PRIVATE_H__

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include "physics-types.h"

#define LIPHY_MOTION_TOLERANCE 0.25f
#define LIPHY_ROTATION_TOLERANCE 0.1f
#define LIPHY_BROADPHASE_DBVT

class liphyControl;
class LIPhyContactController;
class liphyCharacterController;
class LIPhyObjectShape;
class liphyMotionState;

struct _LIPhyPhysics
{
	void* userdata;
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
	LIAlgList* constraints;
	LIAlgList* contacts;
	LIAlgList* contacts_iter;
	LIAlgList* controllers;
	LIAlgU32dic* objects;
	LICalCallbacks* callbacks;
};

struct _LIPhyShape
{
	int refs;
	LIPhyPhysics* physics;
	btConvexShape* shape;
};

struct _LIPhyObject
{
	int flags;
	uint32_t id;
	LIPhyControlMode control_mode;
	liphyControl* control;
	liphyMotionState* motion;
	LIPhyPhysics* physics;
	btCompoundShape* shape;
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
		LIMatVector angular;
		LIMatVector velocity;
		liphyCallback custom_call;
		LIPhyContactCall contact_call;
	} config;
};

struct _LIPhyConstraint
{
	LIPhyPhysics* physics;
	LIPhyObject* object0;
	LIPhyObject* object1;
	btTypedConstraint* constraint;
};

struct LIPhyContactRecord
{
	float impulse;
	LIMatVector point;
	LIMatVector normal;
	LIPhyObject* object0;
	LIPhyObject* object1;
};

class liphyMotionState : public btMotionState
{
public:
	liphyMotionState (LIPhyObject* object, const btTransform& transform);
	virtual void getWorldTransform (btTransform& transform) const;
	virtual void setWorldTransform (const btTransform& transform);
public:
	LIPhyObject* object;
	btTransform current;
	btTransform previous;
};

class liphyCharacterController : public btActionInterface
{
public:
	liphyCharacterController (LIPhyObject* object);
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
	virtual void debugDraw (btIDebugDraw* debug);
public:
	int ground;
	LIPhyObject* object;
};

class liphyControl
{
public:
	liphyControl (LIPhyObject* object, btCollisionShape* shape);
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
	virtual void get_velocity (btVector3* value);
	virtual void set_velocity (const btVector3& value);
public:
	LIPhyObject* object;
	LIPhyContactController* contact_controller;
};

class liphyRigidControl : public liphyControl
{
public:
	liphyRigidControl (LIPhyObject* object, btCollisionShape* shape);
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
	virtual void get_velocity (btVector3* value);
	virtual void set_velocity (const btVector3& value);
public:
	btRigidBody body;
};

class liphyCharacterControl : public liphyRigidControl
{
public:
	liphyCharacterControl (LIPhyObject* object, btCollisionShape* shape);
	virtual ~liphyCharacterControl ();
public:
	virtual bool get_ground ();
public:
	liphyCharacterController controller;
};

class liphyStaticControl : public liphyControl
{
public:
	liphyStaticControl (LIPhyObject* object, btCollisionShape* shape);
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
	liphyVehicleControl (LIPhyObject* object, btCollisionShape* shape);
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
