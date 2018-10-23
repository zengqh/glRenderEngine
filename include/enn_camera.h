/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_camera.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_frustum.h"
#include "enn_viewport.h"

namespace enn
{
	class Viewport;
	class Camera : public Frustum
	{
	public:
		Camera() : m_viewport(0) {}

		virtual ~Camera() 
		{
			ENN_SAFE_DELETE(m_viewport);
		}

	public:
		void setViewport( Viewport* vp ) { m_viewport = vp; }
		Viewport* getViewport() const { return m_viewport; }

public:
	Ray getScreenRay(float screenX, float screenY);
	void getScreenRay(float screenX, float screenY, Ray* outRay);

	bool screenToWorldPositionClipGround(float screenX, float screenY, Vector3& out_pos);
	Vector2 worldPositionToScreen(const Vector3& world_pos);

	protected:
		Viewport* m_viewport;
	};
}