/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_memory_alloc.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_std_header.h"

namespace enn
{

class StdAlloc
{
public:
	static void* allocBytes( size_t size )
	{
		return ::operator new( size );
	}

	static void  freeBytes( void* p )
	{
		return ::operator delete( p );
	}
};

class DefaultAlloc
{
public:
	static void* allocBytes( size_t size );
	static void  freeBytes( void* p );
};

}