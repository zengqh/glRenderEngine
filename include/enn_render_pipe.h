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
// File: enn_render_pipe.h
//

#pragma once

#include "enn_render_pipe.h"
#include "enn_begin_pass.h"
#include "enn_end_pass.h"
#include "enn_helper_pass.h"
#include "enn_pre_z_pass.h"
#include "enn_shadow_pre_pass.h"
#include "enn_skybox_pass.h"
#include "enn_skydome_pass.h"
#include "enn_general_render_pass.h"
#include "enn_water_pass.h"
#include "enn_gbuffer_pass.h"
#include "enn_light_buffer_pass.h"
#include "enn_plane_reflect_pass.h"
#include "enn_post_system_pass.h"
#include "enn_simple_ssao_pass.h"
#include "enn_scattering_sky_pass.h"
#include "enn_scene_manager.h"


namespace enn
{

class CameraNode;
class LightsInfo;
class RenderPipe : public AllocatedObject, public Noncopyable
{
public:
	typedef SceneFindUtil::RenderableNodeList	RenderableNodeList;
	typedef SceneFindUtil::LightNodeList		LightNodeList;

public:
	enum
	{
		FORWARD,
		DEFERRED_LIGHTING
	};

public:
	RenderPipe();
	virtual ~RenderPipe();

	virtual void render(CameraNode* cam);
	virtual void queryLightInfo(const AxisAlignedBox& world_aabb, LightsInfo& litsInfo, bool check_sm) const;

public:
	EffectTemplateManager* getEffTemplateManager() const
	{
		return eff_template_mgr_;
	}

	GeneralRenderPass* getGeneralRenderPass() const
	{
		return general_obj_pass_;
	}

	HelperPass* getHelperPass() const
	{
		return helper_pass_;
	}

	GBufferPass* getGBufferPass() const
	{
		return gbuffer_pass_;
	}

	LightBufferPass* getLitBufferPass() const
	{
		return lit_buffer_pass_;
	}

	SPSSAOPass* getSPSSAOPass() const
	{
		return sp_ssao_pass_;
	}

	const RenderableNodeList& getMainRenderResults() const
	{
		return main_render_results_;
	}

	Light* getMainDirLit() const;

	RenderTarget* getPost0() const
	{
		return post0_;
	}

	RenderTarget* getPost1() const
	{
		return post1_;
	}

	void find_scene(Camera* cam, RenderableNodeList& main_results, LightNodeList& light_results, const Flags32& flag);

	void render_reflect(Camera* cam);

protected:
	void dispatch_render_nodes(uint32 pass);
	void dispatch_skybox(SkyboxSceneData* skybox);
	void dispatch_shadow_pre();
	void dispatch_light_buffer();
	void dispatch_water_plane();

protected:
	Light* fetchMainDirLit(const LightNodeList& lits) const;

protected:
	void registerEffectTemplate();
	void checkBackRTT(Camera* cam);

protected:
	RenderableNodeList				main_render_results_;
	LightNodeList					main_light_results_;

	BeginPass*						begin_pass_;
	EndPass*						end_pass_;
	GeneralRenderPass*				general_obj_pass_;
	HelperPass*						helper_pass_;
	PrezPass*						pre_z_pass_;
	ShadowPrePass*					shadow_pre_pass_;
	SkyboxPass*						skybox_pass_;
	SkydomePass*					skydome_pass_;
	WaterPass*						water_pass_;
	GBufferPass*					gbuffer_pass_;
	LightBufferPass*				lit_buffer_pass_;
	PostSystemPass*					post_system_pass_;
	SPSSAOPass*						sp_ssao_pass_;
	ScatteringSkyPass*				scattering_sky_pass_;

	EffectTemplateManager*			eff_template_mgr_;
	SceneManager*					scene_mgr_;

	uint32							render_pipe_type_;

	RenderTarget*					post0_;
	RenderTarget*					post1_;
	TextureObj*						post0_color_;
	TextureObj*						post1_color_;
	Viewport						post_viewport_;
};

}