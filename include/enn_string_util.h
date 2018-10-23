/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/04/16
* File: enn_string_util.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_vector2.h"
#include "enn_vector3.h"
#include "enn_vector4.h"

namespace enn
{
float		to_float(const enn::String& source);
float		to_float(const char* source);
int32		to_int(const enn::String& source);
int32		to_int(const char* source);
uint32		to_uint(const enn::String& source);
uint32		to_uint(const char* source);
vec2f		to_vec2(const enn::String& source);
vec2f		to_vec2(const char* source);
vec3f		to_vec3(const enn::String& source);
vec3f		to_vec3(const char* source);
vec4f		to_vec4(const enn::String& source);
vec4f		to_vec4(const char* source);
String		to_string( int i );
void		replace_ch(String& str, char replaceThis, char replaceWith);
void		replace_str(String& str, const String& replaceThis, const String& replaceWith);
}