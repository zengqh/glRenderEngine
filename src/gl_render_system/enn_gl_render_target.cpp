/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/23
* File: enn_gl_render_target.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_render_target.h"
#include "enn_gl_texture_obj.h"
#include "enn_gl_render_buffer.h"
#include "enn_gl_render_system.h"
#include "enn_root.h"

namespace enn
{
RenderTarget* RenderTarget::createInstance()
{
	return ENN_NEW RenderTarget();
}

void  RenderTarget::release()
{
	ENN_DELETE this;
}

RenderTarget::RenderTarget()
: depth_(0)
, is_screen_rtt_(false)
{
	ENN_CLEAR_ZERO(rtt_);
}

RenderTarget::~RenderTarget()
{
	clearAllViewports();
}

Viewport* RenderTarget::createViewport(int z_order)
{
	if (view_ports_.find(z_order) != view_ports_.end())
	{
		ENN_ASSERT(0);
		return 0;
	}

	Viewport* vp = ENN_NEW Viewport;
	vp->setZOrder(z_order);
	view_ports_[z_order] = vp;

	return vp;
}

Viewport* RenderTarget::getOrCreateViewport(int z_order)
{
	Viewport*& vp = view_ports_[z_order];
	if (vp)
	{
		return vp;
	}

	vp = ENN_NEW Viewport;
	vp->setZOrder(z_order);
	return vp;
}

Viewport* RenderTarget::getViewport(int z_order) const
{
	ViewportMap::const_iterator it = view_ports_.find(z_order);
	if (it != view_ports_.end())
	{
		return it->second;
	}

	return 0;
}

void RenderTarget::destroyViewport(int z_order)
{
	ViewportMap::const_iterator it = view_ports_.find(z_order);
	if (it != view_ports_.end())
	{
		Viewport* vp = it->second;
		ENN_SAFE_DELETE(vp);

		view_ports_.erase(it);
	}
}

void RenderTarget::clearAllViewports()
{
	ENN_FOR_EACH(ViewportMap, view_ports_, it)
	{
		Viewport* vp = it->second;
		ENN_SAFE_DELETE(vp);
	}

	view_ports_.clear();
}

RenderTarget::Iterator RenderTarget::getViewports()
{
	return Iterator(view_ports_);
}

int RenderTarget::getViewportCounts() const
{
	return (int)view_ports_.size();
}

int RenderTarget::getWidth() const
{
	return rtt_[0] ? rtt_[0]->getWidth() : 0;
}

int RenderTarget::getHeight() const
{
	return rtt_[0] ? rtt_[0]->getHeight() : 0;
}

void RenderTarget::link_rtt(TextureObj* rtt)
{
	link_rtt(0, rtt);
}

void RenderTarget::link_rtt(int idx, TextureObj* rtt)
{
	ENN_ASSERT(0 <= idx && idx < MAX_FRAME_BUFFER_CHANNEL);

	rtt_[idx] = rtt;
}

void RenderTarget::link_depth(RenderBuffer* depth)
{
	depth_ = depth;
}

TextureObj* RenderTarget::get_rtt(int idx) const
{
	ENN_ASSERT(0 <= idx && idx < MAX_FRAME_BUFFER_CHANNEL);
	return rtt_[idx];
}

RenderBuffer* RenderTarget::get_depth() const
{
	return depth_;
}

void RenderTarget::begin_render()
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->push_active_render_target();

	rs->set_render_target(this);
}

void RenderTarget::end_render()
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->pop_render_target();
}

void RenderTarget::setScreenRTT(int w, int h)
{
	screen_width_ = w;
	screen_height_ = h;
	is_screen_rtt_ = true;
}

bool RenderTarget::isScreenRTT() const
{
	return is_screen_rtt_;
}

}