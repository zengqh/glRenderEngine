/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_texture_obj.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_texture_obj.h"
#include <enn_image.h>

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
#include <direct.h>
#endif

namespace enn
{
TextureObj::TextureObj()
: handle_(0)
{
	memset(&tex_desc_, 0x0, sizeof(tex_desc_));
}

TextureObj::~TextureObj()
{
	destroyTexture();
}

TextureObj* TextureObj::createInstance()
{
	return ENN_NEW TextureObj;
}

void TextureObj::release()
{
	ENN_DELETE this;
}

void   TextureObj::createTexture()
{
	destroyTexture();
	glGenTextures( 1, &handle_ );

	ENN_ASSERT(handle_);

	checkGLError();
}

void   TextureObj::destroyTexture()
{
	if ( handle_ )
	{
		glDeleteTextures( 1, &handle_ );
		checkGLError();
		handle_ = 0;
	}
}

void TextureObj::buildTexture2D(const TextureObj::TexObjDesc& tex_desc, void* data)
{
	tex_desc_ = tex_desc;

	if (tex_desc_.is_mipmap_)
	{
		do_buildMipMapTexture(data);
	}
	else
	{
		do_buildTexture(data);
	}
}

void TextureObj::buildTextureCube(const TextureObj::TexObjDesc& tex_desc, void* data[6])
{
	tex_desc_ = tex_desc;
	do_buildCubeTexture(data);
}

void TextureObj::updateTexture(void* data)
{
	glBindTexture( GL_TEXTURE_2D, handle_ );

	glTexSubImage2D( GL_TEXTURE_2D, 0, tex_desc_.internal_format_, 
		tex_desc_.width_, tex_desc_.height_, 0, tex_desc_.external_format_, 
		tex_desc_.type_, data );
	checkGLError();
}

void TextureObj::updateCubeTexture(int face, void* data)
{
	glBindTexture( GL_TEXTURE_CUBE_MAP, handle_ );

	GLenum target = face + GL_TEXTURE_CUBE_MAP_POSITIVE_X;

	glTexSubImage2D( target, 0, tex_desc_.internal_format_, 
		tex_desc_.width_, tex_desc_.height_, 0, 
		tex_desc_.external_format_, tex_desc_.type_, data );
	
}

bool TextureObj::isCubeTexture() const
{
	return (tex_desc_.texture_type_ == TEXTURE_CUBE);
}

/** 2d texture */
void   TextureObj::do_buildTexture(void* data)
{
	if (!handle_)
	{
		return;
	}

	glBindTexture( GL_TEXTURE_2D, handle_ );

	glTexImage2D( GL_TEXTURE_2D, 0, tex_desc_.internal_format_, 
		tex_desc_.width_, tex_desc_.height_, 0, tex_desc_.external_format_, 
		tex_desc_.type_, data );

	checkGLError();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_desc_.min_filter_ );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_desc_.mag_filter_ );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_desc_.wrap_s_ );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_desc_.wrap_t_ );

	glBindTexture( GL_TEXTURE_2D, 0 );
	checkGLError();

	tex_desc_.texture_type_ = TEXTURE_2D;
}

void   TextureObj::do_buildMipMapTexture(void* data)
{
	if (!handle_)
	{
		return;
	}

	glBindTexture( GL_TEXTURE_2D, handle_ );

	gl.gluBuild2DMipmaps( GL_TEXTURE_2D, tex_desc_.internal_format_, 
		tex_desc_.width_, tex_desc_.height_, 0, tex_desc_.external_format_,
		tex_desc_.type_, data );
	checkGLError();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, tex_desc_.min_filter_ );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, tex_desc_.mag_filter_ );

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tex_desc_.wrap_s_ );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tex_desc_.wrap_t_ );

	glBindTexture( GL_TEXTURE_2D, 0 );
	checkGLError();
}

/** cube texture */
void TextureObj::do_buildCubeTexture( void* data[6] )
{
	if (!handle_)
	{
		return;
	}

	glBindTexture( GL_TEXTURE_CUBE_MAP, handle_ );

	for ( int i=0; i<6; ++i )
	{
		GLenum target = i+GL_TEXTURE_CUBE_MAP_POSITIVE_X;

		glTexImage2D( target, 0, tex_desc_.internal_format_, 
			tex_desc_.width_, tex_desc_.height_, 0, 
			tex_desc_.external_format_, tex_desc_.type_, data[i] );
	}

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, tex_desc_.min_filter_ );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, tex_desc_.mag_filter_ );

	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
#endif

	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	checkGLError();

	tex_desc_.texture_type_ = TEXTURE_CUBE;
}

void TextureObj::saveFile( const char* file )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	if ( !handle_ )
	{
		return;
	}

	glBindTexture( GL_TEXTURE_2D, handle_ );
	checkGLError();

	int width = 0, height = 0;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width );
	checkGLError();
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height );
	checkGLError();

	char* data = new char[width * height * 4];
	glGetTexImage( GL_TEXTURE_2D, 0, GL_BGRA, GL_UNSIGNED_BYTE, data );
	checkGLError();
	saveBmp( file, width, height, data, false );
	delete []data;

	glBindTexture( GL_TEXTURE_2D, 0 );
	checkGLError();
#endif
}

void TextureObj::saveCubeFile( int face, const char* file )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	if ( !handle_ )
	{
		ENN_ASSERT(0);
		return;
	}

	glBindTexture( GL_TEXTURE_CUBE_MAP, handle_ );
	checkGLError();

	GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + face;

	int width = 0, height = 0;
	glGetTexLevelParameteriv( target, 0, GL_TEXTURE_WIDTH, &width );
	checkGLError();
	glGetTexLevelParameteriv( target, 0, GL_TEXTURE_HEIGHT, &height );
	checkGLError();

	char* data = new char[width * height * 4];
	glGetTexImage( target, 0, GL_BGRA, GL_UNSIGNED_BYTE, data );
	checkGLError();
	saveBmp( file, width, height, data, true );
	delete []data;

	glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	checkGLError();
#endif
}

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
void create_path( const char* file )
{
	char buf[1024] = {};
	char* dest = buf;
	char ch;

	while ( ch = *file )
	{
		if ( ch == '/' || ch == '\\' )
		{
			mkdir( buf );
		}

		*dest = ch;

		++file;
		++dest;
	}

}

#endif

void TextureObj::saveBmp( const char* file, int width, int height, const void* data, bool flip )
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	create_path( file );

	BITMAPFILEHEADER bfh = {0};
	BITMAPINFOHEADER bih = {0};

	int crbytes    = 4;
	int data_bytes = width * height * crbytes;

	bfh.bfType    = 0x4D42;
	bfh.bfOffBits = sizeof(bfh) + sizeof(bih);
	bfh.bfSize    = bfh.bfOffBits + data_bytes;

	bih.biSize          = sizeof(bih);
	bih.biWidth         = width;
	bih.biHeight        = flip ? -height : height;
	bih.biPlanes        = 1;
	bih.biBitCount      = crbytes * 8;
	bih.biCompression   = BI_RGB;

	FILE* fp = 0;
	fopen_s( &fp, file, "wb" );
	if ( !fp )
		return;

	fwrite( &bfh, 1, sizeof(bfh), fp );
	fwrite( &bih, 1, sizeof(bih), fp );
	fwrite( data, 1, data_bytes, fp );
	fclose( fp );
#endif
}

}