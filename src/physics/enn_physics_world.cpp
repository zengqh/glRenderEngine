/**
* Copyright (c) 2013 Pateo Software Ltd
*
* This source file is part of Pateo gis engine.
* For latest info, see http://www.pateo.com.cn/
*
* Time: 2013/09/26
* File: enn_physics_world.cpp
**/

#include "enn_physics_world.h"
#include "enn_root.h"

#include <BulletCollision/BroadphaseCollision/btDbvtBroadphase.h>
#include <BulletCollision/CollisionDispatch/btDefaultCollisionConfiguration.h>
#include <BulletCollision/CollisionDispatch/btInternalEdgeUtility.h>
#include <BulletCollision/CollisionShapes/btBoxShape.h>
#include <BulletCollision/CollisionShapes/btSphereShape.h>
#include <BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h>
#include <BulletDynamics/Dynamics/btDiscreteDynamicsWorld.h>

namespace enn
{

static const int MAX_SOLVER_ITERATIONS = 256;
static const int DEFAULT_FPS = 60;
static const Vector3 DEFAULT_GRAVITY = Vector3(0.0f, -9.81f, 0.0f);

//////////////////////////////////////////////////////////////////////////
static bool CompareRaycastResults(const PhysicsRaycastResult& lhs, 
								  const PhysicsRaycastResult& rhs)
{
	return lhs.distance_ < rhs.distance_;
}

void InternalPreTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	static_cast<PhysicsWorld*>(world->getWorldUserInfo())->preStep(timeStep);
}

void InternalTickCallback(btDynamicsWorld *world, btScalar timeStep)
{
	static_cast<PhysicsWorld*>(world->getWorldUserInfo())->postStep(timeStep);
}

// Callback for physics world queries
struct PhysicsQueryCallback : public btCollisionWorld::ContactResultCallback
{
	typedef enn::vector<RigidBody*>::type PhysicsQueryResults;

	PhysicsQueryCallback(enn::vector<RigidBody*>::type& result) : result_(result)
	{

	}

	btScalar addSingleResult(btManifoldPoint&, const btCollisionObjectWrapper*
		colObj0Wrap, int, int, const btCollisionObjectWrapper *colObj1Wrap, int, int)
	{
		RigidBody* body = reinterpret_cast<RigidBody*>(colObj0Wrap->getCollisionObject()->getUserPointer());
		if (body)
		{
			PhysicsQueryResults::iterator it = std::find(result_.begin(), result_.end(), body);
			if (it == result_.end())
			{
				result_.push_back(body);
			}
		}

		body = reinterpret_cast<RigidBody*>(colObj1Wrap->getCollisionObject()->getUserPointer());
		if (body)
		{
			PhysicsQueryResults::iterator it = std::find(result_.begin(), result_.end(), body);
			if (it == result_.end())
			{
				result_.push_back(body);
			}
		}

		return 0.0f;
	}

	PhysicsQueryResults& result_;
};


//////////////////////////////////////////////////////////////////////////
PhysicsWorld::PhysicsWorld()
: interpolation_(true)
, scene_mgr_(ROOTPTR->getSceneManager())
, collision_configuration_(0)
, collision_dispatcher_(0)
, broadphase_(0)
, solver_(0)
, world_(0)
, fps_(DEFAULT_FPS)
, time_acc_(0.0f)
, debugMode_(btIDebugDraw::DBG_DrawWireframe | btIDebugDraw::DBG_DrawConstraints | btIDebugDraw::DBG_DrawConstraintLimits)
{
	collision_configuration_ = new btDefaultCollisionConfiguration();
	collision_dispatcher_ = new btCollisionDispatcher(collision_configuration_);
	broadphase_ = new btDbvtBroadphase();
	solver_ = new btSequentialImpulseConstraintSolver();
	world_ = new btDiscreteDynamicsWorld(collision_dispatcher_, broadphase_, solver_, collision_configuration_);

	world_->setGravity(ToBtVector3(DEFAULT_GRAVITY));
	world_->getDispatchInfo().m_useContinuous = true;
	world_->getSolverInfo().m_splitImpulse = false;
	world_->setDebugDrawer(this);
	world_->setInternalTickCallback(InternalPreTickCallback, static_cast<void*>(this), true);
	world_->setInternalTickCallback(InternalPreTickCallback, static_cast<void*>(this), false);
}

PhysicsWorld::~PhysicsWorld()
{
	
}

