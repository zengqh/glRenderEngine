//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/09/12
// File: enn_profiler.h
//

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

struct RSBlock
{
	uint32		prims;
	uint32		dps;
};

//////////////////////////////////////////////////////////////////////////
class TimeSystem;
class _EnnExport ProfilerBlock : public AllocatedObject
{
public:
	typedef enn::list<ProfilerBlock*>::type		ProfilerBlockList;

public:
	ProfilerBlock(ProfilerBlock* parent, const String& name);
	virtual ~ProfilerBlock();

	void begin();
	void end();
	void end_frame();
	void begin_interval();
	ProfilerBlock* getChild(const String& name);

public:
	bool operator==(const ProfilerBlock& block) const;
	bool operator<(const ProfilerBlock& block) const;

public:
	String						name_;
	uint64						interval_time_;
	uint32						interval_count_;
	ProfilerBlock*				parent_;
	ProfilerBlockList			children_;
	HiresTimer					hires_timer_;
};


//////////////////////////////////////////////////////////////////////////
// Profiler system
class _EnnExport Profiler : public AllocatedObject
{
public:
	Profiler();
	virtual ~Profiler();

	void begin_block(const String& name);
	void end_block();

	void begin_frame();
	void end_frame();

public:
	void updateRSBlock(const RSBlock& block);
	RSBlock& getRSBlock();

public:
	void getFPSString(String& text);
	void getProfilerString(String& text);

public:
	uint32 getFPS() const;
	uint32 getFrameFps() const;

protected:
	void getData(ProfilerBlock* block, String& output);
	void destroy();

protected:
	ProfilerBlock*		current_;
	ProfilerBlock*		root_;
	RSBlock				rs_block_;
	uint32				fps_;
};

//////////////////////////////////////////////////////////////////////////
// AutoProfileBlock
class _EnnExport AutoProfileBlock : public enn::AllocatedObject
{
public:
	AutoProfileBlock(const String& name);
	virtual ~AutoProfileBlock();
};

#if ENN_ENABLE_PROFILING
#	define ENN_PROFILE(name)		enn::AutoProfileBlock profile_##name(#name)
#	define ENN_BEGIN_PROFILE(name)	ROOTPTR->getProfiler()->begin_block(#name)
#	define ENN_END_PROFILE()		ROOTPTR->getProfiler()->end_block()
#else
#	define ENN_PROFILE(name)
#	define ENN_BEGIN_PROFILE(name)
#	define ENN_END_PROFILE()
#endif

}