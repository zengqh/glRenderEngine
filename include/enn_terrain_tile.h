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
// File: enn_terrain_tile.h
//

#pragma once

#include "enn_terrain_option.h"

namespace enn
{

class TerrainIndexHandler;
class TerrainTile : public AllocatedObject
{
public:
	TerrainTile(TerrainManager* tm, TerrainIndexHandler* idx_handler, TerrainInfo& ti,
		const TerrainOption& opt, size_t startx, size_t startz);

	~TerrainTile();

	uint32 getLOD() const;

	void updateTile();

protected:
	void createVertexData();
	void calculateMinLevelDist2();
	void calculateVertexNormals();
	void calculateVertexTangents();

protected:
	TerrainManager*			terrain_mgr_;
	TerrainIndexHandler*	terrain_idx_handler_;
	TerrainOption			terrain_opt_;
	TerrainInfo&			terrain_info_;

	size_t					tile_x_, tile_z_;
	size_t					start_x_, start_z_;

	size_t					lod_;
	vector<float>::type		lod_change_min_dist_sqr_;
	AxisAlignedBox			bounds_;
	float					bound_radius_;
	Vector3					center_;

	SubMesh*				terrain_mesh_;
};

}