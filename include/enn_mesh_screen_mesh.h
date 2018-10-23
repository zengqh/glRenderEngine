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
// Time: 2013/09/03
// File: enn_mesh_screen_mesh.h
//

#pragma once

#include "enn_mesh.h"

namespace enn
{

struct ScreenRectDesc
{
	int x, y, w, h, vw, vh;
	bool is_full_uv;
	ScreenRectDesc(int x_, int y_, int w_, int h_, int vw_, int vh_, bool is_full_uv_ = true)
		: x(x_)
		, y(y_)
		, w(w_)
		, h(h_)
		, vw(vw_)
		, vh(vh_)
		, is_full_uv(is_full_uv)
	{

	}
};

//////////////////////////////////////////////////////////////////////////
// ScreenMeshCreatorImpl
class ScreenMeshCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	ScreenMeshCreatorImpl(const ScreenRectDesc& rect_desc)
		: rect_desc_(rect_desc)
	{

	}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context )
	{
		vec3f v0, v1, v2, v3;

		int x, y, w, h, vw, vh;
		x = rect_desc_.x;
		y = rect_desc_.y;
		w = rect_desc_.w;
		h = rect_desc_.h;
		vw = rect_desc_.vw;
		vh = rect_desc_.vh;

		Math::screen_to_cube_space(v0, vw, vh, x, y + h);
		Math::screen_to_cube_space(v1, vw, vh, x + w, y + h);
		Math::screen_to_cube_space(v2, vw, vh, x + w, y);
		Math::screen_to_cube_space(v3, vw, vh, x, y);

		float ux = x / (float)vw;
		float vy = y / (float)vh;
		float ul = w / (float)vw;
		float vl = h / (float)vh;

		if (rect_desc_.is_full_uv)
		{
			ux = 0;
			vy = 0;
			ul = 1;
			vl = 1;
		}

		VertexP3N3U2 vts[4] =
		{
			// left bottom
			v0.x, v0.y, v0.z,
			0.0f, 1.0f, 0.0f,
			ux, vy + vl,

			// right bottom
			v1.x, v1.y, v1.z,
			0.0f, 1.0f, 0.0f,
			ux + ul, vy + vl,

			// right top
			v2.x, v2.y, v2.z,
			0.0f, 1.0f, 0.0f,
			ux + ul, vy,

			// left top
			v3.x, v3.y, v3.z,
			0.0f, 1.0f, 0.0f,
			ux, vy
		};
		
		ushort ids[6] = 
		{
			0, 1, 2,
			0, 2, 3
		};


		Mesh* mesh = static_cast<Mesh*>(res);
		SubMesh* sub_mesh = mesh->createNewSubMesh(0);

		sub_mesh->setPrimType(PRIM_TRIANGLELIST);
		sub_mesh->setVertexType(MASK_P3N3U2);
		sub_mesh->setVertexCount(ENN_ARRAY_SIZE(vts));

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(ENN_ARRAY_SIZE(ids));

		sub_mesh->setVertexData(vts);
		sub_mesh->setIndexData(ids);

		return true;
	}

	virtual bool buildResource( Resource* res )
	{
		Mesh* mesh = static_cast<Mesh*>(res);
		mesh->makeGPUBuffer();

		return true;
	}

protected:
	ScreenRectDesc		rect_desc_;
};

}