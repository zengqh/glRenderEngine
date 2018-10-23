/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_gpu_buffer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_gpu_buffer.h"

namespace enn
{

GPUBuffer::GPUBuffer()
: m_handle(0)
, m_data(0)
, size_(0)
, usage_(GL_STATIC_DRAW)
, use_memory_(false)
, access_(GL_WRITE_ONLY)
{

}

GPUBuffer::~GPUBuffer()
{
	destroyBuff();
}

void GPUBuffer::setUseMemory(bool use_memory)
{
	use_memory_ = use_memory;
}

bool GPUBuffer::isUseMemory() const
{
	return use_memory_;
}

void  GPUBuffer::release()
{
	ENN_DELETE this;
}

bool  GPUBuffer::createBuff( size_t size )
{
	if (size == 0)
	{
		ENN_ASSERT(0);
		size = sizeof(float);
	}

	destroyBuff();

	size_ = size;

	if (use_memory_)
	{
		m_data = ENN_MALLOC(size);
	}
	else
	{
		gl.glGenBuffers( 1, &m_handle );

		GLenum target = getTarget();
		gl.glBindBuffer( target, m_handle );

		gl.glBufferData( target, size, 0, usage_ );
		gl.glBindBuffer( target, 0 );

		m_data = ENN_MALLOC(size);

		checkGLError();
	}

	return true;
}

void  GPUBuffer::destroyBuff()
{
	ENN_SAFE_FREE(m_data);

	if ( m_handle != 0 )
	{
		gl.glDeleteBuffers( 1, &m_handle );
		m_handle = 0;

		checkGLError();
	}
}

void* GPUBuffer::lock( GLenum access )
{
	access_ = access;

	return m_data;
}

void  GPUBuffer::unlock()
{
	if (!use_memory_ && access_ == GL_WRITE_ONLY)
	{
		GLenum target = getTarget();
		gl.glBindBuffer( target, m_handle );
		gl.glBufferData( target, size_, m_data, usage_ );
		gl.glBindBuffer( target, 0 );

		checkGLError();
	}
}

void  GPUBuffer::setData( const void* data )
{
	if (!use_memory_)
	{
		if (m_data) memcpy(m_data, data, size_);

		GLenum target = getTarget();
		gl.glBindBuffer( target, m_handle );
		gl.glBufferData( target, size_, m_data, usage_ );
		gl.glBindBuffer( target, 0 );

		checkGLError();
	}
	else
	{
		memcpy(m_data, data, size_);
	}
}

void GPUBuffer::bind()
{
	if (!use_memory_)
	{
		ENN_ASSERT(m_handle);
		GLenum target = getTarget();
		gl.glBindBuffer( target, m_handle );
	}
	else
	{
		GLenum target = getTarget();
		gl.glBindBuffer( target, 0 );
	}
}

void GPUBuffer::unbind()
{
	if (!use_memory_)
	{
		GLenum target = getTarget();
		gl.glBindBuffer( target, 0 );
	}
}

void GPUBuffer::updateGPUData()
{
	if (!use_memory_)
	{
		GLenum target = getTarget();
		gl.glBindBuffer( target, m_handle );
		gl.glBufferData( target, size_, m_data, usage_ );
		gl.glBindBuffer( target, 0 );

		checkGLError();
	}
}

void GPUBuffer::freeData()
{
	if (!use_memory_)
	{
		ENN_SAFE_FREE(m_data);
	}
}

}