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
// Time: 2013/09/02
// File: enn_gbuffer_effect_template.cpp
//

#include "enn_gbuffer_effect_template.h"
#include "enn_root.h"

namespace enn
{

GBufferEffectTemplate::GBufferEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, bones_mat_buff_(0)
{
	
}

GBufferEffectTemplate::~GBufferEffectTemplate()
{

}

EffectContext* GBufferEffectTemplate::getEffectContext(Renderable* ra)
{
	ENN_ASSERT(isSupportEffect());

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
	shader_getFileData(base_path + "gbuffer_eff_vs.h", vs_code);
	shader_getFileData(base_path + "gbuffer_eff_ps.h", ps_code);

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

void GBufferEffectTemplate::doSetParams(EffectContext* eff_ctx, 
										Renderable* ra, EffectParams* params)
{
	GBufferEffectParams* gbuffer_params = static_cast<GBufferEffectParams*>(params);
	ENN_ASSERT(gbuffer_params);
	Camera* cam = gbuffer_params->curr_render_cam_;

	SubMaterial* mtr = ra->getMaterial();

	Program* pro = eff_ctx->eff_;

	/** geom */
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	rs->setVB(ra->getVB());
	rs->setIB(ra->getIB());

	const Matrix4& world_matrix = ra->getWorldMatrix();
	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();
	float zFar = cam->getZFar();

	Matrix4 matWorldIT = world_matrix.inverse();
	Matrix3 matWorldIT3 = matWorldIT.toMatrix3();

	Matrix4 wvp = proj_matrix * view_matrix * world_matrix;
	wvp = wvp.transpose();

	Matrix4 wv = view_matrix * world_matrix;
	wv = wv.transpose();

	pro->setMatrix4("matWVP", wvp[0]);
	pro->setMatrix4("matWV", wv[0]);
	pro->setFloat("zFar", zFar);

	// set material shininess
	float mtrShininess = 0.0f;
	if (eff_ctx->eff_method_.hasSpecular())
	{
		mtrShininess = mtr->getSpecularShiness().a;
	}

	mtrShininess = Math::clamp(mtrShininess / 255, 0.0f, 1.0f);

	pro->setFloat("mtrShininess", mtrShininess);

	// skeleton
	setSkeletonParams(eff_ctx, ra, params);

	rs->enableCullFace(mtr->getCullMode() != CULL_NONE);
	rs->setWireFrame(false);
	if (mtr->getCullMode() == CULL_CCW)
	{
		rs->setFrontFace(true);
	}
	else
	{
		rs->setFrontFace(false);
	}
}

EffectMethod GBufferEffectTemplate::getMethod(Renderable* ra)
{
	EffectMethod em;

	SubMaterial* mat = ra->getMaterial();
	LightsInfo* lits_info = ra->getLightsInfo();
	SkeletonInstance* skel = ra->getSkelInst();

	if (mat->isEnableLighting() && lits_info)
	{
		em.enableLighting();

		LightsInfo::LightList* dir_light_list = lits_info->getLightList(LT_DIRECTIONAL);
		LightsInfo::LightList* point_light_list = lits_info->getLightList(LT_POINT);
		LightsInfo::LightList* spot_light_list = lits_info->getLightList(LT_SPOT);

		if (dir_light_list && dir_light_list->size() > 0)
		{
			em.enableDirLit();
		}

		if (mat->getTexNormal())
		{
			em.enableNormalMap();
		}

		if (mat->getTexSpecular())
		{
			em.enableSpecularMap();
		}

		if (mat->hasSpecular())
		{
			em.enableSpecular();
		}

		if (mat->isEnablePointLighting() && point_light_list && point_light_list->size() > 0)
		{
			em.enablePointLits();
		}

		if (mat->isEnableSpotLighting() && spot_light_list && spot_light_list->size() > 0)
		{
			em.enableSpotLits();
		}
	}

	if (mat->hasEmissive())
	{
		em.enableEmissive();
	}

	if (mat->getTexClr())
	{
		em.enableColorMap();
	}

	/** diffuse tex */
	if (mat->getTexDiff())
	{
		em.enableDiffuseMap();
	}

	/** diffuse tex 2 */
	if (mat->getTexDiff2())
	{
		em.enableDiffuse2Map();
	}

	if (mat->getTexAnimate())
	{
		em.enableTexAni();
	}

	if (mat->isEnableFog())
	{
		em.enableFog();
	}

	if (mat->hasAlphaTest())
	{
		em.enableAlphaTest();
	}

	if (skel)
	{
		em.enableSkeleton();
	}


	return em;
}


void GBufferEffectTemplate::setSkeletonParams(EffectContext* eff_ctx, 
											  Renderable* ra, EffectParams* params)
{
	SkeletonInstance* skel_inst = ra->getSkelInst();
	if (!skel_inst)
	{
		return;
	}

	EffectMethod em = eff_ctx->eff_method_;
	Program* pro = eff_ctx->eff_;

	int sub_idx = ra->getSubIdx();

	if (!bones_mat_buff_)
	{
		bones_mat_buff_ = ENN_NEW_ARRAY_T(Matrix4, MAX_BONES_NUM);
	}

	int cnt = 0;
	if (skel_inst->getSubMatBones(sub_idx, bones_mat_buff_, &cnt))
	{
		ENN_ASSERT(cnt <= MAX_BONES_NUM);

		pro->setMatrix4Array("matBones", &(bones_mat_buff_->m[0][0]), cnt);
	}
	else
	{
		const float* v = &(skel_inst->getMatBones()->m[0][0]);

		cnt = skel_inst->getSkeleton()->getBoneNodeCount();

		ENN_ASSERT(cnt <= MAX_BONES_NUM);

		pro->setMatrix4Array( "matBones", v, cnt );
	}
}

}