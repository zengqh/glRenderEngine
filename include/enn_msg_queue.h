/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_msg_queue.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_os_headers.h"
#include "enn_std_header.h"
#include "enn_std_types.h"
#include "enn_memory.h"
#include "enn_math.h"
#include "enn_object_recycle.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// 消息结构和事件
struct Message : public AllocatedObject
{
	Message() : lparam(0), wparam(0), msg(0), priority_(0) {}
	void*   lparam;
	void*   wparam;
	String  sparam;
	uint32  msg;
	uint32	priority_;
};

struct IMsgQueueListener
{
	virtual void onWorkImpl( Message* msg ) = 0;
};

//////////////////////////////////////////////////////////////////////////
// 消息队列基本
class MsgQueueBase : public AllocatedObject
{
public:
	ENN_DECLARE_RTTI(MsgQueueBase)
protected:
	ENN_OBJECT_MUTEX
public:
	typedef ObjectRecycle<Message>							MsgPool;
	typedef enn::list<Message*>::type						MsgList;
	typedef enn::map<uint32, IMsgQueueListener*>::type		ListenerMap;

public:
	MsgQueueBase() {}
	virtual ~MsgQueueBase() {}

public:
	virtual void postMessage(uint32 msg, void* wparam, void* lparam, 
		const String& sparam, uint32 priority = Math::ENN_MAX_UNSIGNED)
	{

	}

	virtual void clearMessage(Message* msg)
	{
		if (!msg)
		{
			return;
		}

		ENN_LOCK_MUTEX

		msg_pool_.freeObject(msg);
	}

	virtual void clearMessageNoLock(Message* msg)
	{
		if (!msg)
		{
			return;
		}

		msg_pool_.freeObject(msg);
	}

	virtual void update(float time_piece = 0.0f)
	{
		
	}

	virtual void registerMsg(uint32 msg, IMsgQueueListener* listener )
	{
		ENN_LOCK_MUTEX
		ENN_ASSERT(listener_map_.find(msg) == listener_map_.end());
		listener_map_.insert(ListenerMap::value_type(msg, listener));
	}

protected:
	MsgPool		msg_pool_;
	MsgList		msg_list_;
	ListenerMap listener_map_;
};

//////////////////////////////////////////////////////////////////////////
// 工作线程消息队列
class WorkMsgQueue : public MsgQueueBase
{
	ENN_DECLARE_RTTI(WorkMsgQueue)

public:
	WorkMsgQueue()
	{

	}

	virtual ~WorkMsgQueue()
	{

	}

	virtual void postMessage(uint32 msg, void* wparam, void* lparam, 
		const String& sparam, uint32 priority = 0);

	void setWorkQueue(WorkQueue* work_queue)
	{
		work_queue_ = work_queue;
	}

protected:
	WorkQueue* work_queue_;
};

//////////////////////////////////////////////////////////////////////////
// ui消息队列
class UIMsgQueue : public MsgQueueBase
{
	ENN_DECLARE_RTTI(UIMsgQueue)

public:
	UIMsgQueue() {}
	virtual ~UIMsgQueue() {}

public:
	virtual void postMessage(uint32 msg, void* wparam, void* lparam, 
		const String& sparam, uint32 priority = Math::ENN_MAX_UNSIGNED);

	virtual void update(float time_piece = 0.0f);
protected:
	void postMessageNoLock(Message* msg);

	bool  hasTimePiece() const;
	float getTimePieceLeft() const;

protected:
	float deadline_time_;
};

//////////////////////////////////////////////////////////////////////////
// 消息队列管理
class MsgQueueManager : public Noncopyable, public AllocatedObject
{
	ENN_OBJECT_MUTEX
public:
	enum
	{
		UI_QUEUE,
		WORK_QUEUE
	};

	typedef enn::map<uint32, MsgQueueBase*>::type QueueMap;

public:
	MsgQueueManager(WorkQueue* work_queue);
	~MsgQueueManager();

public:
	void registerQueue( uint32 queue_id, MsgQueueBase* queue);
	void registerMsg( uint32 queue_id, uint32 msg, IMsgQueueListener* listener );
	void updateQueue( uint32 queue_id, float timePiece );

	void postMessage( uint32 queue_id, uint32 msg, void* wparam, void* lparam, const String& sparam, uint32 priority = Math::ENN_MAX_UNSIGNED );
	void clearMessage( uint32 queue_id, Message* msg);

protected:
	WorkMsgQueue	work_msg_queue_;
	UIMsgQueue		ui_msg_queue_;
	QueueMap		other_msg_queue_;

protected:
	WorkQueue*		work_queue_;
};

}