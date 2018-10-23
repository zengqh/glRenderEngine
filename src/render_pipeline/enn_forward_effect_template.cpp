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
// Time: 2013/11/29
// File: enn_forward_effect_template.cpp
//

#include "enn_forward_effect_template.h"
#include "enn_root.h"

namespace enn
{

ForwardEffectTemplate::ForwardEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
, bones_mat_buff_(0)
{

}

ForwardEffectTemplate::~ForwardEffectTemplate()
{

}

EffectContext* ForwardEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "general_eff_vs.h", vs_code);
	shader_getFileData(base_path + "general_eff_ps.h", ps_code);


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

void ForwardEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	setGeomParams(eff_ctx, ra, params);
	setMtrParams(eff_ctx, ra, params);
	setSkeletonParams(eff_ctx, ra, params);
	setLightInfosParams(eff_ctx, ra, params);
}

void ForwardEffectTemplate::setGeomParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setVB(ra->getVB());
	rs->setIB(ra->getIB());

	DefaultEffectParams* default_params = static_cast<DefaultEffectParams*>(params);
	ENN_ASSERT(default_params);
	Camera* cam = default_params->curr_render_cam_;

	SubMaterial* mtr = ra->getMaterial();

	if (mtr->isRenderHomogeneous())
	{
		Matrix4 mat_identity = Matrix4::IDENTITY;
		Matrix3 mat_identity3 = Matrix3::IDENTITY;

		Program* pro = eff_ctx->eff_;
		pro->setMatrix4("matWorld", mat_identity.transpose()[0]);
		pro->setMatrix4("matWVP", mat_identity.transpose()[0]);
		pro->setMatrix3("matWorldIT", mat_identity3.transpose()[0]);
	}
	else
	{
		const Matrix4& world_matrix = ra->getWorldMatrix();
		const Matrix4& view_matrix = cam->getViewMatrix();
		const Matrix4& proj_matrix = cam->getProjMatrix();

		Matrix4 world_matrix_tranpose = world_matrix.transpose();
		Matrix4 wvp = proj_matrix * view_matrix * world_matrix;
		wvp = wvp.transpose();

		Matrix4 matWorldIT = world_matrix.inverse();
		Matrix3 matWorldIT3 = matWorldIT.toMatrix3();

		Program* pro = eff_ctx->eff_;
		pro->setMatrix4("matWorld", world_matrix_tranpose[0]);
		pro->setMatrix4("matWVP", wvp[0]);
		pro->setMatrix3("matWorldIT", matWorldIT3[0]);

		const vec3f& camera_pos = cam->getEyePos();

		pro->setFloatVector("camPos", camera_pos.ptr(), 3);

		checkGLError();
	}
}

void ForwardEffectTemplate::setMtrParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	Program* pro = eff_ctx->eff_;
	EffectMethod& em = eff_ctx->eff_method_;
	SubMaterial* mtr = ra->getMaterial();

	if (em.hasColorMap())
	{
		Color* tex_clr = mtr->getTexClr();
		pro->setFloatVector("texClr", tex_clr->getAsVector4().ptr(), 4);
	}

	/** texture */
	if (em.hasDiffuseMap())
	{
		apply_tex(mtr->getTexDiff(), 0);
		pro->setInt( "texDiff", 0 );
	}

	if (em.hasDiffuse2Map())
	{
		apply_tex(mtr->getTexDiff2(), 1);
	}

	if (em.hasNormalMap())
	{
		apply_tex(mtr->getTexNormal(), 2);
		pro->setInt( "texNormal", 2);
	}

	if (em.hasSpecularMap())
	{
		apply_tex(mtr->getTexSpecular(), 3);
	}

	// 4 and 5 is used in shadow
	if (em.hasEmissiveMap())
	{
		apply_tex(mtr->getTexEmissive(), 6);
	}

	/** render state */
	rs->enableCullFace(mtr->getCullMode() != CULL_NONE);

	if (mtr->isWireFrame())
	{
		rs->setWireFrame(true);
	}
	else
	{
		rs->setWireFrame(false);
	}

	if (mtr->getCullMode() == CULL_CCW)
	{
		rs->setFrontFace(true);
	}
	else
	{
		rs->setFrontFace(false);
	}

	/** set ref alpha */
	pro->setFloat( "mtr_alpha", mtr->getAlpha() );
}

