/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_render_pass_base.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_effect_template.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
class Renderable;
class IAddRenderable
{
public:
	virtual void addRenderable(Renderable* ra) = 0;
};

//////////////////////////////////////////////////////////////////////////
class Camera;
class RenderPipe;
class RenderPassBase : public IAddRenderable, public AllocatedObject
{
public:
	RenderPassBase(RenderPipe* render_pipe) : render_pipe_(render_pipe) {}
	virtual ~RenderPassBase() {}

	virtual void prepare() {}
	virtual void render(Camera* cam) = 0;
	virtual void addRenderable(Renderable* ra) {}

protected:
	RenderPipe*			render_pipe_;
};

//////////////////////////////////////////////////////////////////////////
class RenderableContext : public AllocatedObject
{
public:
	Renderable*		ra_obj_;
	EffectContext*	eff_;
	float			dist_;		/** dist to camera */
};

//////////////////////////////////////////////////////////////////////////
class RenderableContextList : public AllocatedObject
{
public:
	typedef enn::vector<RenderableContext>::type RCList;
public:
	RenderableContextList();
	~RenderableContextList();

	void sortBackToFront();
	void sortFrontToBack();

	void addRenderable(const RenderableContext& rc);

	int getSize() const
	{
		return rc_list_.size(); 
	}

	const RenderableContext& at(int idx) const
	{
		return rc_list_[idx];
	}

	RenderableContext& at(int idx)
	{
		return rc_list_[idx];
	}

	void clear();

protected:
	RCList rc_list_;
};

//////////////////////////////////////////////////////////////////////////
// KindRenderableList
class KindRenderableList : public AllocatedObject
{
public:
	typedef enn::map<EffectContext*, RenderableContextList*>::type RCListMap;
	typedef ObjectRecycle<RenderableContextList, SPFM_DELETE_T> RcListRecycle;

public:
	KindRenderableList();
	~KindRenderableList();

	void addRenderable(const RenderableContext& rc);
	void clear();

	RCListMap& getRenderables()
	{
		return rc_list_;
	}

	const RCListMap& getRenderables() const
	{
		return rc_list_;
	}

	void sortBackToFront();
	void sortFrontToBack();

protected:
	RCListMap rc_list_;
	RcListRecycle list_pool_;
};

}