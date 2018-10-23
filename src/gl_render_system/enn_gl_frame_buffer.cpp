/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_frame_buffer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_frame_buffer.h"
#include "enn_gl_texture_obj.h"
#include "enn_gl_render_buffer.h"

namespace enn
{

FrameBuffer::FrameBuffer()
: handle_(0)
{

}

FrameBuffer::~FrameBuffer()
{
	destroyFB();
}

FrameBuffer* FrameBuffer::createInstance()
{
	return ENN_NEW FrameBuffer;
}

void FrameBuffer::release()
{
	ENN_DELETE this;
}

void FrameBuffer::createFB()
{
	destroyFB();
	gl.glGenFramebuffers( 1, &handle_ );
	checkGLError();
}

void FrameBuffer::destroyFB()
{
	if ( handle_ )
	{
		gl.glDeleteFramebuffers( 1, &handle_ );
		checkGLError();
		handle_ = 0;
	}
}

void FrameBuffer::attachRTT( int channel, TextureObj* rtt )
{
	ENN_ASSERT(0 <= channel && channel < MAX_FRAME_BUFFER_CHANNEL);

	GLuint handle = rtt ? rtt->getHandle() : 0;

	gl.glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + channel, GL_TEXTURE_2D, handle, 0 );
	checkGLError();
}

void FrameBuffer::attachCubeRTT( int channel, TextureObj* rtt, int face )
{
	ENN_ASSERT(0 <= channel && channel < MAX_FRAME_BUFFER_CHANNEL);

	gl.glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + channel, GL_TEXTURE_CUBE_MAP_POSITIVE_X+face, rtt->getHandle(), 0 );
	checkGLError();
}

void FrameBuffer::attachDepthStencil( RenderBuffer* ds )
{
	if (!ds)
	{
		return;
	}

	GLuint handle = ds->getHandle();

	gl.glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, handle );
	checkGLError();
}

void FrameBuffer::active( bool b )
{
	gl.glBindFramebuffer( GL_FRAMEBUFFER, b ? handle_ : 0 );
	checkGLError();
}

}