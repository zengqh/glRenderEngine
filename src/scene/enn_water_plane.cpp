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
// File: enn_water_plane.cpp
//

#include "enn_water_plane.h"
#include "enn_root.h"

namespace enn
{

WaterPlane::WaterPlane()
{
	water_color_.setValue(0, 0.15f, 0.225f);
	water_scale_.setValue(1.53f, 1.53f * 1.1f);
	water_speed_.setValue(0.000083f, 0.000083f * 2.1f);
	water_refract_turb_ = 1;
	water_reflect_turb_ = 1;
	water_refract_fade_ = 0.3f;
	specular_power_ = 200.0f;
	fresnel_power_ = 5.57f;
	fresnel_scale_ = 2;

	water_mesh_ = 0;
	water_plane_desc_ = 0;
	normal_tex_ = 0;
}

WaterPlane::~WaterPlane()
{

}

void WaterPlane::create_water(float w, float h)
{
	create_mesh(w, h);
}

void WaterPlane::create_mesh(float w, float h)
{
	float half_w = w * 0.5f;
	float half_h = h * 0.5f;
	float tw = 1;
	float th = h / w;

	VertexP3U2U2 vts[4] =
	{
		-half_w, 0, half_h, 0, 0, 0, 0,
		half_w, 0, half_h, tw, 0, 1, 0,
		half_w, 0, -half_h, tw, th, 1, 1,
		-half_w, 0, -half_h, 0, th, 0, 1
	};

	uint16 ids[6] = 
	{
		0, 1, 2,
		0, 2, 3
	};

	ENN_ASSERT(!water_name_.empty());

	water_mesh_ = (Mesh*)ROOTPTR->get_mesh_mgr()->createResource(water_name_);

	SubMesh* sub_mesh = water_mesh_->createNewSubMesh(0);

	sub_mesh->setPrimType(PRIM_TRIANGLELIST);
	sub_mesh->setVertexType(MASK_P3U2U2);
	sub_mesh->setVertexCount(ENN_ARRAY_SIZE(vts));

	sub_mesh->setIdxType(IDX_16);
	sub_mesh->setIndexCount(ENN_ARRAY_SIZE(ids));

	sub_mesh->setVertexData(vts);
	sub_mesh->setIndexData(ids);

	sub_mesh->makeGPUBuffer();

	water_mesh_->updateAABB(true);
}

void WaterPlane::create_mtr()
{
	normal_tex_ = ENN_NEW TextureUnit();
	normal_tex_->setTexName(water_normal_tex_name_);
}

}