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
// Time: 2013/10/12
// File: enn_post_gaussian_blur.h
//

#pragma once

#include "enn_post_block.h"
#include "enn_mesh.h"
#include "enn_gl_program.h"
#include "enn_gl_render_target.h"

namespace enn
{

class PostGaussianblur : public PostBlock
{
public:
	PostGaussianblur();
	~PostGaussianblur();

public:
	virtual bool is_post_enable() const;
	virtual void prepare();
	virtual void render(RenderTarget* dest, RenderTarget* src, Camera* cam);

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);

	String makeMacro(bool is_ps);
	void prepareMesh();

protected:
	void processHBlur();
	void processVBlur();

protected:
	bool getFileData(const String& file_name, String& data);

protected:
	MeshPtr			screen_mesh_;
	Program*		render_eff_v_blur_;
	Program*		render_eff_h_blur_;
	bool			is_prepared_;
	RenderTarget*	curr_src_;
	RenderTarget*	curr_dest_;

	float			blur_weight_[13];
	vec2f			h_texel_kernel_[13];
	vec2f			v_texel_kernel_[13];

	Camera*			curr_cam_;
};

}