// Check if an AABB is visible for debug drawing.
bool PhysicsWorld::isVisible(const btVector3& aabbMin, const btVector3& aabbMax)
{
	return false;
}

// Draw a physics debug line.
void PhysicsWorld::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{

}

// Log warning from the physics engine.
void PhysicsWorld::reportErrorWarning(const char* warningString)
{

}

void PhysicsWorld::update(float timeStep)
{
	float internalTimeStep = 1.0f / fps_;

	if (interpolation_)
	{
		int maxSubSteps = (int)(timeStep * fps_) + 1;
		world_->stepSimulation(timeStep, maxSubSteps, internalTimeStep);
	}
	else
	{
		time_acc_ += timeStep;
		while (time_acc_ >= internalTimeStep)
		{
			world_->stepSimulation(internalTimeStep, 0, internalTimeStep);
			time_acc_ -= internalTimeStep;
		}
	}
}

void PhysicsWorld::updateCollisions()
{
	world_->performDiscreteCollisionDetection();
}

void PhysicsWorld::setFPS(int fps)
{
	fps_ = Math::clamp(fps, 1, 1000);
}

void PhysicsWorld::setGravity(const Vector3& gravity)
{
	world_->setGravity(ToBtVector3(gravity));
}

void PhysicsWorld::setNumIterations(int num)
{
	num = Math::clamp(num, 1, MAX_SOLVER_ITERATIONS);
	world_->getSolverInfo().m_numIterations = num;
}

void PhysicsWorld::setInterpolation(bool enable)
{
	interpolation_ = enable;
}

void PhysicsWorld::raycast(enn::vector<PhysicsRaycastResult>::type& result, 
	const Ray& ray, 
	float maxDistance, uint32 collisionMask)
{
	btCollisionWorld::AllHitsRayResultCallback rayCallback(ToBtVector3(
		ray.getOrigin()), ToBtVector3(ray.getOrigin() + maxDistance * ray.getDirection()));

	rayCallback.m_collisionFilterGroup = (short)0xffff;
	rayCallback.m_collisionFilterMask = collisionMask;

	world_->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

	for (int i = 0; i < rayCallback.m_collisionObjects.size(); ++i)
	{
		PhysicsRaycastResult newResult;

		newResult.body_ = static_cast<RigidBody*>(rayCallback.m_collisionObjects[i]->getUserPointer());
		newResult.position_ = ToVector3(rayCallback.m_hitPointWorld[i]);
		newResult.normal_ = ToVector3(rayCallback.m_hitNormalWorld[i]);
		newResult.distance_ = (newResult.position_ - ray.getOrigin()).length();

		result.push_back(newResult);
	}

	std::sort(result.begin(), result.end(), CompareRaycastResults);
}

void PhysicsWorld::raycastSingle(PhysicsRaycastResult& result, const Ray& ray,
	float maxDistance, uint32 collisionMask)
{
	btCollisionWorld::ClosestRayResultCallback rayCallback(ToBtVector3(
		ray.getOrigin()), ToBtVector3(ray.getOrigin() + maxDistance * ray.getDirection()));

	rayCallback.m_collisionFilterGroup = (short)0xffff;
	rayCallback.m_collisionFilterMask = collisionMask;

	world_->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);

	if (rayCallback.hasHit())
	{
		result.body_ = static_cast<RigidBody*>(rayCallback.m_collisionObject->getUserPointer());
		result.position_ = ToVector3(rayCallback.m_hitPointWorld);
		result.normal_ = ToVector3(rayCallback.m_hitNormalWorld);
		result.distance_ = (result.position_ - ray.getOrigin()).length();
	}
	else
	{
		result.body_ = 0;
		result.position_ = Vector3::ZERO;
		result.normal_ = Vector3::ZERO;
		result.distance_ = 0;
	}
}

