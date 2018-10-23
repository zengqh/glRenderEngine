/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_gpu_buffer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{

struct VertexP3N3
{
	float x, y, z;
	float nx, ny, nz;
};

struct VertexP3N3U2
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct VertexP3U2
{
	float x, y, z;
	float u, v;
};

struct VertexP3U2U2
{
	float x, y, z;
	float u, v;
	float u1, v1;
};

class GPUBuffer : public Noncopyable, public AllocatedObject
{
protected:
	GPUBuffer();

	virtual ~GPUBuffer();

public:
	void setUseMemory(bool use_memory = true);

	bool isUseMemory() const;

	void  release();

	bool  createBuff( size_t size);

	void  destroyBuff();

	void* lock( GLenum access = GL_WRITE_ONLY );

	void  unlock();

	void  setData( const void* data );

	void bind();

	void unbind();

public:
	GLuint getHandle() const { return m_handle; }

	size_t getSize() const { return size_; };

	GLenum getUsage() const { return usage_; }

	void* getData() const { return m_data; }

	void updateGPUData();

	void freeData();

protected:
	virtual GLenum getTarget() const = 0;

protected:
	GLuint m_handle;
	void*  m_data;
	size_t size_;
	GLenum usage_;
	GLenum access_;
	bool use_memory_;
};
}