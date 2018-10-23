/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_ib.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_gpu_buffer.h"

namespace enn
{
class IndexBuffer : public GPUBuffer
{
public:
	static IndexBuffer* createInstance();
private:
	IndexBuffer();
	virtual ~IndexBuffer();

protected:
	virtual GLenum getTarget() const;
};
}