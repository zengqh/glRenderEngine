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
// File: enn_post_system_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_post_gaussian_blur.h"
#include "enn_post_bloom.h"
#include "enn_post_dof.h"
#include "enn_post_god_ray.h"
#include "enn_post_motion_blur.h"

namespace enn
{

class PostSystemPass : public RenderPassBase
{
public:
	PostSystemPass(RenderPipe* render_pipe);
	virtual ~PostSystemPass();

	virtual void prepare();
	virtual void render(Camera* cam);

protected:
	void test_gaussian_blur();
	void test_bloom();
	void test_dof();
	void test_god_ray();
	void test_motion_blur();

protected:
	RenderTarget*		src_rtt_;
	RenderTarget*		dest_rtt_;

	Camera*				curr_cam_;

	PostGaussianblur*	gaussian_blur_;
	PostBloom*			post_bloom_;
	PostDOF*			post_dof_;
	PostGodray*			post_god_ray_;
	PostMotionBlur*		post_motion_blur_;
};

}