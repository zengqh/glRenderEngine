/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_frame_buffer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{
class TextureObj;
class RenderBuffer;

class FrameBuffer : public Noncopyable, public AllocatedObject
{
private:
	FrameBuffer();

	~FrameBuffer();

public:
	static FrameBuffer* createInstance();

public:
	void release();

public:
	void createFB();

	void destroyFB();

public:
	void active( bool b );

	void attachRTT( int channel, TextureObj* rtt );

	void attachCubeRTT( int channel, TextureObj* rtt, int face );

	void attachDepthStencil( RenderBuffer* ds );

public:
	GLuint getHandle() const { return handle_; }

private:
	GLuint handle_;
};

}