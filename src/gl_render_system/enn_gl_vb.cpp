/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_vb.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_vb.h"

namespace enn
{
VertexBuffer* VertexBuffer::createInstance()
{
	return ENN_NEW VertexBuffer;
}

VertexBuffer::VertexBuffer()
: element_mask_(0)
{

}

VertexBuffer::~VertexBuffer()
{

}

GLenum VertexBuffer::getTarget() const
{
	return GL_ARRAY_BUFFER;
}

}