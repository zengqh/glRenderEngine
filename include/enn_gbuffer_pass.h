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
// File: enn_gbuffer_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_gl_render_target.h"
#include "enn_helper_pass.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class GeneralRenderPass;
class GBufferPass : public RenderPassBase
{
public:
	GBufferPass(RenderPipe* owner);
	~GBufferPass();

public:
	virtual void prepare();
	virtual void render(Camera* cam);

public:
	TextureObj* getColor0() const;
	TextureObj* getColor1() const;

protected:
	void generateGBuffers(Camera* cam);
	void checkRTT(Viewport* vp);

	void debugRTT();

	RenderTarget*		rtt_;

	TextureObj*			color_0_;
	TextureObj*			color_1_;

	Viewport			rtt_view_port_;

	GeneralRenderPass*	general_render_pass_;

	DebugRTTDesc*		debug_color_0_;
	DebugRTTDesc*		debug_color_1_;
};

}