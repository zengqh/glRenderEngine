/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_memory.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include <enn_memory.h>
#include <enn_memory_alloc.h>
#include <enn_memory_track.h>


#if ENN_MEMORY_ALLOCATOR == ENN_MEMORY_ALLOCATOR_STD
#define MemAlloc_ StdAlloc
#elif ENN_MEMORY_ALLOCATOR == ENN_MEMORY_ALLOCATOR_DEFAULT
#define MemAlloc_ DefaultAlloc
#else
#define MemAlloc_ UserAlloc
#endif

namespace enn
{

//////////////////////////////////////////////////////////////////////////
void* Memory::allocBytes( size_t size )
{
	void* p = MemAlloc_::allocBytes( size );
#if ENN_MEMORY_TRACKER
	g_memoryTracker.registerBytes( p, size, "", -1, "" );
#endif
	return p;
}

void* Memory::allocBytes( size_t size, const char* file, int line, const char* func )
{
	void* p = MemAlloc_::allocBytes( size );
#if ENN_MEMORY_TRACKER
	g_memoryTracker.registerBytes( p, size, file, line, func );
#endif
	return p;
}

void Memory::freeBytes( void* p )
{
	MemAlloc_::freeBytes( p );
#if ENN_MEMORY_TRACKER
	g_memoryTracker.unregisterBytes( p );
#endif
}

size_t Memory::getMaxAllocBytes()
{
	return std::numeric_limits<size_t>::max();
}

}