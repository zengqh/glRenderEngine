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
// File: enn_post_system_pass.cpp
//

#include "enn_post_system_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

PostSystemPass::PostSystemPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, src_rtt_(0)
, dest_rtt_(0)
, curr_cam_(0)
, gaussian_blur_(0)
, post_bloom_(0)
, post_motion_blur_(0)
{

}

PostSystemPass::~PostSystemPass()
{

}

void PostSystemPass::prepare()
{
	if (!src_rtt_)
	{
		src_rtt_ = render_pipe_->getPost0();
	}

	if (!dest_rtt_)
	{
		dest_rtt_ = render_pipe_->getPost1();
	}

	if (!gaussian_blur_)
	{
		gaussian_blur_ = ENN_NEW PostGaussianblur();
	}

	if (!post_bloom_)
	{
		post_bloom_ = ENN_NEW PostBloom();
	}

	if (!post_dof_)
	{
		post_dof_ = ENN_NEW PostDOF();
	}

	if (!post_god_ray_)
	{
		post_god_ray_ = ENN_NEW PostGodray();
	}

	if (!post_motion_blur_)
	{
		post_motion_blur_ = ENN_NEW PostMotionBlur();
	}
}

void PostSystemPass::render(Camera* cam)
{
	curr_cam_ = cam;
	test_bloom();
	//test_dof();
	//test_motion_blur();
}

void PostSystemPass::test_gaussian_blur()
{
	gaussian_blur_->prepare();
	gaussian_blur_->render(dest_rtt_, src_rtt_, curr_cam_);
}

void PostSystemPass::test_bloom()
{
	post_bloom_->prepare();
	post_bloom_->render(dest_rtt_, src_rtt_, curr_cam_);
}

void PostSystemPass::test_dof()
{
	post_dof_->prepare();
	post_dof_->render(src_rtt_, dest_rtt_, curr_cam_);
}

void PostSystemPass::test_god_ray()
{
	post_god_ray_->prepare();
	post_god_ray_->render(src_rtt_, dest_rtt_, curr_cam_);
}

void PostSystemPass::test_motion_blur()
{
	post_motion_blur_->prepare();
	post_motion_blur_->render(src_rtt_, dest_rtt_, curr_cam_);
}

}