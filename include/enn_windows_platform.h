/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_windows_platform.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_system_platform.h"

#if ENN_PLATFORM == ENN_PLATFORM_WIN32

namespace enn
{
class WindowSystemPlatform : public ISystemPlatform, public AllocatedObject
{
public:
	struct CreateParam
	{
		CreateParam()
		{
			is_multisample_enabled = true;
			muti_samples = 2;
		}

		~CreateParam()
		{

		}

		bool is_multisample_enabled;
		int muti_samples;
	};

public:
	WindowSystemPlatform();
	~WindowSystemPlatform();

	virtual void release();
	virtual void message_box(const char* info, const char* caption);
	virtual void create_device(void* hwnd, void* create_param = 0);
	virtual void destroy_device();
	virtual void present();

protected:
	void check_multisample();

protected:
	void*						hwnd_;
	HDC							hdc_;
	HGLRC						hglrc_;
	PIXELFORMATDESCRIPTOR		m_pfd;
	bool						is_multisample_enabled_;
	int							multi_samples_;
	bool						is_multisample_support_;
	int							pixel_format_;
};

}

#endif