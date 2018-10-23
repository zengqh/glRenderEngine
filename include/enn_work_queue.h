/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_work_queue.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_thread.h"
#include "enn_memory.h"
#include "enn_singleton.h"
#include "enn_noncopyable.h"
#include "enn_std_types.h"
#include "enn_timer.h"

namespace enn
{
class WorkerThread;

struct WorkItem : public AllocatedObject
{
	WorkItem() :
	priority_(0xffffffff),
	completed_(false)
	{

	}

	bool operator < (const WorkItem& other) const
	{
		return priority_ < other.priority_;
	}

	bool operator > (const WorkItem& other) const
	{
		return priority_ > other.priority_;
	}

	void (*workFunction_)(const WorkItem*, unsigned);
	void* start_;
	void* end_;
	void* aux_;
	unsigned priority_;
	volatile bool completed_;
};

class WorkQueue : public AllocatedObject, public Noncopyable
{
	friend class WorkerThread;

public:
	WorkQueue();
	~WorkQueue();

	void init();
	void uninit();

	void create_threads(unsigned numThreads);

	void add_work_item(const WorkItem& item);

	unsigned get_thead_count() const 
	{ 
		return threads_.size(); 
	}

protected:
	void process(unsigned threadIndex);

protected:
	enn::vector<WorkerThread*>::type	threads_;
	enn::list<WorkItem>::type			workItems_;
	Mutex								queueMutex_;
	volatile bool						is_quit_;
	Signal								signal_;
};

}