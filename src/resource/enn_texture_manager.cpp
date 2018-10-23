/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/01
* File: enn_texture_manager.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_texture_manager.h"
#include "enn_root.h"
#include "enn_image.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
TextureAutoResCreator::TextureAutoResCreator()
{
	m_basePath = "texture";
}

TextureAutoResCreator::~TextureAutoResCreator()
{

}

bool TextureAutoResCreator::buildResource( Resource* res )
{
	Image* temp_image_2d = static_cast<Image*>(res->getUserData());

	ENN_ASSERT(temp_image_2d);

	if (!temp_image_2d)
	{
		return false;
	}

	Texture* tex = static_cast<Texture*>(res);

	Texture::TexParams2D& param2d = tex->getTexParam2D();

	param2d.width = temp_image_2d->getWidth();
	param2d.height = temp_image_2d->getHeight();
	param2d.pixel_format = temp_image_2d->getPixelFormat();
	param2d.data = temp_image_2d->getData();

	bool ret = tex->load_as_texture_2d();

	ENN_ASSERT(ret);
	ENN_SAFE_DELETE(temp_image_2d);

	return true;
}

bool TextureAutoResCreator::prepareResourceImpl( Resource* res, MemoryBuffer* buff )
{
	Image* image = ENN_NEW Image;
	bool ret = image->load(*buff);
	ENN_ASSERT(ret);

	Root::getSingletonPtr()->get_file_system()->freeFileData(buff);

	res->setUserData(image);

	return true;
}

//////////////////////////////////////////////////////////////////////////
TextureManager::TextureManager() : m_default(0), m_random(0)
{
	
}

TextureManager::~TextureManager()
{
	ENN_SAFE_RELEASE(m_default);
}

void TextureManager::init()
{
	ResourceManager::init();

	setAutoCreator(ENN_NEW TextureAutoResCreator());

	getDefaultTexture();
	getRandomTexture();
}

Resource* TextureManager::createImpl()
{
	Resource* res = ENN_NEW Texture;
	res->setManager(this);
	return res;
}

Texture* TextureManager::getDefaultTexture()
{
	if ( m_default )
		return m_default;

	m_default = static_cast<Texture*>(createResource( "$DEFAULT" ));
	if ( m_default )
		m_default->setResCreator( ENN_NEW TextureColorCreatorImpl(Color::WHITE) );

	m_default->load(false);

	return m_default;
}

Texture* TextureManager::getRandomTexture()
{
	if (m_random)
	{
		return m_random;
	}

	m_random = static_cast<Texture*>(createResource( "random.png" ));

	m_random->load(false);

	return m_random;
}

}