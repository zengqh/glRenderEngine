/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_vb.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"
#include "enn_gl_gpu_buffer.h"

namespace enn
{
class VertexBuffer : public GPUBuffer
{
public:
	static VertexBuffer* createInstance();

	void setElementMask(uint32 mask)
	{
		element_mask_ = mask;
	}

	uint32 getElementMask() const
	{
		return element_mask_;
	}

private:
	VertexBuffer();
	virtual ~VertexBuffer();

protected:
	virtual GLenum getTarget() const;

protected:
	uint32		element_mask_;
};

}