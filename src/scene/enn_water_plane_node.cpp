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
// Time: 2013/11/28
// File: enn_water_plane_node.cpp
//

#include "enn_water_plane_node.h"

namespace enn 
{

WaterPlaneNode::WaterPlaneNode()
: water_plane_(0)
{
	type_flag_.set_flag(NODE_WATER_PLANE);
}

WaterPlaneNode::~WaterPlaneNode()
{
	ENN_SAFE_DELETE(water_plane_)
}

void WaterPlaneNode::createWaterPlane(const String& name, float w, float h)
{
	water_plane_ = ENN_NEW WaterPlane();

	water_plane_->water_name_ = name;
	water_plane_->create_water(w, h);
}

WaterPlane* WaterPlaneNode::getWaterPlane()
{
	return water_plane_;
}

const AxisAlignedBox& WaterPlaneNode::getObjectAABB() const
{
	return water_plane_->water_mesh_->getAABB();
}

}