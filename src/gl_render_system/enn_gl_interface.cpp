/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_gl_interface.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_interface.h"

namespace enn
{
GLInterface gl;
//////////////////////////////////////////////////////////////////////////
// buffer object
void        GLInterface::glGenBuffers( GLsizei n, GLuint * buffers )
{
	::glGenBuffers(n, buffers);
}

void        GLInterface::glBindBuffer( GLenum target, GLuint buffer )
{
	::glBindBuffer( target, buffer );
}

void        GLInterface::glBufferData( GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage )
{
	::glBufferData( target, size, data, usage );
}

void        GLInterface::glDeleteBuffers( GLsizei n, const GLuint * buffers )
{
	::glDeleteBuffers( n, buffers );
}

/** not support on gles 2/3 */
GLvoid*     GLInterface::glMapBuffer( GLenum target, GLenum access )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	return ::glMapBuffer( target, access );
#else
	return 0;
#endif
}

GLboolean   GLInterface::glUnmapBuffer( GLenum target )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	return ::glUnmapBuffer( target );
#else
	return 0;
#endif
}

//////////////////////////////////////////////////////////////////////////
// frame/render buffer
void        GLInterface::glGenFramebuffers( GLsizei n, GLuint * framebuffers )
{
	::glGenFramebuffers( n, framebuffers );
}

void        GLInterface::glDeleteFramebuffers( GLsizei n, GLuint * framebuffers )
{
	::glDeleteFramebuffers( n, framebuffers );
}

void        GLInterface::glBindFramebuffer( GLenum target, GLuint framebuffer )
{
	::glBindFramebuffer( target, framebuffer );
}

void        GLInterface::glFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
	::glFramebufferTexture2D( target, attachment, textarget, texture, level );
}

void        GLInterface::glGenRenderbuffers( GLsizei n, GLuint * renderbuffers )
{
	::glGenRenderbuffers( n, renderbuffers );
}

void        GLInterface::glDeleteRenderbuffers( GLsizei n, GLuint * renderbuffers )
{
	::glDeleteRenderbuffers( n, renderbuffers );
}

void        GLInterface::glBindRenderbuffer( GLenum target, GLuint renderbuffer )
{
	::glBindRenderbuffer( target, renderbuffer );
}

void        GLInterface::glRenderbufferStorage( GLenum target, GLenum internalformat, GLsizei width, GLsizei height )
{
	::glRenderbufferStorage( target, internalformat, width, height );
}

void        GLInterface::glFramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
{
	::glFramebufferRenderbuffer( target, attachment, renderbuffertarget, renderbuffer );
}

void		GLInterface::glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels)
{
	::glReadPixels(x, y, width, height, format, type, pixels);
}

//////////////////////////////////////////////////////////////////////////
// shader
GLuint      GLInterface::glCreateProgram()
{
	return ::glCreateProgram();
}

void        GLInterface::glAttachShader( GLuint program, GLuint shader )
{
	::glAttachShader( program, shader );
}

void        GLInterface::glLinkProgram( GLuint program )
{
	::glLinkProgram( program );
}

void        GLInterface::glUseProgram( GLuint program )
{
	::glUseProgram( program );
}

void        GLInterface::glDeleteProgram( GLuint program )
{
	::glDeleteProgram( program );
}

GLuint      GLInterface::glCreateShader( GLenum type )
{
	return ::glCreateShader( type );
}

void        GLInterface::glShaderSource( GLuint shader, GLsizei count, const GLchar* * string, const GLint * length )
{
	::glShaderSource( shader, count, string, length );
}

void        GLInterface::glCompileShader( GLuint shader )
{
	::glCompileShader( shader );
}

void        GLInterface::glGetShaderiv( GLuint shader, GLenum pname, GLint * params )
{
	::glGetShaderiv( shader, pname, params );
}

void        GLInterface::glGetProgramiv( GLuint program, GLenum pname, GLint * params )
{
	::glGetProgramiv( program, pname, params );
}

void        GLInterface::glGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog )
{
	::glGetShaderInfoLog( shader, bufSize, length, infoLog );
}

void        GLInterface::glGetProgramInfoLog( GLuint program, GLsizei maxLength, GLsizei * length, GLchar * infoLog )
{
	::glGetProgramInfoLog( program, maxLength, length, infoLog );
}

