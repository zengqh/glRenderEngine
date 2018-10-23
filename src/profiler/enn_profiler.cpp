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
// File: enn_profiler.cpp
//

#include "enn_profiler.h"
#include "enn_root.h"

namespace enn
{

static const int LINE_MAX_LENGTH = 256;
static const int NAME_MAX_LENGTH = 30;

//////////////////////////////////////////////////////////////////////////
ProfilerBlock::ProfilerBlock(ProfilerBlock* parent, const String& name)
: parent_(parent)
, name_(name)
{

}

ProfilerBlock::~ProfilerBlock()
{
	ENN_FOR_EACH(ProfilerBlockList, children_, it)
	{
		ProfilerBlock* blk = *it;
		ENN_SAFE_DELETE(blk);
	}
}

void ProfilerBlock::begin()
{
	hires_timer_.reset();
	interval_count_++;
}

void ProfilerBlock::end()
{
	interval_time_ += hires_timer_.getHiresTime();
}

void ProfilerBlock::end_frame()
{
	
}

void ProfilerBlock::begin_interval()
{
	interval_time_ = 0;
	interval_count_ = 0;
}

ProfilerBlock* ProfilerBlock::getChild(const String& name)
{
	ENN_FOR_EACH_CONST(ProfilerBlockList, children_, it)
	{
		ProfilerBlock* block = *it;
		if (block->name_ == name)
		{
			return block;
		}
	}

	ProfilerBlock* new_block = ENN_NEW ProfilerBlock(this, name);
	children_.push_back(new_block);

	return new_block;
}

bool ProfilerBlock::operator==(const ProfilerBlock& block) const
{
	return name_ == block.name_;
}

bool ProfilerBlock::operator<(const ProfilerBlock& block) const
{
	return name_ < block.name_;
}

//////////////////////////////////////////////////////////////////////////
Profiler::Profiler()
: current_(0)
, root_(0)
, fps_(0)
{
	root_ = ENN_NEW ProfilerBlock(0, "root");
	current_ = root_;

	ENN_CLEAR_BLOCK_ZERO(rs_block_);
}

Profiler::~Profiler()
{
	destroy();
}

void Profiler::begin_block(const String& name)
{
	current_ = current_->getChild(name);
	current_->begin();
}

void Profiler::end_block()
{
	if (current_ != root_)
	{
		current_->end();
		current_ = current_->parent_;
	}
}

void Profiler::begin_frame()
{
	begin_block("run_frame");
}

void Profiler::end_frame()
{
	if (current_ != root_)
	{
		end_block();
	}

	rs_block_.prims = ROOTPTR->get_render_system()->getRenderPrims();
	rs_block_.dps = ROOTPTR->get_render_system()->getRenderDPs();
}

void Profiler::updateRSBlock(const RSBlock& block)
{
	rs_block_ = block;
}

RSBlock& Profiler::getRSBlock()
{
	return rs_block_;
}

void Profiler::getFPSString(String& text)
{
	// fps
	String fps_text = String("FPS     ") + to_string(getFPS()); 
	// prims
	String primvs_text = String("PRIM   ") + to_string(rs_block_.prims);
	// dp
	String dp_text = String("DP      ") + to_string(rs_block_.dps);

	String delimiter = String("\n");

	text = fps_text + delimiter + primvs_text + delimiter + dp_text;
}

void Profiler::getProfilerString(String& text)
{
	text += String("block                               avg\n");

	getData(root_, text);
}

uint32 Profiler::getFPS() const
{
	return fps_;
}

uint32 Profiler::getFrameFps() const
{
	float elapsed_time = ROOTREF.get_time_system()->getElapsedTime();
	return uint32(1.0f / elapsed_time);
}

void Profiler::getData(ProfilerBlock* block, String& output)
{
	if (block != root_)
	{
		char line[LINE_MAX_LENGTH] = {0};
		char indentedName[LINE_MAX_LENGTH] = {0};;

		memset(indentedName, ' ', NAME_MAX_LENGTH);
		indentedName[0] = 0;
		strcat(indentedName, block->name_.c_str());
		indentedName[strlen(indentedName)] = ' ';
		indentedName[NAME_MAX_LENGTH] = 0;

		float avg = block->interval_time_ / (float)block->interval_count_;

		sprintf(line, "%s %8.3f\n", indentedName, avg);
		output += line;

		if (block->name_ == String("run_frame"))
		{
			if (enn::Math::isZero(avg))
			{
				fps_ = 0;
			}
			else
			{
				fps_ = uint32(1000.0f / avg);
			}
			
		}
	}

	ENN_FOR_EACH(ProfilerBlock::ProfilerBlockList, block->children_, it)
	{
		ProfilerBlock* child = *it;
		getData(child, output);
	}
}

void Profiler::destroy()
{
	ENN_SAFE_DELETE(root_);
}

//////////////////////////////////////////////////////////////////////////
// AutoProfileBlock
AutoProfileBlock::AutoProfileBlock(const String& name)
{
	ROOTPTR->getProfiler()->begin_block(name);
}

AutoProfileBlock::~AutoProfileBlock()
{
	ROOTPTR->getProfiler()->end_block();
}

}