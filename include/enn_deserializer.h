/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/18
* File: deserializer.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_platform_headers.h>

namespace enn
{
/// Abstract stream for reading.
class Deserializer
{
public:
	/// Construct with zero size.
	Deserializer();
	/// Construct with defined size.
	Deserializer(unsigned size);
	/// Destruct.
	virtual ~Deserializer();

	/// Read bytes from the stream. Return number of bytes actually read.
	virtual unsigned Read(void* dest, unsigned size) = 0;
	/// Set position from the beginning of the stream.
	virtual unsigned Seek(unsigned position) = 0;
	/// Return current position.
	unsigned GetPosition() const { return position_; }
	/// Return size.
	unsigned GetSize() const { return size_; }
	/// Return whether the end of stream has been reached.
	bool IsEof() const { return position_ >= size_; }

	template<class T>
	unsigned ReadBin( T& t )
	{
		return Read(&t, sizeof(t));
	}

	template<class T>
	unsigned ReadBin( T* t, int n )
	{
		unsigned ret = 0;
		for ( int i = 0; i < n; ++i )
		{
			ret += ReadBin( *t );
			++t;
		}

		return ret;
	}

	/// Read a null-terminated string.
	enn::String ReadString();
	/// Read a text line.
	enn::String ReadLine();

	/// Read a len + null-terminated string.
	virtual enn::String ReadLenString();
	/// Read a len + null-terminated string, align 4.
	virtual enn::String ReadLenStringAlign4();

protected:
	/// Stream position.
	unsigned position_;
	/// Stream size.
	unsigned size_;
};

}