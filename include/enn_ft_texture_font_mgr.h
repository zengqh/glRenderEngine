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
// File: enn_ft_texture_font_mgr.h
//

#pragma once

#include "enn_ft_texture_font.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct FTTextureFontDesc : public AllocatedObject
{
	String			name;
	int				size;
	FTTextureFont*	tf;

	FTTextureFontDesc() : size(0), tf(0)
	{

	}
};

//////////////////////////////////////////////////////////////////////////
class FTTextureFontMgr : public AllocatedObject
{
	typedef enn::vector<FTTextureFontDesc>::type	FTTextureFontDescList;
	typedef enn::vector<FTTextureAtlas*>::type		FTTextureAtlasList;

public:
	FTTextureFontMgr();
	virtual ~FTTextureFontMgr();

public:
	bool initalize(int w, int h);

public:
	FTTextureFont* getFromFileName(const String& name, int size);

public:
	bool get_glyph_region(nv::vec4i& region, FTTextureAtlas*& atlas, int w, int h);

protected:
	void destroy();

public:
	FT_Library					ft_library_;

protected:
	FTTextureFontDescList		tf_desc_list_;
	FTTextureAtlasList			atlas_list_;
	int							atlas_w_;
	int							atlas_h_;
};

}