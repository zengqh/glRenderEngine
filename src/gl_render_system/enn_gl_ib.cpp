/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_ib.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_ib.h"

namespace enn
{
IndexBuffer* IndexBuffer::createInstance()
{
	return ENN_NEW IndexBuffer;
}

IndexBuffer::IndexBuffer()
{
}

IndexBuffer::~IndexBuffer()
{
}

GLenum IndexBuffer::getTarget() const
{
	return GL_ELEMENT_ARRAY_BUFFER;
}
}