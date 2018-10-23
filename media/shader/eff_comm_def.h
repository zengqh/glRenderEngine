//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/09/07
// File: eff_comm_def.h
//

//////////////////////////////////////////////////////////////////////////
// Platform
#ifdef ENN_PLATFORM_WIN32

#define DEFP
#define HIGHP
#define MEDIUMP
#define LOWP

#else

#ifdef NOT_SUPPROT_HIGHP
#define DEFP    mediump
#define HIGHP   mediump
#else
#define DEFP    highp
#define HIGHP   highp
#endif
#define MEDIUMP mediump
#define LOWP    lowp

#endif


//////////////////////////////////////////////////////////////////////////
// Macro
#define VEC4    DEFP vec4
#define HVEC4   HIGHP vec4
#define MVEC4   MEDIUMP vec4
#define LVEC4   LOWP vec4

#define VEC3    DEFP vec3
#define HVEC3   HIGHP vec3
#define MVEC3   MEDIUMP vec3
#define LVEC3   LOWP vec3

#define VEC2    DEFP vec2
#define HVEC2   HIGHP vec2
#define MVEC2   MEDIUMP vec2
#define LVEC2   LOWP vec2

#define FLOAT   DEFP float
#define HFLOAT  HIGHP float
#define MFLOAT  MEDIUMP float
#define LFLOAT  LOWP float

#define VIEC2   DEFP ivec2
#define HIVEC2  HIGHP ivec2
#define MIVEC2  MEDIUMP ivec2
#define LIVEC2  LOWP ivec2

#define VIEC4   DEFP ivec4
#define HIVEC4  HIGHP ivec4
#define MIVEC4  MEDIUMP ivec4
#define LIVEC4  LOWP ivec4

#define INT     DEFP int
#define HINT    HIGHP int
#define MINT    MEDIUMP int
#define LINT    LOWP int

#define MAT4    DEFP mat4
#define HMAT4   HIGHP mat4
#define MMAT4   MEDIUMP mat4
#define LMAT4   LOWP mat4

#define MAT3    DEFP mat3
#define HMAT3   HIGHP mat3
#define MMAT3   MEDIUMP mat3
#define LMAT3   LOWP mat3

#define MAT2    DEFP mat2
#define HMAT2   HIGHP mat2
#define MMAT2   MEDIUMP mat2
#define LMAT2   LOWP mat2

#define SAMPLER2D   DEFP sampler2D
#define HSAMPLER2D  HIGHP sampler2D
#define MSAMPLER2D  MEDIUMP sampler2D
#define LSAMPLER2D  LOWP sampler2D

#define M_PI 3.141592654

//////////////////////////////////////////////////////////////////////////
// Functions
FLOAT square( FLOAT x )
{
    return x * x;
}

FLOAT toLumin( VEC3 clr )
{
    // Y = 0.2126 R + 0.7152 G + 0.0722 B
    return dot(vec3(0.2126, 0.7152, 0.0722), clr);
}

