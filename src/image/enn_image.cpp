/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/03/04
* File: image.cpp
**/

#include <enn_image.h>
#include <enn_stb_image.h>
#include <enn_stb_image_write.h>
#include <enn_file.h>
#include <enn_render_def.h>
#include "enn_math.h"

#ifndef MAKEFOURCC
#define MAKEFOURCC(ch0, ch1, ch2, ch3) ((unsigned)(ch0) | ((unsigned)(ch1) << 8) | ((unsigned)(ch2) << 16) | ((unsigned)(ch3) << 24))
#endif

#define FOURCC_DXT1 (MAKEFOURCC('D','X','T','1'))
#define FOURCC_DXT2 (MAKEFOURCC('D','X','T','2'))
#define FOURCC_DXT3 (MAKEFOURCC('D','X','T','3'))
#define FOURCC_DXT4 (MAKEFOURCC('D','X','T','4'))
#define FOURCC_DXT5 (MAKEFOURCC('D','X','T','5'))

namespace enn
{

struct DDColorKey
{
	unsigned dwColorSpaceLowValue_;
	unsigned dwColorSpaceHighValue_;
};

struct DDPixelFormat
{
	unsigned dwSize_;
	unsigned dwFlags_;
	unsigned dwFourCC_;
	union
	{
		unsigned dwRGBBitCount_;
		unsigned dwYUVBitCount_;
		unsigned dwZBufferBitDepth_;
		unsigned dwAlphaBitDepth_;
		unsigned dwLuminanceBitCount_;
		unsigned dwBumpBitCount_;
		unsigned dwPrivateFormatBitCount_;
	};
	union
	{
		unsigned dwRBitMask_;
		unsigned dwYBitMask_;
		unsigned dwStencilBitDepth_;
		unsigned dwLuminanceBitMask_;
		unsigned dwBumpDuBitMask_;
		unsigned dwOperations_;
	};
	union
	{
		unsigned dwGBitMask_;
		unsigned dwUBitMask_;
		unsigned dwZBitMask_;
		unsigned dwBumpDvBitMask_;
		struct
		{
			unsigned short wFlipMSTypes_;
			unsigned short wBltMSTypes_;
		} multiSampleCaps_;
	};
	union
	{
		unsigned dwBBitMask_;
		unsigned dwVBitMask_;
		unsigned dwStencilBitMask_;
		unsigned dwBumpLuminanceBitMask_;
	};
	union
	{
		unsigned dwRGBAlphaBitMask_;
		unsigned dwYUVAlphaBitMask_;
		unsigned dwLuminanceAlphaBitMask_;
		unsigned dwRGBZBitMask_;
		unsigned dwYUVZBitMask_;
	};
};

struct DDSCaps2
{
	unsigned dwCaps_;
	unsigned dwCaps2_;
	unsigned dwCaps3_;
	union
	{
		unsigned dwCaps4_;
		unsigned dwVolumeDepth_;
	};
};

struct DDSurfaceDesc2
{
	unsigned dwSize_;
	unsigned dwFlags_;
	unsigned dwHeight_;
	unsigned dwWidth_;
	union
	{
		long lPitch_;
		unsigned dwLinearSize_;
	};
	union
	{
		unsigned dwBackBufferCount_;
		unsigned dwDepth_;
	};
	union
	{
		unsigned dwMipMapCount_;
		unsigned dwRefreshRate_;
		unsigned dwSrcVBHandle_;
	};
	unsigned dwAlphaBitDepth_;
	unsigned dwReserved_;
	void* lpSurface_;
	union
	{
		DDColorKey ddckCKDestOverlay_;
		unsigned dwEmptyFaceColor_;
	};
	DDColorKey ddckCKDestBlt_;
	DDColorKey ddckCKSrcOverlay_;
	DDColorKey ddckCKSrcBlt_;
	union
	{
		DDPixelFormat ddpfPixelFormat_;
		unsigned dwFVF_;
	};
	DDSCaps2 ddsCaps_;
	unsigned dwTextureStage_;
};


Image::Image()
	: width_(0)
	, height_(0)
	, components_(0)
	, data_(0)
	, pixel_format_(Math::ENN_MAX_UNSIGNED)
	, compress_format_(CF_NONE)
{

}

Image::~Image()
{
	ENN_SAFE_FREE(data_);
	width_ = 0;
	height_ = 0;
	components_ = 0;
	data_ = 0;
}

bool Image::load(Deserializer& source)
{
	/** read file id. */
	enn::String file_id;
	file_id.resize(4);

	source.Read(&file_id[0], 4);

	if (file_id == enn::String("DDS "))
	{
		// DDS compressed format
		DDSurfaceDesc2 ddsd;
		source.Read(&ddsd, sizeof(ddsd));

		 switch (ddsd.ddpfPixelFormat_.dwFourCC_)
		 {
		 case FOURCC_DXT1:
			 compress_format_ = CF_DXT1;
			 components_ = 3;
			 break;

		 case FOURCC_DXT3:
			 compress_format_ = CF_DXT3;
			 components_ = 4;
			 break;

		 case FOURCC_DXT5:
			 compress_format_ = CF_DXT5;
			 components_ = 4;
			 break;

		 default:
			 ENN_ASSERT(0);
			 return false;
		 }

		 unsigned dataSize = source.GetSize() - source.GetPosition();
		 data_ = (uint8*)ENN_MALLOC(dataSize);
		 width_ = ddsd.dwWidth_;
		 height_ = ddsd.dwHeight_;

		 source.Read(data_, dataSize);
	}
	else
	{
		source.Seek(0);
		int width, height;
		int components;
		unsigned char* pixelData = 0;

		uint32 data_size = source.GetSize();
		uchar* file_buffer = (uchar*)ENN_MALLOC(data_size);
		source.Read(file_buffer, data_size);

		pixelData = stbi_load_from_memory(file_buffer, 
			data_size, &width, &height, &components, 0);

		ENN_SAFE_FREE(file_buffer);

		if (!pixelData)
		{
			ENN_ASSERT(0);
			return false;
		}

		width_ = width;
		height_ = height;
		components_ = components;

		int image_size = width_ * height_ * components_;	/** only support 8 byte. */

		ENN_SAFE_FREE(data_);

		data_ = (uint8*)ENN_MALLOC(image_size);

		memcpy(data_, pixelData, image_size);

		stbi_image_free(pixelData);
	}

	return true;
}

bool Image::load_raw(const enn::String& name)
{
	File file(name);

	if (!file.IsOpen())
	{
		ENN_ASSERT(0);
		return false;
	}

	uint32 image_size = file.GetSize();

	data_ = (uint8*)ENN_MALLOC(image_size);

	file.Read(data_, image_size);

	width_ = height_ = (int)(Math::sqrt(image_size / sizeof(uint32)) + 0.5f);

	components_ = 4;

	pixel_format_ = U4_RGBA;

	return true;
}

bool Image::load(const enn::String& name)
{
	File file(name, FILE_READ);
	return load(file);
}

void Image::setData(uint32 w, uint32 h, uint32 components, uint8* data)
{
	width_ = w;
	height_ = h;
	components_ = components;
	
	uint32 image_size = w * h * components;

	ENN_SAFE_FREE(data_);
	data_ = (uint8*)ENN_MALLOC(image_size);

	memcpy(data_, data, image_size);
}

bool Image::saveBMP(const enn::String& name)
{
	if (!data_)
	{
		return false;
	}

	return stbi_write_bmp(name.c_str(), width_, height_, components_, data_) ? true : false;
}

bool Image::saveTGA(const enn::String& name)
{
	if (!data_)
	{
		return false;
	}

	return stbi_write_tga(name.c_str(), width_, height_, components_, data_) ? true : false;
}

uint32 Image::getPixelFormat() const
{
	if (pixel_format_ == Math::ENN_MAX_UNSIGNED)
	{
		if (isCompressed())
		{
			switch (compress_format_)
			{
			case CF_DXT1:
				pixel_format_ = DXT1;
				break;
			case CF_DXT3:
				pixel_format_ = DXT3;
				break;
			case CF_DXT5:
				pixel_format_ = DXT5;
				break;
			default:
				ENN_ASSERT(0);
				pixel_format_ = Math::ENN_MAX_UNSIGNED;
			}
		}
		else
		{
			switch (components_)
			{
			case 1:
				pixel_format_ = U1;
				break;
			case 3:
				pixel_format_ = U3_RGB;
				break;
			case 4:
				pixel_format_ = U4_RGBA;
				break;
			default:
				ENN_ASSERT(0);
				pixel_format_ = Math::ENN_MAX_UNSIGNED;
			}
		}
	}

	return pixel_format_;
}
}