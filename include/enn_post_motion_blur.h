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
// Time: 2013/11/25
// File: enn_post_motion_blur.h
//

#pragma once

#include "enn_post_block.h"
#include "enn_mesh.h"
#include "enn_gl_program.h"
#include "enn_gl_render_target.h"
#include "enn_viewport.h"

namespace enn
{

class PostMotionBlur : public PostBlock
{
public:
	PostMotionBlur();
	virtual ~PostMotionBlur();

	virtual void prepare();
	virtual void render(RenderTarget* dest, RenderTarget* src, Camera* cam);

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);
	String makeMacro(bool is_ps);

protected:
	uint32			num_of_samples_;
	float			motion_blur_factor_;
	float			target_fps_;

	Camera*			curr_cam_;
	RenderTarget*	rtt_src_;
	RenderTarget*	rtt_dst_;
	TextureObj*		depth_obj_;

	Program*		eff_;
	MeshPtr			screen_mesh_;
	bool			is_prepared_;
	bool			is_mat_first_;

	Matrix4			prevWorldToClipSpaceMat;
};

}