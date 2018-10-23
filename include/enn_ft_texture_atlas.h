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
// Time: 2013/09/18
// File: enn_ft_texture_atlas.h
//

#pragma once

#include "enn_ft_headers.h"

namespace enn
{

class FTTextureAtlas : public AllocatedObject
{
	enum
	{
		ATLAS_DIRTY = 0,
		ATLAS_FULL
	};

public:
	typedef enn::list<nv::vec3i*>::type		NodesList;
	typedef NodesList::iterator				NodesIter;

public:
	FTTextureAtlas();
	virtual ~FTTextureAtlas();

	bool initalize(int w, int h);

public:
	void setRegion(int x, int y, int w, int h, const uchar* data, int stride);
	void upload();
	bool getRegion(int w, int h, nv::vec4i& out_reg);
	bool bindTexture();

protected:
	void destroy();

protected:
	int texture_atlas_fit(NodesIter index, int width, int height);
	void texture_atlas_merge();

public:
	int			w_;
	int			h_;
	uchar*		data_;
	uint32		tex_id_;
	bool		is_first_upload_;
	NodesList	nodes_;
	size_t		used;
	Flags32		atlas_flags_;
};

}