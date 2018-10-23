/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_std_types.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_std_header.h"
#include "enn_memory.h"

namespace enn
{
template <class T> 
struct vector 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::vector<T, STLAllocator<T> > type;
#else
	typedef typename std::vector<T> type;
#endif
};

template <class T> 
struct list 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::list<T, STLAllocator<T> > type;
#else
	typedef typename std::list<T> type;
#endif
}; 

template <class K, class V, class P = std::less<K> > 
struct map
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::map<K, V, P, STLAllocator<std::pair<const K, V> > > type;
#else
	typedef typename std::map<K, V, P> type;
#endif
};

template <class T, class P = std::less<T> > 
struct set 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::set<T, P, STLAllocator<T> > type;
#else
	typedef typename std::set<T, P> type;
#endif
}; 

template <class K, class V, class P = std::less<K> > 
struct multimap 
{ 
#if ENN_CONTAINERS_USE_CUSTOM_MEMORY_ALLOCATOR
	typedef typename std::multimap<K, V, P, STLAllocator<std::pair<const K, V> > > type;
#else
	typedef typename std::multimap<K, V, P> type;
#endif
};

template <size_t N>
struct bitset
{
	typedef typename std::bitset<N> type;
};
//////////////////////////////////////////////////////////////////////////
#if ENN_STRING_USE_CUSTOM_MEMORY_ALLOCATOR
typedef std::basic_string<char, std::char_traits<char>, STLAllocator<char> >            String;
typedef std::basic_string<wchar_t, std::char_traits<wchar_t>, STLAllocator<wchar_t> >   WString;
#else
typedef std::string     String;
typedef std::wstring    WString;
#endif
}