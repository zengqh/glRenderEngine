//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/09/26
// File: enn_physics_world.h
//

#pragma once

#include "enn_collision_shape.h"
#include "enn_constraint.h"
#include "enn_rigid_body.h"

#include <LinearMath/btIDebugDraw.h>

class btCollisionConfiguration;
class btBroadphaseInterface;
class btConstraintSolver;
class btDiscreteDynamicsWorld;
class btDispatcher;
class btDynamicsWorld;
class btPersistentManifold;

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct PhysicsRaycastResult
{
	Vector3			position_;
	Vector3			normal_;
	float			distance_;
	RigidBody*		body_;
};

struct CollisionGeometryData;
class SceneManager;
//////////////////////////////////////////////////////////////////////////
class PhysicsWorld : public AllocatedObject, public btIDebugDraw
{

	friend void InternalPreTickCallback(btDynamicsWorld *world, btScalar timeStep);
	friend void InternalTickCallback(btDynamicsWorld *world, btScalar timeStep);

public:
	typedef enn::vector<RigidBody*>::type						RigidBodies;
	typedef enn::vector<CollisionShape*>::type					CollisionShapes;
	typedef enn::vector<Constraint*>::type						Constraints;
	typedef std::pair<RigidBody*, RigidBody*>					RigidPair;
	typedef enn::map<RigidPair, btPersistentManifold*>::type	RigidPairMap;
	typedef enn::map<String, CollisionGeometryData*>::type		GeometryCacheMap;

public:
	PhysicsWorld();
	virtual ~PhysicsWorld();

public:
	// Check if an AABB is visible for debug drawing.
	virtual bool isVisible(const btVector3& aabbMin, const btVector3& aabbMax);
	// Draw a physics debug line.
	virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	// Log warning from the physics engine.
	virtual void reportErrorWarning(const char* warningString);
	// Draw a physics debug contact point. Not implemented.
	virtual void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color) {}
	// Draw physics debug 3D text. Not implemented.
	virtual void draw3dText(const btVector3& location,const char* textString) {}
	// Set debug draw flags.
	virtual void setDebugMode(int debugMode) { debugMode_ = debugMode; }
	// Return debug draw flags.
	virtual int getDebugMode() const { return debugMode_; }


public:
	void update(float timeStep);
	void updateCollisions();
	void setFPS(int fps);
	void setGravity(const Vector3& gravity);
	void setNumIterations(int num);
	void setInterpolation(bool enable);

	void raycast(enn::vector<PhysicsRaycastResult>::type& result, const Ray& ray, 
		float maxDistance, uint32 collisionMask = Math::ENN_MAX_UNSIGNED);
	void raycastSingle(PhysicsRaycastResult& result, const Ray& ray,
		float maxDistance, uint32 collisionMask = Math::ENN_MAX_UNSIGNED);

	void sphereCast(PhysicsRaycastResult& result, const Ray& ray, float radius,
		float maxDistance, uint32 collisionMask = Math::ENN_MAX_UNSIGNED);

	void getRigidBodies(enn::vector<RigidBody*>::type& result, const Sphere& sphere,
		unsigned collisionMask = Math::ENN_MAX_UNSIGNED);

	void getRigidBodies(enn::vector<RigidBody*>::type& result, const AxisAlignedBox& box,
		uint32 collisionMask = Math::ENN_MAX_UNSIGNED);
	void getRigidBodies(enn::vector<RigidBody*>::type& result, const RigidBody* body);

	Vector3 getGravity() const;

	int getNumIterations() const;

	bool getInterpolation() const;

	int getFPS() const;

	void addRigidBody(RigidBody* body);

	void removeRigidBody(RigidBody* body);

	void addCollisionShape(CollisionShape* shape);

	void removeCollisionShape(CollisionShape* shape);

	void addConstraint(Constraint* joint);

	void removeConstraint(Constraint* joint);

	btDiscreteDynamicsWorld* getWorld() const;

public:
	GeometryCacheMap& getGeometryCache();
	void cleanGeometryCache();

public:
	virtual void setNode(SceneNode* node);

protected:
	void preStep(float timeStep);
	void postStep(float timeStep);

protected:
	btCollisionConfiguration*		collision_configuration_;
	btDispatcher*					collision_dispatcher_;
	btBroadphaseInterface*			broadphase_;
	btConstraintSolver*				solver_;
	btDiscreteDynamicsWorld*		world_;
	RigidBodies						rigid_bodies_;
	CollisionShapes					collision_shapes_;
	Constraints						constraints_;
	RigidPairMap					current_collisions_;
	RigidPairMap					previous_collisions_;
	uint32							fps_;
	float							time_acc_;
	bool							interpolation_;
	GeometryCacheMap				geometry_cache_map_;
	SceneManager*					scene_mgr_;
	int								debugMode_;
};

}