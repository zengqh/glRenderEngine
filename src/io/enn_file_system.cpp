/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/19
* File: file_system.cpp
**/

#include "enn_file_system.h"
#include "enn_file.h"
#include "enn_memory_buffer.h"
#include "enn_msg_id.h"
#include "enn_resource.h"

namespace enn
{
FileSystem::FileSystem()
	: msg_queue_mgr_(0)
	, file_listener_(0)
{

}

FileSystem::~FileSystem()
{

}

bool FileSystem::getFileData( const String& fileName, MemoryBuffer*& buff )
{
	File file(root_path_ + '/' + fileName);

	if (!file.IsOpen())
	{
		buff = 0;
		return false;
	}

	uchar* file_buffer = (uchar*)ENN_MALLOC(file.GetSize());

	file.Read(file_buffer, file.GetSize());

	buff = ENN_NEW MemoryBuffer(file_buffer, file.GetSize(), true);

	return true;
}

void FileSystem::freeFileData(MemoryBuffer* buff)
{
	ENN_DELETE buff;
}

void FileSystem::getAsyncFileData( const String& fileName, void* userData )
{
	Resource* res = static_cast<Resource*>(userData);
	msg_queue_mgr_->postMessage(MsgQueueManager::WORK_QUEUE, MQMSG_DEFAULT_FILE_SYSTEM, 0, userData, fileName, res->getLoadPriority());
}

void FileSystem::onWorkImpl( Message* msg )
{
	ENN_ASSERT(msg->msg == MQMSG_DEFAULT_FILE_SYSTEM);
	ENN_ASSERT(file_listener_);

	void* userData = msg->lparam;
	const String& strFile = msg->sparam;

	MemoryBuffer* mb = 0;
	if (getFileData(strFile, mb))
	{
		file_listener_->onGetFileResult(strFile, userData, mb, FS_GR_OK);
	}
	else
	{
		file_listener_->onGetFileResult(strFile, userData, mb, FS_GR_FAILED);
	}
}

bool shader_getFileData(const String& file_name, String& data)
{
	data.clear();

	FILE* fp = NULL;

	fp = fopen( file_name.c_str(), "rb" );

	if ( !fp )
	{
		return false;
	}

	fseek( fp, 0, SEEK_END );
	size_t len = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	data.resize( len );
	fread( &data[0], 1, len, fp );
	fclose( fp );
	return true;
}

}
