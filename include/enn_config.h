/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_config.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform.h"

#if ENN_DEBUG
#define ENN_MEMORY_TRACKER 0
#else
#define ENN_MEMORY_TRACKER 0
#endif

#define ENN_MEMORY_ALLOCATOR_STD      1
#define ENN_MEMORY_ALLOCATOR_DEFAULT  2
#define ENN_MEMORY_ALLOCATOR_USER     3

#ifndef ENN_MEMORY_ALLOCATOR
#define ENN_MEMORY_ALLOCATOR  ENN_MEMORY_ALLOCATOR_DEFAULT
#endif

/** stl container alloc policy */
#ifndef ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
#define ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR 1
#endif

/** stl string alloc policy */
#ifndef ENN_STRING_USE_CUSTOM_MEMORY_ALLOCATOR
#define ENN_STRING_USE_CUSTOM_MEMORY_ALLOCATOR 0
#endif