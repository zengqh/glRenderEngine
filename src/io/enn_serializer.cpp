/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/02/18
* File: serializer.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include <enn_serializer.h>

namespace enn
{
	Serializer::~Serializer()
	{
	}

	bool Serializer::WriteString(const enn::String& value)
	{
		const char* str = value.c_str();

		unsigned length = strlen(str);

		return Write(str, length + 1) == length + 1;
	}

	bool Serializer::WriteLine(const enn::String& value)
	{
		bool ret = true;
		unsigned char b1 = 13;
		unsigned char b2 = 10;

		const char* str = value.c_str();

		unsigned length = strlen(str);

		ret &= (Write(str, length) == length);
		ret &= (WriteBin(b1) == 1);
		ret &= (WriteBin(b2) == 1);

		return ret;
	}

	bool Serializer::WriteLenString(const enn::String& value)
	{
		const char* str = value.c_str();

		unsigned length = strlen(str) + 1;

		WriteBin(length);

		return Write(str, length) == length;
	}

	bool Serializer::WriteLenStringAlign4(const enn::String& value)
	{
		const char* str = value.c_str();

		unsigned length = strlen(str) + 1;

		unsigned adj_len = ENN_ALIGN_4_BYTES(length);
		unsigned pad_len = adj_len - length;

		WriteBin(adj_len);

		if (!(Write(str, length) == length))
		{
			ENN_ASSERT(0);
			return false;
		}

		if (pad_len > 0)
		{
			char dummy[4] = {0};

			Write(dummy, pad_len);
		}

		return true;
	}
}