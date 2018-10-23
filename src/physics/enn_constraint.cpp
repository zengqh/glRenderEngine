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
// File: enn_constraint.cpp
//

#include "enn_constraint.h"
#include "enn_physics_world.h"

#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletDynamics/ConstraintSolver/btConeTwistConstraint.h>
#include <BulletDynamics/ConstraintSolver/btHingeConstraint.h>
#include <BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h>
#include <BulletDynamics/ConstraintSolver/btSliderConstraint.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

namespace enn
{

static const char* typeNames[] =
{
	"Point",
	"Hinge",
	"Slider",
	"ConeTwist",
	0
};

//////////////////////////////////////////////////////////////////////////
Constraint::Constraint()
: constraint_(0)
, constraint_type_(CONSTRAINT_POINT)
, position_(Vector3::ZERO)
, rotation_(Quaternion::IDENTITY)
, other_position_(Vector3::ZERO)
, other_rotation_(Quaternion::IDENTITY)
, high_limit_(Vector2::ZERO)
, low_limit_(Vector2::ZERO)
, erp_(0.0f)
, cfm_(0.0f)
, other_body_node_id_(0)
, disable_collision_(false)
, recreate_constraint_(true)
, frame_dirty_(false)
{

}

Constraint::~Constraint()
{
	releaseConstraint();
	if (physics_world_)
	{
		physics_world_->removeConstraint(this);
	}
}

void Constraint::getDependencyNodes(enn::vector<SceneNode*>::type& dest)
{
	if (other_body_ && other_body_->getNode())
	{
		dest.push_back(other_body_->getNode());
	}
}

void Constraint::setConstraintType(ConstraintType type)
{
	if (type != constraint_type_)
	{
		constraint_type_ = type;
		createConstraint();
	}
}

void Constraint::setOtherBody(RigidBody* body)
{
	if (other_body_ != body)
	{
		if (other_body_)
		{
			other_body_->removeConstraiant(this);
		}

		other_body_ = body;

		SceneNode* other_node = other_body_ ? other_body_->getNode() : 0;
		other_body_node_id_ = 0;

		createConstraint();
	}
}

void Constraint::setPostion(const Vector3& position)
{
	if (position.positionEquals(position_))
	{
		position_ = position;
		applyFrames();
	}
}

void Constraint::setRotation(const Quaternion& quat)
{
	if (!quat.equals(rotation_, 0.001f))
	{
		rotation_ = quat;
		applyFrames();
	}
}

void Constraint::setAxis(const Vector3& axis)
{
	switch (constraint_type_)
	{
	case CONSTRAINT_POINT:
	case CONSTRAINT_HINGE:
		rotation_ = Quaternion(Vector3::UNIT_Z, axis);
		break;

	case CONSTRAINT_SLIDER:
	case CONSTRAINT_CONETWIST:
		rotation_ = Quaternion(Vector3::UNIT_X, axis);
		break;

	default:
		break;
	}

	applyFrames();
}

void Constraint::setOtherPosition(const Vector3& position)
{
	if (position.positionEquals(other_position_))
	{
		other_position_ = position;
		applyFrames();
	}
}

void Constraint::setOtherRotation(const Quaternion& rotation)
{
	if (!rotation.equals(other_rotation_))
	{
		other_rotation_ = rotation;
		applyFrames();
	}
}

void Constraint::setOtherAxis(const Vector3& axis)
{
	switch (constraint_type_)
	{
	case CONSTRAINT_POINT:
	case CONSTRAINT_HINGE:
		other_rotation_ = Quaternion(Vector3::UNIT_Z, axis);
		break;

	case CONSTRAINT_SLIDER:
	case CONSTRAINT_CONETWIST:
		other_rotation_ = Quaternion(Vector3::UNIT_X, axis);
		break;

	default:
		break;
	}

	applyFrames();
}

void Constraint::setWorldPosition(const Vector3& position)
{
	if (constraint_)
	{
		btTransform ownBodyInverse = constraint_->getRigidBodyA().getWorldTransform().inverse();
		btTransform otherBodyInverse = constraint_->getRigidBodyB().getWorldTransform().inverse();
		btVector3 worldPos = ToBtVector3(position);

		position_ = (ToVector3(ownBodyInverse * worldPos) + own_body_->getCenterOfMass()) / cached_world_scale_;
		other_position_ = ToVector3(otherBodyInverse * worldPos);

		if (other_body_)
		{
			other_position_ += other_body_->getCenterOfMass();
			other_position_ /= other_body_->getNode()->getDerivedMatrix().getScaleVector();
		}

		applyFrames();
	}
}

void Constraint::SetHighLimit(const Vector2& limit)
{
	if (!limit.positionEquals(high_limit_))
	{
		high_limit_ = limit;
		applyLimits();
	}
}

void Constraint::setLowLimit(const Vector2& limit)
{
	if (!limit.positionEquals(low_limit_))
	{
		low_limit_ = limit;
		applyLimits();
	}
}

// Set constraint error reduction parameter. Zero = leave to default.
void Constraint::setERP(float erp)
{
	erp = Math::maxVal(erp, 0.0f);
	
	if (!Math::realEqual(erp, erp_))
	{
		erp_ = erp;
		applyLimits();	
	}
}

// Set constraint force mixing parameter. Zero = leave to default.
void Constraint::setCFM(float cfm)
{
	cfm = Math::maxVal(cfm, 0.0f);

	if (!Math::realEqual(cfm, cfm_))
	{
		cfm_ = cfm;
		applyLimits();	
	}
}

// Set whether to disable collisions between connected bodies.
void Constraint::setDisableCollision(bool disable)
{
	if (disable != disable_collision_)
	{
		disable_collision_ = disable;
		createConstraint();
	}
}

PhysicsWorld* Constraint::getPhysicsWorld() const
{
	return physics_world_;
}

btTypedConstraint* Constraint::getConstraint() const
{
	return constraint_;
}

ConstraintType Constraint::getConstraintType() const
{
	return constraint_type_;
}

RigidBody* Constraint::getOwnBody() const
{
	return own_body_;
}

RigidBody* Constraint::getOtherBody() const
{
	return other_body_;
}

const Vector3& Constraint::getPosition() const
{
	return position_;
}

const Quaternion& Constraint::getRotation() const
{
	return rotation_;
}

const Vector3& Constraint::getOtherPosition() const
{
	return other_position_;
}

const Quaternion& Constraint::getOtherRotation() const
{
	return other_rotation_;
}

Vector3 Constraint::getWorldPosition() const
{
	if (constraint_)
	{
		btTransform ownBody = constraint_->getRigidBodyA().getWorldTransform();
		return ToVector3(ownBody * ToBtVector3(position_ * cached_world_scale_ - own_body_->getCenterOfMass()));
	}
	else
	{
		return Vector3::ZERO;
	}
}

const Vector2& Constraint::getHighLimit() const
{
	return high_limit_;
}

const Vector2& Constraint::getLowLimit() const
{
	return low_limit_;
}

float Constraint::getERP() const
{
	return erp_;
}

float Constraint::getCFM() const
{
	return cfm_;
}

bool Constraint::getDisableCollision() const
{
	return disable_collision_;
}

void Constraint::releaseConstraint()
{
	if (constraint_)
	{
		if (own_body_)
		{
			own_body_->removeConstraiant(this);
		}

		if (other_body_)
		{
			other_body_->removeConstraiant(this);
		}

		if (physics_world_)
		{
			physics_world_->getWorld()->removeConstraint(constraint_);
		}

		delete (constraint_);
	}
}

void Constraint::applyFrames()
{
	if (!constraint_)
	{
		return;
	}

	if (node_)
	{
		cached_world_scale_ = node_->getDerivedMatrix().getScaleVector();
	}

	Vector3 ownBodyScaledPosition = position_ * cached_world_scale_ - own_body_->getCenterOfMass();
	Vector3 otherBodyScaledPosition = other_body_ ? other_position_ * 
		other_body_->getNode()->getDerivedMatrix().getScaleVector() - 
		other_body_->getCenterOfMass() : other_position_;

	switch (constraint_->getConstraintType())
	{
	case POINT2POINT_CONSTRAINT_TYPE:
		{
			btPoint2PointConstraint* pointConstraint = static_cast<btPoint2PointConstraint*>(constraint_);
			pointConstraint->setPivotA(ToBtVector3(ownBodyScaledPosition));
			pointConstraint->setPivotB(ToBtVector3(otherBodyScaledPosition));
		}

		break;

	case HINGE_CONSTRAINT_TYPE:
		{
			btHingeConstraint* hingeConstraint = static_cast<btHingeConstraint*>(constraint_);
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));
			hingeConstraint->setFrames(ownFrame, otherFrame);
		}

