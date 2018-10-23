/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/21
* File: enn_render_pass_base.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_render_pass_base.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
static bool _sortF2B( const RenderableContext&  lhs, const RenderableContext& rhs )
{
	return lhs.dist_ < rhs.dist_;
}

static bool _sortB2F( const RenderableContext& lhs, const RenderableContext& rhs )
{
	return lhs.dist_ > rhs.dist_;
}


//////////////////////////////////////////////////////////////////////////
RenderableContextList::RenderableContextList()
{

}

RenderableContextList::~RenderableContextList()
{

}

void RenderableContextList::sortBackToFront()
{
	std::sort(rc_list_.begin(), rc_list_.end(), _sortB2F);
}

void RenderableContextList::sortFrontToBack()
{
	std::sort(rc_list_.begin(), rc_list_.end(), _sortF2B);
}

void RenderableContextList::addRenderable(const RenderableContext& rc)
{
	rc_list_.push_back(rc);
}

void RenderableContextList::clear()
{
	rc_list_.clear();
}

//////////////////////////////////////////////////////////////////////////
KindRenderableList::KindRenderableList()
{

}

KindRenderableList::~KindRenderableList()
{
	clear();
}

void KindRenderableList::addRenderable(const RenderableContext& rc)
{
	RenderableContextList*& rcl = rc_list_[rc.eff_];
	if (!rcl)
	{
		rcl = list_pool_.requestObject();
	}

	rcl->addRenderable(rc);
}

void KindRenderableList::clear()
{
	ENN_FOR_EACH(RCListMap, rc_list_, it)
	{
		RenderableContextList* rcl = it->second;
		rcl->clear();
		list_pool_.freeObject(rcl);
	}

	rc_list_.clear();
}

void KindRenderableList::sortBackToFront()
{
	ENN_FOR_EACH( RCListMap, rc_list_, it )
	{
		RenderableContextList* rcList = it->second;
		rcList->sortBackToFront();
	}
}

void KindRenderableList::sortFrontToBack()
{
	ENN_FOR_EACH( RCListMap, rc_list_, it )
	{
		RenderableContextList* rcList = it->second;
		rcList->sortFrontToBack();
	}
}

}