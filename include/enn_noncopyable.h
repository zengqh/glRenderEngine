/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/30
* File: Noncopyable.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_config.h>
#include <enn_std_header.h>

namespace enn
{
class Noncopyable
{
protected:
	Noncopyable() {}
	~Noncopyable() {}
private:
	Noncopyable( const Noncopyable& );
	const Noncopyable& operator=( const Noncopyable& );
};
}