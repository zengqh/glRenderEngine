/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/23
* File: enn_gl_render_target.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{
class Viewport;
class TextureObj;
class RenderBuffer;
class RenderTarget : public Noncopyable, public AllocatedObject
{
public:
	typedef enn::map<int, Viewport*>::type ViewportMap;
	typedef RangeIterator<ViewportMap> Iterator;

public:
	static RenderTarget* createInstance();
	void  release();

protected:
	RenderTarget();
	~RenderTarget();

public:
	Viewport* createViewport(int z_order);
	Viewport* getOrCreateViewport(int z_order);
	Viewport* getViewport(int z_order) const;

	void destroyViewport(int z_order);
	void clearAllViewports();

	Iterator getViewports();
	int getViewportCounts() const;

public:
	int getWidth() const;
	int getHeight() const;

public:
	void link_rtt(TextureObj* rtt);
	void link_rtt(int idx, TextureObj* rtt);
	void link_depth(RenderBuffer* depth);
	
	TextureObj* get_rtt(int idx) const;
	RenderBuffer* get_depth() const;

	void begin_render();
	void end_render();

	void setScreenRTT(int w, int h);
	bool isScreenRTT() const;

protected:
	ViewportMap			view_ports_;
	TextureObj*			rtt_[MAX_FRAME_BUFFER_CHANNEL];
	RenderBuffer*		depth_;

	// screen render target
	bool				is_screen_rtt_;
	int					screen_width_;
	int					screen_height_;

	// cube render target
	bool				is_cube_rtt_;
	int					face_;
};
}