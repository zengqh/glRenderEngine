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
// File: enn_effect_template_manager.h
//

#pragma once

#include "enn_default_effect_template.h"
#include "enn_shadowpre_effect_template.h"
#include "enn_gbuffer_effect_template.h"
#include "enn_uirect_effect_template.h"
#include "enn_litacc_effect_template.h"
#include "enn_forward_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// EffectTemplateManager
class SkeletonInstance;
class EffectTemplateManager : public AllocatedObject
{
	typedef enn::map<int, EffectTemplate*>::type	EffectTemplateMap;
public:
	EffectTemplateManager();
	~EffectTemplateManager();

public:
	void registerEffectTemplate(int et_id, EffectTemplate* et);
	EffectTemplate* getEffectTemplate(int et_id) const;
	void destroy();

protected:
	EffectTemplateMap		eff_template_map_;
};

}