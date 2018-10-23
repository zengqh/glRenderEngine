/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/02
* File: enn_gl_conv.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_render_def.h"
#include "enn_gl_interface.h"

namespace enn
{

inline int _convGLAddr( int dm3dAddr )
{
	switch ( dm3dAddr )
	{
	case TEXADDR_WRAP:
		return GL_REPEAT;

	case TEXADDR_CLAMP:
		return GL_CLAMP_TO_EDGE;

	default:
		return GL_REPEAT;
	}
}

inline int _convGLFilter( int dm3dFilter )
{
	switch ( dm3dFilter )
	{
	case TEXF_POINT:
		return GL_NEAREST;

	case TEXF_LINEAR:
		return GL_LINEAR;

	case TEXF_MIP_LINEAR:
		return GL_LINEAR_MIPMAP_LINEAR;

	default:
		return GL_LINEAR;
	}
}

inline int _convGLInternalFormat(uint32 pixel_format)
{
#if defined(GL_ES_VERSION_2_0)
	switch (pixel_format)
	{
	case U1:
		return GL_ALPHA;
	case U3_RGB:
		return GL_RGB;
	case U4_RGBA:
		return GL_RGBA;
	case D16:
		return GL_DEPTH_COMPONENT16;
	default:
		ENN_ASSERT(0);
		return GL_RGBA;
	}
#else
	switch (pixel_format)
	{
	case U1:
		return GL_R8;
	case U3_RGB:
		return GL_RGB8;
	case U3_BGR:
		return GL_RGB8;
	case U4_RGBA:
		return GL_RGBA8;
	case U4_BGRA:
		return GL_RGBA8;
	case U16_1:
		return GL_R16;
	case U16_2:
		return GL_RG16;
	case U16_4:
		return GL_RGBA16;
	case D16:
		return GL_DEPTH_COMPONENT16;
	case D24:
		return GL_DEPTH_COMPONENT24;
	case D32:
		return GL_DEPTH_COMPONENT32;
	case DXT1:
		return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
	case DXT3:
		return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
	case DXT5:
		return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	case F16_1:
		return GL_R16F;
	case F16_2:
		return GL_RG16F;
	case F16_4:
		return GL_RGBA16F;
	case F32_1:
		return GL_R32F;
	case F32_2:
		return GL_RG32F;
	case F32_4:
		return GL_RGBA32F;
	default:
		ENN_ASSERT(0);
		return GL_RGBA8;
	}
#endif
}

inline uint32 _convGLExternalFormat(uint32 pixel_format)
{
#if defined(GL_ES_VERSION_2_0)
	switch (pixel_format)
	{
	case U1:
		return GL_ALPHA;
	case U3_RGB:
		return GL_RGB;
	case U4_RGBA:
		return GL_RGBA;
	case D16:
		return GL_DEPTH_COMPONENT16;
	default:
		ENN_ASSERT(0);
		return GL_RGBA;
	}
#else
	switch (pixel_format)
	{
	case U1:
	case U16_1:
	case F16_1:
	case F32_1:
		return GL_RED;
	case U16_2:
	case F16_2:
	case F32_2:
		return GL_RG;
	case D16:
	case D24:
	case D32:
		return GL_DEPTH_COMPONENT;
	case U3_RGB:
	case DXT1:
		return GL_RGB;
	case U3_BGR:
		return GL_BGR;
	case DXT3:
	case DXT5:
	case U16_4:
	case F16_4:
	case F32_4:
	case U4_RGBA:
		return GL_RGBA;
	case U4_BGRA:
		return GL_BGRA;
	default:
		ENN_ASSERT(0);
		return GL_RGBA;
	}
#endif
}

inline uint32 _convGLDataType(uint32 pixel_format)
{
	switch (pixel_format)
	{
	case D16:
	case U16_1:
	case U16_4:
	case U16_2:
		return GL_UNSIGNED_SHORT;
	case D24:
	case D32:
		return GL_UNSIGNED_INT;
	case DXT1:
	case DXT3:
	case DXT5:
	case U1:
	case U3_RGB:
	case U3_BGR:
	case U4_RGBA:
	case U4_BGRA:
		return GL_UNSIGNED_BYTE;
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	case F16_1:
	case F16_4:
	case F16_2:
		return GL_HALF_FLOAT;
	case F32_1:
	case F32_4:
		return GL_FLOAT;
#endif
	default:
		ENN_ASSERT(0);
		return GL_UNSIGNED_BYTE;
	}
}
}

