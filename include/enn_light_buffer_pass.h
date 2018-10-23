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
// Time: 2013/09/07
// File: enn_light_buffer_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_light.h"
#include "enn_light_node.h"
#include "enn_helper_pass.h"

namespace enn
{


class LightBufferPass : public RenderPassBase
{
	typedef enn::list<LightNode*>::type		RALightList;

public:
	LightBufferPass(RenderPipe* render_pipe);
	virtual ~LightBufferPass();

public:
	void beginAdd();
	void addLight(LightNode* lit);
	void endAdd();

public:
	virtual void render(Camera* cam);

	TextureObj* getLitBuffer() const;

protected:
	void renderPointLits();
	void renderSpotLits();

protected:
	void checkRTT(Viewport* vp);
	void debugRTT();

protected:
	RALightList				point_lit_list_;
	RALightList				spot_lit_list_;
	Camera*					curr_cam_;
	MeshPtr					screen_mesh_;

	RenderTarget*			rtt_;
	TextureObj*				light_buffer_;
	Viewport				rtt_view_port_;

	DebugRTTDesc*			debug_lit_buf_rtt_;
};

}