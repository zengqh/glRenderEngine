/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/30
* File: Flags.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_std_header.h"

namespace enn
{
template <size_t N> class Flags
{
public:
	Flags()
	{

	}

	virtual ~Flags()
	{

	}

	void set_flag()
	{
		flags_.set();
	}

	void set_flag(size_t pos)
	{
		flags_.set(pos);
	}

	void reset_flag()
	{
		flags_.reset();
	}

	void reset_flag(size_t pos)
	{
		flags_.reset(pos);
	}

	bool test_flag(size_t pos) const
	{
		return flags_.test(pos);
	}

	unsigned long to_ulong_flag() const
	{
		return flags_.to_ulong();
	}

	enn::String to_string_flag() const
	{
		return flags_.to_string();
	}

	bool any_flag() const
	{
		return flags_.any();
	}

	bool operator==(const Flags<N>& rhs) const
	{
		return flags_ == rhs.flags_;
	}

	/** it will fail in some case... */
	bool operator<(const Flags<N>& rhs) const
	{
		return flags_.to_ulong() < rhs.flags_.to_ulong();
	}
	

protected:
	std::bitset<N>	flags_;
};

class Flags32 : public Flags<32>
{

};

class Flags64 : public Flags<64>
{

};

class Flags128 : public Flags<128>
{

};


inline uint32 BITS(uint32 i)
{
	return 1 << i;
}

}