void        GLInterface::glDeleteShader( GLuint shader )
{
	::glDeleteShader( shader );
}

GLint       GLInterface::glGetUniformLocation( GLuint program, const GLchar * name )
{
	return ::glGetUniformLocation( program, name );
}

void        GLInterface::glUniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
{
	::glUniform4f( location, v0, v1, v2, v3 );
}

void        GLInterface::glUniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
{
	::glUniform3f( location, v0, v1, v2 );
}

void        GLInterface::glUniform2f( GLint location, GLfloat v0, GLfloat v1 )
{
	::glUniform2f( location, v0, v1 );
}

void        GLInterface::glUniform1f( GLint location, GLfloat v0 )
{
	::glUniform1f( location, v0 );
}

void        GLInterface::glUniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
{
	::glUniform4i( location, v0, v1, v2, v3 );
}

void        GLInterface::glUniform3i( GLint location, GLint v0, GLint v1, GLint v2 )
{
	::glUniform3i( location, v0, v1, v2 );
}

void        GLInterface::glUniform2i( GLint location, GLint v0, GLint v1 )
{
	::glUniform2i( location, v0, v1 );
}

void        GLInterface::glUniform1i( GLint location, GLint v0 )
{
	::glUniform1i( location, v0 );
}

void        GLInterface::glUniform1fv( GLint location, GLsizei count, const GLfloat* value )
{
	::glUniform1fv( location, count, value );
}

void        GLInterface::glUniform1iv( GLint location, GLsizei count, const GLint* value )
{
	::glUniform1iv( location, count, value );
}

void        GLInterface::glUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value )
{
	::glUniformMatrix3fv( location, count, transpose, value );
}

void        GLInterface::glUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value )
{
	::glUniformMatrix4fv( location, count, transpose, value );
}

GLint       GLInterface::glGetAttribLocation( GLuint program, const GLchar* name )
{
	return ::glGetAttribLocation( program, name );
}

void        GLInterface::glEnableVertexAttribArray( GLuint index )
{
	::glEnableVertexAttribArray( index );
}

void        GLInterface::glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer )
{
	::glVertexAttribPointer( index, size, type, normalized, stride, pointer );
}

void        GLInterface::glDisableVertexAttribArray(	GLuint index )
{
	::glDisableVertexAttribArray( index );
}

// misc
void        GLInterface::glActiveTexture( GLenum texture )
{
	::glActiveTexture( texture );
}

void        GLInterface::gluBuild2DMipmaps( GLenum target, GLint internalformat, GLsizei width, GLsizei height, 
	GLint border, GLenum format, GLenum type, const GLvoid* pixels )
{
#if defined(GL_ES_VERSION_2_0)
	::glTexImage2D( target, 0, internalformat, width, height, border, format, type, pixels );
	return ::glGenerateMipmap( target );

#elif (ENN_PLATFORM == ENN_PLATFORM_WIN32)
	if (_GLEE_VERSION_3_0)
	{
		::glTexImage2D( target, 0, internalformat, width, height, border, format, type, pixels );
		return ::glGenerateMipmap( target );
	}
	else
	{
		::glTexParameteri( target, GL_GENERATE_MIPMAP, GL_TRUE );
		::glTexImage2D( target, 0, internalformat, width, height, border, format, type, pixels );
	}
#else

	::glTexParameteri( target, GL_GENERATE_MIPMAP, GL_TRUE );
	::glTexImage2D( target, 0, internalformat, width, height, border, format, type, pixels );
#endif
}

void        GLInterface::glGenerateMipmap( GLenum target )
{
	::glGenerateMipmap( target );
}

void        GLInterface::glClearDepth( GLclampf depth )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	::glClearDepth( depth );
#else
	::glClearDepthf( depth );
#endif
}

void		GLInterface::glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
	::glClearColor(red, green, blue, alpha);
}

void		GLInterface::glClear(GLbitfield mask)
{
	::glClear(mask);
}

void		GLInterface::glEnable(GLenum cap)
{
	::glEnable(cap);
}

void		GLInterface::glDisable(GLenum cap)
{
	::glDisable(cap);
}
}