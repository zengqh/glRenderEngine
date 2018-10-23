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
// Time: 2014/01/15
// File: enn_terrain_index_handler.cpp
//

#include "enn_terrain_index_handler.h"

namespace enn
{

TerrainIndexHandler::TerrainIndexHandler(size_t tileSize, uint32 maxLOD)
: tile_size_(tileSize)
, indexes_(maxLOD)
{
	
}

TerrainIndexHandler::~TerrainIndexHandler()
{
	
}

uint16* TerrainIndexHandler::requestIndexData(uint32 lod, uint32 neighbourState)
{
	uint16* idx = 0;
	IndexMap::iterator it = indexes_[lod].find(neighbourState);
	if (it != )
	{
	}
}

uint16* TerrainIndexHandler::createIndexes(uint32 lod, uint32 neighbourState)
{
	
}

int TerrainIndexHandler::stitchEdge(int direction, uint32 hiLOD, uint32 loLOD, bool omitFirstTri, 
	bool omitLastTri, uint16* ppIdx)
{
	
}

uint16 TerrainIndexHandler::index(size_t x, size_t z) const
{

}

}