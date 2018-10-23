/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/12
* File: SceneNode.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_node.h"
#include "enn_collision_shape.h"
#include "enn_rigid_body.h"

namespace enn
{
class SceneManager;
class IAddRenderable;
class LightsInfo;
class SceneNode : public Node
{
	ENN_DECLARE_RTTI(SceneNode)

public:
	SceneNode();
	virtual ~SceneNode();

public:

	virtual const AxisAlignedBox& getWorldAABB() const;

	virtual const AxisAlignedBox& getObjectAABB() const;

public:
	void setSceneManager(SceneManager* scene_mgr)
	{
		scene_mgr_ = scene_mgr;
	}

	SceneManager* getSceneManager() const
	{
		return scene_mgr_;
	}

public:
	CollisionShape* createCollisionShape();
	RigidBody* createRigidBody();
	
	CollisionShape* getCollisionShape() const;
	RigidBody* getRigidBody() const;

protected:
	virtual void setSelfTransformDirty();
	virtual void setParentTransformDirty();

	virtual void update();

protected:
	virtual void setWorldAABBDirty();
	virtual void checkUpdateWorldAABB();

protected:
	SceneManager*						scene_mgr_;
	mutable AxisAlignedBox				world_aabb_;
	mutable AxisAlignedBox				object_aabb_;

	CollisionShape*						collision_shape_;
	RigidBody*							rigid_body_;
};

class RenderableNode : public SceneNode
{
	ENN_DECLARE_RTTI(RenderableNode)
public:
	RenderableNode();

	virtual ~RenderableNode();

public:
	virtual void addToRender(IAddRenderable* ad) {}

	LightsInfo* getLightsInfo();

	void setCastShadow( bool en ) 
	{ 
		if (en)
		{
			features_flag_.set_flag(SHADOW_ENABLE_CAST); 
		}
		else
		{
			features_flag_.reset_flag(SHADOW_ENABLE_CAST); 
		}
	}

	bool isCastShadow() const 
	{ 
		return features_flag_.test_flag(SHADOW_ENABLE_CAST); 
	}

	void setReceiveShadow( bool en ) 
	{ 
		if (en)
		{
			features_flag_.set_flag(SHADOW_ENABLE_RECEIVE); 
		}
		else
		{
			features_flag_.reset_flag(SHADOW_ENABLE_RECEIVE); 
		}
	}

	bool isReceiveShadow() const 
	{ 
		return features_flag_.test_flag(SHADOW_ENABLE_RECEIVE); 
	}

protected:
	LightsInfo* light_info_;
	uint32		lights_info_update_;
};
}