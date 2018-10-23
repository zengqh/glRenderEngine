//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/08/30
// File: enn_gbuffer_pass.cpp
//

#include "enn_gbuffer_pass.h"
#include "enn_root.h"
#include "enn_render_pipe.h"

namespace enn
{

GBufferPass::GBufferPass(RenderPipe* owner)
: RenderPassBase(owner)
, rtt_(0)
, color_0_(0)
, color_1_(0)
, general_render_pass_(owner->getGeneralRenderPass())
, debug_color_0_(0)
, debug_color_1_(0)
{

}

GBufferPass::~GBufferPass()
{
	
}

void GBufferPass::prepare()
{

}

void GBufferPass::render(Camera* cam)
{
	generateGBuffers(cam);
	debugRTT();
}

TextureObj* GBufferPass::getColor0() const
{
	return color_0_;
}

TextureObj* GBufferPass::getColor1() const
{
	return color_1_;
}

void GBufferPass::generateGBuffers(Camera* cam)
{
	Viewport* vp = cam->getViewport();
	checkRTT(vp);
	
	const RenderPipe::RenderableNodeList& main_render_results = render_pipe_->getMainRenderResults();

	rtt_->begin_render();

	

	GLenum DrawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	glDrawBuffers(2, DrawBuffers);

	

	general_render_pass_->begin_insert_obj(GeneralRenderPass::GR_DEFERRED);

	ENN_FOR_EACH_CONST(RenderPipe::RenderableNodeList, main_render_results, it)
	{
		RenderableNode* rn = *it;
		general_render_pass_->insert_obj(rn);
	}

	general_render_pass_->end_insert_obj();

	rtt_view_port_.apply();
	general_render_pass_->render(cam);
	
	rtt_->end_render();
}

void GBufferPass::checkRTT(Viewport* vp)
{
	int need_w, need_h;
	need_w = vp->getWidth();
	need_h = vp->getHeight();

	int w, h;
	w = rtt_view_port_.getWidth();
	h = rtt_view_port_.getHeight();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	if (!rtt_)
	{
		rtt_ = RenderTarget::createInstance();
	}

	if (!(w == need_w && h == need_h))
	{
		if (color_0_) rs->getRttPool()->destroyRTTBuffer(color_0_);
		if (color_1_) rs->getRttPool()->destroyRTTBuffer(color_1_);

		color_0_ = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);
		color_1_ = rs->getRttPool()->getRTTBuffer(F32_1, need_w, need_h);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, need_w, need_h);

		rtt_->link_rtt(0, color_0_);
		rtt_->link_rtt(1, color_1_);
		rtt_->link_depth(depth_temp->depth_);

		rtt_view_port_ = *vp;
		rtt_view_port_.setClearColor(Color::BLACK);

		checkGLError();
	}
}

void GBufferPass::debugRTT()
{
	HelperPass* helper_pass = render_pipe_->getHelperPass();

	if (!debug_color_0_)
	{
		debug_color_0_ = helper_pass->prepareDebugRTT(10, 390, 200, 200, color_0_);
	}

	if (!debug_color_1_)
	{
		debug_color_1_ = helper_pass->prepareDebugRTT(220, 390, 200, 200, color_1_);
	}

	debug_color_0_->tex = color_0_;
	debug_color_1_->tex = color_1_;

	helper_pass->addRenderDebugRTT(debug_color_0_);
	helper_pass->addRenderDebugRTT(debug_color_1_);
}

}