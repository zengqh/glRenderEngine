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
// Time: 2013/09/09
// File: enn_ft_font_record.h
//

#pragma once

#include "enn_ft_font_base.h"
#include "enn_gl_vb.h"
#include "enn_gl_ib.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
struct FontMeshRecord : public AllocatedObject
{
	VertexBuffer*		vb_;
	IndexBuffer*		ib_;

	int					vb_count_;
	int					ib_count_;
	int					vb_cap_count_;
	int					ib_cap_count_;

	uchar*				vb_temp_buff_;
	uchar*				ib_temp_buff_;

	FontMeshRecord() : vb_(0), ib_(0), vb_count_(0), ib_count_(0)
		, vb_temp_buff_(0)
		, ib_temp_buff_(0)
		, vb_cap_count_(0)
		, ib_cap_count_(0)
	{

	}
};

//////////////////////////////////////////////////////////////////////////
// screen coordinate font record system
class FTFontManager;
class Root;
class FTFontRecord : public FTFontBase
{
public:
	typedef enn::map<FTTextureAtlas*, FontMeshRecord*>::type	FontMeshRecordMap;
	typedef enn::vector<WString>::type				FontStrList;
	typedef enn::vector<TextLineMetrics>::type		FontLineMetricsList;

	struct vertex_t
	{
		float x, y, z;
		Color clr;
		float u, v;
	};

	enum
	{
		DIRTY,
	};

	enum { FONT_ABT_WORLD = L'H' };

public:
	FTFontRecord();
	virtual ~FTFontRecord();

protected:
	virtual void destroy();

public:
	virtual void render();
	virtual void apply();
	virtual void dirty();

public:
	virtual void setFontDesc(const FontRecordDesc& font_desc);
	virtual const FontRecordDesc& getFontDesc() const;
	virtual FontRecordDesc& getFontDesc();

	virtual void setText(const WString& text);
	virtual const WString& getText() const;

	virtual void setViewport(const IntRect& vp);
	virtual const IntRect& getViewport() const;

	virtual void setPosition(const nv::vec2i& pos);
	virtual const nv::vec2i& getPosition() const;

	virtual void setScale(const vec3f& sca);
	virtual const vec3f& getScale() const;

	virtual bool testVisibility() const;

	virtual bool isDynamic() const;
	virtual bool isDirty() const;

	virtual IntRectangle& getDrawRect();

protected:
	virtual bool isEnableStyle(uint32 style) const;

protected:
	virtual void applyNormalStyle();
	virtual void applyShadowStyle();
	virtual void applyOutlineStyle();

protected:
	virtual TextLineMetrics getLineMetrics(const WString& str);
	virtual TextStringMetrics getRectMetrics(const WString& str);
	virtual void checkTextMetrics();
	virtual void checkRectMetrics(const WString& str);

	virtual void parseTextIntoList(const WString& str, FontStrList& str_list);

protected:
	virtual void ft_add_line(const WString& str, vec2f pen, enn::Color& clr);
	virtual void create_font_handle();

protected:
	virtual IntRect getCullRect() const;
	virtual vec2f getStyleTrans() const;

protected:
	virtual void push_vb(FTTextureAtlas* atlas, vertex_t* vb, int vb_cnt);
	virtual void push_ib(FTTextureAtlas* atlas, short* ib, int ib_cnt);
	virtual FontMeshRecord* getMeshRecord(FTTextureAtlas* atlas);
	virtual void push_vi_done();
	virtual void updateFontMeshRec(FontMeshRecord* mesh_rec);
	virtual void clearFontMesh();

protected:
	FontRecordDesc						font_desc_;
	IntRect								vp_;
	WString								text_;
	FTTextureFont*						font_handle_;
	FTFontManager*						font_mgr_;
	Root*								root_;
	
	FontMeshRecordMap					mesh_record_list_;

	TextStringMetrics					text_rect_;
	FontLineMetricsList					str_metrics_list_;

	nv::vec2i							position_;
	vec3f								scale_;

	Flags32								font_rec_flag_;

	int									font_abt_w_;
	int									font_abt_h_;

	bool								is_draw_rect_valid_;
	IntRectangle						draw_rect_;
};

}