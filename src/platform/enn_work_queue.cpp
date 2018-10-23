/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_work_queue.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_work_queue.h"

namespace enn
{
static bool sort_work_item(const WorkItem& i1, const WorkItem& i2)
{
	return i1.priority_ > i2.priority_;
}

//////////////////////////////////////////////////////////////////////////
class WorkerThread : public AllocatedObject, public Noncopyable, public Thread
{
public:
	WorkerThread(WorkQueue* owner, unsigned index) : owner_(owner), index_(index)
	{

	}

	virtual void ThreadFunction()
	{
	  owner_->process(index_);
	}

	unsigned GetIndex() const { return index_; }

private:
	WorkQueue* owner_;
	unsigned index_;
};

//////////////////////////////////////////////////////////////////////////
WorkQueue::WorkQueue()
: is_quit_(false)
{

}

WorkQueue::~WorkQueue()
{
	
}

void WorkQueue::init()
{

}

void WorkQueue::uninit()
{
	if (is_quit_)
	{
		return;
	}

	is_quit_ = true;

	signal_.broadcast();

	for (unsigned i = 0; i < threads_.size(); ++i)
	{
		threads_[i]->stop();

		ENN_SAFE_DELETE(threads_[i]);
	}
}

void WorkQueue::create_threads(unsigned numThreads)
{
	ENN_ASSERT(threads_.empty())

	for (unsigned i = 0; i < numThreads; ++i)
	{
		WorkerThread* thr(ENN_NEW WorkerThread(this, i));
		thr->start();
		threads_.push_back(thr);
	}
}

void WorkQueue::add_work_item(const WorkItem& item)
{
	queueMutex_.acquire();

	workItems_.push_back(item);
	WorkItem* itemPtr = &workItems_.back();
	itemPtr->completed_ = false;

	workItems_.sort(sort_work_item);

	queueMutex_.release();

	signal_.set();
}

void WorkQueue::process(unsigned threadIndex)
{
	for (;;)
	{
		queueMutex_.acquire();

		if (workItems_.empty() && !is_quit_)
		{
			signal_.wait(&queueMutex_);
		}

		if (is_quit_)
		{
			queueMutex_.release();
			pthread_exit(0);
		}

		if (workItems_.empty())
		{
			queueMutex_.release();
			continue;
		}

		WorkItem item = workItems_.front();
		workItems_.pop_front();
		queueMutex_.release();

		item.workFunction_(&item, threadIndex);
		item.completed_ = true;
	}
}

}