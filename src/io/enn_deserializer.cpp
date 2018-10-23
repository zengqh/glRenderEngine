/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/18
* File: deserializer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_deserializer.h"

namespace enn
{

Deserializer::Deserializer() :
position_(0),
	size_(0)
{
}

Deserializer::Deserializer(unsigned size) :
position_(0),
	size_(size)
{
}

Deserializer::~Deserializer()
{
}


enn::String Deserializer::ReadString()
{
	enn::String ret;

	for (;;)
	{
		char c;
		ReadBin(c);
		if (!c)
			break;
		else
			ret += c;
	}

	return ret;
}

enn::String Deserializer::ReadLine()
{
	enn::String ret;

	while (!IsEof())
	{
		char c;
		ReadBin(c);

		if (c == 10)
			break;
		if (c == 13)
		{
			// Peek next char to see if it's 10, and skip it too
			if (!IsEof())
			{
				char next;
				ReadBin(next);
				if (next != 10)
					Seek(position_ - 1);
			}
			break;
		}

		ret += c;
	}

	return ret;
}

/// Read a len + null-terminated string.
enn::String Deserializer::ReadLenString()
{
	String str;

	int len = 0;
	ReadBin(len);

	char* raw_str = (char*)ENN_MALLOC(len);
	Read(raw_str, len);
	str = raw_str;

	ENN_SAFE_FREE(raw_str);

	return str;
}

/// Read a len + null-terminated string, align 4.
enn::String Deserializer::ReadLenStringAlign4()
{
	String str;

	int len = 0;
	ReadBin(len);

	ENN_ASSERT(len % 4 == 0);

	char* raw_str = (char*)ENN_MALLOC(len);
	Read(raw_str, len);
	str = raw_str;

	ENN_SAFE_FREE(raw_str);

	return str;
}

}