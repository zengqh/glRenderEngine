/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/18
* File: serializer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_platform_headers.h>

namespace enn
{
class Serializer
{
public:
	/// Destruct.
	virtual ~Serializer();

	/// Write bytes to the stream. Return number of bytes actually written.
	virtual unsigned Write(const void* data, unsigned size) = 0;

	virtual bool WriteString(const enn::String& value);
	virtual bool WriteLine(const enn::String& value);

	virtual bool WriteLenString(const enn::String& value);
	virtual bool WriteLenStringAlign4(const enn::String& value);

	template<class T>
	unsigned WriteBin( T& t )
	{
		return Write(&t, sizeof(t));
	}

	template<class T>
	unsigned WriteBin( T* t, int n )
	{
		unsigned ret = 0;
		for ( int i = 0; i < n; ++i )
		{
			ret += WriteBin( *t );
			++t;
		}

		return ret;
	}

	template<class T>
	unsigned WriteObject( T& t )
	{
		return Write((const void*)(t.Data()), sizeof(t));
	}
};
}