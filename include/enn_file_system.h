/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/19
* File: file_system.h
**/

#pragma once

#include "enn_file.h"
#include "enn_memory_buffer.h"
#include "enn_msg_queue.h"
#include "enn_msg_id.h"

namespace enn
{

enum FileSystemGetResult
{
	FS_GR_OK,
	FS_GR_FAILED
};

struct IFileSystemListener
{
	virtual void onGetFileResult( const String& fileName, void* userData, MemoryBuffer* buff, FileSystemGetResult result ) = 0;
};

class FileSystem : public IMsgQueueListener, public Noncopyable, public AllocatedObject
{
public:
	FileSystem();
	virtual ~FileSystem();

	void setRootPath(const String& path)
	{
		root_path_ = path;
	}

	const String& getRootPath() const
	{
		return root_path_;
	}
	void setQueueManager(MsgQueueManager* msg_queue_mgr)
	{
		msg_queue_mgr_ = msg_queue_mgr;

		msg_queue_mgr_->registerMsg(MsgQueueManager::WORK_QUEUE, MQMSG_DEFAULT_FILE_SYSTEM, this);
	}

	void setFileListener(IFileSystemListener* listener)
	{
		file_listener_ = listener;
	}

	bool getFileData( const String& fileName, MemoryBuffer*& buff );
	void getAsyncFileData( const String& fileName, void* userData );
	void freeFileData(MemoryBuffer* buff);

	virtual void onWorkImpl( Message* msg );

protected:
	String					root_path_;
	MsgQueueManager*		msg_queue_mgr_;
	IFileSystemListener*	file_listener_;
};

bool shader_getFileData(const String& file_name, String& data);

}