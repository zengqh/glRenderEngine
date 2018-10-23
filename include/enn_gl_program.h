/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_program.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{
class Program : public Noncopyable, public AllocatedObject
{
private:
	Program();
	virtual ~Program();

public:
	static Program* createInstance();
	void release();

	GLuint getHandle() const;

public:
	bool load( const void* vsData, size_t vsSize, const void* psData, size_t psSize );
	void destroy();

	void setFloatVector( const char* name, const float* v, size_t sz );

	void setFloatVectorArray(const char* name, const float* v, size_t sz, size_t count);

	void setFloatArray( const char* name, const float* v, size_t sz );

	void setFloat( const char* name, float v );

	void setIntVector( const char* name, const int* v, size_t sz );

	void setIntArray( const char* name, const int* v, size_t sz );

	void setInt( const char* name, int v );

	void setMatrix3( const char* name, const float* v );

	void setMatrix4( const char* name, const float* v );

	void setMatrix4Array( const char* name, const float* v, size_t sz );

public:
	GLint getAttribSlot( const char* name );

protected:
	GLuint _loadShader( const void* data, size_t size, GLenum type );

	GLint  _getVarHandle( const char* name );

protected:
	GLuint m_vs;
	GLuint m_ps;
	GLuint m_eff;
	bool   m_isValid;
};
}