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
// Time: 2013/08/22
// File: enn_default_effect_template.h
//

#pragma once

#include "enn_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// DefaultEffectParams
class DefaultEffectParams : public EffectParams
{
public:
	Camera*				curr_render_cam_;
	TextureObj*			lit_buffer_;
	TextureObj*			map_ao_;
};

//////////////////////////////////////////////////////////////////////////
// DefaultEffectTemplate
class DefaultEffectTemplate : public EffectTemplate
{
public:
	DefaultEffectTemplate(EffectTemplateManager* owner);
	virtual ~DefaultEffectTemplate();

public:
	virtual EffectContext* getEffectContext(Renderable* ra);
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);

protected:
	void setGeomParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);
	void setMtrParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);
	void setSkeletonParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);
	void setLightInfosParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);
	void setMapAO(EffectContext* eff_ctx, Renderable* ra, EffectParams* params);

	void apply_tex(TextureUnit* tu, int sampler);
	void apply_main_dir_shadow_params(Light* lit, EffectContext* eff_context);
protected:
	virtual EffectMethod getMethod(Renderable* ra);
	virtual String makeMacro(const EffectMethod& em);

protected:
	Matrix4*					bones_mat_buff_;
};

}