void ForwardEffectTemplate::setSkeletonParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
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

void ForwardEffectTemplate::setLightInfosParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	EffectMethod em = eff_ctx->eff_method_;
	if (!em.hasLighting())
	{
		return;
	}

	// set mtr
	Program* pro = eff_ctx->eff_;
	DefaultEffectParams* default_params = static_cast<DefaultEffectParams*>(params);
	RenderSystem* rs = RENDER_SYS;
	SubMaterial* mtr = ra->getMaterial();

	const Color& diff_clr = mtr->getDiffuse();
	pro->setFloatVector("mtr_diffuse", diff_clr.getAsVector3().ptr(), 3);

	if (em.hasSpecular())
	{
		Color spec_clr = mtr->getSpecularShiness();

		pro->setFloatVector("mtr_spec", spec_clr.getAsVector4().ptr(), 4);
	}


	LightsInfo* lights_info = ra->getLightsInfo();
	if (!lights_info)
	{
		return;
	}

	vec3f gloabl_amb = Root::getSingletonPtr()->getSceneManager()->getGlobalAmb().getAsVector3();
	pro->setFloatVector("global_amb", gloabl_amb.ptr(), 3);

	if (em.hasDirLit())
	{
		LightsInfo::LightList* dir_light_list = lights_info->getLightList(LT_DIRECTIONAL);

		if (dir_light_list && dir_light_list->size() > 0)
		{
			Light* main_dir_light = (*dir_light_list)[0].lit;

			const vec3f& dir = -main_dir_light->getDirection();
			const vec3f& diffuse = main_dir_light->getDiffuse().getAsVector3();

			pro->setFloatVector("dir_light_dir", dir.ptr(), 3);
			pro->setFloatVector("dir_light_diff", diffuse.ptr(), 3);

			apply_main_dir_shadow_params(main_dir_light, eff_ctx);
		}

		checkGLError();
	}
}


void ForwardEffectTemplate::apply_tex(TextureUnit* tu, int sampler)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	TextureManager* tm = Root::getSingletonPtr()->get_texture_mgr();

	TextureObj* obj = 0;

	Texture* tex_res = tu->getTex();
	if (tex_res->checkLoad())
	{
		obj = tex_res->getTextureObj();
	}
	else
	{
		obj = tm->getDefaultTexture()->getTextureObj();
	}

	rs->setTexture(sampler, obj);
}

void ForwardEffectTemplate::apply_main_dir_shadow_params(Light* lit, 
														 EffectContext* eff_context)
{
	return;
}

EffectMethod ForwardEffectTemplate::getMethod(Renderable* ra)
{
	EffectMethod em;

	SubMaterial* mat = ra->getMaterial();
	LightsInfo* lits_info = ra->getLightsInfo();
	SkeletonInstance* skel = ra->getSkelInst();

	if (mat->isEnableLighting() && lits_info)
	{
		em.enableLighting();

		LightsInfo::LightList* dir_light_list = lits_info->getLightList(LT_DIRECTIONAL);

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


		if (dir_light_list && dir_light_list->size() > 0)
		{
			LightsInfo::LightItem main_dir_light_item = (*dir_light_list)[0];
			if (main_dir_light_item.in_sm)
			{
				em.enableShadow();
			}

			/*
			if (main_dir_light_item.lit->getShadowInfo()->type_ == ST_PSSM)
			{
				em.enablePSSM();
			}
			*/
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

String ForwardEffectTemplate::makeMacro(const EffectMethod& em)
{
	String str_macro = EffectTemplate::makeMacro(em);
	addMacro(str_macro, "HAS_AO");

	return str_macro;
}

}