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
// File: enn_effect_template.h
//

#pragma once

#include "enn_gl_program.h"
#include "enn_material.h"
#include "enn_renderable.h"
#include "enn_camera.h"
#include "enn_light.h"
#include "enn_mesh.h"

namespace enn
{

class EffectTemplateManager;

#define _DECL_EFF_METHOD_ITEM(item) \
void enable##item(); \
bool has##item() const;

//////////////////////////////////////////////////////////////////////////
// EffectMethod
class EffectMethod : public AllocatedObject
{
public:
	_DECL_EFF_METHOD_ITEM(ColorMap)
	_DECL_EFF_METHOD_ITEM(DiffuseMap)
	_DECL_EFF_METHOD_ITEM(Diffuse2Map)
	_DECL_EFF_METHOD_ITEM(NormalMap)
	_DECL_EFF_METHOD_ITEM(SpecularMap)
	_DECL_EFF_METHOD_ITEM(EmissiveMap)
	_DECL_EFF_METHOD_ITEM(Specular)
	_DECL_EFF_METHOD_ITEM(Emissive)
	_DECL_EFF_METHOD_ITEM(Lighting)
	_DECL_EFF_METHOD_ITEM(Fog)
	_DECL_EFF_METHOD_ITEM(TexAni)
	_DECL_EFF_METHOD_ITEM(Shadow)
	_DECL_EFF_METHOD_ITEM(Skeleton)
	_DECL_EFF_METHOD_ITEM(AlphaTest)
	_DECL_EFF_METHOD_ITEM(DirLit)
	_DECL_EFF_METHOD_ITEM(PointLits)
	_DECL_EFF_METHOD_ITEM(SpotLits)
	_DECL_EFF_METHOD_ITEM(PSSM)

public:
	EffectMethod()
	{

	}

public:
	bool operator<( const EffectMethod& ec ) const
	{
		return types_ < ec.types_;
	}

	bool operator==( const EffectMethod& ec ) const
	{
		return types_ == ec.types_;
	}

public:
	Flags64		types_;
};

//////////////////////////////////////////////////////////////////////////
// EffectContext
class EffectTemplate;
class EffectContext : public AllocatedObject
{
public:
	EffectContext()
	{
		eff_ = 0;
	}

	~EffectContext()
	{
		ENN_SAFE_RELEASE(eff_);
	}

	EffectTemplate*			owner_;
	Program*				eff_;
	EffectMethod			eff_method_;
};

//////////////////////////////////////////////////////////////////////////
// EffectParams
class EffectParams : public enn::AllocatedObject
{
public:
	virtual ~EffectParams() {}
};

//////////////////////////////////////////////////////////////////////////
// EffectTemplate
class EffectTemplate : public enn::AllocatedObject
{
public:
	typedef enn::map<EffectMethod, EffectContext*>::type		EffectContextMap;

public:
	EffectTemplate(EffectTemplateManager* owner);
	virtual ~EffectTemplate();

	virtual void destory();

public:
	virtual EffectContext* getEffectContext(Renderable* ra) = 0;
	virtual void doSetParams(EffectContext* eff_ctx, Renderable* ra, EffectParams* params) = 0;

protected:
	virtual EffectMethod getMethod(Renderable* ra) = 0;

protected:
	bool isSupportEffect() const;
	virtual String makeMacro(const EffectMethod& em);

protected:
	void addMacro(String& str_macro, const String& mac_name);
	void addMacro(String& str_macro, const String& mac_name, int val);

protected:
	EffectTemplateManager*				owner_;
	EffectContextMap					eff_ctx_map_;
};

}