		break;

	case SLIDER_CONSTRAINT_TYPE:
		{
			btSliderConstraint* sliderConstraint = static_cast<btSliderConstraint*>(constraint_);
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));

			sliderConstraint->setFrames(ownFrame, otherFrame);
		}

		break;

	case CONETWIST_CONSTRAINT_TYPE:
		{
			btConeTwistConstraint* coneTwistConstraint = static_cast<btConeTwistConstraint*>(constraint_);
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));

			coneTwistConstraint->setFrames(ownFrame, otherFrame);
		}

		break;

	default:
		break;
	}
}

void Constraint::createConstraint()
{
	cached_world_scale_ = node_->getDerivedMatrix().getScaleVector();

	releaseConstraint();

	btRigidBody* ownBody = own_body_->getBody();
	btRigidBody* otherBody = other_body_->getBody();

	if (!physics_world_ || !ownBody)
	{
		return;
	}

	if (!otherBody)
	{
		otherBody = &btTypedConstraint::getFixedBody();
	}

	 Vector3 ownBodyScaledPosition = position_ * cached_world_scale_ - own_body_->getCenterOfMass();
	 Vector3 otherBodyScaledPosition = other_body_ ? other_position_ * other_body_->getNode()->getDerivedMatrix().getScaleVector() -
		 other_body_->getCenterOfMass() : other_position_;

	switch (constraint_type_)
	{
	case CONSTRAINT_POINT:
		{
			constraint_ = new btPoint2PointConstraint(*ownBody, *otherBody,
				ToBtVector3(ownBodyScaledPosition), 
				ToBtVector3(otherBodyScaledPosition));
		}

		break;

	case CONSTRAINT_HINGE:
		{
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));

			constraint_ = new btHingeConstraint(*ownBody, *otherBody, ownFrame, otherFrame);
		}

		break;

	case CONSTRAINT_SLIDER:
		{
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));

			constraint_ = new btSliderConstraint(*ownBody, *otherBody, ownFrame, otherFrame, false);
		}

		break;

	case CONSTRAINT_CONETWIST:
		{
			btTransform ownFrame(ToBtQuaternion(rotation_), ToBtVector3(ownBodyScaledPosition));
			btTransform otherFrame(ToBtQuaternion(other_rotation_), ToBtVector3(otherBodyScaledPosition));

			constraint_ = new btConeTwistConstraint(*ownBody, *otherBody, ownFrame, otherFrame);
		}

		break;

	default:
		break;
	}

	if (constraint_)
	{
		constraint_->setUserConstraintPtr(this);
		constraint_->setEnabled(true);
		own_body_->addConstraint(this);
		if (other_body_)
		{
			other_body_->addConstraint(this);
		}

		applyLimits();

		physics_world_;
	}

	recreate_constraint_ = false;
	frame_dirty_ = false;
}

