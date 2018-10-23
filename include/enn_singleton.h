/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/30
* File: Singleton.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_noncopyable.h>

namespace enn
{
template <typename T> class Singleton : public Noncopyable
{
protected:
	static T* singleton_;

public:
	Singleton()
	{
		singleton_ = static_cast<T*>(this);
	}

	static T& getSingleton()
	{
		ENN_ASSERT(singleton_);
		return *singleton_;
	}
	static T* getSingletonPtr()
	{
		return singleton_;
	}
};
}