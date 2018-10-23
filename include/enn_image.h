/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/03/04
* File: image.h
**/

#pragma once

#include <enn_platform_headers.h>

namespace enn
{
enum CompressedFormat
{
	CF_NONE = 0,
	CF_DXT1,
	CF_DXT3,
	CF_DXT5,
	CF_ETC1,
	CF_PVRTC_RGB_2BPP,
	CF_PVRTC_RGBA_2BPP,
	CF_PVRTC_RGB_4BPP,
	CF_PVRTC_RGBA_4BPP,
};

class Deserializer;
class Image : public AllocatedObject
{
public:
	Image();
	virtual ~Image();

	virtual bool load(Deserializer& source);
	virtual bool load(const enn::String& name);
	virtual bool load_raw(const enn::String& name);

	virtual void setData(uint32 w, uint32 h, uint32 components, uint8* data);

	bool saveBMP(const enn::String& name);
	bool saveTGA(const enn::String& name);

	uint32 getWidth() const
	{
		return width_;
	}

	uint32 getHeight() const
	{
		return height_;
	}

	uint32 getComponents() const
	{
		return components_;
	}

	uint8* getData() const
	{
		return data_;
	}

	bool isCompressed() const
	{
		return (compress_format_ != CF_NONE);
	}

	uint32 getCompressFormat() const
	{
		return compress_format_;
	}

	uint32 getPixelFormat() const;

protected:
	uint32 width_;
	uint32 height_;
	uint32 components_;
	mutable uint32 pixel_format_;
	CompressedFormat compress_format_;
	uint8* data_;
};
}