void Constraint::applyLimits()
{
	if (!constraint_)
	{
		return;
	}

	switch (constraint_->getConstraintType())
	{
	case HINGE_CONSTRAINT_TYPE:
		{
			btHingeConstraint* hingeConstraint = static_cast<btHingeConstraint*>(constraint_);
			hingeConstraint->setLimit(Math::toRadians(low_limit_.x), 
				Math::toRadians(high_limit_.x));
		}

		break;

	case SLIDER_CONSTRAINT_TYPE:
		{
			btSliderConstraint* sliderConstraint = static_cast<btSliderConstraint*>(constraint_);
			
			sliderConstraint->setUpperLinLimit(high_limit_.x);
			sliderConstraint->setUpperAngLimit(high_limit_.y * Math::s_deg2Rad);
			sliderConstraint->setLowerLinLimit(low_limit_.x);
			sliderConstraint->setLowerAngLimit(low_limit_.y * Math::s_deg2Rad);
		}

		break;

	case CONETWIST_CONSTRAINT_TYPE:
		{
			btConeTwistConstraint* coneTwistConstraint = static_cast<btConeTwistConstraint*>(constraint_);
			coneTwistConstraint->setLimit(high_limit_.y * Math::s_deg2Rad, 
				high_limit_.y * Math::s_deg2Rad, high_limit_.x * Math::s_deg2Rad);
		}

		break;

	default:
		break;
	}

	if (erp_ != 0)
	{
		constraint_->setParam(BT_CONSTRAINT_STOP_ERP, erp_);
	}

	if (cfm_)
	{
		constraint_->setParam(BT_CONSTRAINT_STOP_CFM, cfm_);
	}
}

}