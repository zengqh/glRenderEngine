/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_camera_node.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_camera_node.h"
#include "enn_camera.h"

namespace enn
{
	//////////////////////////////////////////////////////////////////////////
	CameraNode::CameraNode() : m_camera(ENN_NEW Camera)
	{
		type_flag_.set_flag( NODE_CAMERA );
	}

	CameraNode::~CameraNode()
	{
		ENN_DELETE m_camera;
	}

	bool CameraNode::checkUpdateTransform()
	{
		if ( SceneNode::checkUpdateTransform() )
		{
			m_camera->setTransform(derived_matrix_);
			return true;
		}

		return false;
	}

	void CameraNode::checkUpdateWorldAABB()
	{
		checkUpdateTransform();

		if ( dirty_flag_.test_flag(DF_WORLD_AABB) )
		{
			world_aabb_ = m_camera->getAABB();
			dirty_flag_.reset_flag( DF_WORLD_AABB );
		}
	}

	bool CameraNode::testVisibility( const AxisAlignedBox& bound ) const
	{
		const_cast<CameraNode*>(this)->checkUpdateTransform();
		return m_camera->testVisibility( bound );
	}

	Frustum::Visibility CameraNode::testVisibilityEx( const AxisAlignedBox& bound ) const
	{
		const_cast<CameraNode*>(this)->checkUpdateTransform();
		return m_camera->testVisibilityEx( bound );
	}
}