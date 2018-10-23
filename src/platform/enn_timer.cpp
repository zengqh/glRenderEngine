/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_timer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_timer.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// Timer
Timer::Timer()
{
	start_time_ = getCurrSystemTime();
}

Timer::~Timer()
{

}

uint32 Timer::getCurrSystemTime()
{
	uint32 start_time = 0;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	start_time = timeGetTime();
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	start_time = time.tv_sec * 1000 + time.tv_usec / 1000;
#endif

	return start_time;
}

float Timer::getTime() const
{
	uint32 curr_time = getCurrSystemTime();
	return (curr_time - start_time_) * 0.001f;
}

//////////////////////////////////////////////////////////////////////////
// TimeSystem
TimeSystem::TimeSystem()
{
	cur_time_ = timer_.getTime();
}

TimeSystem::~TimeSystem()
{

}

void TimeSystem::update()
{
	float cur_time = timer_.getTime();
	elapsed_time_ = cur_time - cur_time_;
	cur_time_ = cur_time;
}

float TimeSystem::getElapsedTime() const
{
	return elapsed_time_;
}

void TimeSystem::sleep(uint32 mill_sec)
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	::Sleep(mill_sec);
#else
	usleep(mill_sec * 1000);
#endif
}

uint32 TimeSystem::getCurrSystemTime()
{
	return Timer::getCurrSystemTime();
}

//////////////////////////////////////////////////////////////////////////
// HiresTimer
HiresTimer::HiresTimer()
: start_time_(0)
{
	reset();
}

HiresTimer::~HiresTimer()
{

}

void HiresTimer::reset()
{
	start_time_ = getCurrSystemTime();
}

uint32 HiresTimer::getStartTime() const
{
	return start_time_;
}

uint32 HiresTimer::getHiresTime() const
{
	uint32 curr_time = getCurrSystemTime();

	return curr_time - start_time_;
}

}