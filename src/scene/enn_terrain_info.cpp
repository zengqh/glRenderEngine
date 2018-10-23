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
// File: enn_terrain_info.cpp
//

#include "enn_terrain_info.h"

namespace enn
{

TerrainInfo::TerrainInfo()
: width_(0)
, height_(0)
, offset_(Vector3::ZERO)
, scale_(Vector3::UNIT_SCALE)
{

}

TerrainInfo::TerrainInfo(size_t width, size_t height, const float* heightmap)
: width_(0)
, height_(0)
, offset_(Vector3::ZERO)
, scale_(Vector3::UNIT_SCALE)
{
	setHeightmap(width, height, heightmap);
}

TerrainInfo::TerrainInfo(size_t width, size_t height, const vector<float>::type& heightmap)
: width_(0)
, height_(0)
, offset_(Vector3::ZERO)
, scale_(Vector3::UNIT_SCALE)
{
	setHeightmap(width, height, heightmap);
}

void TerrainInfo::setHeightmap(size_t width, size_t height, const float* heightmap)
{
	height_map_.assign(heightmap, heightmap + width * height);
	width_ = width;
	height_ = height;
}

void TerrainInfo::setHeightmap(size_t width, size_t height, const vector<float>::type& heightmap)
{
	height_map_ = heightmap;
	width_ = width;
	height_ = height;
}

const vector<float>::type& TerrainInfo::getHeightmapData() const
{
	return height_map_;
}

float& TerrainInfo::at(size_t x, size_t y)
{
	return height_map_[x + y * height_];
}

const float TerrainInfo::at(size_t x, size_t y) const
{
	return height_map_[x + y * height_];
}

size_t TerrainInfo::getWidth() const
{
	return width_;
}

size_t TerrainInfo::getHeight() const
{
	return height_;
}

void TerrainInfo::setExtents(const AxisAlignedBox& extents)
{
	offset_ = extents.getMinimum();
	scale_ = extents.getMaximum() - extents.getMinimum();

	scale_.x /= width_;
	scale_.y /= height_;
}

AxisAlignedBox TerrainInfo::getExtents() const
{
	Vector3 maximum = scale_;
	maximum.x *= width_;
	maximum.z *= height_;
	maximum += offset_;
	return AxisAlignedBox(offset_, maximum);
}

const Vector3& TerrainInfo::getScaling() const
{
	return scale_;
}

const Vector3& TerrainInfo::getOffset() const
{
	return offset_;
}

float TerrainInfo::getHeightAt(float x, float z) const
{
	x -= offset_.x;
	z -= offset_.z;
	x /= scale_.x;
	z /= scale_.z;
	if (x < 0.0 || x > width_ - 1 || z < 0.0 || z > height_ - 1)
	{
		// out of bounds
		return offset_.y - 1;
	}

	size_t xi = (size_t) x, zi = (size_t) z;
	float xpct = x - xi, zpct = z - zi;
	if (xi == width_-1)
	{
		// one too far
		--xi;
		xpct = 1.0f;
	}
	if (zi == height_-1)
	{
		--zi;
		zpct = 1.0f;
	}

	float heights[4];
	heights[0] = at(xi, zi);
	heights[1] = at(xi, zi + 1);
	heights[2] = at(xi + 1, zi);
	heights[3] = at(xi + 1, zi + 1);

	// interpolate
	float w[4];
	w[0] = (1.0 - xpct) * (1.0 - zpct);
	w[1] = (1.0 - xpct) * zpct;
	w[2] = xpct * (1.0 - zpct);
	w[3] = xpct * zpct;
	float ipHeight = w[0]*heights[0] + w[1]*heights[1] + w[2]*heights[2] + w[3]*heights[3];

	ipHeight *= scale_.y;
	ipHeight += offset_.y;

	return ipHeight;
}

Vector3 TerrainInfo::getNormalAt(float x, float z) const
{
	int flip = 1;
	Vector3 here(x, getHeightAt(x, z), z);
	Vector3 left(x - 1, getHeightAt(x - 1, z), z);
	Vector3 down(x, getHeightAt(x, z + 1), z + 1);

	if (left.x < 0.0)
	{
		flip *= -1;
		left = Vector3(x + 1, getHeightAt(x + 1, z), z);
	}

	if (down.z >= offset_.z + scale_.z * (height_ - 1))
	{
		flip *= -1;
		down = Vector3(x, getHeightAt(x, z - 1), z - 1);
	}

	left -= here;
	down -= here;

	Vector3 norm = flip * left.crossProduct(down);
	norm.normalise();

	return norm;
}

Vector3 TerrainInfo::getTangentAt(float x, float z) const
{
	Vector3 v3Return;
	int flip = 1;
	Vector3 here (x, getHeightAt(x, z), z);
	Vector3 left (x - 1, getHeightAt(x - 1, z), z);
	if (left.x < 0.0)
	{
		flip *= -1;
		left = Vector3(x + 1, getHeightAt(x + 1, z), z);
	}

	left -= here;
	v3Return = flip * left;
	v3Return.normalise();
	return v3Return;
}

std::pair<bool, Vector3> TerrainInfo::rayIntersects(const Ray& ray) const
{
	AxisAlignedBox box = getExtents();
	Vector3 point = ray.getOrigin();
	Vector3 dir = ray.getDirection();

	if (!box.contains(point))
	{
		// not inside the box, so let's see if we are actually
		// colliding with it
		std::pair<bool, float> res = ray.intersects(box);
		if (!res.first)
		{
			return std::make_pair(false, Vector3::ZERO);
		}

		// update point to the collision position
		point = ray.getPoint(res.second);
	}

	// now move along the ray until we intersect or leave the bounding box
	while (true)
	{
		// have we arived at or under the terrain height?
		// note that this approach means that ray queries from below won't work
		// correctly, but then again, that shouldn't be a usual case...
		float height = getHeightAt(point.x, point.z);
		if (point.y <= height)
		{
			point.y = height;
			return std::make_pair(true, point);
		}

		// move further...
		point += dir;

		// check if we are still inside the boundaries
		if (point.x < box.getMinimum().x || point.z < box.getMinimum().z
			|| point.x > box.getMaximum().x || point.z > box.getMaximum().z)
		{
			return std::make_pair(false, Vector3::ZERO);
		}
	}
}

}