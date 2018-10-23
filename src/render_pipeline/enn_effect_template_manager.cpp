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
// File: enn_effect_template_manager.cpp
//

#include "enn_effect_template_manager.h"
#include "enn_root.h"
#include "enn_mesh.h"
#include "enn_shadow.h"

namespace enn
{

EffectTemplateManager::EffectTemplateManager()
{

}

EffectTemplateManager::~EffectTemplateManager()
{
	destroy();
}

void EffectTemplateManager::registerEffectTemplate(int et_id, EffectTemplate* et)
{
	ENN_ASSERT(eff_template_map_.find(et_id) == eff_template_map_.end());

	eff_template_map_[et_id] = et;
}

EffectTemplate* EffectTemplateManager::getEffectTemplate(int et_id) const
{
	EffectTemplateMap::const_iterator it = eff_template_map_.find(et_id);
	if (it != eff_template_map_.end())
	{
		return it->second;
	}

	return 0;
}

void EffectTemplateManager::destroy()
{
	ENN_FOR_EACH(EffectTemplateMap, eff_template_map_, it)
	{
		EffectTemplate* et = it->second;
		ENN_SAFE_DELETE(et);
	}
}

}