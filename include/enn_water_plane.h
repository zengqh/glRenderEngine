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
// Time: 2013/11/28
// File: enn_water_plane.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_node_def.h"

namespace enn
{

class RenderTarget;
class Mesh;
class TextureUnit;
struct WaterPlaneDesc
{
	float			plane;
	RenderTarget*	reflect_tex;
	Matrix4			reflect_view_proj;
};

class WaterPlane : public AllocatedObject
{
public:
	WaterPlane();
	virtual ~WaterPlane();

	void create_water(float w, float h);

public:
	void create_mesh(float w, float h);
	void create_mtr();

public:
	vec3f 				water_color_;
	vec2f 				water_speed_;
	vec2f 				water_scale_;
	vec2f 				water_offset_;
	vec2f 				water_rot_;
	vec2f 				flow_dir_;
	float				water_refract_turb_;
	float				water_reflect_turb_;
	float				water_refract_fade_;
	float				specular_power_;
	float				fresnel_power_;
	float				fresnel_scale_;
	String				water_name_;

	Mesh*				water_mesh_;
	WaterPlaneDesc*		water_plane_desc_;

	String				water_normal_tex_name_;
	TextureUnit*		normal_tex_;
};

}