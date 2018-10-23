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
// File: enn_uirect_effect_template.cpp
//

#include "enn_uirect_effect_template.h"
#include "enn_root.h"

namespace enn
{

UIRectEffectTemplate::UIRectEffectTemplate(EffectTemplateManager* owner)
: EffectTemplate(owner)
{

}

UIRectEffectTemplate::~UIRectEffectTemplate()
{

}

EffectContext* UIRectEffectTemplate::getEffectContext(Renderable* ra)
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
	shader_getFileData(base_path + "uirect_eff_vs.h", vs_code);
	shader_getFileData(base_path + "uirect_eff_ps.h", ps_code);

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

void UIRectEffectTemplate::doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params)
{
	UIRectEffectParams* uirect_params = static_cast<UIRectEffectParams*>(params);

	TextureObj* obj = uirect_params->tex_;

	Program* pro = eff_ctx->eff_;

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->enableCullFace(false);
	rs->setWireFrame(false);
	rs->setFrontFace(true);

	if (uirect_params->swap_uv)
	{
		pro->setFloat("isSwapV", 1.0f);
	}
	else
	{
		pro->setFloat("isSwapV", 0.0f);
	}

	rs->setTexture(0, obj);
	pro->setInt( "texDiff", 0 );
}

EffectMethod UIRectEffectTemplate::getMethod(Renderable* ra)
{
	EffectMethod em;
	return em;
}

}