/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_thread.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_os_headers.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
/// Operating system thread.
class Thread
{
public:
	/// Construct. Does not start the thread yet.
	Thread();
	/// Destruct. If running, stop and wait for thread to finish.
	virtual ~Thread();

	/// The function to run in the thread.
	virtual void ThreadFunction() = 0;

	/// Start running the thread. Return true if successful, or false if already running or if can not create the thread.
	bool start();
	/// Set the running flag to false and wait for the thread to finish.
	void stop();

	/// Return whether thread exists.
	bool is_started() const { return handle_ != 0; }

protected:
	/// Thread handle.
	void* handle_;
	/// Running flag.
	volatile bool shouldRun_;
};

//////////////////////////////////////////////////////////////////////////
/// Operating system mutual exclusion primitive.
class Mutex
{
public:
	/// Construct.
	Mutex();
	/// Destruct.
	~Mutex();

	/// Acquire the mutex. Block if already acquired.
	void acquire();
	/// Release the mutex.
	void release();

	void* getHandle() const
	{
		return handle_;
	}

private:
	/// Mutex handle.
	void* handle_;
};

//////////////////////////////////////////////////////////////////////////
/// Lock that automatically acquires and releases a mutex.
class MutexLock
{
public:
	/// Construct and acquire the mutex.
	MutexLock(Mutex& mutex);
	/// Destruct. Release the mutex.
	~MutexLock();

private:
	/// Mutex reference.
	Mutex& mutex_;
};

//////////////////////////////////////////////////////////////////////////
// Signal
class Signal
{
public:
	Signal();
	~Signal();

public:
	void set();
	void wait(Mutex* mutex);
	void broadcast();

protected:
	void*		handle_;
};

}

#define ENN_OBJECT_MUTEX  enn::Mutex e_mutex_;
#define ENN_LOCK_MUTEX    enn::MutexLock e_lg_(e_mutex_);