/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/24
* File: enn_scene_find.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_scene_node.h"
#include "enn_light.h"
#include "enn_light_node.h"
#include "enn_camera.h"
#include "enn_camera_node.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// SceneFindOption
class SceneFindOption
{
public:
	typedef bool (*PreCullCallbackFunc)(void*, SceneNode*);
	typedef void (*FindCallbackFunc)(void*, SceneNode*);

public:
	enum Option
	{
		OP_CAMERA_FULL
	};

	enum
	{
		MK_INCDISABLE,
		MK_PRECULL,
		MK_RENDER,
		MK_LIGHT
	};

	SceneFindOption()
		: option_(OP_CAMERA_FULL)
		, context_(0)
		, cam_(0)
		, pre_cull_(0)
		, result_renders_(0)
		, result_lights_(0)
	{

	}

public:
	Option					option_;
	Flags32					mask_;
	void*					context_;
	Camera*					cam_;

	PreCullCallbackFunc		pre_cull_;
	FindCallbackFunc		result_renders_;
	FindCallbackFunc		result_lights_;
};

//////////////////////////////////////////////////////////////////////////
// SceneFindBase
class SceneFindBase : public AllocatedObject
{
public:
	enum Visibility
	{
		NONE,
		PARTIAL,
		FULL
	};

public:
	SceneFindBase(SceneManager* sm, SceneFindOption* sfo);
	virtual ~SceneFindBase();

public:
	virtual Visibility onTestAABB(const AxisAlignedBox& box) = 0;
	virtual bool onTestObject(SceneNode* node) = 0;

protected:
	SceneFindOption::FindCallbackFunc pre_check(SceneNode* sn) const;

protected:
	SceneManager*									sm_;
	SceneFindOption*								sfo_;
	void*											context_;
	bool											cull_disabled_;

	SceneFindOption::PreCullCallbackFunc			pre_cull_;
	SceneFindOption::FindCallbackFunc				result_renders_;
	SceneFindOption::FindCallbackFunc				result_lights_;
};

//////////////////////////////////////////////////////////////////////////
// FindByCamera
class FindByCamera : public SceneFindBase
{
	friend class SceneManager;
public:
	FindByCamera(SceneManager* sm, SceneFindOption* sfo)
		: SceneFindBase(sm, sfo)
		, cam_(sfo->cam_)
	{

	}

public:
	virtual Visibility onTestAABB(const AxisAlignedBox& box);
	virtual bool onTestObject(SceneNode* node);

protected:
	Camera*		cam_;
};

//////////////////////////////////////////////////////////////////////////
class SceneFindUtil : public AllocatedObject
{
public:
	typedef enn::list<RenderableNode*>::type	RenderableNodeList;
	typedef enn::list<LightNode*>::type			LightNodeList;

public:
	SceneFindUtil() : curr_render_results_(0), curr_light_results_(0)
	{

	}

protected:
	void setResult(RenderableNodeList* render_results, LightNodeList* light_results);

	void find_op(SceneManager* sg, Camera* cam, const Flags32& flag);

	virtual bool on_pre_cull_node(SceneNode* node)
	{
		return false;
	}

	virtual void on_find_render_node(SceneNode* node)
	{
		curr_render_results_->push_back(static_cast<RenderableNode*>(node));
	}

	virtual void on_find_light_node(SceneNode* node)
	{
		curr_light_results_->push_back(static_cast<LightNode*>(node));
	}

	static bool on_pre_cull_node_static(void* context, SceneNode* node)
	{
		return static_cast<SceneFindUtil*>(context)->on_pre_cull_node(node);
	}

	static void on_find_render_node_static(void* context, SceneNode* node)
	{
		return static_cast<SceneFindUtil*>(context)->on_find_render_node(node);
	}

	static void on_find_light_node_static(void* context, SceneNode* node)
	{
		return static_cast<SceneFindUtil*>(context)->on_find_light_node(node);
	}

protected:
	RenderableNodeList*			curr_render_results_;
	LightNodeList*				curr_light_results_;
};

//////////////////////////////////////////////////////////////////////////
// SceneFindCameraUtil
class SceneFindCameraUtil : public SceneFindUtil
{
public:
	void find(SceneManager* sm, Camera* cam, RenderableNodeList* rlist, LightNodeList* llist, const Flags32& flag)
	{
		setResult(rlist, llist);
		find_op(sm, cam, flag);
	}
};


//////////////////////////////////////////////////////////////////////////
// SceneFindShadowCasterByCamera
class SceneFindShadowCasterByCamera : public SceneFindUtil
{
public:
	SceneFindShadowCasterByCamera()
	{

	}

protected:
	virtual bool on_pre_cull_node(SceneNode* node)
	{
		return !static_cast<RenderableNode*>(node)->isCastShadow();
	}

public:
	void find(SceneManager* sm, Camera* cam, RenderableNodeList* rlist)
	{
		setResult(rlist, 0);

		Flags32 flags;

		flags.set_flag(SceneFindOption::MK_RENDER);
		flags.set_flag(SceneFindOption::MK_PRECULL);

		find_op(sm, cam, flags);
	}
};
}