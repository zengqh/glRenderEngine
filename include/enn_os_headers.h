/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_os_headers.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform.h"

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

/** required to stop windows.h messing up std::min */
#if !defined(NOMINMAX) && defined(_MSC_VER)
#define NOMINMAX 
#endif

#include <windows.h>
#include <process.h>
#include <mmsystem.h>
#include <pthread_win32/pthread.h>
#elif ENN_PLATFORM == ENN_PLATFORM_ANDROID
#include <pthread.h>
#include <android/log.h>
#elif ENN_PLATFORM == ENN_PLATFORM_IOS
#include <pthread.h>
#endif

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#if ENN_DEBUG
#define ENN_ASSERT(p)	{ if (!(p)) ::DebugBreak(); }
#define ENN_ASSERT_IMPL(p)	ENN_ASSERT(p)
#else
#define ENN_ASSERT(p)
#define ENN_ASSERT_IMPL(p)		(p)
#endif
#else
#if ENN_DEBUG
#define ENN_ASSERT(p)	assert(p)
#define ENN_ASSERT_IMPL(p)	ENN_ASSERT(p)
#else
#define ENN_ASSERT(p)
#define ENN_ASSERT_IMPL(p)		(p)
#endif
#endif


#if ENN_PLATFORM == ENN_PLATFORM_ANDROID
#define  LOG_TAG    "enn"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif