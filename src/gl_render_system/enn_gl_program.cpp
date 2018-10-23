/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_program.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_program.h"

namespace enn
{
Program::Program() : m_vs(0), m_ps(0), m_eff(0), m_isValid(false)
{
}

Program::~Program()
{
	destroy();
}

Program* Program::createInstance()
{
	return ENN_NEW Program;
}

void Program::release()
{
	ENN_DELETE this;
}

GLuint Program::getHandle() const
{
	return m_eff;
}

bool Program::load( const void* vsData, size_t vsSize, const void* psData, size_t psSize )
{
	destroy();

	if ( vsData )
		m_vs = _loadShader( vsData, vsSize, GL_VERTEX_SHADER );

	if ( psData )
		m_ps = _loadShader( psData, psSize, GL_FRAGMENT_SHADER );

	if ( !m_vs && !m_ps )
		return false;

	m_eff = gl.glCreateProgram();
	if ( !m_eff )
		return false;

	if ( m_vs )
		gl.glAttachShader( m_eff, m_vs );

	if ( m_ps )
		gl.glAttachShader( m_eff, m_ps );

	gl.glLinkProgram( m_eff );

	GLint result = 0;
	gl.glGetProgramiv( m_eff, GL_LINK_STATUS, &result );

	if ( result == GL_FALSE )
	{
		GLchar  msg[1024] = {0};
		GLsizei msgLen = sizeof(msg)/sizeof(msg[0]);

		gl.glGetProgramInfoLog( m_eff, msgLen, &msgLen, msg ); 
		printf( "link error: %s\n", msg );

		m_isValid = true;
		destroy();
		m_isValid = false;

		ENN_ASSERT(0);
		return false;
	}

	checkGLError();

	m_isValid = true;
	return true;
}

void Program::destroy()
{
	if ( !m_isValid )
		return;

	if ( m_eff )
	{
		gl.glDeleteProgram( m_eff );
		m_eff = 0;
	}

	if ( m_ps )
	{
		gl.glDeleteShader( m_ps );
		m_ps = 0;
	}

	if ( m_vs )
	{
		gl.glDeleteShader( m_vs );
		m_vs = 0;
	}

	m_isValid = false;
}

GLuint Program::_loadShader( const void* data, size_t size, GLenum type )
{
	GLuint shader = gl.glCreateShader( type );
	if ( !shader )
		return 0;

	GLint isize = size;
	gl.glShaderSource( shader, 1, (const GLchar **)&data, &isize ); 
	gl.glCompileShader( shader );

	GLint result = 0;
	gl.glGetShaderiv( shader, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE )
	{
		GLchar  msg[1024] = {0};
		GLsizei msgLen = sizeof(msg)/sizeof(msg[0]);

		gl.glGetShaderInfoLog( shader, msgLen, &msgLen, msg ); 
		printf( "compile error: %s\n", msg );

		gl.glDeleteShader( shader );
		ENN_ASSERT(0);
		return 0;
	}

	return shader;
}

void Program::setFloatVector( const char* name, const float* v, size_t sz )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );

	switch ( sz )
	{
	case 4:
		return gl.glUniform4f( handle, v[0], v[1], v[2], v[3] );

	case 3:
		return gl.glUniform3f( handle, v[0], v[1], v[2] );

	case 2:
		return gl.glUniform2f( handle, v[0], v[1] );

	case 1:
		return gl.glUniform1f( handle, *v );
	}
}

void Program::setFloatVectorArray(const char* name, const float* v, size_t sz, size_t count)
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );

	switch ( sz )
	{
	case 4:
		return glUniform4fv( handle, count, v );

	case 3:
		return glUniform3fv( handle, count, v );

	case 2:
		return glUniform2fv( handle, count, v );

	case 1:
		return glUniform1fv( handle, count, v );
	}
}

void Program::setIntVector( const char* name, const int* v, size_t sz )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );

	switch ( sz )
	{
	case 4:
		return gl.glUniform4i( handle, v[0], v[1], v[2], v[3] );

	case 3:
		return gl.glUniform3i( handle, v[0], v[1], v[2] );

	case 2:
		return gl.glUniform2i( handle, v[0], v[1] );

	case 1:
		return gl.glUniform1i( handle, *v );
	}
}

void Program::setFloatArray( const char* name, const float* v, size_t sz )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniform1fv( handle, sz, v );
}

void Program::setIntArray( const char* name, const int* v, size_t sz )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniform1iv( handle, sz, v );
}

void Program::setFloat( const char* name, float v )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniform1f( handle, v );

	checkGLError();
}

void Program::setInt( const char* name, int v )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniform1i( handle, v );
}

void Program::setMatrix3( const char* name, const float* v )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniformMatrix3fv( handle, 1, false, v );
}

void Program::setMatrix4( const char* name, const float* v )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniformMatrix4fv( handle, 1, false, v );

	checkGLError();
}

void Program::setMatrix4Array( const char* name, const float* v, size_t sz )
{
	if ( !m_isValid )
		return;

	GLint handle = _getVarHandle( name );
	gl.glUniformMatrix4fv( handle, sz, false, v );
}

GLint Program::_getVarHandle( const char* name )
{
	GLint h = gl.glGetUniformLocation( m_eff, name );
	checkGLError();
	return h;
}

GLint Program::getAttribSlot( const char* name )
{
	GLint h = gl.glGetAttribLocation( m_eff, name );
	checkGLError();
	return h;
}
}