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
// Time: 2013/09/07
// File: enn_light_buffer_pass.cpp
//

#include "enn_light_buffer_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

LightBufferPass::LightBufferPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, screen_mesh_(0)
, curr_cam_(0)
, rtt_(0)
, light_buffer_(0)
, debug_lit_buf_rtt_(0)
{

}

LightBufferPass::~LightBufferPass()
{

}

void LightBufferPass::beginAdd()
{
	point_lit_list_.clear();
	spot_lit_list_.clear();
}

void LightBufferPass::addLight(LightNode* lit)
{
	switch (lit->getLigth()->getLightType())
	{
	case LT_POINT:
		point_lit_list_.push_back(lit);
		break;
	case LT_SPOT:
		spot_lit_list_.push_back(lit);
		break;
	}
}

void LightBufferPass::endAdd()
{

}

void LightBufferPass::render(Camera* cam)
{
	curr_cam_ = cam;

	Viewport* vp = curr_cam_->getViewport();
	checkRTT(vp);

	rtt_->begin_render();
	rtt_view_port_.apply();

	renderPointLits();
	renderSpotLits();

	rtt_->end_render();

	debugRTT();
}

TextureObj* LightBufferPass::getLitBuffer() const
{
	return light_buffer_;
}

void LightBufferPass::renderPointLits()
{
	ENN_FOR_EACH(RALightList, point_lit_list_, it)
	{
		LightNode* lit = *it;
		PointLight* pt_lit = lit->getLightAs<PointLight>();
		Mesh* sphere_mesh = pt_lit->getVolume();
		

		// render lit buffer.
		LitAccEffectParams params;

		GBufferPass* gbuffer_pass = render_pipe_->getGBufferPass();
		params.color0_ = gbuffer_pass->getColor0();
		params.color1_ = gbuffer_pass->getColor1();
		params.pt_light_ = pt_lit;
		params.curr_render_cam_ = curr_cam_;

		RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_LITACC);
		EffectContext* ec = eff_template->getEffectContext(0);

		rs->setProgram(ec->eff_);

		VertexBuffer* vb = sphere_mesh->getSubMesh(0)->getVB();
		IndexBuffer* ib = sphere_mesh->getSubMesh(0)->getIB();

		rs->setVB(vb);
		rs->setIB(ib);

		eff_template->doSetParams(ec, 0, &params);

		uint32 element_mask = vb->getElementMask();

		rs->setFVF(element_mask, true);

		rs->drawIndexedPrimitives(PRIM_TRIANGLELIST, 0, sphere_mesh->getSubMesh(0)->getIndexCount(), true);

		rs->setFVF(element_mask, false);
	}
}

void LightBufferPass::renderSpotLits()
{
	ENN_FOR_EACH(RALightList, spot_lit_list_, it)
	{
		LightNode* lit = *it;
		SpotLight* spot_lit = lit->getLightAs<SpotLight>();
		Mesh* cone_mesh = spot_lit->getVolume();

		// render lit buffer.
		LitAccEffectParams params;

		GBufferPass* gbuffer_pass = render_pipe_->getGBufferPass();
		params.color0_ = gbuffer_pass->getColor0();
		params.color1_ = gbuffer_pass->getColor1();
		params.curr_render_cam_ = curr_cam_;

		RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_LITACC);
		EffectContext* ec = eff_template->getEffectContext(0);

		rs->setProgram(ec->eff_);

		VertexBuffer* vb = cone_mesh->getSubMesh(0)->getVB();
		IndexBuffer* ib = cone_mesh->getSubMesh(0)->getIB();

		rs->setVB(vb);
		rs->setIB(ib);

		eff_template->doSetParams(ec, 0, &params);

		uint32 element_mask = vb->getElementMask();

		rs->setFVF(element_mask, true);

		rs->drawIndexedPrimitives(PRIM_TRIANGLELIST, 0, cone_mesh->getSubMesh(0)->getIndexCount(), true);

		rs->setFVF(element_mask, false);
	}
}

void LightBufferPass::checkRTT(Viewport* vp)
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
		if (light_buffer_) rs->getRttPool()->destroyRTTBuffer(light_buffer_);

		light_buffer_ = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D16, need_w, need_h);

		rtt_->link_rtt(0, light_buffer_);
		rtt_->link_depth(depth_temp->depth_);

		rtt_view_port_ = *vp;
		rtt_view_port_.setClearColor(Color::BLACK);
	}
}

void LightBufferPass::debugRTT()
{
	HelperPass* helper_pass = render_pipe_->getHelperPass();
	if (!debug_lit_buf_rtt_)
	{
		debug_lit_buf_rtt_ = helper_pass->prepareDebugRTT(450, 390, 200, 200, light_buffer_);
	}

	debug_lit_buf_rtt_->tex = light_buffer_;

	helper_pass->addRenderDebugRTT(debug_lit_buf_rtt_);
}

}