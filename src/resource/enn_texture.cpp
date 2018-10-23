/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/01
* File: enn_texture.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_texture.h"
#include "enn_gl_conv.h"

namespace enn
{
Texture::Texture()
: texture_obj_(0)
{
	setLoadPriority(TEX_LOAD_PRIORITY);
}

Texture::~Texture()
{
	ENN_SAFE_RELEASE(texture_obj_);
}

bool Texture::load_as_texture_2d()
{
	tex_type_ = TEXTURE_2D;
	
	TextureObj::TexObjDesc tex_obj_desc;

	tex_obj_desc.width_ = tex_param_2d_.width;
	tex_obj_desc.height_ = tex_param_2d_.height;
	tex_obj_desc.texture_type_ = tex_type_;
	tex_obj_desc.pixel_format_ = tex_param_2d_.pixel_format;
	tex_obj_desc.internal_format_ = _convGLInternalFormat(tex_param_2d_.pixel_format);
	tex_obj_desc.external_format_ = _convGLExternalFormat(tex_param_2d_.pixel_format);

	tex_obj_desc.min_filter_ = _convGLFilter(sampler_state_.min_filter_);
	tex_obj_desc.mag_filter_ = _convGLFilter(sampler_state_.mag_filter_);

	tex_obj_desc.wrap_s_ = _convGLAddr(sampler_state_.address_u_);
	tex_obj_desc.wrap_t_ = _convGLAddr(sampler_state_.address_v_);
	tex_obj_desc.type_ = _convGLDataType(tex_param_2d_.pixel_format);
	tex_obj_desc.is_mipmap_ = sampler_state_.is_mipmap;

	ENN_ASSERT(!texture_obj_);

	texture_obj_ = TextureObj::createInstance();

	texture_obj_->createTexture();

	texture_obj_->buildTexture2D(tex_obj_desc, tex_param_2d_.data);

	return true;
}

bool Texture::load_as_texture_cube()
{
	tex_type_ = TEXTURE_CUBE;

	TextureObj::TexObjDesc tex_obj_desc;

	tex_obj_desc.width_ = tex_param_cube_.width;
	tex_obj_desc.height_ = tex_param_cube_.height;
	tex_obj_desc.texture_type_ = tex_type_;
	tex_obj_desc.pixel_format_ = tex_param_cube_.pixel_format;
	tex_obj_desc.internal_format_ = _convGLInternalFormat(tex_param_cube_.pixel_format);
	tex_obj_desc.external_format_ = _convGLExternalFormat(tex_param_cube_.pixel_format);


	tex_obj_desc.min_filter_ = _convGLFilter(sampler_state_.min_filter_);
	tex_obj_desc.mag_filter_ = _convGLFilter(sampler_state_.mag_filter_);

	tex_obj_desc.wrap_s_ = _convGLAddr(sampler_state_.address_u_);
	tex_obj_desc.wrap_t_ = _convGLAddr(sampler_state_.address_v_);
	tex_obj_desc.type_ = _convGLDataType(tex_param_cube_.pixel_format);
	tex_obj_desc.is_mipmap_ = sampler_state_.is_mipmap;

	ENN_ASSERT(!texture_obj_);

	texture_obj_ = TextureObj::createInstance();

	texture_obj_->createTexture();

	texture_obj_->buildTextureCube(tex_obj_desc, tex_param_cube_.data);

	return true;
}

bool Texture::create_as_texture_2d()
{
	tex_type_ = TEXTURE_2D;

	TextureObj::TexObjDesc tex_obj_desc;

	tex_obj_desc.width_ = tex_param_2d_.width;
	tex_obj_desc.height_ = tex_param_2d_.height;
	tex_obj_desc.texture_type_ = tex_type_;
	tex_obj_desc.pixel_format_ = tex_param_2d_.pixel_format;
	tex_obj_desc.internal_format_ = _convGLInternalFormat(tex_param_2d_.pixel_format);
	tex_obj_desc.external_format_ = _convGLExternalFormat(tex_param_2d_.pixel_format);

	tex_obj_desc.min_filter_ = _convGLFilter(sampler_state_.min_filter_);
	tex_obj_desc.mag_filter_ = _convGLFilter(sampler_state_.mag_filter_);

	tex_obj_desc.wrap_s_ = _convGLAddr(sampler_state_.address_u_);
	tex_obj_desc.wrap_t_ = _convGLAddr(sampler_state_.address_v_);
	tex_obj_desc.type_ = _convGLDataType(tex_param_2d_.pixel_format);
	tex_obj_desc.is_mipmap_ = sampler_state_.is_mipmap;

	ENN_ASSERT(!texture_obj_);

	texture_obj_ = TextureObj::createInstance();

	texture_obj_->createTexture();

	texture_obj_->buildTexture2D(tex_obj_desc, tex_param_2d_.data);

	return true;
}

bool Texture::create_as_texture_cube()
{
	tex_type_ = TEXTURE_CUBE;

	TextureObj::TexObjDesc tex_obj_desc;

	tex_obj_desc.width_ = tex_param_cube_.width;
	tex_obj_desc.height_ = tex_param_cube_.height;
	tex_obj_desc.texture_type_ = tex_type_;
	tex_obj_desc.pixel_format_ = tex_param_cube_.pixel_format;
	tex_obj_desc.internal_format_ = _convGLInternalFormat(tex_param_cube_.pixel_format);
	tex_obj_desc.external_format_ = _convGLExternalFormat(tex_param_cube_.pixel_format);

	tex_obj_desc.min_filter_ = _convGLFilter(sampler_state_.min_filter_);
	tex_obj_desc.mag_filter_ = _convGLFilter(sampler_state_.mag_filter_);

	tex_obj_desc.wrap_s_ = _convGLAddr(sampler_state_.address_u_);
	tex_obj_desc.wrap_t_ = _convGLAddr(sampler_state_.address_v_);
	tex_obj_desc.type_ = _convGLDataType(tex_param_cube_.pixel_format);
	tex_obj_desc.is_mipmap_ = sampler_state_.is_mipmap;

	ENN_ASSERT(!texture_obj_);

	texture_obj_ = TextureObj::createInstance();

	texture_obj_->createTexture();

	texture_obj_->buildTextureCube(tex_obj_desc, tex_param_cube_.data);

	return true;
}

void Texture::destroy()
{
	ENN_SAFE_RELEASE(texture_obj_)
}

void Texture::setSamplerState(const SamplerState& ss)
{
	sampler_state_ = ss;
}

const Texture::SamplerState& Texture::getSamplerState() const
{
	return sampler_state_;
}

Texture::SamplerState& Texture::getSamplerState()
{
	return sampler_state_;
}

void Texture::setTexParam2D(const TexParams2D& param2d)
{
	tex_param_2d_ = param2d;
}

const Texture::TexParams2D& Texture::getTexParam2D() const
{
	return tex_param_2d_;
}

Texture::TexParams2D& Texture::getTexParam2D()
{
	return tex_param_2d_;
}

void Texture::setTexParamCube(const TexParamsCube& param3d)
{
	tex_param_cube_ = param3d;
}

const Texture::TexParamsCube& Texture::getTexParamCube() const
{
	return tex_param_cube_;
}

Texture::TexParamsCube& Texture::getTexParamCube()
{
	return tex_param_cube_;
}

int Texture::getWidth() const
{
	switch (tex_type_)
	{
	case TEXTURE_2D:
		return tex_param_2d_.width;
	case TEXTURE_CUBE:
		return tex_param_cube_.width;
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

int Texture::getHeight() const
{
	switch (tex_type_)
	{
	case TEXTURE_2D:
		return tex_param_2d_.height;
	case TEXTURE_CUBE:
		return tex_param_cube_.height;
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

bool Texture::isMipMap() const
{
	return sampler_state_.is_mipmap;
}

void Texture::destructResImpl()
{
	destroy();
}

}