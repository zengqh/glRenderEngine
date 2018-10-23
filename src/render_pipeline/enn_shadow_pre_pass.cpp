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
// Time: 2013/08/21
// File: enn_shadow_pre_pass.cpp
//

#include "enn_shadow_pre_pass.h"
#include "enn_gl_rtt_pool.h"
#include "enn_root.h"
#include "enn_gl_render_target.h"
#include "enn_render_pipe.h"

namespace enn
{

ShadowPrePass::ShadowPrePass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, curr_cull_cam_(0)
, forward_general_render_(0)
, main_dir_lit_(0)
, shadow_rtt_(0)
{

}

ShadowPrePass::~ShadowPrePass()
{

}

void ShadowPrePass::render(Camera* cam)
{
	if (!main_dir_lit_)
	{
		return;
	}

	renderShadowMap(cam, main_dir_lit_);
}

void ShadowPrePass::setMainDirLit(Light* light)
{
	main_dir_lit_ = light;
}

void ShadowPrePass::setGeneralRender(GeneralRenderPass* general_render)
{
	forward_general_render_ = general_render;
}

void ShadowPrePass::renderShadowMap(Camera* cam, Light* lit)
{
	lit->updateShadowInfo(cam);

	if (!lit->isShadowCurrActive())
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	ShadowInfo* si = lit->getShadowInfo();

	int rtt_size = si->need_rtt_size_;

	if (!shadow_rtt_) 
	{
		shadow_rtt_ = rs->getRttPool()->getRTTBuffer(F32_1, rtt_size, rtt_size);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, rtt_size, rtt_size);

		si->link_rtt(shadow_rtt_, depth_temp);
	}

	/** render each split */
	RenderTarget* render_target = si->render_target_;
	RenderTarget::Iterator itvp = render_target->getViewports();

	/** bind rtt */
	render_target->begin_render();
	rs->enableBlend(false);

	SceneManager* sm = Root::getSingletonPtr()->getSceneManager();

	bool is_first = true;

	while (itvp.hasNext())
	{
		Viewport* vp = itvp.get().second;

		curr_cull_cam_ = si->getCullCaster() + vp->getZOrder();

		SceneFindShadowCasterByCamera finder;

		finder.find(sm, curr_cull_cam_, &shadow_split_render_results_);

		forward_general_render_->begin_insert_obj(GeneralRenderPass::GR_SHADOW);

		ENN_FOR_EACH(RenderableNodeList, shadow_split_render_results_, it)
		{
			RenderableNode* rn = *it;
			forward_general_render_->insert_obj(rn);
		}

		forward_general_render_->end_insert_obj();

		if (is_first)
		{
			vp->apply();
			is_first = false;
		}
		else
		{
			vp->applyOnlyVP();
		}

		forward_general_render_->render(vp->getCamera());
	}

	render_target->end_render();
	rs->restoreMainViewport();
}

}