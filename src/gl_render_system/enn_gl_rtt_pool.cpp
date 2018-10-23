/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/24
* File: enn_gl_rtt_pool.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_rtt_pool.h"
#include "enn_gl_conv.h"

namespace enn
{
void RttPool::destroyAllBuffers()
{
	destroyBuffers(rtt_map_, true);
	destroyBuffers(depth_map_, false);
}

void RttPool::resetAllBuffers()
{
	resetBuffers(rtt_map_);
	resetBuffers(depth_map_);
}

TextureObj* RttPool::getRTTBuffer(PixelFormat fmt, int width, int height)
{
	return getRTTBufferImpl(fmt, width, height)->rtt_;
}

RenderBuffer* RttPool::getDepthBuffer(PixelFormat fmt, int width, int height)
{
	return getDepthBufferImpl(fmt, width, height)->depth_;
}

RttPool::ItemTemp RttPool::getRTTBufferTemp(PixelFormat fmt, int width, int height)
{
	return ItemTemp(getRTTBufferImpl(fmt, width, height));
}

RttPool::ItemTemp RttPool::getDepthBufferTemp(PixelFormat fmt, int width, int height)
{
	return ItemTemp(getDepthBufferImpl(fmt, width, height));
}

void RttPool::destroyRTTBuffer(TextureObj* rtt)
{
	uint32 fmt = rtt->getTexDesc().pixel_format_;
	uint32 w = rtt->getWidth();
	uint32 h = rtt->getHeight();

	uint64 key = ((uint64)fmt << 32) | (w << 16) | h;

	ItemListMap::iterator it = rtt_map_.find(key);
	if (it == rtt_map_.end())
	{
		ENN_ASSERT(0);
		return;
	}

	ItemList& item_list = it->second;
	ENN_FOR_EACH(ItemList, item_list, iter)
	{
		Item& item = *iter;
		if (item.rtt_ == rtt)
		{
			ENN_SAFE_RELEASE(rtt);
		}

	}

	item_list.clear();
}

void RttPool::destroyDepthBuffer(RenderBuffer* depth)
{
	uint32 fmt = depth->getFmt();
	uint32 w = depth->getWidth();
	uint32 h = depth->getHeight();

	uint64 key = ((uint64)fmt << 32) | (w << 16) | h;

	ItemListMap::iterator it = depth_map_.find(key);
	if (it == depth_map_.end())
	{
		ENN_ASSERT(0);
		return;
	}

	ItemList& item_list = it->second;
	ENN_FOR_EACH(ItemList, item_list, iter)
	{
		Item& item = *iter;
		if (item.depth_ == depth)
		{
			ENN_SAFE_RELEASE(depth);
		}

		item_list.erase(iter);
	}
}

RttPool::Item* RttPool::getRTTBufferImpl(PixelFormat fmt, int width, int height)
{
	uint64 key = ((uint64)fmt << 32) | (width << 16) | height;

	ItemList& item_list = rtt_map_[key];

	ENN_FOR_EACH(ItemList, item_list, it)
	{
		Item& item = *it;

		if (item.is_unused_)
		{
			item.set_used();
			return &item;
		}
	}

	TextureObj::TexObjDesc tex_obj_desc;

	tex_obj_desc.width_ = width;
	tex_obj_desc.height_ = height;
	tex_obj_desc.texture_type_ = TEXTURE_2D;
	tex_obj_desc.pixel_format_ = fmt;
	tex_obj_desc.internal_format_ = _convGLInternalFormat(fmt);
	tex_obj_desc.external_format_ = _convGLExternalFormat(fmt);

	tex_obj_desc.min_filter_ = _convGLFilter(TEXF_LINEAR);
	tex_obj_desc.mag_filter_ = _convGLFilter(TEXF_LINEAR);

	tex_obj_desc.wrap_s_ = _convGLAddr(TEXADDR_WRAP);
	tex_obj_desc.wrap_t_ = _convGLAddr(TEXADDR_WRAP);
	tex_obj_desc.type_ = _convGLDataType(fmt);
	tex_obj_desc.is_mipmap_ = false;

	TextureObj* tex_obj = TextureObj::createInstance();
	tex_obj->createTexture();
	tex_obj->buildTexture2D(tex_obj_desc, 0);

	Item new_item(tex_obj);
	new_item.set_used();

	item_list.push_back(new_item);

	return &item_list.back();
}

RttPool::Item* RttPool::getDepthBufferImpl(PixelFormat fmt, int width, int height)
{
	uint64 key = ((uint64)fmt << 32) | (width << 16) | height;

	ItemList& item_list = depth_map_[key];

	ENN_FOR_EACH(ItemList, item_list, it)
	{
		Item& item = *it;

		if (item.is_unused_)
		{
			item.set_used();
			return &item;
		}
	}

	RenderBuffer* rb = RenderBuffer::createInstance();
	rb->createRB();
	rb->buildRB(width, height, fmt);

	item_list.push_back(Item(rb));
	return &item_list.back();
}

template <typename T>
void RttPool::destroyBuffers(T& items, bool rtt)
{
	for (typename T::iterator it = items.begin(), ite = items.end(); it != ite; ++it)
	{
		destroyBuffers(it->second, rtt);
	}

	items.clear();
}

template <typename T>
void RttPool::resetBuffers(T& items)
{
	for (typename T::iterator it = items.begin(), ite = items.end(); it != ite; ++it)
	{
		resetBuffers(it->second);
	}
}

void RttPool::destroyBuffers(RttPool::ItemList& items, bool rtt)
{
	ENN_FOR_EACH(RttPool::ItemList, items, it)
	{
		Item& item = *it;

		if (rtt)
		{
			ENN_SAFE_RELEASE(item.rtt_);
		}
		else
		{
			ENN_SAFE_RELEASE(item.depth_);
		}
	}

	items.clear();
}

void RttPool::resetBuffers(RttPool::ItemList& items)
{
	ENN_FOR_EACH(RttPool::ItemList, items, it)
	{
		Item& item = *it;
		item.set_unused();
	}
}

}