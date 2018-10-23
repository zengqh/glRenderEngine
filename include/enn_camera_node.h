/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/18
* File: enn_camera_node.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_scene_node.h"
#include "enn_camera.h"

namespace enn
{
class Camera;
class CameraNode : public SceneNode
{
	ENN_DECLARE_RTTI(CameraNode)

public:
	CameraNode();
	virtual ~CameraNode();

public:
	Camera* getCamera() const { return m_camera; }

	bool testVisibility( const AxisAlignedBox& bound ) const;
	Frustum::Visibility testVisibilityEx( const AxisAlignedBox& bound ) const;

private:
	virtual bool checkUpdateTransform();
	virtual void checkUpdateWorldAABB();

private:
	Camera* m_camera;
};

}