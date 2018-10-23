/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/01
* File: enn_texture.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_resource.h"
#include "enn_render_def.h"
#include "enn_gl_texture_obj.h"

namespace enn
{
class Texture : public Resource
{
	ENN_DECLARE_RTTI(Texture)

	enum {TEX_LOAD_PRIORITY = 1};

public:
	struct TexParams2D
	{
		int width;
		int height;
		void* data;
		uint32 pixel_format;
	};

	struct TexParamsCube
	{
		int width;
		int height;
		void* data[6];
		uint32 pixel_format;
	};

	struct SamplerState
	{
		uint32 address_u_;
		uint32 address_v_;
		uint32 address_w_;

		uint32 min_filter_;
		uint32 mag_filter_;
		bool is_mipmap;
	};

public:
	Texture();
	virtual ~Texture();

public:
	bool load_as_texture_2d();
	bool load_as_texture_cube();

	bool create_as_texture_2d();
	bool create_as_texture_cube();

	void destroy();

public:
	void setSamplerState(const SamplerState& ss);
	const SamplerState& getSamplerState() const;
	SamplerState& getSamplerState();

	void setTexParam2D(const TexParams2D& param2d);
	const TexParams2D& getTexParam2D() const;
	TexParams2D& getTexParam2D();

	void setTexParamCube(const TexParamsCube& param3d);
	const TexParamsCube& getTexParamCube() const;
	TexParamsCube& getTexParamCube();

	int getWidth() const;
	int getHeight() const;

	bool isMipMap() const;

	TextureObj* getTextureObj() const
	{
		return texture_obj_;
	}

public:
	virtual void destructResImpl();

protected:
	SamplerState	sampler_state_;
	TexParams2D		tex_param_2d_;
	TexParamsCube	tex_param_cube_;
	uint32			tex_type_;
	TextureObj*		texture_obj_;
};

typedef ResPtr<Texture>	TexturePtr;

}