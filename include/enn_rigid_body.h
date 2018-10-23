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
// File: enn_rigid_body.h
//

#pragma once

#include <LinearMath/btMotionState.h>
#include "enn_collision_shape.h"

class btCompoundShape;
class btRigidBody;

namespace enn
{
	
class CollisionShape;
class Constraint;
class PhysicsWorld;

// Rigid body collision event signaling mode
enum CollisionEventMode
{
	COLLISION_NEVER = 0,
	COLLISION_ACTIVE,
	COLLISION_ALWAYS
};


//////////////////////////////////////////////////////////////////////////
// RigidBody
class RigidBody : public AllocatedObject, public btMotionState
{
public:
	RigidBody();
	virtual ~RigidBody();

	virtual void getWorldTransform(btTransform& worldTrans) const;
	virtual void setWorldTransform(const btTransform& worldTrans);

	void setMass(float mass);

	void setPosition(const Vector3& position);
	void setRotation(const Quaternion& rotation);
	void setTransform(const Vector3& position, const Quaternion& rotation);

	void setLinearVelocity(const Vector3& velocity);
	void setLinearFactor(const Vector3& factor);
	void setLinearRestThreshold(float threshold);
	void setLinearDamping(float damping);

	void setAngularVelocity(const Vector3& angularVelocity);
	void setAngularFactor(const Vector3& factor);
	void setAngularRestThreshold(float threshold);
	void setAngularDamping(float factor);

	void setFriction(float friction);
	void setRollingFriction(float friction);
	void setRestitution(float restitution);
	void setContactProcessingThreshold(float threshold);
	void setCcdRadius(float radius);
	void setCcdMotionThreshold(float threshold);
	void setUseGravity(bool enable);
	void setGravityOverride(const Vector3& gravity);
	void setKinematic(bool enable);
	void setPhantom(bool enable);

	void setCollisionLayer(uint32 layer);
	void setCollisionMask(uint32 mask);
	void setCollisionLayerAndMask(uint32 layer, uint32 mask);
	void setCollisionEventMode(CollisionEventMode mode);
	
	void applyForce(const Vector3& force);
	void applyForce(const Vector3& force, const Vector3& position);
	void applyTorque(const Vector3& torque);
	void applyImpulse(const Vector3& impulse);
	void applyImpulse(const Vector3& impulse, const Vector3& position);
	void applyTorqueImpulse(const Vector3& torque);
	void resetForces();

	void activate();
	void reAddBodyToWorld();

	PhysicsWorld* getPhysicsWorld() const;
	btRigidBody* getBody() const;
	btCompoundShape* getCompoundShape() const;
	float getMass() const;
	Vector3 getPosition() const;
	Quaternion  getRotation() const;

	Vector3 getLinearVelocity() const;
	Vector3 getLinearFactor() const;
	Vector3 getVelocityAtPoint(const Vector3& position) const;
	float getLinearRestThreshold() const;
	float getLinearDamping() const;

	Vector3 getAngularVelocity() const;
	Vector3 getAngularFactor() const;
	float getAngularRestThreshold() const;
	float getAngularDamping() const;
	float getFricition() const;
	float getRollingFriction() const;
	float getRestitution() const;
	float getContactProcessingThreshold() const;
	float getCcdRadius() const;
	float getCcdMotionThreshold() const;

	bool getUseGravity() const;
	const Vector3& getGravityOverride() const;
	const Vector3& getCenterOfMass() const;
	bool isKinematic() const;
	bool isPhantom() const;
	bool isActive() const;
	
	uint32 getCollisionLayer() const;
	uint32 getCollisionMask() const;
	CollisionEventMode getCollisionEvent() const;

	void getCollidingBodies(enn::vector<RigidBody*>::type& result) const;
	void applyWorldTransform(const Vector3& newWorldPosition, 
		const Quaternion& newWorldRotation);
	void updateMass();
	void updateGravity();

	void addConstraint(Constraint* constraint);
	void removeConstraiant(Constraint* constraint);
	void releaseBody();

public:
	void setNode(SceneNode* node);
	SceneNode* getNode() const;

protected:
	void addBodyToWorld();
	void removeBodyFromWorld();

protected:
	btRigidBody*						body_;
	btCompoundShape*					compoundShape_;
	btCompoundShape*					shfitedCompoundShape_;
	PhysicsWorld*						physics_world_;
	enn::vector<Constraint*>::type		constraints_;
	Vector3								gravity_override_;
	Vector3								center_of_mass_;
	float								mass_;
	uint32								collision_layer_;
	uint32								collision_mask_;
	CollisionEventMode					collision_event_mode_;
	mutable Vector3						last_position_;
	mutable Quaternion					last_rotation_;
	bool								kinematic_;
	bool								phantom_;
	bool								use_gravity_;
	bool								readd_body_;
	bool								in_world_;

	SceneNode*							node_;
};

}