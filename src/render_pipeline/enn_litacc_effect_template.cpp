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
// File: enn_litacc_effect_template.cpp
//

#include "enn_litacc_effect_template.h"
#include "enn_root.h"
#include "enn_effect_template_manager.h"

namespace enn
{

LitAccEffectTemplate::LitAccEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, curr_cam_(0)
{

}

LitAccEffectTemplate::~LitAccEffectTemplate()
{

}

EffectContext* LitAccEffectTemplate::getEffectContext(Renderable* ra)
{
	EffectMethod em = getMethod(ra);

	EffectContextMap::iterator iter = eff_ctx_map_.find(em);
	if (iter != eff_ctx_map_.end())
	{
		return iter->second;
	}

	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";

	shader_getFileData(base_path + "eff_comm_def.h", def_code);
	shader_getFileData(base_path + "litacc_eff_vs.h", vs_code);
	shader_getFileData(base_path + "litacc_eff_ps.h", ps_code);

	String str_macro_vs = makeMacro(em) + def_code;
	String str_macro_ps = makeMacro(em) + def_code;

	String str_vs = str_macro_vs + vs_code;
	String str_ps = str_macro_ps + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();

		return 0;
	}

	EffectContext*& ec = eff_ctx_map_[em];

	ENN_ASSERT(!ec);

	ec = ENN_NEW EffectContext();

	ec->eff_ = pro;
	ec->eff_method_ = em;
	ec->owner_ = this;

	eff_ctx_map_[em] = ec;

	return ec;
}

void LitAccEffectTemplate::doSetParams(EffectContext* eff_ctx, 
									   Renderable* ra, EffectParams* params)
{
	Program* pro = eff_ctx->eff_;
	LitAccEffectParams* lit_acc_params = static_cast<LitAccEffectParams*>(params);
	Camera* cam = lit_acc_params->curr_render_cam_;
	curr_cam_ = cam;

	// set wvp
	const Matrix4& world_matrix = lit_acc_params->pt_light_->getVolumeWorldMatrix();
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();

	Matrix4 wvp = proj_matrix * view_matrix * world_matrix;
	wvp = wvp.transpose();

	Matrix4 wv = view_matrix * world_matrix;
	wv = wv.transpose();

	pro->setMatrix4("matWVP", wvp[0]);
	pro->setMatrix4("matWV", wv[0]);

	// set zfar
	float zFar = cam->getZFar();
	pro->setFloat("zFar", zFar);

	Viewport* vp = cam->getViewport();


	// set light
	if (lit_acc_params->is_point_light_)
	{
		setPointLit(lit_acc_params->pt_light_, pro);
	}
	else
	{
		setSpotLit(lit_acc_params->spot_light_, pro);
	}

	// set normal and depth rtt
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	rs->setTexture(0, lit_acc_params->color0_);
	pro->setInt( "normalGB", 0 );

	rs->setTexture(1, lit_acc_params->color1_);
	pro->setInt("depthViewGB", 1);

	rs->enableCullFace(false);
	rs->setWireFrame(false);
	rs->setFrontFace(false);
}

void LitAccEffectTemplate::setPointLit(PointLight* lit, Program* pro)
{
	Vector3 lit_view_pos = curr_cam_->getViewMatrix().transformAffine(lit->getPosition());
	vec4f pos_w(lit_view_pos, lit->getFadeScale());
	pro->setFloatVector("point_light_pos", pos_w.ptr(), 4);

	vec3f point_diff(lit->getDiffuse().getAsVector3());
	pro->setFloatVector("point_light_diff", point_diff.ptr(), 3);
}

void LitAccEffectTemplate::setSpotLit(SpotLight* lit, Program* pro)
{
	// spot_light_pos
	vec4f spot_pos_face_scale(lit->getPosition(), lit->getFadeScale());
	pro->setFloatVector("spot_light_pos", spot_pos_face_scale.ptr(), 4);

	// spot_light_direction
	vec4f spot_dir_angles(-lit->getDirection(), lit->getSpotAngles().x);
	pro->setFloatVector("spot_light_direction", spot_dir_angles.ptr(), 4);

	// spot_light_diff
	vec4f spot_diff_angles(lit->getDiffuse().getAsVector3(), lit->getSpotAngles().y);
	pro->setFloatVector("spot_light_diff", spot_diff_angles.ptr(), 4);
}

EffectMethod LitAccEffectTemplate::getMethod(Renderable* ra)
{
	EffectMethod em;

	em.enableLighting();
	em.enablePointLits();

	return em;
}

}