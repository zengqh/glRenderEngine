/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/10
* File: enn_light_node.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_light_node.h"

namespace enn
{
LightNode::LightNode()
: light_(0)
{
	type_flag_.set_flag(NODE_LIGHT);
}

LightNode::~LightNode()
{
	ENN_SAFE_DELETE(light_);
}

void LightNode::setLightType(int type)
{
	if (light_ && light_->getLightType() != type)
	{
		ENN_SAFE_DELETE(light_);
	}

	if (!light_)
	{
		light_ = LightFactory::createLight(type);
		setSelfTransformDirty();
	}
}

bool  LightNode::intersects( const AxisAlignedBox& aabb ) const
{
	const_cast<LightNode*>(this)->checkUpdateTransform();
	return light_->intersect(aabb);
}

float LightNode::squaredDistance( const Vector3& pos ) const
{
	const_cast<LightNode*>(this)->checkUpdateTransform();
	return light_->squaredDistance(pos);
}

bool LightNode::checkUpdateTransform()
{
	if (SceneNode::checkUpdateTransform())
	{
		light_->setTransform(derived_matrix_);
		return true;
	}

	return false;
}

void LightNode::checkUpdateWorldAABB()
{
	checkUpdateTransform();
	if (dirty_flag_.test_flag(DF_WORLD_AABB))
	{
		world_aabb_ = light_->getAABB();
		dirty_flag_.reset_flag(DF_WORLD_AABB);
	}
}
}
