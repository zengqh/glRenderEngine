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
// Time: 2013/08/29
// File: enn_render_pipe.cpp
//

#include "enn_render_pipe.h"
#include "enn_camera_node.h"
#include "enn_light_node.h"
#include "enn_scene_find.h"
#include "enn_root.h"

namespace enn
{

RenderPipe::RenderPipe()
	: scene_mgr_(0)
	, post0_color_(0)
	, post1_color_(0)
	, post0_(0)
	, post1_(0)
{
	render_pipe_type_ = FORWARD;

	begin_pass_ = ENN_NEW BeginPass(this);
	end_pass_ = ENN_NEW EndPass(this);
	general_obj_pass_ = ENN_NEW GeneralRenderPass(this);
	helper_pass_ = ENN_NEW HelperPass(this);
	pre_z_pass_ = ENN_NEW PrezPass(this);
	shadow_pre_pass_ = ENN_NEW ShadowPrePass(this);
	skybox_pass_ = ENN_NEW SkyboxPass(this);
	skydome_pass_ = ENN_NEW SkydomePass(this);
	water_pass_ = ENN_NEW WaterPass(this);
	gbuffer_pass_ = ENN_NEW GBufferPass(this);
	lit_buffer_pass_ = ENN_NEW LightBufferPass(this);
	post_system_pass_ = ENN_NEW PostSystemPass(this);
	sp_ssao_pass_ = ENN_NEW SPSSAOPass(this);
	scattering_sky_pass_ = ENN_NEW ScatteringSkyPass(this);

	eff_template_mgr_ = ENN_NEW EffectTemplateManager();
	registerEffectTemplate();
}

RenderPipe::~RenderPipe()
{
	ENN_SAFE_DELETE(begin_pass_);
	ENN_SAFE_DELETE(end_pass_);
	ENN_SAFE_DELETE(general_obj_pass_);
	ENN_SAFE_DELETE(helper_pass_);
	ENN_SAFE_DELETE(pre_z_pass_);
	ENN_SAFE_DELETE(shadow_pre_pass_);
	ENN_SAFE_DELETE(skybox_pass_);
	ENN_SAFE_DELETE(skydome_pass_);
	ENN_SAFE_DELETE(water_pass_);
	ENN_SAFE_DELETE(gbuffer_pass_);
	ENN_SAFE_DELETE(lit_buffer_pass_);
	ENN_SAFE_DELETE(post_system_pass_);
	ENN_SAFE_DELETE(scattering_sky_pass_);

	ENN_SAFE_DELETE(eff_template_mgr_);
}

void RenderPipe::render(CameraNode* node)
{
	scene_mgr_ = node->getSceneManager();

	Camera* cam = node->getCamera();

	SceneFindCameraUtil finder;

	Flags32 find_flag;
	find_flag.set_flag(SceneFindOption::MK_RENDER);
	find_flag.set_flag(SceneFindOption::MK_LIGHT);

	finder.find(scene_mgr_, cam, &main_render_results_, &main_light_results_, find_flag);

	checkBackRTT(cam);

	/*
	dispatch_skybox(scene_mgr_->getSkyboxData());
	skybox_pass_->prepare();
	*/
	water_pass_->prepare();

	dispatch_water_plane();
	water_pass_->render(cam);
	water_pass_->render_reflect();

	scattering_sky_pass_->prepare();

	helper_pass_->prepare();

	begin_pass_->render(cam);
	pre_z_pass_->render(cam);

	/*
	skybox_pass_->render(cam);
	*/

	scattering_sky_pass_->render(cam);

	skydome_pass_->render(cam);

	dispatch_shadow_pre();
	shadow_pre_pass_->render(cam);

	gbuffer_pass_->render(cam);

	dispatch_light_buffer();
	lit_buffer_pass_->render(cam);

	sp_ssao_pass_->prepare();
	sp_ssao_pass_->render(cam);

	dispatch_render_nodes(GeneralRenderPass::GR_MATERIAL);
	general_obj_pass_->render(cam);

	water_pass_->render_water();

	post_system_pass_->prepare();
	post_system_pass_->render(cam);

	helper_pass_->render(cam);

	end_pass_->prepare();
	end_pass_->render(cam);
}

void RenderPipe::queryLightInfo(const AxisAlignedBox& world_aabb, LightsInfo& litsInfo, bool check_sm) const
{
	litsInfo.clear();
	Vector3 centre_pos = world_aabb.getCenter();

	ENN_FOR_EACH_CONST(LightNodeList, main_light_results_, it)
	{
		LightNode* node = *it;
		if (node->intersects(world_aabb))
		{
			float dist = node->squaredDistance(centre_pos);

			if (check_sm)
			{
				if (node->getLigth()->isShadowCurrActive())
				{
					check_sm = node->getLigth()->getShadowInfo()->getCullReceiver()->testVisibility(world_aabb);
				}
				else
				{
					check_sm = false;
				}
			}

			litsInfo.addLight(node->getLigth(), dist, check_sm);
		}
	}
}

void RenderPipe::find_scene(Camera* cam, RenderableNodeList& main_results, 
							LightNodeList& light_results, const Flags32& flag)
{
	SceneFindCameraUtil finder;
	finder.find(scene_mgr_, cam, &main_results, &light_results, flag);
}

void RenderPipe::render_reflect(Camera* cam)
{
	RenderPipe::RenderableNodeList	ra_list;
	RenderPipe::LightNodeList		lit_list;

	Flags32 find_flag;
	find_flag.set_flag(SceneFindOption::MK_RENDER);
	find_flag.set_flag(SceneFindOption::MK_LIGHT);

	find_scene(cam, ra_list, lit_list, find_flag);

	scattering_sky_pass_->prepare();
	scattering_sky_pass_->render(cam);

	dispatch_render_nodes(GeneralRenderPass::GR_FORWARD);
	general_obj_pass_->render(cam);
}

void RenderPipe::dispatch_render_nodes(uint32 pass)
{
	general_obj_pass_->begin_insert_obj(pass);

	ENN_FOR_EACH(RenderableNodeList, main_render_results_, it)
	{
		RenderableNode* rn = *it;

		if (rn->isMeshNode())
		{
			MeshNode* mn = static_cast<MeshNode*>(rn);
			general_obj_pass_->insert_obj(mn);
		}
	}

	general_obj_pass_->end_insert_obj();
}

void RenderPipe::dispatch_skybox(SkyboxSceneData* skybox)
{
	if (!skybox_pass_->getRenderSceneData())
	{
		skybox_pass_->setRenderSceneData(skybox);
	}
}

void RenderPipe::dispatch_shadow_pre()
{
	Light* lit = fetchMainDirLit(main_light_results_);
	shadow_pre_pass_->setMainDirLit(lit);
	shadow_pre_pass_->setGeneralRender(general_obj_pass_);
}

void RenderPipe::dispatch_light_buffer()
{
	lit_buffer_pass_->beginAdd();
	ENN_FOR_EACH(LightNodeList, main_light_results_, it)
	{
		LightNode* ln = *it;
		
		lit_buffer_pass_->addLight(ln);
	}
	lit_buffer_pass_->endAdd();
}

void RenderPipe::dispatch_water_plane()
{
	water_pass_->begin_insert_obj();

	ENN_FOR_EACH(RenderableNodeList, main_render_results_, it)
	{
		RenderableNode* rn = *it;

		if (rn->isWaterPlaneNode())
		{
			WaterPlaneNode* mn = static_cast<WaterPlaneNode*>(rn);
			water_pass_->insert_obj(mn);
		}
	}

	general_obj_pass_->end_insert_obj();
}

Light* RenderPipe::fetchMainDirLit(const LightNodeList& lits) const
{
	ENN_FOR_EACH_CONST(LightNodeList, lits, it)
	{
		LightNode* ln = *it;
		Light* lit = ln->getLigth();

		if (lit->getLightType() == LT_DIRECTIONAL)
		{
			return lit;
		}
	}

	return 0;
}

Light* RenderPipe::getMainDirLit() const
{
	return fetchMainDirLit(main_light_results_);
}

void RenderPipe::registerEffectTemplate()
{
	eff_template_mgr_->registerEffectTemplate(ET_DEF_MATERIAL, ENN_NEW DefaultEffectTemplate(eff_template_mgr_));
	eff_template_mgr_->registerEffectTemplate(ET_DEF_SHADOW, ENN_NEW ShadowpreEffectTemplate(eff_template_mgr_));
	eff_template_mgr_->registerEffectTemplate(ET_DEF_DEFPRE, ENN_NEW GBufferEffectTemplate(eff_template_mgr_));
	eff_template_mgr_->registerEffectTemplate(ET_DEF_UI, ENN_NEW UIRectEffectTemplate(eff_template_mgr_));
	eff_template_mgr_->registerEffectTemplate(ET_DEF_LITACC, ENN_NEW LitAccEffectTemplate(eff_template_mgr_));
	eff_template_mgr_->registerEffectTemplate(ET_DEF_FORWARD, ENN_NEW ForwardEffectTemplate(eff_template_mgr_));
}

void RenderPipe::checkBackRTT(Camera* cam)
{
	Viewport* vp = cam->getViewport();

	int need_w, need_h;
	need_w = vp->getWidth();
	need_h = vp->getHeight();

	int w, h;
	w = post_viewport_.getWidth();
	h = post_viewport_.getHeight();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	if (!post0_)
	{
		post0_ = RenderTarget::createInstance();
	}

	if (!post1_)
	{
		post1_ = RenderTarget::createInstance();
	}

	if (!(w == need_w && h == need_h))
	{
		if (post0_color_) rs->getRttPool()->destroyRTTBuffer(post0_color_);
		if (post1_color_) rs->getRttPool()->destroyRTTBuffer(post1_color_);

		post0_color_ = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);
		post1_color_ = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, need_w, need_h);

		post0_->link_rtt(0, post0_color_);
		post0_->link_depth(depth_temp->depth_);

		post1_->link_rtt(0, post1_color_);
		post1_->link_depth(depth_temp->depth_);

		post_viewport_ = *vp;

		checkGLError();
	}
}

}