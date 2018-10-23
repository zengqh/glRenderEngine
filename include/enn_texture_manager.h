/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/01
* File: enn_texture_manager.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_resource_manager.h"
#include "enn_texture.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
class Image;
class TextureAutoResCreator : public ResAutoCreatorBase
{
public:
	TextureAutoResCreator();
	virtual ~TextureAutoResCreator();

public:
	virtual bool buildResource( Resource* res );

protected:
	virtual bool prepareResourceImpl( Resource* res, MemoryBuffer* buff );
};

//////////////////////////////////////////////////////////////////////////
class TextureColorCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	TextureColorCreatorImpl( const Color& clr ) : m_color(clr) {}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context )
	{
		return true;
	}

	virtual bool buildResource( Resource* res )
	{
		/** gles2 minimal texture size */
		const int texSize = 64;		

		uchar* data = (uchar*)ENN_MALLOC(texSize * texSize * 4);
		
		for (int y = 0; y < texSize; ++y)
		{
			for (int x = 0; x < texSize; ++x)
			{
				uchar* curr = data + y * texSize * 4 + x * 4;

				curr[0] = static_cast<uchar>(m_color.r * 0xff);
				curr[1] = static_cast<uchar>(m_color.g * 0xff);
				curr[2] = static_cast<uchar>(m_color.b * 0xff);
				curr[3] = static_cast<uchar>(m_color.a * 0xff);
			}
		}

		Texture* tex = static_cast<Texture*>(res);

		Texture::TexParams2D param;
		param.width = texSize;
		param.height = texSize;
		param.pixel_format = U4_RGBA;

		Texture::SamplerState sampler_state;
		sampler_state.address_u_ = TEXADDR_WRAP;
		sampler_state.address_v_ = TEXADDR_WRAP;
		sampler_state.address_w_ = TEXADDR_WRAP;
		sampler_state.min_filter_ = TEXF_POINT;
		sampler_state.mag_filter_ = TEXF_POINT;
		sampler_state.is_mipmap = false;
		param.data = data;
		tex->setTexParam2D(param);
		tex->setSamplerState(sampler_state);
		
		tex->load_as_texture_2d();

		ENN_SAFE_FREE(data);
		
		return true;
	}

private:
	Color m_color;
};


//////////////////////////////////////////////////////////////////////////
class TextureManager : public ResourceManager
{
public:
	TextureManager();
	virtual ~TextureManager();

public:
	virtual void init();

	Texture* getDefaultTexture();
	Texture* getRandomTexture();

protected:
	virtual Resource* createImpl();

private:
	Texture* m_default;
	Texture* m_random;
};
}