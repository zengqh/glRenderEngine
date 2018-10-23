/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_windows_platform.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_windows_platform.h"
#include "enn_gl.h"

#if ENN_PLATFORM == ENN_PLATFORM_WIN32

namespace enn
{
WindowSystemPlatform::WindowSystemPlatform()
: hwnd_(0)
, hdc_(0)
, hglrc_(0)
, is_multisample_support_(false)
, is_multisample_enabled_(false)
, pixel_format_(0)
, multi_samples_(2)
{

}

WindowSystemPlatform::~WindowSystemPlatform()
{
	destroy_device();
}

void WindowSystemPlatform::release()
{
	ENN_DELETE this;
}

void WindowSystemPlatform::message_box(const char* info, const char* caption)
{
	::MessageBoxA(0, info, caption, MB_OK);
}

void WindowSystemPlatform::create_device(void* hwnd, void* create_param)
{
	if (hglrc_)
	{
		return;
	}

	if (create_param)
	{
		WindowSystemPlatform::CreateParam* cp = (WindowSystemPlatform::CreateParam*)create_param;
		is_multisample_enabled_ = cp->is_multisample_enabled;
		multi_samples_ = cp->muti_samples;
	}

	hwnd_ = hwnd;
	hdc_ = ::GetDC((HWND)hwnd_);

	ZeroMemory(&m_pfd, sizeof(m_pfd));
	m_pfd.nSize = sizeof(m_pfd);
	m_pfd.nVersion = 1;
	m_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	m_pfd.iPixelType = PFD_TYPE_RGBA;
	m_pfd.cColorBits = 32;
	m_pfd.cDepthBits = 24;
	m_pfd.iLayerType = PFD_MAIN_PLANE;

	if (is_multisample_enabled_)
	{
		check_multisample();
	}
	else
	{
		pixel_format_ = ChoosePixelFormat(hdc_, &m_pfd);
	}

	SetPixelFormat(hdc_, pixel_format_, &m_pfd);

	hglrc_ = wglCreateContext(hdc_);
	wglMakeCurrent(hdc_, hglrc_);

	wglSwapIntervalEXT(0);
}

void WindowSystemPlatform::destroy_device()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hglrc_);
	ReleaseDC((HWND)hwnd_, hdc_);
}

void WindowSystemPlatform::present()
{
	SwapBuffers(hdc_);
}

/** only check msaa, fsaa and csaa is not supported now :) */
void WindowSystemPlatform::check_multisample()
{
	ENN_ASSERT(hwnd_);

	HWND hwnd = ::CreateWindowA("STATIC", "", WS_CHILD, 0, 0, 0, 0, (HWND)hwnd_,
		0, 0, 0);

	int pixel_format = 0;
	HDC hdc = ::GetDC(hwnd);

	pixel_format = ChoosePixelFormat(hdc, &m_pfd);

	ENN_ASSERT(pixel_format != 0);

	if (!SetPixelFormat(hdc, pixel_format, &m_pfd))
	{
		ENN_ASSERT(0);
	}

	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);

	GLint format = 0;

	GLeeInit();

	/** check msaa */
	if (GLEE_WGL_ARB_multisample)
	{
		is_multisample_support_ = true;
	}
	
	if (is_multisample_support_)
	{
		int iAttributes[] =
		{
			WGL_DRAW_TO_WINDOW_ARB,GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,GL_TRUE,
			WGL_ACCELERATION_ARB,WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB,24,
			WGL_ALPHA_BITS_ARB,8,
			WGL_DEPTH_BITS_ARB,24,
			WGL_STENCIL_BITS_ARB,0,
			WGL_DOUBLE_BUFFER_ARB,GL_TRUE,
			WGL_SAMPLE_BUFFERS_ARB,GL_TRUE,
			WGL_SAMPLES_ARB,multi_samples_,
			0,0
		};

		float fAttributes[] = {0,0};
		int pixelFormat;
		UINT numFormats;

		int is_valid = wglChoosePixelFormatARB(hdc_,iAttributes,fAttributes,1,&pixelFormat,&numFormats);

		if ((is_valid && numFormats > 0))
		{
			format = pixelFormat;
		}
		else
		{
			format = ChoosePixelFormat(hdc_, &m_pfd);
		}
	}
	else
	{
		format = ChoosePixelFormat(hdc_, &m_pfd);
	}

	wglMakeCurrent(0, 0);
	wglDeleteContext(hglrc);
	ReleaseDC(hwnd, hdc);
	DestroyWindow(hwnd);

	pixel_format_ = format;
}

}

#endif