/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/23
* File: enn_viewport.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_viewport.h"
#include "enn_camera.h"
#include "enn_gl_render_target.h"
#include "enn_root.h"

namespace enn
{
void Viewport::setParent(RenderTarget* rtt)
{
	parent_ = rtt;
}

RenderTarget* Viewport::getParent() const
{
	return parent_;
}

void Viewport::link_camera(Camera* cam)
{
	cam_ = cam;
	cam_->setViewport(this);
}

Camera* Viewport::getCamera() const
{
	return cam_;
}

void Viewport::apply()
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, m_clearColor);

	const IntRect& rect = getRect();

	glViewport(rect.left, rect.top, rect.getWidth(), rect.getHeight());
}

void Viewport::applyOnlyVP()
{
	const IntRect& rect = getRect();

	glViewport(rect.left, rect.top, rect.getWidth(), rect.getHeight());
}

}