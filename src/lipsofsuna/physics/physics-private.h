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

#ifndef assert
#define assert(a)
#endif
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <lipsofsuna/algorithm.h>
#include <lipsofsuna/math.h>
#include <lipsofsuna/model.h>
#include "physics-types.h"

#define LIPHY_BROADPHASE_DBVT
#define PRIVATE_CCD_MOTION_THRESHOLD 1.0f

class LIPhyControl;
class LIPhyContactController;
class LIPhyCharacterAction;
class LIPhyObjectShape;
class LIPhyMotionState;

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
	LIPhyPhysics* physics;
	btCompoundShape* shape;
};

struct _LIPhyObject
{
	int flags;
	uint32_t id;
	LIPhyControlMode control_mode;
	LIPhyControl* control;
	LIPhyMotionState* motion;
	LIPhyPhysics* physics;
	LIPhyShape* shape;
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
		LIMatVector gravity;
		LIMatVector velocity;
		LIPhyCallback custom_call;
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

class LIPhyMotionState : public btMotionState
{
public:
	LIPhyMotionState (LIPhyObject* object, const btTransform& transform);
	virtual void getWorldTransform (btTransform& transform) const;
	virtual void setWorldTransform (const btTransform& transform);
public:
	LIPhyObject* object;
	btTransform current;
	btTransform previous;
};

class LIPhyCharacterAction : public btActionInterface
{
public:
	LIPhyCharacterAction (LIPhyObject* object);
	virtual void updateAction (btCollisionWorld* world, btScalar delta);
	virtual void debugDraw (btIDebugDraw* debug);
public:
	int ground;
	float timer;
	LIPhyObject* object;
};

class LIPhyControl
{
public:
	LIPhyControl (LIPhyObject* object, btCollisionShape* shape);
	virtual ~LIPhyControl ();
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

class LIPhyControlRigid : public LIPhyControl
{
public:
	LIPhyControlRigid (LIPhyObject* object, btCollisionShape* shape);
	virtual ~LIPhyControlRigid ();
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

class LIPhyCharacterControl : public LIPhyControlRigid
{
public:
	LIPhyCharacterControl (LIPhyObject* object, btCollisionShape* shape);
	virtual ~LIPhyCharacterControl ();
public:
	virtual bool get_ground ();
public:
	LIPhyCharacterAction action;
};

class LIPhyControlStatic : public LIPhyControl
{
public:
	LIPhyControlStatic (LIPhyObject* object, btCollisionShape* shape);
	virtual ~LIPhyControlStatic ();
public:
	virtual void transform (const btTransform& value);
	virtual void set_contacts (bool value);
	virtual btCollisionObject* get_object ();
public:
	btRigidBody body;
};

class LIPhyControlVehicle : public LIPhyControl
{
public:
	LIPhyControlVehicle (LIPhyObject* object, btCollisionShape* shape);
	virtual ~LIPhyControlVehicle ();
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
