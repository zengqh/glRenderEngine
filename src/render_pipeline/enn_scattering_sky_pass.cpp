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
// File: enn_scattering_sky_pass.cpp
//

#include "enn_scattering_sky_pass.h"
#include "enn_root.h"

namespace enn
{

static const float EARTH_RADIUS = (6378.0f * 1000.0f);
static const float EARTH_ATMOSPHERE_RADIUS = EARTH_RADIUS * 1.015f;

ScatteringSkyPass::ScatteringSkyPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, sky_eff_(0)
, is_prepared_(false)
{
	kr_ = 0.0025f;
	km_ = 0.0015f;
	sun_brightness_ = 18.0f;
	mie_phase_asymmetry_factor_ = -0.98f;
	wave_length_.setValue(0.650f, 0.570f, 0.475f);
	outer_radius_ = EARTH_ATMOSPHERE_RADIUS;
	inner_radius_ = EARTH_RADIUS;
	scale_ = 1.0f / (outer_radius_ - inner_radius_);

	kr_4pi_ = kr_ * 4 * Math::PI;
	km_4pi_ = km_ * 4 * Math::PI;

	inv_wave_length_.x = 1.0f / Math::pow(wave_length_.x, 4.0f);
	inv_wave_length_.y = 1.0f / Math::pow(wave_length_.y, 4.0f);
	inv_wave_length_.z = 1.0f / Math::pow(wave_length_.z, 4.0f);

	rayleigh_scale_depth_ = 0.25f;
}

ScatteringSkyPass::~ScatteringSkyPass()
{
	
}

void ScatteringSkyPass::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	prepareMesh();
	prepareEffect();

	is_prepared_ = true;
}

void ScatteringSkyPass::render(Camera* cam)
{
	curr_cam_ = cam;

	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setFrontFace(false);

	rs->setProgram(sky_eff_);

	SubMesh* sky_sub_mesh = sky_sphere_mesh_->getSubMesh(0);
	VertexBuffer* vb = sky_sub_mesh->getVB();
	IndexBuffer* ib = sky_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	// set params
	sky_eff_->setFloatVector("vInvWavelength", inv_wave_length_.ptr(), 3);
	sky_eff_->setFloat("fInnerRadius", inner_radius_);
	sky_eff_->setFloat("fKrESun", kr_* sun_brightness_);
	sky_eff_->setFloat("fKmESun", km_ * sun_brightness_);
	sky_eff_->setFloat("fKr4PI", kr_4pi_);
	sky_eff_->setFloat("fKm4PI", km_4pi_);
	sky_eff_->setFloat("fScale", scale_);
	sky_eff_->setFloat("fScaleDepth", rayleigh_scale_depth_);
	sky_eff_->setFloat("fScaleOverScaleDepth", scale_ / rayleigh_scale_depth_);
	sky_eff_->setFloat("g", mie_phase_asymmetry_factor_);
	sky_eff_->setFloat("g2", mie_phase_asymmetry_factor_ * mie_phase_asymmetry_factor_);

	float camera_height = inner_radius_ + curr_cam_->getEyePos().y;
	sky_eff_->setFloatVector("vCameraPos", Vector3(0, camera_height, 0).ptr(), 3);

	Light* main_lit = render_pipe_->getMainDirLit();
	Vector3 main_dir = -main_lit->getDirection();
	sky_eff_->setFloatVector("vLightPos", main_dir.ptr(), 3);
	sky_eff_->setFloat("fCameraHeight", camera_height);

	// set wvp
	const Matrix4& world_view_proj = curr_cam_->getViewProjMatrix();
	
	sky_eff_->setMatrix4("matWVP", world_view_proj.transpose()[0]);

	uint32 element_mask = sky_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(sky_sub_mesh->getPrimType(), 0, 
		sky_sub_mesh->getIndexCount(), 
		sky_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rs->setFrontFace(true);
}

void ScatteringSkyPass::prepareMesh()
{
	sky_sphere_mesh_.attach(ROOTPTR->get_mesh_mgr()->createSphere("___sky_sphere", outer_radius_ * 2, 64));
	sky_sphere_mesh_->load(false);
}

void ScatteringSkyPass::prepareEffect()
{
	sky_eff_ = getRenderEffect("scattering_sky_vs.h", "scattering_sky_ps.h");
}

Program* ScatteringSkyPass::getRenderEffect(const String& vs_file, const String& ps_file)
{
	String def_code, comm_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	shader_getFileData(base_path + "eff_comm_def.h", def_code);
	shader_getFileData(base_path + vs_file, vs_code);
	shader_getFileData(base_path + ps_file, ps_code);

	String str_vs = makeMacro(false) + def_code + comm_code + vs_code;
	String str_ps = makeMacro(true) + def_code + comm_code + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();

		return 0;
	}

	return pro;
}

String ScatteringSkyPass::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

}