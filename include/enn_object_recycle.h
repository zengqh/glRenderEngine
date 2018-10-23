/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/29
* File: enn_object_recycle.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_os_headers.h"
#include "enn_std_header.h"
#include "enn_std_types.h"
#include "enn_memory.h"

namespace enn
{

template<uint32 n>
struct IntToType
{
	static const uint32 value = n;
};

template<class T, int FREE_METHOD = SPFM_DELETE>
class ObjectRecycle : public AllocatedObject
{
public:
	typedef typename enn::vector<T*>::type ObjList;

public:
	~ObjectRecycle()
	{
		clearPool();
	}

	void clearPool()
	{
		for ( typename ObjList::iterator it = m_unusedList.begin(), ite = m_unusedList.end(); it != ite; ++it )
		{
			T* obj = *it;
			_destroy( obj, IntToType<FREE_METHOD>() );
		}

		m_unusedList.clear();
	}

	T* requestObject()
	{
		if ( m_unusedList.size() )
		{
			T* obj = m_unusedList.back();
			m_unusedList.resize( m_unusedList.size() - 1 );
			return obj;
		}
		else
		{
			T* obj = _create( IntToType<FREE_METHOD>() );
			return obj;
		}
	}

	void freeObject( T* obj )
	{
		m_unusedList.push_back( obj );
	}

private:
	T* _create( IntToType<SPFM_FREE> )
	{
		return ENN_MALLOC_T(T);
	}

	void _destroy( T* obj, IntToType<SPFM_FREE> )
	{
		ENN_FREE(obj);
	}

	T* _create( IntToType<SPFM_DELETE_T> )
	{
		return ENN_NEW_T(T)();
	}

	void _destroy( T* obj, IntToType<SPFM_DELETE_T> )
	{
		ENN_DELETE_T(obj);
	}

	T* _create( IntToType<SPFM_DELETE> )
	{
		return ENN_NEW T;
	}

	void _destroy( T* obj, IntToType<SPFM_DELETE> )
	{
		ENN_DELETE obj;
	}

private:
	ObjList m_unusedList;
};
}