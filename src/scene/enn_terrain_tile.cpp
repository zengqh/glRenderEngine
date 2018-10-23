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
// File: enn_terrain_tile.cpp
//

#include "enn_terrain_tile.h"

namespace enn
{

TerrainTile::TerrainTile(TerrainManager* tm, TerrainIndexHandler* idx_handler, TerrainInfo& ti,
	const TerrainOption& opt, size_t startx, size_t startz)
	: terrain_mgr_(tm)
	, terrain_idx_handler_(idx_handler)
	, terrain_opt_(opt)
	, start_x_(startx)
	, start_z_(startz)
	, lod_change_min_dist_sqr_(opt.maxMipMapLevel)
	, terrain_mesh_(0)
{
	tile_x_ = startx / (opt.tile_size - 1);
	tile_z_ = startz / (opt.tile_size - 1);

	createVertexData();
	calculateMinLevelDist2();
}

TerrainTile::~TerrainTile()
{

}

uint32 TerrainTile::getLOD() const
{

}

void TerrainTile::updateTile()
{

}

void TerrainTile::createVertexData()
{
	terrain_mesh_ = ENN_NEW SubMesh(0);

	size_t vertex_cnt = terrain_opt_.tile_size * terrain_opt_.tile_size;

	VertexP3N3U2* vb = ENN_MALLOC_ARRAY_T(VertexP3N3U2, vertex_cnt);

	terrain_mesh_->setVertexType(MASK_P3N3U2);

	size_t endx = start_x_ + terrain_opt_.tile_size;
	size_t endz = start_z_ + terrain_opt_.tile_size;

	VertexP3N3U2* curr_vb = vb;

	for (size_t j = start_z_; j < endz; ++j)
	{
		for (size_t i = start_x_; i < endx; ++i)
		{
			float x = terrain_info_.getOffset().x + terrain_info_.getScaling().x * i;
			float y = terrain_info_.getOffset().y + terrain_info_.at(i, j) * terrain_info_.getScaling().y;
			float z = terrain_info_.getOffset().z + terrain_info_.getScaling().z * j;

			float u = float(i) / (terrain_info_.getWidth() - 1);
			float v = float(j) / (terrain_info_.getHeight() - 1);

			curr_vb->x = x;
			curr_vb->y = y;
			curr_vb->z = z;
			curr_vb->nx = 0;
			curr_vb->ny = 1;
			curr_vb->nz = 0;
			curr_vb->u = u;
			curr_vb->v = v;
			
			curr_vb++;
		}
	}


}

void TerrainTile::calculateMinLevelDist2()
{

}

void TerrainTile::calculateVertexNormals()
{

}

void TerrainTile::calculateVertexTangents()
{

}


}