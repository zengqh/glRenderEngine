/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_texture_obj.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"

namespace enn
{
class Image;
class TextureObj : public Noncopyable, public AllocatedObject
{
private:
	TextureObj();
	~TextureObj();
public:
	struct TexObjDesc
	{
		int width_;
		int height_;
		GLint internal_format_;
		GLenum external_format_;
		int min_filter_;
		int mag_filter_;
		int wrap_s_;
		int wrap_t_;
		GLenum type_;
		uint32 texture_type_;
		uint32 pixel_format_;
		bool is_mipmap_;
	};

	static TextureObj* createInstance();

	void release();

	void createTexture();
	void destroyTexture();

	void buildTexture2D(const TexObjDesc& tex_desc, void* data);
	void buildTextureCube(const TexObjDesc& tex_desc, void* data[6]);

	void updateTexture(void* data);
	void updateCubeTexture(int face, void* data);

	bool isCubeTexture() const;

protected:
	/** 2d texture */
	void do_buildTexture(void* data);

	/** 2d mipmap texture. */
	void do_buildMipMapTexture(void* data);

	/** cube texture */
	void do_buildCubeTexture(void* data[6]);

public:
	GLuint getHandle() const { return handle_; }
	const TexObjDesc& getTexDesc() const { return tex_desc_; }

	int getWidth() const
	{
		return tex_desc_.width_;
	}
	
	int getHeight() const
	{
		return tex_desc_.height_;
	}

public:
	void saveFile( const char* file );

	void saveCubeFile( int face, const char* file );

protected:
	void saveBmp( const char* file, int width, int height, const void* data, bool flip );

protected:
	TexObjDesc tex_desc_;
	GLuint handle_;
};


}

