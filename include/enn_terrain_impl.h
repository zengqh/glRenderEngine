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
// File: enn_terrain_impl.h
//

#pragma once

#include "enn_terrain_option.h"

namespace enn
{

class TerrainTile;
class TerrainIndexHandler;
class TerrainImpl : public AllocatedObject
{
	typedef vector<TerrainTile*>::type		TileCol;
	typedef vector<TileCol*>::type			TileGrid;

public:
	TerrainImpl();
	~TerrainImpl();

	void createTerrain(const TerrainInfo& info, size_t tile_size, uint32 maxLOD);
	void destroyTerrain();

	bool isTerrainLoaded() const { return terrain_loaded_; }
	const TerrainInfo& getTerrainInfo() const { return terrain_info_; }
	
	void setLODErrorMargin(uint32 maxPixelError, uint32 viewportHeight);
	void setUseLodMorphing(bool lodMorph, float startMorphing, const String& morphParamName);

	uint32 getNeighbourState(size_t x, size_t z) const;

	void deform(int x, int z, const Brush& brush, float intensity);
	
protected:
	void createTiles();
	void updateTiles(int fromX, int fromZ, int toX, int toZ);

protected:
	bool							terrain_loaded_;
	TerrainIndexHandler*			idx_handler_;
	TerrainInfo						terrain_info_;
	TerrainOption					terrain_option_;
	int								viewport_height_;
	TileGrid						tiles_;
	bool							maxLODAtBorders;
};

}