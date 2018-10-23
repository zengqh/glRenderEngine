/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/30
* File: enn_msg_queue.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_msg_queue.h"
#include "enn_work_queue.h"
#include "enn_timer.h"

namespace enn
{

void msg_workFunction(const WorkItem* item, unsigned thread_idx)
{
	if (!item)
	{
		return;
	}

	WorkMsgQueue* queue = static_cast<WorkMsgQueue*>(item->start_);
	Message* msg = static_cast<Message*>(item->aux_);
	IMsgQueueListener* listener = static_cast<IMsgQueueListener*>(item->end_);

	if (listener)
	{
		listener->onWorkImpl(msg);
	}

	queue->clearMessage(msg);
}

void WorkMsgQueue::postMessage(uint32 msg, void* wparam, void* lparam, 
							   const String& sparam, uint32 priority)
{
	ENN_LOCK_MUTEX

	ListenerMap::iterator iter = listener_map_.find(msg);
	ENN_ASSERT(iter != listener_map_.end());

	IMsgQueueListener* listener = iter->second;
	if (!listener)
	{
		return;
	}

	WorkItem item;

	Message* message = msg_pool_.requestObject();
	message->wparam = wparam;
	message->lparam = lparam;
	message->sparam = sparam;
	message->priority_ = priority;
	message->msg = msg;

	item.aux_ = message;
	item.priority_ = priority;
	item.start_ = this;
	item.end_ = listener;
	item.workFunction_ = msg_workFunction;

	work_queue_->add_work_item(item);
}

void UIMsgQueue::postMessage(uint32 msg, void* wparam, void* lparam, 
						 const String& sparam, uint32 priority)
{
	ENN_LOCK_MUTEX

	Message* message = msg_pool_.requestObject();
	message->wparam = wparam;
	message->lparam = lparam;
	message->sparam = sparam;
	message->priority_ = priority;
	message->msg = msg;

	for (MsgList::iterator i = msg_list_.begin(); i != msg_list_.end(); ++i)
	{
		if ((*i)->priority_ <= message->priority_)
		{
			msg_list_.insert(i, message);
			return;
		}
	}

	msg_list_.push_back(message);
}

void UIMsgQueue::update(float time_piece)
{
	deadline_time_ =  TimeSystem::getCurrSystemTime() * 0.001f + time_piece;

	ENN_LOCK_MUTEX
	while (msg_list_.size() && hasTimePiece())
	{
		Message* msg = msg_list_.front();
		IMsgQueueListener* listener = listener_map_[msg->msg];
		if (listener) listener->onWorkImpl(msg);
		msg_list_.pop_front();
		clearMessageNoLock(msg);
	}
}

void UIMsgQueue::postMessageNoLock(Message* msg)
{
	for (MsgList::iterator i = msg_list_.begin(); i != msg_list_.end(); ++i)
	{
		if ((*i)->priority_ <= msg->priority_)
		{
			msg_list_.insert(i, msg);
			return;
		}
	}

	msg_list_.push_back(msg);
}

bool  UIMsgQueue::hasTimePiece() const
{
	return TimeSystem::getCurrSystemTime() * 0.001f < deadline_time_;
}

float UIMsgQueue::getTimePieceLeft() const
{
	return deadline_time_ - TimeSystem::getCurrSystemTime() * 0.001f;
}

MsgQueueManager::MsgQueueManager(WorkQueue* work_queue)
: work_queue_(work_queue)
{
	ENN_ASSERT(work_queue);
	work_msg_queue_.setWorkQueue(work_queue);
}

MsgQueueManager::~MsgQueueManager()
{

}

void MsgQueueManager::registerQueue( uint32 queue_id, MsgQueueBase* queue)
{
	ENN_LOCK_MUTEX
	ENN_ASSERT(other_msg_queue_.find(queue_id) == other_msg_queue_.end());

	other_msg_queue_.insert(QueueMap::value_type(queue_id, queue));
}

void MsgQueueManager::registerMsg( uint32 queue_id, uint32 msg, 
								  IMsgQueueListener* listener )
{
	if (queue_id == UI_QUEUE)
	{
		ui_msg_queue_.registerMsg(msg, listener);
	}
	else if (queue_id == WORK_QUEUE)
	{
		work_msg_queue_.registerMsg(msg, listener);
	}
	else
	{
		MsgQueueBase* queue = 0;

		{
			ENN_LOCK_MUTEX
			QueueMap::iterator iter = other_msg_queue_.find(queue_id);
			ENN_ASSERT(iter != other_msg_queue_.end());

			queue = iter->second;
		}
		
		if (queue) queue->registerMsg(msg, listener);
	}
}

void MsgQueueManager::updateQueue( uint32 queue_id, float timePiece )
{
	if (queue_id == UI_QUEUE)
	{
		ui_msg_queue_.update(timePiece);
	}
	else if (queue_id == WORK_QUEUE)
	{
		work_msg_queue_.update(timePiece);
	}
	else
	{
		MsgQueueBase* queue = 0;

		{
			ENN_LOCK_MUTEX
				QueueMap::iterator iter = other_msg_queue_.find(queue_id);
			ENN_ASSERT(iter != other_msg_queue_.end());

			queue = iter->second;
		}


		if (queue) queue->update(timePiece);
	}
}

void MsgQueueManager::postMessage( uint32 queue_id, uint32 msg, 
								  void* wparam, void* lparam, const String& sparam, 
								  uint32 priority )
{
	if (queue_id == UI_QUEUE)
	{
		ui_msg_queue_.postMessage(msg, wparam, lparam, sparam, priority);
	}
	else if (queue_id == WORK_QUEUE)
	{
		work_msg_queue_.postMessage(msg, wparam, lparam, sparam, priority);
	}
	else
	{
		MsgQueueBase* queue = 0;

		{
			ENN_LOCK_MUTEX
			QueueMap::iterator iter = other_msg_queue_.find(queue_id);
			ENN_ASSERT(iter != other_msg_queue_.end());

			queue = iter->second;
		}

		ENN_ASSERT(queue);

		if (queue) queue->postMessage(msg, wparam, lparam, sparam, priority);
	}
}

void MsgQueueManager::clearMessage( uint32 queue_id, Message* msg)
{
	if (queue_id == UI_QUEUE)
	{
		ui_msg_queue_.clearMessage(msg);
	}
	else if (queue_id == WORK_QUEUE)
	{
		work_msg_queue_.clearMessage(msg);
	}
	else
	{
		MsgQueueBase* queue = 0;

		{
			ENN_LOCK_MUTEX
			QueueMap::iterator iter = other_msg_queue_.find(queue_id);
			ENN_ASSERT(iter != other_msg_queue_.end());

			queue = iter->second;
		}


		if (queue) queue->clearMessage(msg);
	}
}

}