void PhysicsWorld::sphereCast(PhysicsRaycastResult& result, const Ray& ray, 
	float radius,
	float maxDistance, uint32 collisionMask)
{
	btSphereShape shape(radius);

	btCollisionWorld::ClosestConvexResultCallback convexCallback(ToBtVector3(
		ray.getOrigin()), ToBtVector3(ray.getOrigin() + maxDistance * ray.getDirection()));

	convexCallback.m_collisionFilterGroup = (short)0xffff;
	convexCallback.m_collisionFilterMask = collisionMask;

	world_->convexSweepTest(&shape, btTransform(btQuaternion::getIdentity(),
		convexCallback.m_convexFromWorld), btTransform(btQuaternion::getIdentity(),
		convexCallback.m_convexToWorld), convexCallback);

	if (convexCallback.hasHit())
	{
		result.body_ = static_cast<RigidBody*>(convexCallback.m_hitCollisionObject->getUserPointer());
		result.position_ = ToVector3(convexCallback.m_hitPointWorld);
		result.normal_ = ToVector3(convexCallback.m_hitNormalWorld);
		result.distance_ = (result.position_ - ray.getOrigin()).length();
	}
	else
	{
		result.body_ = 0;
		result.position_ = Vector3::ZERO;
		result.normal_ = Vector3::ZERO;
		result.distance_ = 0;
	}
}

void PhysicsWorld::getRigidBodies(enn::vector<RigidBody*>::type& result, 
								  const Sphere& sphere, unsigned collisionMask)
{
	btSphereShape sphereShape(sphere.getRadius());

	btRigidBody* tempRigidBody = new btRigidBody(1.0f, 0, &sphereShape);

	tempRigidBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), ToBtVector3(sphere.getCenter())));

	tempRigidBody->activate();

	world_->addRigidBody(tempRigidBody, (short)0xffff, (short)collisionMask);

	PhysicsQueryCallback callback(result);

	world_->contactTest(tempRigidBody, callback);

	world_->removeRigidBody(tempRigidBody);

	delete tempRigidBody;
}


void PhysicsWorld::getRigidBodies(enn::vector<RigidBody*>::type& result, 
	const AxisAlignedBox& box,
	uint32 collisionMask)
{
	result.clear();

	btBoxShape boxShape(ToBtVector3(box.getHalfSize()));

	btRigidBody* tempRigidBody = new btRigidBody(1.0f, 0, &boxShape);

	tempRigidBody->setWorldTransform(btTransform(btQuaternion::getIdentity(), ToBtVector3(box.getCenter())));

	tempRigidBody->activate();

	world_->addRigidBody(tempRigidBody, (short)0xffff, (short)collisionMask);

	PhysicsQueryCallback callback(result);

	world_->contactTest(tempRigidBody, callback);

	world_->removeRigidBody(tempRigidBody);

	delete tempRigidBody;
}

void PhysicsWorld::getRigidBodies(enn::vector<RigidBody*>::type& result, 
	const RigidBody* body)
{
	result.clear();

	ENN_FOR_EACH(RigidPairMap, current_collisions_, it)
	{
		if (it->first.first == body)
		{
			result.push_back(it->first.second);
		}
		else if (it->first.second == body)
		{
			result.push_back(it->first.first);
		}
	}
}

Vector3 PhysicsWorld::getGravity() const
{
	return ToVector3(world_->getGravity());
}

int PhysicsWorld::getNumIterations() const
{
	return world_->getSolverInfo().m_numIterations;
}

bool PhysicsWorld::getInterpolation() const
{
	return interpolation_;
}

int PhysicsWorld::getFPS() const
{
	return fps_;
}

void PhysicsWorld::addRigidBody(RigidBody* body)
{
	rigid_bodies_.push_back(body);
}

void PhysicsWorld::removeRigidBody(RigidBody* body)
{
	std::remove(rigid_bodies_.begin(), rigid_bodies_.end(), body);
}

void PhysicsWorld::addCollisionShape(CollisionShape* shape)
{
	collision_shapes_.push_back(shape);
}

void PhysicsWorld::removeCollisionShape(CollisionShape* shape)
{
	std::remove(collision_shapes_.begin(), collision_shapes_.end(), shape);
}

void PhysicsWorld::addConstraint(Constraint* joint)
{
	constraints_.push_back(joint);
}

void PhysicsWorld::removeConstraint(Constraint* joint)
{
	std::remove(constraints_.begin(), constraints_.end(), joint);
}

btDiscreteDynamicsWorld* PhysicsWorld::getWorld() const
{
	return world_;
}

PhysicsWorld::GeometryCacheMap& PhysicsWorld::getGeometryCache()
{
	return geometry_cache_map_;
}

void PhysicsWorld::cleanGeometryCache()
{
	geometry_cache_map_.clear();
}

void PhysicsWorld::preStep(float timeStep)
{
	
}

void PhysicsWorld::postStep(float timeStep)
{

}

void PhysicsWorld::setNode(SceneNode* node)
{
	
}

}