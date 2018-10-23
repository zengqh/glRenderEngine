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
// File: enn_helper_pass.h
//

#pragma once

#include "enn_render_pass_base.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct DebugRTTDesc
{
	int x, y, w, h;
	TextureObj* tex;
	Mesh* screen_mesh;
};

//////////////////////////////////////////////////////////////////////////
class HelperPass : public RenderPassBase
{	
public:
	typedef enn::list<DebugRTTDesc*>::type		DebugRTTList;

public:
	HelperPass(RenderPipe* render_pipe);
	~HelperPass();

	virtual void prepare();
	virtual void render(Camera* cam);

	DebugRTTDesc* prepareDebugRTT(int x, int y, int w, int h, TextureObj* tex);
	void freeDebugRTT(DebugRTTDesc*& rtt);
	void addRenderDebugRTT(DebugRTTDesc* rtt);
	void renderDebugRTT(DebugRTTDesc* rtt);

protected:
	DebugRTTList			debug_rtt_list_;
	Mesh*					screen_temp_mesh_;
	Camera*					curr_cam_;
};

}