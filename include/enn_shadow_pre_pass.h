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
// File: enn_shadow_pre_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_light.h"
#include "enn_scene_find.h"

namespace enn
{
	
class GeneralRenderPass;
class ShadowPrePass : public RenderPassBase
{
	typedef SceneFindUtil::RenderableNodeList	RenderableNodeList;

public:
	ShadowPrePass(RenderPipe* render_pipe);
	~ShadowPrePass();

	virtual void render(Camera* cam);

	void setMainDirLit(Light* light);
	void setGeneralRender(GeneralRenderPass* general_render);
protected:
	void renderShadowMap(Camera* cam, Light* lit);


protected:
	Camera*							curr_cull_cam_;
	RenderableNodeList				shadow_split_render_results_;
	GeneralRenderPass*				forward_general_render_;
	Light*							main_dir_lit_;
	TextureObj*						shadow_rtt_;
};

}