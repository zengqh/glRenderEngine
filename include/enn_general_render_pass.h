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
// File: enn_general_render_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_scene_node.h"
#include "enn_effect_template_manager.h"

namespace enn
{

class Light;
class GeneralRenderPass : public RenderPassBase
{
	typedef enn::vector<RenderableNode*>::type GeneralObjList;
	typedef enn::vector<RenderableNode*>::type ShadowObjList;
	typedef enn::vector<RenderableNode*>::type GBufferObjList;

public:
	enum
	{
		GR_MATERIAL,
		GR_SHADOW,
		GR_DEFERRED,
		GR_FORWARD
	};

public:
	GeneralRenderPass(RenderPipe* render_pipe);
	~GeneralRenderPass();

	virtual void render(Camera* cam);

	void render_mtr();
	void render_shadow();
	void render_gbuffer();
	void render_forward();

	void prepare(Camera* cam);
	void render_solid();
	void render_alpha();

	void begin_insert_obj(uint32 pass);
	void insert_obj(RenderableNode* node);
	void end_insert_obj();

	void dispatch_general_obj();
	void dispatch_shadow_obj();
	void dispatch_gbuffer_obj();
	void dispatch_forward_obj();

	bool isShadowPass() const;
	bool isMateriallPass() const;
	bool isGBufferPass() const;
	bool isForwardPass() const;

	void renderSolidObjs();
	void renderAlphaObjs();
	void renderShadowObj();
	void renderGBufferObj();

	void renderForwardSolidObjs();
	void renderForwardAlphaObjs();

	virtual void addRenderable(Renderable* ra);

public:
	void render_single_general_mesh(RenderableNode* node, Camera* cam);
	void render_single_raw_mesh(Mesh* mesh);

protected:
	void addRenderable_mtr_pass(Renderable* ra);
	void addRenderable_shadow_pass(Renderable* ra);
	void addRenderable_gbuffer_pass(Renderable* ra);
	void addRenderable_forward_pass(Renderable* ra);

protected:
	void doSolidRenderable(Renderable* ra);
	void doAlphaRenderable(Renderable* ra);

	void apply_fvf(Renderable* ra, bool en);

protected:
	uint32						curr_render_pass_;

	/** material */
	GeneralObjList				general_obj_list_;
	KindRenderableList			solid_ra_list_;
	RenderableContextList		alpha_ra_list_;

	// shadow
	ShadowObjList				shadow_obj_list_;
	KindRenderableList			shadow_ra_list_;

	// deferred pre gbuffer
	GBufferObjList				gbuffer_obj_list_;
	KindRenderableList			gbuffer_ra_list_;

	// forward
	GeneralObjList				forward_obj_list_;
	KindRenderableList			forward_solid_ra_list_;
	RenderableContextList		forward_alpha_ra_list_;

	Camera*						curr_render_cam_;
};

}