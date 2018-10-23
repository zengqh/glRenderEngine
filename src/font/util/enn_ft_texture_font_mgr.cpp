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
// File: enn_ft_texture_font_mgr.cpp
//

#include "enn_ft_texture_font_mgr.h"
#include "enn_math.h"

namespace enn
{

FTTextureFontMgr::FTTextureFontMgr()
: atlas_w_(0)
, atlas_h_(0)
{

}

FTTextureFontMgr::~FTTextureFontMgr()
{
	destroy();
}

bool FTTextureFontMgr::initalize(int w, int h)
{
	atlas_w_ = w;
	atlas_h_ = h;

	FT_Error error = FT_Init_FreeType( &ft_library_ );
	if (error)
	{
		ENN_ASSERT(0);
		return false;
	}

	return true;
}

FTTextureFont* FTTextureFontMgr::getFromFileName(const String& name, int size)
{
	// check if exist
	ENN_FOR_EACH(FTTextureFontDescList, tf_desc_list_, it)
	{
		FTTextureFontDesc& ft_desc = *it;
		if (ft_desc.name == name && ft_desc.size == size)
		{
			ENN_ASSERT(ft_desc.tf);

			return ft_desc.tf;
		}
	}

	// create texture font
	FTTextureFont* new_texture_font = ENN_NEW FTTextureFont(this);
	ENN_ASSERT_IMPL(new_texture_font->initalize(name, size));

	FTTextureFontDesc new_ft_desc;
	new_ft_desc.name = name;
	new_ft_desc.size = size;
	new_ft_desc.tf = new_texture_font;

	tf_desc_list_.push_back(new_ft_desc);

	return new_texture_font;
}

bool FTTextureFontMgr::get_glyph_region(nv::vec4i& region, FTTextureAtlas*& atlas, int w, int h)
{
	ENN_FOR_EACH(FTTextureAtlasList, atlas_list_, it)
	{
		FTTextureAtlas* atlas_temp = *it;
		if (atlas_temp->getRegion(w, h, region))
		{
			atlas = atlas_temp;
			return true;
		}
	}

	// not found, then create one.
	FTTextureAtlas* atlas_new = ENN_NEW FTTextureAtlas();
	atlas_new->initalize(atlas_w_, atlas_h_);

	atlas_list_.push_back(atlas_new);

	ENN_ASSERT_IMPL(atlas_new->getRegion(w, h, region));

	atlas = atlas_new;

	return true;
}

void FTTextureFontMgr::destroy()
{
	ENN_FOR_EACH(FTTextureFontDescList, tf_desc_list_, it)
	{
		FTTextureFontDesc& font_desc = *it;

		ENN_SAFE_DELETE(font_desc.tf);
	}

	ENN_FOR_EACH(FTTextureAtlasList, atlas_list_, it)
	{
		FTTextureAtlas* atlas = *it;
		ENN_SAFE_DELETE(atlas);
	}

	atlas_list_.clear();
	tf_desc_list_.clear();

	FT_Done_FreeType( ft_library_ );
}

}