/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_render_buffer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{
class RenderBuffer : public Noncopyable, public AllocatedObject
{
private:
	RenderBuffer();

	~RenderBuffer();

public:
	static RenderBuffer* createInstance();

public:
	void release();

public:
	void createRB();

	void destroyRB();

	void buildRB( int width, int height, PixelFormat fmt );

	void bindHandle( GLuint h );

	int getWidth() const;

	int getHeight() const;

	PixelFormat getFmt() const;

public:
	void active( bool b );

public:
	GLuint getHandle() const { return handle_; }

private:
	GLuint				handle_;
	int					width_;
	int					height_;
	PixelFormat			fmt_;
};
}