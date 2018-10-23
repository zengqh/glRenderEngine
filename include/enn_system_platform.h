/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_system_platform.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

namespace enn
{

struct ISystemPlatform
{
	virtual void release() = 0;
	virtual void message_box(const char* info, const char* caption) = 0;
	virtual void create_device(void* hwnd, void* create_param = 0) = 0;
	virtual void destroy_device() = 0;
	virtual void present() = 0;
};
	
}