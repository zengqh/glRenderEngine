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
// File: enn_water_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_water_plane_node.h"

namespace enn
{

class WaterPass : public RenderPassBase
{
	typedef enn::vector<RenderableNode*>::type		WaterPlaneList;
	typedef enn::vector<WaterPlaneDesc>::type		WaterPlaneDescList;
	typedef enn::vector<RenderTarget*>::type		WaterReflectRTTList;

public:
	WaterPass(RenderPipe* render_pipe);
	~WaterPass();

	virtual void prepare();
	virtual void render(Camera* cam);

	void begin_insert_obj();
	void insert_obj(RenderableNode* node);
	void end_insert_obj();

public:
	void render_reflect();
	void render_water();

protected:
	WaterPlaneDesc* insert_plane(float plane);

	void render_water_obj(WaterPlaneNode* water_node);
	void render_plane(WaterPlaneDesc* plane);

	void check_reflect_rtt();
	void set_water_params(WaterPlaneNode* water_node);
	void calc_rot_offset(int tick, WaterPlane* water, int i, Vector4& rot, Vector2* offset);

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);
	String makeMacro(bool is_ps);

protected:
	WaterPlaneList				water_plane_list_;
	WaterPlaneDescList			water_plane_desc_list_;
	WaterReflectRTTList			water_reflect_rtt_list_;

	Camera*						main_cam_;
	Viewport					reflect_vp_;

	Program*					water_eff_;
	TextureObj*					refract_tex_;
	bool						is_prepared_;
};

}