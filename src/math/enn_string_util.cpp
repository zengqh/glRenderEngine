/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/16
* File: string_util.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_string_util.h"

namespace enn
{
unsigned CountElements(const char* buffer, char separator)
{
	if (!buffer) return 0;

	const char* endPos = buffer + strlen(buffer);
	const char* pos = buffer;
	unsigned ret = 0;

	while (pos < endPos)
	{
		if (*pos != separator)
			break;
		++pos;
	}

	while (pos < endPos)
	{
		const char* start = pos;

		while (start < endPos)
		{
			if (*start == separator)
				break;

			++start;
		}

		if (start == endPos)
		{
			++ret;
			break;
		}

		const char* end = start;

		while (end < endPos)
		{
			if (*end != separator)
				break;

			++end;
		}

		++ret;
		pos = end;
	}

	return ret;
}

float		to_float(const enn::String& source)
{
	return to_float(source.c_str());
}

float		to_float(const char* source)
{
	if (!source)
	{
		return 0;
	}

	return (float)strtod(source, 0);
}

int32		to_int(const enn::String& source)
{
	return to_int(source.c_str());
}

int32		to_int(const char* source)
{
	if (!source) return 0;

	return strtol(source, 0, 0);
}

uint32		to_uint(const enn::String& source)
{
	return to_uint(source.c_str());
}

uint32		to_uint(const char* source)
{
	if (!source) return 0;

	return strtoul(source, 0, 0);
}

vec2f		to_vec2(const enn::String& source)
{
	return to_vec2(source.c_str());
}

vec2f		to_vec2(const char* source)
{
	vec2f ret(vec2f::ZERO);

	unsigned elements = CountElements(source, ' ');
	if (elements < 2)
		return ret;

	char* ptr = (char*)source;
	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);

	return ret;
}

vec3f		to_vec3(const enn::String& source)
{
	return to_vec3(source.c_str());
}

vec3f		to_vec3(const char* source)
{
	vec3f ret;

	unsigned elements = CountElements(source, ' ');
	if (elements < 3)
		return ret;

	char* ptr = (char*)source;
	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);
	ret.z = (float)strtod(ptr, &ptr);

	return ret;
}

vec4f		to_vec4(const enn::String& source)
{
	return to_vec4(source.c_str());
}

vec4f		to_vec4(const char* source)
{
	vec4f ret(vec4f::ZERO);

	unsigned elements = CountElements(source, ' ');
	char* ptr = (char*)source;
	
	if (elements < 4)
		return ret;

	ret.x = (float)strtod(ptr, &ptr);
	ret.y = (float)strtod(ptr, &ptr);
	ret.z = (float)strtod(ptr, &ptr);
	ret.w = (float)strtod(ptr, &ptr);

	return ret;
}

String		to_string( int i )
{
	char buf[32] = {0};
	sprintf(buf, "%d", i );
	return buf;
}

void replace_ch(String& str, char replaceThis, char replaceWith)
{
	for (unsigned i = 0; i < str.size(); ++i)
	{
		if (str[i] == replaceThis)
		{
			str[i] = replaceWith;
		}
	}
}

void replace_str(String& str, const String& replaceThis, const String& replaceWith)
{
	unsigned nextPos = 0;

	int length = str.size();

	while ((int)nextPos < length)
	{
		unsigned pos = str.find(replaceThis, nextPos);

		if (pos == String::npos) break;

		str.replace(pos, replaceThis.size(), replaceWith);

		nextPos = pos + replaceWith.size();
	}
}

}