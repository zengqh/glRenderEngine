//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2014/01/14
// File: enn_terrain_brush.cpp
//

#include "enn_terrain_brush.h"

namespace enn
{

TerrainBrush::TerrainBrush()
: width_(0)
, height_(0)
, brush_array_(0)
{
	
}

TerrainBrush::TerrainBrush(const float* brush, size_t width, size_t height)
{
	setBrush(brush, width, height);
}

TerrainBrush::TerrainBrush(const vector<float>::type& brush, size_t width, size_t height)
{
	setBrush(brush, width, height);
}

TerrainBrush::TerrainBrush(const TerrainBrush& other)
: width_(other.width_)
, height_(other.height_)
{
	size_t size = width_ * height_ * sizeof(float);
	brush_array_ = (float*)ENN_MALLOC(size);

	memcpy(brush_array_, other.brush_array_, size);
}

TerrainBrush::~TerrainBrush()
{
	ENN_SAFE_FREE(brush_array_);
}

TerrainBrush& TerrainBrush::operator=(const TerrainBrush& other)
{
	width_ = other.width_;
	height_ = other.height_;

	ENN_SAFE_FREE(brush_array_);

	size_t size = width_ * height_ * sizeof(float);

	brush_array_ = (float*)ENN_MALLOC(size);

	memcpy(brush_array_, other.brush_array_, size);

	return *this;
}

void TerrainBrush::setBrush(const float* brush, size_t width, size_t height)
{
	ENN_SAFE_FREE(brush_array_);

	width_ = width; height_ = height;

	size_t size = width * height * sizeof(float);
	brush_array_ = (float*)ENN_MALLOC(size);
	memcpy(brush_array_, brush, size);
}

void TerrainBrush::setBrush(const vector<float>::type& brush, size_t width, size_t height)
{
	ENN_SAFE_FREE(brush_array_);

	width_ = width; height_ = height;

	size_t size = width * height * sizeof(float);
	brush_array_ = (float*)ENN_MALLOC(size);

	memcpy(brush_array_, &brush[0], size);
}

void TerrainBrush::loadBrushFromImage(const Image& img)
{
	int w = img.getWidth();
	int h = img.getHeight();

	vector<float>::type brush(w * h);

	int bpp = img.getComponents();
	uint32 maxValue = (1 << (bpp*8)) - 1;

	const uchar* imageData = img.getData();

	for (size_t i = 0; i < brush.size(); ++i)
	{
		uint32 val = 0;
		memcpy(&val, imageData, bpp);
		imageData += bpp;
		brush[i] = float(val) / maxValue;
	}

	setBrush(brush, w, h);
}

void TerrainBrush::saveBrushToImage(Image& img)
{
	// not impl
	ENN_ASSERT(0);
}

}