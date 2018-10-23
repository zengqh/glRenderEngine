/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/24
* File: enn_scene_find.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_scene_find.h"
#include "enn_scene_manager.h"

namespace enn
{
SceneFindBase::SceneFindBase(SceneManager* sm, SceneFindOption* sfo)
: sm_(sm)
, sfo_(sfo)
, context_(sfo->context_)
{
	cull_disabled_ = !sfo_->mask_.test_flag(SceneFindOption::MK_INCDISABLE);

	if (sfo_->mask_.test_flag(SceneFindOption::MK_PRECULL))
	{
		pre_cull_ = sfo_->pre_cull_;
	}
	else
	{
		pre_cull_ = 0;
	}

	if (sfo_->mask_.test_flag(SceneFindOption::MK_RENDER))
	{
		result_renders_ = sfo_->result_renders_;
	}
	else
	{
		result_renders_ = 0;
	}

	if (sfo_->mask_.test_flag(SceneFindOption::MK_LIGHT))
	{
		result_lights_ = sfo_->result_lights_;
	}
	else
	{
		result_lights_ = 0;
	}
}

SceneFindBase::~SceneFindBase()
{

}

SceneFindOption::FindCallbackFunc SceneFindBase::pre_check(SceneNode* sn) const
{
	SceneFindOption::FindCallbackFunc find_func = 0;

	if (sn->isLightNode())
	{
		find_func = result_lights_;
	}
	else if (sn->isRenderableNode())
	{
		find_func = result_renders_;
	}

	if (!find_func)
	{
		return 0;
	}

	if (pre_cull_ && pre_cull_(context_, sn))
	{
		return 0;
	}

	return find_func;
}

//////////////////////////////////////////////////////////////////////////
SceneFindBase::Visibility FindByCamera::onTestAABB(const AxisAlignedBox& box)
{
	return (SceneFindBase::Visibility)cam_->testVisibilityEx(box);
}

bool FindByCamera::onTestObject(SceneNode* node)
{
	if (SceneFindOption::FindCallbackFunc func = pre_check(node))
	{
		if (cam_->testVisibility(node->getWorldAABB()))
		{
			func(context_, node);
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
// SceneFindUtil
void SceneFindUtil::setResult(SceneFindUtil::RenderableNodeList* render_results, 
							  SceneFindUtil::LightNodeList* light_results)
{
	curr_render_results_ = render_results;
	curr_light_results_ = light_results;

	if (render_results)
	{
		render_results->clear();
	}

	if (light_results)
	{
		light_results->clear();
	}
}

void SceneFindUtil::find_op(SceneManager* sg, Camera* cam, const Flags32& flag)
{
	SceneFindOption sfo;

	sfo.mask_ = flag;
	sfo.cam_ = cam;
	sfo.context_ = this;
	sfo.pre_cull_ = &SceneFindUtil::on_pre_cull_node_static;
	sfo.result_renders_ = &SceneFindUtil::on_find_render_node_static;
	sfo.result_lights_ = &SceneFindUtil::on_find_light_node_static;

	sg->find(sfo);
}

}
