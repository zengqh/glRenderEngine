/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/25
* File: enn_gl_interface.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl.h"
#include "enn_render_def.h"
#include "enn_math.h"

namespace enn
{
class GLInterface
{
public:
	//////////////////////////////////////////////////////////////////////////
	// buffer object
	void        glGenBuffers( GLsizei n, GLuint * buffers );

	void        glBindBuffer( GLenum target, GLuint buffer );

	void        glBufferData( GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage );

	void        glDeleteBuffers( GLsizei n, const GLuint * buffers );

	GLvoid*     glMapBuffer( GLenum target, GLenum access );

	GLboolean   glUnmapBuffer( GLenum target );

	//////////////////////////////////////////////////////////////////////////
	// frame/render buffer
	void        glGenFramebuffers( GLsizei n, GLuint * framebuffers );

	void        glDeleteFramebuffers( GLsizei n, GLuint * framebuffers ); 

	void        glBindFramebuffer( GLenum target, GLuint framebuffer );

	void        glFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level );

	void        glGenRenderbuffers( GLsizei n, GLuint * renderbuffers );

	void        glDeleteRenderbuffers( GLsizei n, GLuint * renderbuffers );

	void        glBindRenderbuffer( GLenum target, GLuint renderbuffer );

	void        glRenderbufferStorage( GLenum target, GLenum internalformat, GLsizei width, GLsizei height );

	void        glFramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer );

	void		glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels);

	//////////////////////////////////////////////////////////////////////////
	// shader
	GLuint      glCreateProgram();

	void        glAttachShader( GLuint program, GLuint shader );

	void        glLinkProgram( GLuint program );

	void        glUseProgram( GLuint program );

	void        glDeleteProgram( GLuint program );

	GLuint      glCreateShader( GLenum type );

	void        glShaderSource( GLuint shader, GLsizei count, const GLchar* * string, const GLint * length );

	void        glCompileShader( GLuint shader );

	void        glGetShaderiv( GLuint shader, GLenum pname, GLint * params );

	void        glGetProgramiv( GLuint program, GLenum pname, GLint * params );

	void        glGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog );

	void        glGetProgramInfoLog( GLuint program, GLsizei maxLength, GLsizei * length, GLchar * infoLog );

	void        glDeleteShader( GLuint shader );

	GLint       glGetUniformLocation( GLuint program, const GLchar * name );

	void        glUniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 );

	void        glUniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 );

	void        glUniform2f( GLint location, GLfloat v0, GLfloat v1 );

	void        glUniform1f( GLint location, GLfloat v0 );

	void        glUniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 );

	void        glUniform3i( GLint location, GLint v0, GLint v1, GLint v2 );

	void        glUniform2i( GLint location, GLint v0, GLint v1 );

	void        glUniform1i( GLint location, GLint v0 );

	void        glUniform1fv( GLint location, GLsizei count, const GLfloat* value );

	void        glUniform1iv( GLint location, GLsizei count, const GLint* value );

	void        glUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value ); 

	void        glUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat * value );

	GLint       glGetAttribLocation( GLuint program, const GLchar* name ); 

	void        glEnableVertexAttribArray( GLuint index ); 

	void        glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer ); 

	void        glDisableVertexAttribArray(	GLuint index );
	
	//////////////////////////////////////////////////////////////////////////
	// misc
	void        glActiveTexture( GLenum texture );

	void        gluBuild2DMipmaps( GLenum target, GLint internalformat, GLsizei width, GLsizei height, 
		GLint border, GLenum format, GLenum type, const GLvoid* pixels );

	void        glGenerateMipmap( GLenum target );

	void        glClearDepth( GLclampf depth );

	void		glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);

	void		glClear(GLbitfield mask);

	void		glEnable(GLenum cap);

	void		glDisable(GLenum cap);
};


extern GLInterface gl;
}

#if ENN_DEBUG

static inline void checkGLError()
{
	int ret = glGetError();

	if ( ret != GL_NO_ERROR )
	{
		ENN_ASSERT(0);
	}
}

#else

#define checkGLError()

#endif