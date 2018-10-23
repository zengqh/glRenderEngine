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
// Time: 2013/09/27
// File: enn_constraint.h
//

#pragma once

#include "enn_physics_utils.h"
#include "enn_scene_node.h"

class btTypedConstraint;

namespace enn
{

enum ConstraintType
{
	CONSTRAINT_POINT = 0,
	CONSTRAINT_HINGE,
	CONSTRAINT_SLIDER,
	CONSTRAINT_CONETWIST
};

class PhysicsWorld;
class RigidBody;

//////////////////////////////////////////////////////////////////////////
class Constraint : public AllocatedObject
{
	friend class RigidBody;
public:
	Constraint();
	virtual ~Constraint();

public:
	virtual void getDependencyNodes(enn::vector<SceneNode*>::type& dest);

	void setConstraintType(ConstraintType type);
	void setOtherBody(RigidBody* body);
	
	void setPostion(const Vector3& position);
	void setRotation(const Quaternion& quat);
	void setAxis(const Vector3& axis);
	void setOtherPosition(const Vector3& position);
	void setOtherRotation(const Quaternion& rotation);
	void setOtherAxis(const Vector3& axis);
	void setWorldPosition(const Vector3& position);
	void SetHighLimit(const Vector2& limit);
	void setLowLimit(const Vector2& limit);

	// Set constraint error reduction parameter. Zero = leave to default.
	void setERP(float erp);

	// Set constraint force mixing parameter. Zero = leave to default.
	void setCFM(float cfm);

	// Set whether to disable collisions between connected bodies.
	void setDisableCollision(bool disable);

	PhysicsWorld* getPhysicsWorld() const;
	btTypedConstraint* getConstraint() const;
	ConstraintType getConstraintType() const;
	RigidBody* getOwnBody() const;
	RigidBody* getOtherBody() const;

	const Vector3& getPosition() const;
	const Quaternion& getRotation() const;
	const Vector3& getOtherPosition() const;
	const Quaternion& getOtherRotation() const;
	Vector3 getWorldPosition() const;

	const Vector2& getHighLimit() const;
	const Vector2& getLowLimit() const;
	float getERP() const;
	float getCFM() const;
	bool getDisableCollision() const;

	void releaseConstraint();
	void applyFrames();

protected:
	void createConstraint();
	void applyLimits();

protected:
	PhysicsWorld*				physics_world_;
	RigidBody*					own_body_;
	RigidBody*					other_body_;
	btTypedConstraint*			constraint_;
	ConstraintType				constraint_type_;
	Vector3						position_;
	Quaternion					rotation_;
	Vector3						other_position_;
	Quaternion					other_rotation_;
	Vector3						cached_world_scale_;
	Vector2						high_limit_;
	Vector2						low_limit_;
	float						erp_;
	float						cfm_;
	int							other_body_node_id_;
	bool						disable_collision_;
	bool						recreate_constraint_;
	bool						frame_dirty_;

	SceneNode*					node_;
};	

}