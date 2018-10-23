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
// Time: 2013/11/03
// File: enn_simple_ssao_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_mesh.h"
#include "enn_gl_program.h"
#include "enn_gl_render_target.h"
#include "enn_viewport.h"

namespace enn
{

class GBufferPass;
// A Simple and Practical Approach to SSAO
// http://www.gamedev.net/page/reference/index.html/_/technical/graphics-programming-and-theory/a-simple-and-practical-approach-to-ssao-r2753
class SPSSAOPass : public RenderPassBase
{
public:
	SPSSAOPass(RenderPipe* render_pipe);
	~SPSSAOPass();

public:
	virtual void prepare();
	virtual void render(Camera* cam);

	TextureObj* getMapAOBuffer();

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);
	String makeMacro(bool is_ps);
	void prepareMesh();
	void checkRTT(Viewport* vp);

	void doSSAO();

protected:
	MeshPtr				screen_mesh_;
	Viewport			rtt_view_port_;
	bool				is_prepared_;
	RenderTarget*		temp_rtt_;
	Program*			ssao_eff_;
	Camera*				curr_cam_;
	GBufferPass*		gbuffer_pass_;
	TextureObj*			normal_rtt_;
	TextureObj*			depth_rtt_;
	TextureUnit*		random_normal_tex_;
};

}