/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_memory_track.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_memory_track.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
static std::string _limitStr( const std::string& str, size_t len )
{
	if ( str.size() <= len )
		return str;    
	return str.substr(0, len);
}

//////////////////////////////////////////////////////////////////////////
MemoryTracker::MemoryTracker() : m_logFile("memory.txt"), m_maxAllocBytes(0), m_currAllocBytes(0)
{
}

MemoryTracker::~MemoryTracker()
{
	report();
}

MemoryTracker& MemoryTracker::instance()
{
	static MemoryTracker inst;
	return inst;
}

void MemoryTracker::setReportFile( const std::string& file )
{
	m_logFile = file;
}

void MemoryTracker::registerBytes( void* p, size_t size, const char* file, int line, const char* func )
{
	ENN_LOCK_MUTEX
		std::pair<BlockDataMap::iterator, bool> ret =
		m_blkDataMap.insert( BlockDataMap::value_type(p, BlockData(file, line, func, size)) );

	if (!ret.second)
	{
		//ENN_ASSERT(0);
	}
	m_currAllocBytes += size;
	if ( m_currAllocBytes > m_maxAllocBytes )
		m_maxAllocBytes = m_currAllocBytes;
}

void MemoryTracker::unregisterBytes( void* p )
{
	ENN_LOCK_MUTEX
	BlockDataMap::iterator it = m_blkDataMap.find( p );
	if ( it != m_blkDataMap.end() )
	{
		m_currAllocBytes -= it->second.bytes;
		m_blkDataMap.erase( it );
	}
	else
	{
		//ENN_ASSERT(0);
	}
}

void MemoryTracker::report()
{
	ENN_LOCK_MUTEX

		FILE* fp = 0;
	if ( !m_logFile.empty() )
		fp = fopen( m_logFile.c_str(), "w" );

	char buff[4096] = {};
	sprintf( buff, "Memory MaxAllocBytes: %d\n", m_maxAllocBytes );

	if ( fp ) fputs( buff, fp );

	for ( BlockDataMap::const_iterator it = m_blkDataMap.begin(), ite = m_blkDataMap.end(); it != ite; ++it )
	{
		void* ptr = it->first;
		const BlockData& bd = it->second;
		if ( bd.line < 0 )
			continue;

		sprintf( buff, "[0x%X] file:%s line:%d @%s -- %d bytes leak!\n", 
			ptr, _limitStr(bd.file, 1024).c_str(), bd.line, _limitStr(bd.func, 1024).c_str(), bd.bytes );

		if ( fp ) fputs( buff, fp );
	}

	if ( fp ) fclose(fp);
}
}