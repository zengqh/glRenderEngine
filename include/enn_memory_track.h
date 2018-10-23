/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_memory_track.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_std_header.h"
#include "enn_thread.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
class MemoryTracker
{
	ENN_OBJECT_MUTEX
private:
	struct BlockData
	{
		BlockData() : line(0), bytes(0) {}
		BlockData( const char* file_, int line_, const char* func_, int bytes_ ) :
		file(file_), func(func_), line(line_), bytes(bytes_) {}

		std::string file;
		std::string func;
		int         line;
		int         bytes;
	};

	typedef std::map<void*, BlockData> BlockDataMap;

public:
	MemoryTracker();
	~MemoryTracker();

	static MemoryTracker& instance();

public:
	void setReportFile( const std::string& file );
	void report();

public:
	void registerBytes( void* p, size_t size, const char* file, int line, const char* func );
	void unregisterBytes( void* p );

public:
	std::string  m_logFile;
	BlockDataMap m_blkDataMap;
	size_t       m_maxAllocBytes;
	size_t       m_currAllocBytes;
};

#define g_memoryTracker MemoryTracker::instance()
}