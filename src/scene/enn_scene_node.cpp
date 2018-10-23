/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/12
* File: SceneNode.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_scene_node.h"
#include "enn_light.h"
#include "enn_root.h"
#include "enn_light_node.h"

namespace enn
{
SceneNode::SceneNode()
: scene_mgr_(0)
, collision_shape_(0)
, rigid_body_(0)
{
	type_flag_.set_flag(NODE_SCENE);
	dirty_flag_.set_flag(DF_WORLD_AABB);
}

SceneNode::~SceneNode()
{
	
}

const AxisAlignedBox& SceneNode::getWorldAABB() const
{
	const_cast<SceneNode*>(this)->checkUpdateWorldAABB();
	return world_aabb_;
}

const AxisAlignedBox& SceneNode::getObjectAABB() const
{
	return AxisAlignedBox::BOX_NULL;
}

CollisionShape* SceneNode::createCollisionShape()
{
	if (!collision_shape_)
	{
		collision_shape_ = ENN_NEW CollisionShape();
		collision_shape_->setNode(this);
	}

	return collision_shape_;
}

RigidBody* SceneNode::createRigidBody()
{
	if (!rigid_body_)
	{
		rigid_body_ = ENN_NEW RigidBody();
		rigid_body_->setNode(this);
	}

	return rigid_body_;
}

CollisionShape* SceneNode::getCollisionShape() const
{
	return collision_shape_;
}

RigidBody* SceneNode::getRigidBody() const
{
	return rigid_body_;
}

void SceneNode::setSelfTransformDirty()
{
	Node::setSelfTransformDirty();
	setWorldAABBDirty();
}

void SceneNode::setParentTransformDirty()
{
	Node::setParentTransformDirty();
	setWorldAABBDirty();
}

void SceneNode::update()
{
	checkUpdateWorldAABB();
}

void SceneNode::setWorldAABBDirty()
{
	dirty_flag_.set_flag(DF_WORLD_AABB);
}

void SceneNode::checkUpdateWorldAABB()
{
	checkUpdateTransform();

	if (dirty_flag_.test_flag(DF_WORLD_AABB))
	{
		world_aabb_ = getObjectAABB();

		world_aabb_.transformAffine(derived_matrix_);

		dirty_flag_.reset_flag(DF_WORLD_AABB);
	}
}

RenderableNode::RenderableNode()
: light_info_(0)
, lights_info_update_(0)
{
	type_flag_.set_flag(NODE_RENDERABLE);
}

RenderableNode::~RenderableNode()
{
	ENN_SAFE_DELETE(light_info_);
}

LightsInfo* RenderableNode::getLightsInfo()
{
	if (!light_info_)
	{
		light_info_ = ENN_NEW LightsInfo();
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	/** it will get many times, update per frame */
	uint32 system_frame_no = rs->getCurrFrameNo();
	if (lights_info_update_ < system_frame_no)
	{
		lights_info_update_ = system_frame_no;

		const AxisAlignedBox& world_aabb = getWorldAABB();

		Root::getSingletonPtr()->getRenderPipe()->queryLightInfo(world_aabb, *light_info_, isReceiveShadow());
	}

	return light_info_;
}

}