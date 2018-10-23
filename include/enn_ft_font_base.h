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
// Time: 2013/09/11
// File: enn_ft_font_base.h
//

#pragma once

#include "enn_ft_font_mgr.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
enum FontRecordStyle
{
	NORMAL_STYLE = ENN_BITS(0),
	SHADOW_STYLE = ENN_BITS(1),
	OUTLINE_SYTLE = ENN_BITS(2),
	ALIGN_LEFT_STYLE = ENN_BITS(3),
	ALIGN_RIGHT_STYLE = ENN_BITS(4),
	ALIGN_TOP_STYLE = ENN_BITS(5),
	ALIGN_BOTTOM_STYLE = ENN_BITS(6),
	ALIGN_RT_STYLE = ENN_BITS(7)
};

//////////////////////////////////////////////////////////////////////////
struct FontRecordDesc : public AllocatedObject
{
	// main desc
	uint32				style;
	uint32				font_size;
	String				font_name;
	Color				text_clr;
	Color				second_text_clr;
	float				font_thickness;
	float				font_line_bias;

	// shadow style offset
	nv::vec2i			shadow_offset;

	bool				is_dynamic;

	FontRecordDesc()
	{
		style = NORMAL_STYLE | ALIGN_RIGHT_STYLE;
		font_size = 12;
		text_clr = Color::BLACK;
		second_text_clr = Color::BLACK;
		font_thickness = 0.0f;
		shadow_offset = nv::vec2i(1, 1);
		font_line_bias = 0.0f;
		is_dynamic = false;
	}

	~FontRecordDesc()
	{

	}
};

//////////////////////////////////////////////////////////////////////////
struct TextLineMetrics : public AllocatedObject
{
	int				width;
	int				max_height;
	int				max_offset_y;
	int				max_offset_minus_y;
	int				font_height;

	TextLineMetrics()
	{
		width = 0;
		max_height = 0;
		max_offset_y = 0;
		font_height = 0;
		max_offset_minus_y = 0;
	}
};

//////////////////////////////////////////////////////////////////////////
struct TextStringMetrics : public AllocatedObject
{
	nv::vec2i		draw_rect;
};

//////////////////////////////////////////////////////////////////////////
class FTFontBase : public AllocatedObject
{
public:
	virtual ~FTFontBase() {}

	virtual void render() = 0;
	virtual void apply() = 0;
	virtual bool testVisibility() const = 0;
};

}