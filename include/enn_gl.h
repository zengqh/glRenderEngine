/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_gl.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_platform_headers.h>

/** on windows platform, we request the opengl version >= 2.1 */
/** on mobile, we request the opengl es verison >= 2.0 */
/** more information, please see: http://www.opengl.org/sdk/docs/man2/ */
/** more information, please see: http://www.khronos.org/opengles/sdk/docs/man/ */

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#include "GLee.h"
#elif ENN_PLATFORM == ENN_PLATFORM_ANDROID
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#elif ENN_PLATFORM == ENN_PLATFORM_IOS
#include <OpengLES/ES2/gl.h>
#include <OpenglES/ES2/glext.h>
#endif

#if ENN_PLATFORM != ENN_PLATFORM_WIN32
#ifndef GL_CLAMP
#define GL_CLAMP GL_CLAMP_TO_EDGE
#endif

#ifndef GL_WRITE_ONLY
#define GL_WRITE_ONLY GL_WRITE_ONLY_OES
#endif

#ifndef GL_READ_ONLY
#define GL_READ_ONLY   0x88B8
#endif

#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER GL_FRAMEBUFFER_OES
#endif

#ifndef GL_DEPTH_ATTACHMENT
#define GL_DEPTH_ATTACHMENT GL_DEPTH_ATTACHMENT_OES
#endif

#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 GL_COLOR_ATTACHMENT0_OES
#endif

#ifndef GL_RENDERBUFFER
#define GL_RENDERBUFFER GL_RENDERBUFFER_OES
#endif

#ifndef GL_UNSIGNED_INT
#define GL_UNSIGNED_INT 0x1405
#endif
#endif



