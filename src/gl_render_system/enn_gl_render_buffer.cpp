/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_render_buffer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_render_buffer.h"
#include "enn_gl_conv.h"

namespace enn
{
RenderBuffer::RenderBuffer() : handle_(0)
{
}

RenderBuffer::~RenderBuffer()
{
	destroyRB();
}

RenderBuffer* RenderBuffer::createInstance()
{
	return ENN_NEW RenderBuffer;
}

void RenderBuffer::release()
{
	ENN_DELETE this;
}

void RenderBuffer::createRB()
{
	destroyRB();
	gl.glGenRenderbuffers( 1, &handle_ );
	checkGLError();
}

void RenderBuffer::destroyRB()
{
	if ( handle_ )
	{
		gl.glDeleteRenderbuffers( 1, &handle_ );
		checkGLError();
		handle_ = 0;
	}
}

void RenderBuffer::buildRB( int width, int height, PixelFormat fmt )
{
	GLenum internalformat = _convGLInternalFormat(fmt);

	gl.glBindRenderbuffer( GL_RENDERBUFFER, handle_ );

	gl.glRenderbufferStorage( GL_RENDERBUFFER, internalformat, width, height );

	gl.glBindRenderbuffer( GL_RENDERBUFFER, 0 );

	width_ = width;
	height_ = height;
	fmt_ = fmt;

	checkGLError();
}

void RenderBuffer::bindHandle( GLuint h )
{
	handle_ = h;
}

int RenderBuffer::getWidth() const
{
	return width_;
}

int RenderBuffer::getHeight() const
{
	return height_;
}

PixelFormat RenderBuffer::getFmt() const
{
	return fmt_;
}


void RenderBuffer::active( bool b )
{
	gl.glBindRenderbuffer( GL_RENDERBUFFER, b ? handle_ : 0 );
	checkGLError();
}

}