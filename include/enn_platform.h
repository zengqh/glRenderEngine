/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_platform.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#define ENN_PLATFORM_WIN32		1
#define ENN_PLATFORM_IOS		2
#define ENN_PLATFORM_ANDROID	3

#if defined(WIN32) || defined(_WIN32) || defined(_MSC_VER)
#define ENN_PLATFORM ENN_PLATFORM_WIN32
#elif defined(ENN_ANDROID)
#define ENN_PLATFORM ENN_PLATFORM_ANDROID
#elif defined(ENN_IOS)
#define ENN_PLATFORM ENN_PLATFORM_IOS
#else
#pragma error "Unknown platform."
#endif

#if defined(DEBUG) || defined(_DEBUG)
#define ENN_DEBUG 1
#else
#define ENN_DEBUG 0
#endif

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#if defined(ENN_STATIC)
#	define _EnnExport
#else
#	if defined(ENN_NOCLIENT_BUILD)
#		define _EnnExport __declspec( dllexport )
#	else
#		define _EnnExport __declspec( dllimport )
#endif
#endif
#else
#	define _EnnExport
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#pragma warning(disable:4251)
#endif