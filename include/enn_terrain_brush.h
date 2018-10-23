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
// File: enn_terrain_brush.h
//

#pragma once

#include "enn_terrain_info.h"
#include "enn_image.h"

namespace enn
{

class TerrainBrush : public AllocatedObject
{
public:
	TerrainBrush();
	TerrainBrush(const float* brush, size_t width, size_t height);
	TerrainBrush(const vector<float>::type& brush, size_t width, size_t height);
	TerrainBrush(const TerrainBrush& other);
	~TerrainBrush();

	TerrainBrush& operator=(const TerrainBrush& other);

	void setBrush(const float* brush, size_t width, size_t height);
	void setBrush(const vector<float>::type& brush, size_t width, size_t height);

	size_t getWidth() const { return width_; }
	size_t getHeight() const { return height_; }

	float& at(size_t x, size_t y)
	{
		return brush_array_[x + y * width_];
	}
	
	const float at(size_t x, size_t y) const
	{
		return brush_array_[x + y * width_];
	}

	void loadBrushFromImage(const Image& img);
	void saveBrushToImage(Image& img);

protected:
	size_t width_, height_;
	float* brush_array_;
};

}