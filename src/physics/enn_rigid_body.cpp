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
// File: enn_rigid_body.cpp
//

#include "enn_rigid_body.h"
#include "enn_physics_world.h"
#include "enn_root.h"

#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>
#include <BulletDynamics/Dynamics/btRigidBody.h>
#include <BulletCollision/CollisionShapes/btCompoundShape.h>

namespace enn
{

static const float DEFAULT_MASS = 0.0f;
static const float DEFAULT_FRICTION = 0.5f;
static const float DEFAULT_RESTITUTION = 0.0f;
static const float DEFAULT_ROLLING_FRICTION = 0.0f;
static const unsigned DEFAULT_COLLISION_LAYER = 0x1;
static const unsigned DEFAULT_COLLISION_MASK = Math::ENN_MAX_UNSIGNED;

static const char* collisionEventModeNames[] =
{
	"Never",
	"When Active",
	"Always",
	0
};

//////////////////////////////////////////////////////////////////////////
RigidBody::RigidBody()
: body_(0)
, compoundShape_(0)
, shfitedCompoundShape_(0)
, gravity_override_(Vector3::ZERO)
, center_of_mass_(Vector3::ZERO)
, mass_(DEFAULT_MASS)
, collision_layer_(DEFAULT_COLLISION_LAYER)
, collision_mask_(DEFAULT_COLLISION_MASK)
, collision_event_mode_(COLLISION_ACTIVE)
, last_position_(Vector3::ZERO)
, last_rotation_(Quaternion::IDENTITY)
, kinematic_(false)
, phantom_(false)
, use_gravity_(true)
, readd_body_(false)
, in_world_(false)
{
	compoundShape_ = new btCompoundShape();
	shfitedCompoundShape_ = new btCompoundShape();
	physics_world_ = ROOTPTR->getSceneManager()->getPhysicsWorld();
}

RigidBody::~RigidBody()
{
	releaseBody();

	if (physics_world_)
	{
		physics_world_->removeRigidBody(this);
	}

	delete (compoundShape_);
	delete (shfitedCompoundShape_);

	compoundShape_ = 0;
	shfitedCompoundShape_ = 0;
}

void RigidBody::getWorldTransform(btTransform& worldTrans) const
{
	if (node_)
	{
		last_position_ = node_->getDerivedMatrix().getTrans();
		last_rotation_ = Quaternion(node_->getDerivedMatrix().toMatrix3());

		worldTrans.setOrigin(ToBtVector3(last_position_ + last_rotation_ * center_of_mass_));
		worldTrans.setRotation(ToBtQuaternion(last_rotation_));
	}
}

void RigidBody::setWorldTransform(const btTransform& worldTrans)
{
	Quaternion newWorldRotation = ToQuaternion(worldTrans.getRotation());
	Vector3 newWorldPosition = ToVector3(worldTrans.getOrigin()) - newWorldRotation * center_of_mass_;
	RigidBody* parentRigidBody = 0;

	if (node_)
	{
		applyWorldTransform(newWorldPosition, newWorldRotation);
	}
}

void RigidBody::setMass(float mass)
{
	mass = Math::maxVal(mass, 0.0f);
	if (!Math::realEqual(mass, mass_))
	{
		mass_ = mass;
		addBodyToWorld();
	}
}

void RigidBody::setPosition(const Vector3& position)
{
	if (body_)
	{
		btTransform& worldTrans = body_->getWorldTransform();

		worldTrans.setOrigin(ToBtVector3(position + ToQuaternion(worldTrans.getRotation()) * center_of_mass_));

		// When forcing the physics position, set also interpolated position so that there is no jitter
		btTransform interpTrans = body_->getInterpolationWorldTransform();
		interpTrans.setOrigin(worldTrans.getOrigin());
		body_->setInterpolationWorldTransform(interpTrans);
		body_->updateInertiaTensor();

		activate();
	}
}

void RigidBody::setRotation(const Quaternion& rotation)
{
	if (body_)
	{
		Vector3 oldPosition = getPosition();
		btTransform& worldTrans = body_->getWorldTransform();
		worldTrans.setRotation(ToBtQuaternion(rotation));

		if (!center_of_mass_.positionEquals(oldPosition + rotation * center_of_mass_))
		{
			worldTrans.setOrigin(ToBtVector3(oldPosition + rotation * center_of_mass_));
		}

		btTransform interpTrans = body_->getInterpolationWorldTransform();

		interpTrans.setRotation(worldTrans.getRotation());

		if (!center_of_mass_.positionEquals(Vector3::ZERO))
		{
			interpTrans.setOrigin(worldTrans.getOrigin());
		}

		body_->setInterpolationWorldTransform(interpTrans);
		body_->updateInertiaTensor();

		activate();
	}
}

void RigidBody::setTransform(const Vector3& position, const Quaternion& rotation)
{
	if (body_)
	{
		btTransform& worldTrans = body_->getWorldTransform();
		worldTrans.setRotation(ToBtQuaternion(rotation));
		worldTrans.setOrigin(ToBtVector3(position + rotation * center_of_mass_));

		btTransform interpTrans = body_->getInterpolationWorldTransform();
		interpTrans.setOrigin(worldTrans.getOrigin());
		interpTrans.setRotation(worldTrans.getRotation());
		body_->setInterpolationWorldTransform(interpTrans);
		body_->updateInertiaTensor();

		activate();
	}
}

void RigidBody::setLinearVelocity(const Vector3& velocity)
{
	if (body_)
	{
		body_->setLinearVelocity(ToBtVector3(velocity));

		if (!velocity.positionEquals(Vector3::ZERO))
		{
			activate();
		}
	}
}

void RigidBody::setLinearFactor(const Vector3& factor)
{
	if (body_)
	{
		body_->setLinearFactor(ToBtVector3(factor));
	}
}

void RigidBody::setLinearRestThreshold(float threshold)
{
	if (body_)
	{
		body_->setSleepingThresholds(threshold, body_->getAngularSleepingThreshold());
	}
}

void RigidBody::setLinearDamping(float damping)
{
	if (body_)
	{
		body_->setDamping(damping, body_->getAngularDamping());
	}
}

void RigidBody::setAngularVelocity(const Vector3& angularVelocity)
{
	if (body_)
	{
		body_->setAngularVelocity(ToBtVector3(angularVelocity));
		if (!angularVelocity.positionEquals(Vector3::ZERO))
		{
			activate();
		}
	}
}

void RigidBody::setAngularFactor(const Vector3& factor)
{
	if (body_)
	{
		body_->setAngularFactor(ToBtVector3(factor));
	}
}

void RigidBody::setAngularRestThreshold(float threshold)
{
	if (body_)
	{
		body_->setSleepingThresholds(body_->getLinearSleepingThreshold(), threshold);
	}
}

void RigidBody::setAngularDamping(float factor)
{
	if (body_)
	{
		body_->setDamping(body_->getLinearDamping(), factor);
	}
}

void RigidBody::setFriction(float friction)
{
	if (body_)
	{
		body_->setFriction(friction);
	}
}

void RigidBody::setRollingFriction(float friction)
{
	if (body_)
	{
		body_->setRollingFriction(friction);
	}
}

void RigidBody::setRestitution(float restitution)
{
	if (body_)
	{
		body_->setRestitution(restitution);
	}
}

void RigidBody::setContactProcessingThreshold(float threshold)
{
	if (body_)
	{
		body_->setContactProcessingThreshold(threshold);
	}
}

void RigidBody::setCcdRadius(float radius)
{
	radius = Math::maxVal(radius, 0.0f);
	if (body_)
	{
		body_->setCcdSweptSphereRadius(radius);
	}
}

void RigidBody::setCcdMotionThreshold(float threshold)
{
	threshold = Math::maxVal(threshold, 0.0f);
	if (body_)
	{
		body_->setCcdMotionThreshold(threshold);
	}
}

void RigidBody::setUseGravity(bool enable)
{
	if (enable != use_gravity_)
	{
		use_gravity_ = enable;
		updateGravity();
	}
}

void RigidBody::setGravityOverride(const Vector3& gravity)
{
	if (!gravity.positionEquals(gravity_override_))
	{
		gravity_override_ = gravity;
		updateGravity();
	}
}

void RigidBody::setKinematic(bool enable)
{
	if (enable != kinematic_)
	{
		kinematic_ = enable;
		addBodyToWorld();
	}
}

void RigidBody::setPhantom(bool enable)
{
	if (enable != phantom_)
	{
		phantom_ = enable;
		addBodyToWorld();
	}
}

void RigidBody::setCollisionLayer(uint32 layer)
{
	if (layer != collision_layer_)
	{
		collision_layer_ = layer;
		addBodyToWorld();
	}
}

void RigidBody::setCollisionMask(uint32 mask)
{
	if (mask != collision_mask_)
	{
		collision_mask_ = mask;
		addBodyToWorld();
	}
}

void RigidBody::setCollisionLayerAndMask(uint32 layer, uint32 mask)
{
	if (layer != collision_layer_ || mask != collision_mask_)
	{
		collision_layer_ = layer;
		collision_mask_ = mask;
		addBodyToWorld();
	}
}

void RigidBody::setCollisionEventMode(CollisionEventMode mode)
{
	collision_event_mode_ = mode;
}

void RigidBody::applyForce(const Vector3& force)
{
	if (body_ && !force.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyCentralForce(ToBtVector3(force));
	}
}

void RigidBody::applyForce(const Vector3& force, const Vector3& position)
{
	if (body_ && !force.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyForce(ToBtVector3(force), ToBtVector3(position - center_of_mass_));
	}
}

void RigidBody::applyTorque(const Vector3& torque)
{
	if (body_ && !torque.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyTorque(ToBtVector3(torque));
	}
}

void RigidBody::applyImpulse(const Vector3& impulse)
{
	if (body_ && !impulse.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyCentralImpulse(ToBtVector3(impulse));
	}
}

void RigidBody::applyImpulse(const Vector3& impulse, const Vector3& position)
{
	if (body_ && !impulse.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyImpulse(ToBtVector3(impulse), ToBtVector3(position - center_of_mass_));
	}
}

void RigidBody::applyTorqueImpulse(const Vector3& torque)
{
	if (body_ && !torque.positionEquals(Vector3::ZERO))
	{
		activate();
		body_->applyTorqueImpulse(ToBtVector3(torque));
	}
}

void RigidBody::resetForces()
{
	if (body_)
	{
		body_->clearForces();
	}
}

void RigidBody::activate()
{
	if (body_ && mass_ > 0.0f)
	{
		body_->activate(true);
	}
}

void RigidBody::reAddBodyToWorld()
{
	if (body_ && in_world_)
	{
		addBodyToWorld();
	}
}

PhysicsWorld* RigidBody::getPhysicsWorld() const
{
	return physics_world_;
}

btRigidBody* RigidBody::getBody() const
{
	return body_;
}

btCompoundShape* RigidBody::getCompoundShape() const
{
	return compoundShape_;
}

float RigidBody::getMass() const
{
	return mass_;
}

Vector3 RigidBody::getPosition() const
{
	if (body_)
	{
		const btTransform& transform = body_->getWorldTransform();
		return ToVector3(transform.getOrigin()) - ToQuaternion(transform.getRotation()) * center_of_mass_;
	}
	else
	{
		return Vector3::ZERO;
	}
}

Quaternion RigidBody::getRotation() const
{
	if (body_)
	{
		return ToQuaternion(body_->getWorldTransform().getRotation());
	}
	else
	{
		return Quaternion::IDENTITY;
	}
}

Vector3 RigidBody::getLinearVelocity() const
{
	return body_ ? ToVector3(body_->getLinearVelocity()) : Vector3::ZERO;
}

Vector3 RigidBody::getLinearFactor() const
{
	return body_ ? ToVector3(body_->getLinearFactor()) : Vector3::ZERO;
}

Vector3 RigidBody::getVelocityAtPoint(const Vector3& position) const
{
	if (body_)
	{
		return ToVector3(body_->getVelocityInLocalPoint(ToBtVector3(position - center_of_mass_)));
	}
	else
	{
		return Vector3::ZERO;
	}
}

float RigidBody::getLinearRestThreshold() const
{
	return body_ ? body_->getLinearSleepingThreshold() : 0.0f;
}

float RigidBody::getLinearDamping() const
{
	return body_ ? body_->getLinearDamping() : 0.0f;
}

Vector3 RigidBody::getAngularVelocity() const
{
	return body_ ? ToVector3(body_->getAngularVelocity()) : Vector3::ZERO;
}

Vector3 RigidBody::getAngularFactor() const
{
	return body_ ? ToVector3(body_->getAngularFactor()) : Vector3::ZERO;
}

float RigidBody::getAngularRestThreshold() const
{
	return body_ ? body_->getAngularSleepingThreshold() : 0.0f;
}

float RigidBody::getAngularDamping() const
{
	return body_ ? body_->getAngularDamping() : 0.0f;
}

float RigidBody::getFricition() const
{
	return body_ ? body_->getFriction() : 0.0f;
}

float RigidBody::getRollingFriction() const
{
	return body_ ? body_->getRollingFriction() : 0.0f;
}

float RigidBody::getRestitution() const
{
	return	body_ ? body_->getRestitution() : 0.0f;
}

float RigidBody::getContactProcessingThreshold() const
{
	return body_ ? body_->getContactProcessingThreshold() : 0.0f;
}

float RigidBody::getCcdRadius() const
{
	return body_ ? body_->getCcdSweptSphereRadius() : 0.0f;
}

float RigidBody::getCcdMotionThreshold() const
{
	return body_ ? body_->getCcdMotionThreshold() : 0.0f;
}

bool RigidBody::getUseGravity() const
{
	return use_gravity_;
}

const Vector3& RigidBody::getGravityOverride() const
{
	return gravity_override_;
}

const Vector3& RigidBody::getCenterOfMass() const
{
	return center_of_mass_;
}

bool RigidBody::isKinematic() const
{
	return kinematic_;
}

bool RigidBody::isPhantom() const
{
	return phantom_;
}

bool RigidBody::isActive() const
{
	return body_ ? body_->isActive() : false;
}

uint32 RigidBody::getCollisionLayer() const
{
	return collision_layer_;
}

uint32 RigidBody::getCollisionMask() const
{
	return collision_mask_;
}

CollisionEventMode RigidBody::getCollisionEvent() const
{
	return collision_event_mode_;
}

void RigidBody::getCollidingBodies(enn::vector<RigidBody*>::type& result) const
{
	if (physics_world_)
	{
		physics_world_->getRigidBodies(result, this);
	}
	else
	{
		result.clear();
	}
}

void RigidBody::applyWorldTransform(const Vector3& newWorldPosition, 
	const Quaternion& newWorldRotation)
{
	// keep simple here, it's a bug !!!
	node_->setPosition(newWorldPosition);
	node_->setOrientation(newWorldRotation);
}

void RigidBody::updateMass()
{
	if (body_)
	{
		btTransform principal;
		principal.setRotation(btQuaternion::getIdentity());
		principal.setOrigin(btVector3(0.0f, 0.0f, 0.0f));

		unsigned numShapes = compoundShape_->getNumChildShapes();
		if (numShapes)
		{
			enn::vector<float>::type masses(numShapes);
			for (unsigned i = 0; i < numShapes; ++i)
			{
				// The actual mass does not matter, divide evenly between child shapes
				masses[i] = 1.0f;
			}

			btVector3 inertia(0.0f, 0.0f, 0.0f);
			compoundShape_->calculatePrincipalAxisTransform(&masses[0], principal, inertia);
		}

		// Add child shapes to shifted compound shape with adjusted offset
		while (shfitedCompoundShape_->getNumChildShapes())
			shfitedCompoundShape_->removeChildShapeByIndex(shfitedCompoundShape_->getNumChildShapes() - 1);
		for (unsigned i = 0; i < numShapes; ++i)
		{
			btTransform adjusted = compoundShape_->getChildTransform(i);
			adjusted.setOrigin(adjusted.getOrigin() - principal.getOrigin());
			shfitedCompoundShape_->addChildShape(adjusted, compoundShape_->getChildShape(i));
		}

		// If shifted compound shape has only one child with no offset/rotation, use the child shape
		// directly as the rigid body collision shape for better collision detection performance
		bool useCompound = !numShapes || numShapes > 1;
		if (!useCompound)
		{
			const btTransform& childTransform = shfitedCompoundShape_->getChildTransform(0);
			if (!ToVector3(childTransform.getOrigin()).positionEquals(Vector3::ZERO) ||
				!ToQuaternion(childTransform.getRotation()).equals(Quaternion::IDENTITY))
				useCompound = true;
		}

		body_->setCollisionShape(useCompound ? shfitedCompoundShape_ : shfitedCompoundShape_->getChildShape(0));

		// If we have one shape and this is a triangle mesh, we use a custom material callback in order to adjust internal edges
		if (!useCompound && body_->getCollisionShape()->getShapeType() == SCALED_TRIANGLE_MESH_SHAPE_PROXYTYPE)
			body_->setCollisionFlags(body_->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
		else
			body_->setCollisionFlags(body_->getCollisionFlags() & ~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

		// Reapply rigid body position with new center of mass shift
		Vector3 oldPosition = getPosition();
		center_of_mass_ = ToVector3(principal.getOrigin());
		setPosition(oldPosition);

		// Calculate final inertia
		btVector3 localInertia(0.0f, 0.0f, 0.0f);
		if (mass_ > 0.0f)
		{
			shfitedCompoundShape_->calculateLocalInertia(mass_, localInertia);
		}

		body_->setMassProps(mass_, localInertia);
		body_->updateInertiaTensor();

		// Reapply constraint positions for new center of mass shift
		if (node_)
		{
			ENN_FOR_EACH(enn::vector<Constraint*>::type, constraints_, it)
			{
				(*it)->applyFrames();
			}
		}
	}
}

void RigidBody::updateGravity()
{
	if (physics_world_ && body_)
	{
		btDiscreteDynamicsWorld* world = physics_world_->getWorld();

		int flags = body_->getFlags();
		if (use_gravity_ && gravity_override_.positionEquals(Vector3::ZERO))
		{
			flags &= ~BT_DISABLE_WORLD_GRAVITY;
		}
		else
		{
			flags |= BT_DISABLE_WORLD_GRAVITY;
		}

		body_->setFlags(flags);

		if (use_gravity_)
		{
			// If override vector is zero, use world's gravity
			if (gravity_override_.positionEquals(Vector3::ZERO))
			{
				body_->setGravity(world->getGravity());
			}
			else
			{
				body_->setGravity(ToBtVector3(gravity_override_));
			}
		}
		else
		{
			body_->setGravity(btVector3(0.0f, 0.0f, 0.0f));
		}
	}
}

void RigidBody::addConstraint(Constraint* constraint)
{
	constraints_.push_back(constraint);
}

void RigidBody::removeConstraiant(Constraint* constraint)
{
	std::remove(constraints_.begin(), constraints_.end(), constraint);
}

void RigidBody::releaseBody()
{
	if (body_)
	{
		ENN_FOR_EACH(enn::vector<Constraint*>::type, constraints_, it)
		{
			(*it)->releaseConstraint();
		}
	}

	removeBodyFromWorld();

	delete body_;
	body_ = 0;
}

void RigidBody::setNode(SceneNode* node)
{
	node_ = node;
	physics_world_->addRigidBody(this);

	addBodyToWorld();
}

SceneNode* RigidBody::getNode() const
{
	return node_;
}

void RigidBody::addBodyToWorld()
{
	if (!physics_world_)
	{
		return;
	}

	if (mass_ < 0.0f)
	{
		mass_ = 0.0f;
	}

	if (body_)
	{
		removeBodyFromWorld();
	}
	else
	{
		btVector3 localInertia(0.0f, 0.0f, 0.0f);
		body_ = new btRigidBody(mass_, this, shfitedCompoundShape_, localInertia);
		body_->setUserPointer(this);
	}

	updateMass();
	updateGravity();

	int flags = body_->getCollisionFlags();
	if (phantom_)
	{
		flags |= btCollisionObject::CF_NO_CONTACT_RESPONSE;
	}
	else
	{
		flags &= ~btCollisionObject::CF_NO_CONTACT_RESPONSE;
	}
	if (kinematic_)
	{
		flags |= btCollisionObject::CF_KINEMATIC_OBJECT;
	}
	else
	{
		flags &= ~btCollisionObject::CF_KINEMATIC_OBJECT;
	}

	body_->setCollisionFlags(flags);

	btDiscreteDynamicsWorld* world = physics_world_->getWorld();
	world->addRigidBody(body_, collision_layer_, collision_mask_);

	in_world_ = true;
	readd_body_ = false;

	if (mass_ > 0.0f)
	{
		activate();
	}
	else
	{
		setLinearVelocity(Vector3::ZERO);
		setAngularVelocity(Vector3::ZERO);
	}
}

void RigidBody::removeBodyFromWorld()
{
	if (physics_world_ && body_ && in_world_)
	{
		btDiscreteDynamicsWorld* world = physics_world_->getWorld();
		world->removeRigidBody(body_);
		in_world_ = false;
	}
}

}