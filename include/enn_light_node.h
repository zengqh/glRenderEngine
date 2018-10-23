/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/10
* File: enn_light_node.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_scene_node.h"
#include "enn_light.h"

namespace enn
{
class LightNode : public SceneNode
{
	ENN_DECLARE_RTTI(LightNode)

public:
	LightNode();
	virtual ~LightNode();

public:
	void setLightType(int type);
	Light* getLigth() const
	{
		return light_;
	}

	template<class T>
	T* getLightAs() const { return static_cast<T*>(light_); }

	bool  intersects( const AxisAlignedBox& aabb ) const;
	float squaredDistance( const Vector3& pos ) const;

protected:
	virtual bool checkUpdateTransform();
	virtual void checkUpdateWorldAABB();

protected:
	Light*		light_;
};
}