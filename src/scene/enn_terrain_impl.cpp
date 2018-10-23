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
// File: enn_terrain_impl.cpp
//

#include "enn_terrain_impl.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
uint32 checkTileSizeAndGetMaxLOD(size_t tileSize)
{
	for (size_t i  = 1; i <= 10; ++i)
	{
		if (tileSize == (uint32)(1 << i) + 1)
		{
			return i + 1;
		}
	}

	ENN_ASSERT(0);
	return -1;
}

uint32 checkInfo(const TerrainInfo& info, size_t tileSize)
{
	uint32 ret = checkTileSizeAndGetMaxLOD(tileSize);

	if (info.getWidth() == 1 || info.getWidth() % (tileSize - 1) != 1)
	{
		ENN_ASSERT(0);
	}

	if (info.getHeight() == 1 || info.getHeight() % (tileSize - 1) != 1)
	{
		ENN_ASSERT(0);
	}
}

//////////////////////////////////////////////////////////////////////////
TerrainImpl::TerrainImpl()
: terrain_loaded_(false)
, idx_handler_(0)
, viewport_height_(0)
, maxLODAtBorders(false)
{
	terrain_option_.maxPixelError = 3;
	terrain_option_.useLODMorph = false;
	terrain_option_.factorC = 0;
}

TerrainImpl::~TerrainImpl()
{
	destroyTerrain();
}

void TerrainImpl::createTerrain(const TerrainInfo& info, size_t tile_size, uint32 maxLOD)
{
	if (terrain_option_.factorC <= 0)
	{
		ENN_ASSERT(0);
		return;
	}

	uint32 maxPossibleLOD = checkInfo(info, tile_size);

	destroyTerrain();

	terrain_info_ = info;

	terrain_option_.tile_size = tile_size;

	if (maxLOD < 1)
	{
		maxLOD = 1;
	}

	terrain_option_.maxMipMapLevel = Math::minVal(maxLOD, maxPossibleLOD);

	idx_handler_ = ENN_NEW TerrainIndexHandler(terrain_option_.tile_size, terrain_option_.maxMipMapLevel);

	createTiles();

	terrain_loaded_ = true;
}

void TerrainImpl::destroyTerrain()
{
	
}

void TerrainImpl::setLODErrorMargin(uint32 maxPixelError, uint32 viewportHeight)
{

}

void TerrainImpl::setUseLodMorphing(bool lodMorph, float startMorphing, 
	const String& morphParamName)
{

}

uint32 TerrainImpl::getNeighbourState(size_t x, size_t z) const
{

}

void TerrainImpl::deform(int x, int z, const Brush& brush, float intensity)
{

}

void TerrainImpl::createTiles()
{
	
}

void TerrainImpl::updateTiles(int fromX, int fromZ, int toX, int toZ)
{

}


}