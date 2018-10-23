/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/24
* File: enn_gl_rtt_pool.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_texture_obj.h"
#include "enn_gl_render_buffer.h"

namespace enn
{
class RttPool : public AllocatedObject, public Noncopyable
{
public:
	struct Item
	{
		Item() : is_unused_(true), rtt_(0)
		{
			
		}

		explicit Item(TextureObj* tex) : is_unused_(true), rtt_(tex)
		{

		}

		explicit Item(RenderBuffer* depth) : is_unused_(true), depth_(depth)
		{

		}

	public:
		void set_used()
		{
			is_unused_ = false;
		}

		void set_unused()
		{
			is_unused_ = true;
		}

	public:
		union
		{
			TextureObj*		rtt_;
			RenderBuffer*	depth_;
		};

		bool is_unused_;
	};

	struct ItemTemp
	{
		explicit ItemTemp(Item* i) : item_(i)
		{

		}

		~ItemTemp()
		{
			item_->set_unused();
		}

		Item* operator->() const
		{
			return item_;
		}

		operator TextureObj*() const
		{
			return item_->rtt_;
		}

		operator RenderBuffer*() const
		{
			return item_->depth_;
		}

		Item* item_;
	};

	typedef enn::vector<Item>::type ItemList;
	typedef enn::map<uint64, ItemList>::type ItemListMap;

public:
	~RttPool()
	{
		destroyAllBuffers();
	}

public:
	void destroyAllBuffers();
	void resetAllBuffers();

	TextureObj* getRTTBuffer(PixelFormat fmt, int width, int height);
	RenderBuffer* getDepthBuffer(PixelFormat fmt, int width, int height);

	ItemTemp getRTTBufferTemp(PixelFormat fmt, int width, int height);
	ItemTemp getDepthBufferTemp(PixelFormat fmt, int width, int height);

public:
	void destroyRTTBuffer(TextureObj* rtt);
	void destroyDepthBuffer(RenderBuffer* depth);

protected:
	Item* getRTTBufferImpl(PixelFormat fmt, int width, int height);
	Item* getDepthBufferImpl(PixelFormat fmt, int width, int height);

	template <typename T>
	void destroyBuffers(T& items, bool rtt);

	template <typename T>
	void resetBuffers(T& items);

	void destroyBuffers(ItemList& items, bool rtt);
	void resetBuffers(ItemList& items);

protected:
	ItemListMap		rtt_map_;
	ItemListMap		depth_map_;
};
}