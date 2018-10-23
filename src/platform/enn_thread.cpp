/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_thread.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include <enn_thread.h>

namespace enn
{
//////////////////////////////////////////////////////////////////////////
void* ThreadFunctionStatic(void* data)
{
	Thread* thread = static_cast<Thread*>(data);
	thread->ThreadFunction();
	pthread_exit((void*)0);
	return 0;
}

Thread::Thread() :
handle_(0),
shouldRun_(false)
{
}

Thread::~Thread()
{
	stop();
}

bool Thread::start()
{
	// Check if already running
	if (handle_)
		return false;

	shouldRun_ = true;

	handle_ = new pthread_t;
	pthread_create((pthread_t*)handle_, 0, ThreadFunctionStatic, this);

	return handle_ != 0;
}

void Thread::stop()
{
	// Check if already stopped
	if (!handle_)
		return;

	shouldRun_ = false;

	pthread_t* thread = (pthread_t*)handle_;
	if (thread)
	{
		pthread_join(*thread, 0);
	}

	delete thread;

	handle_ = 0;
}

//////////////////////////////////////////////////////////////////////////
Mutex::Mutex() :
handle_(new pthread_mutex_t)
{
	pthread_mutex_t* mutex = (pthread_mutex_t*)handle_;
	pthread_mutex_init(mutex, 0);
}

Mutex::~Mutex()
{
	pthread_mutex_t* mutex = (pthread_mutex_t*)handle_;
	pthread_mutex_destroy(mutex);
	delete mutex;
	handle_ = 0;
}

void Mutex::acquire()
{
	pthread_mutex_lock((pthread_mutex_t*)handle_);
}

void Mutex::release()
{
	pthread_mutex_unlock((pthread_mutex_t*)handle_);
}

//////////////////////////////////////////////////////////////////////////
MutexLock::MutexLock(Mutex& mutex) :
mutex_(mutex)
{
	mutex_.acquire();
}

MutexLock::~MutexLock()
{
	mutex_.release();
}

//////////////////////////////////////////////////////////////////////////
// Signal
Signal::Signal()
: handle_(new pthread_cond_t)
{
	pthread_cond_t* cond = (pthread_cond_t*)handle_;
	pthread_cond_init(cond, 0);
}

Signal::~Signal()
{
	pthread_cond_t* cond = (pthread_cond_t*)handle_;
	pthread_cond_destroy(cond);

	delete handle_;
}

void Signal::set()
{
	pthread_cond_t* cond = (pthread_cond_t*)handle_;
	pthread_cond_signal(cond);
}

void Signal::wait(Mutex* mutex)
{
	pthread_cond_t* cond = (pthread_cond_t*)handle_;
	pthread_mutex_t* mtx = (pthread_mutex_t*)mutex->getHandle();

	/** wait */
	pthread_cond_wait(cond, mtx);
}

void Signal::broadcast()
{
	pthread_cond_t* cond = (pthread_cond_t*)handle_;
	pthread_cond_broadcast(cond);
}

}

