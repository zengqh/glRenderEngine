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
// File: enn_terrain_info.h
//

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_node_def.h"

namespace enn
{

class TerrainInfo
{
public:
	TerrainInfo();
	TerrainInfo(size_t width, size_t height, const float* heightmap = 0);
	TerrainInfo(size_t width, size_t height, const vector<float>::type& heightmap);

	void setHeightmap(size_t width, size_t height, const float* heightmap = 0);
	void setHeightmap(size_t width, size_t height, const vector<float>::type& heightmap);

	const vector<float>::type& getHeightmapData() const;

	float& at(size_t x, size_t y);
	const float at(size_t x, size_t y) const;

	size_t getWidth() const;
	size_t getHeight() const;

	void setExtents(const AxisAlignedBox& extents);
	AxisAlignedBox getExtents() const;

	const Vector3& getScaling() const;
	const Vector3& getOffset() const;

	float getHeightAt(float x, float z) const;

	Vector3 getNormalAt(float x, float z) const;
	Vector3 getTangentAt(float x, float z) const;

	std::pair<bool, Vector3> rayIntersects(const Ray& ray) const;

protected:
	size_t					width_, height_;
	vector<float>::type		height_map_;
	Vector3					offset_;
	Vector3					scale_;
};

}