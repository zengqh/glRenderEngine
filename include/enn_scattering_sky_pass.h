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
// Time: 2013/11/22
// File: enn_scattering_sky_pass.h
//

#pragma once

#include "enn_render_pass_base.h"
#include "enn_mesh.h"

// reference from: http://www.gamedev.net/topic/584256-atmospheric-scattering-and-dark-sky/
// reference from: http://http.developer.nvidia.com/GPUGems2/gpugems2_chapter16.html
namespace enn
{

class Program;
class CameraNode;
class Mesh;
class SubMesh;
class TextureUnit;
class ScatteringSkyPass : public RenderPassBase
{
public:
	ScatteringSkyPass(RenderPipe* render_pipe);
	virtual ~ScatteringSkyPass();

	virtual void prepare();
	virtual void render(Camera* cam);

protected:
	void prepareMesh();
	void prepareEffect();

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);
	String makeMacro(bool is_ps);

protected:
	Program*				sky_eff_;
	MeshPtr					sky_sphere_mesh_;
	bool					is_prepared_;

	// sky params
	// Rayleigh scattering constant
	float					kr_;
	// Mie scattering constant
	float					km_;
	// Sun brightness constant
	float					sun_brightness_;

	float					rayleigh_scale_depth_;

	float					mie_phase_asymmetry_factor_;

	Vector3					wave_length_;
	Vector3					inv_wave_length_;

	float					kr_4pi_;

	float					km_4pi_;

	float					scale_;

	float					outer_radius_;
	float					inner_radius_;

	Camera*					curr_cam_;
};

}