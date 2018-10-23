/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_timer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

/////////////////////////////////////////////////////////////////////////
// Timer
class Timer : public AllocatedObject, public Noncopyable
{
public:
	Timer();
	~Timer();

public:
	static uint32 getCurrSystemTime();

	float getTime() const;
protected:
	uint32 start_time_;
};

//////////////////////////////////////////////////////////////////////////
// TimeSystem
class TimeSystem : public AllocatedObject, public Noncopyable
{
public:
	TimeSystem();
	~TimeSystem();

	void update();

	float getElapsedTime() const;

	static void sleep(uint32 mill_sec);

	static uint32 getCurrSystemTime();

protected:
	Timer		timer_;
	float		cur_time_;
	float		elapsed_time_;
};

//////////////////////////////////////////////////////////////////////////
// HiresTimer
class HiresTimer : public Timer
{
public:
	HiresTimer();
	virtual ~HiresTimer();

public:
	void reset();
	uint32 getStartTime() const;
	uint32 getHiresTime() const;

protected:
	uint32	start_time_;
};

}