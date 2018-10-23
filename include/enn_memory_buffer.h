/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/19
* File: memory_buffer.h
**/

#pragma once

#include "enn_deserializer.h"
#include "enn_serializer.h"

namespace enn
{
	class MemoryBuffer : public AllocatedObject, public Deserializer, public Serializer
	{
	public:
		MemoryBuffer(void* data, unsigned size, bool own = false);
		virtual ~MemoryBuffer();
		/// Read bytes from the memory area. Return number of bytes actually read.
		virtual unsigned Read(void* dest, unsigned size);
		/// Set position from the beginning of the memory area.
		virtual unsigned Seek(unsigned position);
		/// Write bytes to the memory area.
		virtual unsigned Write(const void* data, unsigned size);

		/// Return the file handle.
		void* GetHandle() const { return buffer_; }

	protected:
		/// Pointer to the memory area.
		unsigned char* buffer_;
		bool own_;
	};
}