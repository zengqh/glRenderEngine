/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/19
* File: memory_buffer.cpp
**/

#include <enn_memory_buffer.h>

namespace enn
{
	MemoryBuffer::MemoryBuffer(void* data, unsigned size, bool own)
		: Deserializer(size)
		, buffer_(0)
		, own_(own)
	{
		if (!data)
		{
			size = 0;
		}

		if (data && !own)
		{
			buffer_ = (uchar*)data;
		}

		if (own)
		{
			buffer_ = (uchar*)data;
		}
	}

	MemoryBuffer::~MemoryBuffer()
	{
		if (own_)
		{
			ENN_SAFE_FREE(buffer_);
		}
	}

	unsigned MemoryBuffer::Read(void* dest, unsigned size)
	{
		if (size + position_ > size_)
			size = size_ - position_;
		if (!size) return 0;

		unsigned char* srcPtr = &buffer_[position_];
		unsigned char* destPtr = (unsigned char*)dest;
		position_ += size;

		unsigned copySize = size;
#if 1
		memcpy(destPtr, srcPtr, copySize);
#else
		while (copySize >= sizeof(unsigned))
		{
			*((unsigned*)destPtr) = *((unsigned*)srcPtr);
			srcPtr += sizeof(unsigned);
			destPtr += sizeof(unsigned);
			copySize -= sizeof(unsigned);
		}

		if (copySize & sizeof(unsigned short))
		{
			*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
			srcPtr += sizeof(unsigned short);
			destPtr += sizeof(unsigned short);
		}
		if (copySize & 1)
			*destPtr = *srcPtr;
#endif
		return size;
	}

	unsigned MemoryBuffer::Seek(unsigned position)
	{
		if (position > size_)
			position = size_;

		position_ = position;
		return position_;
	}

	unsigned MemoryBuffer::Write(const void* data, unsigned size)
	{
		if (size + position_ > size_)
			size = size_ - position_;
		if (!size)
			return 0;

		unsigned char* srcPtr = (unsigned char*)data;
		unsigned char* destPtr = &buffer_[position_];
		position_ += size;

		unsigned copySize = size;

#if 1
		memcpy(destPtr, srcPtr, copySize);
#else
		while (copySize >= sizeof(unsigned))
		{
			*((unsigned*)destPtr) = *((unsigned*)srcPtr);
			srcPtr += sizeof(unsigned);
			destPtr += sizeof(unsigned);
			size -= sizeof(unsigned);
		}
		if (copySize & sizeof(unsigned short))
		{
			*((unsigned short*)destPtr) = *((unsigned short*)srcPtr);
			srcPtr += sizeof(unsigned short);
			destPtr += sizeof(unsigned short);
		}
		if (copySize & 1)
			*destPtr = *srcPtr;
#endif

		return size;
	}
}