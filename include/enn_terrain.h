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
// Time: 2013/10/07
// File: enn_terrain.h
//

#pragma once

#include "enn_terrain_info.h"
#include "enn_terrain_brush.h"

namespace enn
{

class TerrainManager
{
public:
	TerrainManager(const String& name = "Terrain");
	~TerrainManager();

	void createTerrain(const TerrainInfo& info,
		size_t tileSize = 33, unsigned int maxLOD = 255);

	void destroyTerrain();

	/**
     * Specifies the pixel error tolerance when determining LOD to use for each tile.
     * Note that this call does not affect already loaded terrain data.
     * @param maxPixelError  the maximum estimated pixel error to tolerate
     * @param viewportHeight vertical resolution of the viewport to use to estimate the pixel error
     */
	void setLODErrorMargin(unsigned int maxPixelError, unsigned int viewportHeight);

	const TerrainInfo& getTerrainInfo() const;

	void deform(int x, int z, const Brush& brush, float intensity = 1.0f);


};

}