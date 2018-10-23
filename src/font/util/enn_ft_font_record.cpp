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
// File: enn_ft_font_record.cpp
//

#include "enn_ft_font_record.h"
#include "enn_root.h"

namespace enn
{

FTFontRecord::FTFontRecord()
: font_handle_(0)
, font_abt_w_(-1)
, font_abt_h_(-1)
, is_draw_rect_valid_(false)
, scale_(vec3f::UNIT_SCALE)
{
	root_ = ROOTPTR;
	font_mgr_ = root_->getFTFontMgr();

	font_rec_flag_.set_flag(DIRTY);
}

FTFontRecord::~FTFontRecord()
{
	destroy();
}

void FTFontRecord::destroy()
{
	ENN_FOR_EACH(FontMeshRecordMap, mesh_record_list_, it)
	{
		FontMeshRecord* mesh_rec = it->second;

		ENN_SAFE_DELETE_ARRAY_T(mesh_rec->vb_temp_buff_, mesh_rec->vb_cap_count_);
		ENN_SAFE_DELETE_ARRAY_T(mesh_rec->ib_temp_buff_, mesh_rec->ib_cap_count_);

		ENN_SAFE_RELEASE(mesh_rec->vb_);
		ENN_SAFE_RELEASE(mesh_rec->ib_);

		ENN_SAFE_DELETE(mesh_rec);
	}

	mesh_record_list_.clear();
}

void FTFontRecord::render()
{
	// do some check first
	if (text_.empty())
	{
		return;
	}

	// need apply before rendering
	if (font_rec_flag_.test_flag(DIRTY))
	{
		return;
	}

	RenderSystem* rs = ROOTPTR->get_render_system();

	Program* eff = 0; //root_->get_render_pipe()->getEffManager()->getFontEffect();
	if (!eff)
	{
		ENN_ASSERT(0);
		return;
	}

	rs->setProgram(eff);

	nv::matrix4f mat_proj, mat_world, matWVP;
	vec2f trans;

	trans = getStyleTrans();
	trans.x += position_.x;
	trans.y += position_.y;

	nv::make_ortho<float>(mat_proj, (float)vp_.left, (float)vp_.right, (float)vp_.bottom, (float)vp_.top, -1, 1);
	mat_world.set_translate(nv::vec3f(trans.x, trans.y, 0));
	mat_world.set_scale(nv::vec3f(scale_.x, scale_.y, 1.0f));

	matWVP = mat_proj * mat_world;
	
	eff->setMatrix4("matWVP", matWVP._array);
	eff->setInt("texDiff", 0);

	ENN_FOR_EACH(FontMeshRecordMap, mesh_record_list_, it)
	{
		FTTextureAtlas* tex = it->first;
		FontMeshRecord* mesh = it->second;

		if (mesh->vb_count_ <= 0 || mesh->ib_count_ <= 0)
		{
			continue;
		}

		// bind texture
		if (!tex->bindTexture())
		{
			continue;
		}

		VertexBuffer* vb = mesh->vb_;
		IndexBuffer* ib = mesh->ib_;

		rs->setVB(vb);
		rs->setIB(ib);

		rs->setFVF(vb->getElementMask(), true);

		rs->drawIndexedPrimitives(PRIM_TRIANGLELIST, 0, mesh->ib_count_);

		rs->setFVF(vb->getElementMask(), false);
	}
	
	checkGLError();
}

void FTFontRecord::apply()
{
	if (!font_rec_flag_.test_flag(DIRTY))
	{
		return;
	}

	create_font_handle();

	if (isEnableStyle(SHADOW_STYLE))
	{
		
	}
	else if (isEnableStyle(OUTLINE_SYTLE))
	{

	}
	else
	{
		applyNormalStyle();
	}

	font_rec_flag_.reset_flag(DIRTY);
}

void FTFontRecord::dirty()
{
	// could not change static text
	if (!isDynamic() && !isDirty())
	{
		ENN_ASSERT(0);
		return;
	}

	clearFontMesh();

	font_rec_flag_.set_flag(DIRTY);
}

void FTFontRecord::setFontDesc(const FontRecordDesc& font_desc)
{
	font_desc_ = font_desc;
}

const FontRecordDesc& FTFontRecord::getFontDesc() const
{
	return font_desc_;
}

FontRecordDesc& FTFontRecord::getFontDesc()
{
	return font_desc_;
}

void FTFontRecord::setText(const WString& text)
{
	text_ = text;
}

const WString& FTFontRecord::getText() const
{
	return text_;
}

void FTFontRecord::setViewport(const IntRect& vp)
{
	vp_ = vp;
}

const IntRect& FTFontRecord::getViewport() const
{
	return vp_;
}

void FTFontRecord::setPosition(const nv::vec2i& pos)
{
	position_ = pos;
}

const nv::vec2i& FTFontRecord::getPosition() const
{
	return position_;
}

void FTFontRecord::setScale(const vec3f& sca)
{
	scale_ = sca;
}

const vec3f& FTFontRecord::getScale() const
{
	return scale_;
}

bool FTFontRecord::testVisibility() const
{
	// check the rect is intersected with vp
	IntRectangle rect1, rect2;

	initFromIntRect(rect1, vp_);

	IntRect cull_rect = getCullRect();
	initFromIntRect(rect2, cull_rect);

	return rect1.intersects(rect2);
}

bool FTFontRecord::isDynamic() const
{
	return font_desc_.is_dynamic;
}

bool FTFontRecord::isDirty() const
{
	return font_rec_flag_.test_flag(DIRTY);
}

IntRectangle& FTFontRecord::getDrawRect()
{
	if (is_draw_rect_valid_)
	{
		return draw_rect_;
	}

	create_font_handle();

	if (font_abt_w_ == -1 || font_abt_h_ == -1)
	{
		FTTextureGlyph* glyph = font_handle_->getGlyph(FONT_ABT_WORLD);

		font_abt_w_ = (int)glyph->advance_x_;
		font_abt_h_ = glyph->height_;
	}

	FontStrList str_list;

	parseTextIntoList(text_, str_list);

	size_t w = 0, h = 0;
	ENN_FOR_EACH(FontStrList, str_list, it)
	{
		size_t s = it->size();
		size_t ws = s * font_abt_w_;
		if (ws > w)
		{
			w = ws;
		}
	}

	h = font_abt_h_ * str_list.size();

	draw_rect_.set(0, 0, w, h);

	is_draw_rect_valid_ = true;

	return draw_rect_;
}

bool FTFontRecord::isEnableStyle(uint32 style) const
{
	return ((font_desc_.style & style) ? true : false);
}

void FTFontRecord::applyNormalStyle()
{
	checkTextMetrics();

	FontStrList str_list;

	parseTextIntoList(text_, str_list);

	if (str_list.empty())
	{
		return;
	}

	vec2f curr;

	for (int i = 0; i < (int)str_list.size(); ++i)
	{
		WString& str_line = str_list[i];

		if (str_line.empty())
		{
			ENN_ASSERT(0);
			continue;
		}

		Color clr;
		clr.r = font_desc_.text_clr.r;
		clr.g = font_desc_.text_clr.g;
		clr.b = font_desc_.text_clr.b;
		clr.a = 1.0f;

		// check out thickness
		font_handle_->outline_thickness_ = font_desc_.font_thickness;

		vec2f pen;

		TextLineMetrics& line_metrics = str_metrics_list_[i];

		// if it's first line
		// init curr position.
		if (i == 0)
		{
			nv::vec2i& draw_rect = text_rect_.draw_rect;

			float half_y = draw_rect.y / 2.0f;

			curr.x = 0;
			curr.y = -half_y;
		}

		pen.x = curr.x;
		pen.y = curr.y + line_metrics.max_offset_y;

		ft_add_line(str_line, pen, clr);

		curr.y += int(line_metrics.font_height + font_desc_.font_line_bias);
	}

	push_vi_done();
}

void FTFontRecord::applyShadowStyle()
{

}

void FTFontRecord::applyOutlineStyle()
{

}

TextLineMetrics FTFontRecord::getLineMetrics(const WString& str)
{
	TextLineMetrics metrics;

	if (str.empty())
	{
		ENN_ASSERT(0);
		return metrics;
	}

	ENN_ASSERT(font_handle_);

	int x_metrics = 0;
	int max_y_offset = 0;
	int max_height = 0;
	int max_y_minus_offset = 0;

	for (int i = 0; i < (int)str.size(); ++i)
	{
		FTTextureGlyph *glyph = font_handle_->getGlyph(str[i]);

		if (!glyph)
		{
			ENN_ASSERT(0);
			continue;
		}

		x_metrics += glyph->offset_x_ + glyph->width_;

		// y offset
		if (max_y_offset < glyph->offset_y_)
		{
			max_y_offset = glyph->offset_y_;
		}

		// max height
		if (max_height < (int)glyph->height_)
		{
			max_height = glyph->height_;
		}

		if (max_y_minus_offset < (int)(glyph->height_ - glyph->offset_y_))
		{
			max_y_minus_offset = glyph->height_ - glyph->offset_y_;
		}
	}

	metrics.max_height = max_height;
	metrics.max_offset_y = max_y_offset;
	metrics.max_offset_minus_y = max_y_minus_offset;
	metrics.width = x_metrics;
	metrics.font_height = metrics.max_offset_y + metrics.max_offset_minus_y;

	return metrics;
}

TextStringMetrics FTFontRecord::getRectMetrics(const WString& str)
{
	TextStringMetrics metrics;

	FontStrList str_list;
	
	parseTextIntoList(str, str_list);

	int x_max = 0;
	int y_max = 0;

	for (int i = 0; i < (int)str_list.size(); ++i)
	{
		TextLineMetrics line_metrics = getLineMetrics(str_list[i]);

		if (x_max < line_metrics.width)
		{
			x_max = line_metrics.width;
		}

		// last line
		if (i == str_list.size() - 1)
		{
			y_max += line_metrics.font_height;
		}
		else
		{
			y_max += int(font_desc_.font_line_bias + line_metrics.font_height);
		}
	}

	metrics.draw_rect.x = x_max;
	metrics.draw_rect.y = y_max;

	return metrics;
}

void FTFontRecord::checkTextMetrics()
{
	checkRectMetrics(text_);
}

void FTFontRecord::checkRectMetrics(const WString& str)
{
	TextStringMetrics metrics;

	FontStrList str_list;

	parseTextIntoList(str, str_list);

	int x_max = 0;
	int y_max = 0;

	for (int i = 0; i < (int)str_list.size(); ++i)
	{
		TextLineMetrics line_metrics = getLineMetrics(str_list[i]);

		if (x_max < line_metrics.width)
		{
			x_max = line_metrics.width;
		}

		// last line
		if (i == str_list.size() - 1)
		{
			y_max += line_metrics.font_height;
		}
		else
		{
			y_max += int(font_desc_.font_line_bias + line_metrics.font_height);
		}

		str_metrics_list_.push_back(line_metrics);
	}

	text_rect_.draw_rect.x = x_max;
	text_rect_.draw_rect.y = y_max;
}

void FTFontRecord::parseTextIntoList(const WString& str, FontStrList& str_list)
{
	if (str.empty())
	{
		ENN_ASSERT(0);
		return;
	}

	WString temp_str;
	for (int i = 0; i < (int)str.size(); ++i)
	{
		// \n or \r
		if (str[i] == 13 || str[i] == 10)
		{
			if (!temp_str.empty())
			{
				str_list.push_back(temp_str);
				temp_str.clear();
			}

			continue;
		}
		else
		{
			temp_str.push_back(str[i]);
		}

		// push the last word
		if ((i == str.size() - 1) && (!temp_str.empty()))
		{
			str_list.push_back(temp_str);
			temp_str.clear();
		}
	}

	if (str_list.empty())
	{
		ENN_ASSERT(0);
		return;
	}
}

void FTFontRecord::ft_add_line(const WString& str, vec2f pen, enn::Color& clr)
{
	for (size_t i = 0; i < str.size(); ++i)
	{
		FTTextureGlyph *glyph = font_handle_->getGlyph(str[i]);
	
		if (!glyph)
		{
			continue;
		}

		// Actual glyph
		float x0  = (float)(pen.x + glyph->offset_x_ );
		float y0  = (float)(pen.y - glyph->offset_y_ );
		float x1  = (float)( x0 + glyph->width_ );
		float y1  = (float)( y0 + glyph->height_ );

		float s0 = glyph->s0_;
		float t0 = glyph->t0_;
		float s1 = glyph->s1_;
		float t1 = glyph->t1_;

		FTTextureAtlas* atlas = glyph->belong_atlas_;
		FontMeshRecord* curr_rec = getMeshRecord(atlas);

		short index = curr_rec->vb_count_;

		short indices[] = 
		{
			index, index+1, index+2,
			index, index+2, index+3
		};

		vertex_t vertices[] = 
		{
			{ x0,y0,0,  clr, s0,t0 },
			{ x0,y1,0,  clr, s0,t1 },
			{ x1,y1,0,  clr, s1,t1 },
			{ x1,y0,0,  clr, s1,t0 } 
		};


		push_vb(atlas, vertices, 4);
		push_ib(atlas, indices, 6);
		
		pen.x += int(glyph->advance_x_);
	}
}

void FTFontRecord::create_font_handle()
{
	if (!font_handle_)
	{
		font_handle_ = font_mgr_->getFontByName(font_desc_.font_name, font_desc_.font_size);
	}

	ENN_ASSERT(font_handle_);
}


IntRect FTFontRecord::getCullRect() const
{
	IntRect rect;

	int half_x, half_y;

	const nv::vec2i& draw_rect = text_rect_.draw_rect;

	half_x = draw_rect.x / 2;
	half_y = draw_rect.y / 2;

	rect.make(0, 0, draw_rect.x, draw_rect.y);

	if (font_desc_.style & ALIGN_LEFT_STYLE)
	{
		int l = -draw_rect.x;
		int t = -half_y;

		rect.make(l, t, draw_rect.x, draw_rect.y);
	}
	else if (font_desc_.style & ALIGN_RIGHT_STYLE)
	{
		int t = -half_y;
		rect.make(0, t, draw_rect.x, draw_rect.y);
	}
	else if (font_desc_.style & ALIGN_TOP_STYLE)
	{
		int l = -half_x;
		int t = -draw_rect.y;

		rect.make(l, t, draw_rect.x, draw_rect.y);
	}
	else if (font_desc_.style & ALIGN_BOTTOM_STYLE)
	{
		int l = -half_x;
		int t = 0;

		rect.make(l, t, draw_rect.x, draw_rect.y);
	}
	else if (font_desc_.style & ALIGN_RT_STYLE)
	{
		
	}

	int l = rect.left;
	int t = rect.top;

	l = l + position_.x;
	t = t + position_.y;

	rect.make(l, t, draw_rect.x, draw_rect.y);

	return rect;
}

vec2f FTFontRecord::getStyleTrans() const
{
	vec2f trans(0, 0);
	const nv::vec2i& draw_rect = text_rect_.draw_rect;

	if (isEnableStyle(ALIGN_LEFT_STYLE))
	{
		trans.x = float(-draw_rect.x);
		trans.y = 0;
	}
	else if (isEnableStyle(ALIGN_BOTTOM_STYLE))
	{
		trans.x = -draw_rect.x * 0.5f;
		trans.y = draw_rect.y * 0.5f;
	}
	else if (isEnableStyle(ALIGN_TOP_STYLE))
	{
		trans.x = -draw_rect.x * 0.5f;
		trans.y = -draw_rect.y * 0.5f;
	}
	else if (isEnableStyle(ALIGN_RT_STYLE))
	{
		trans.x = 0;
		trans.y = draw_rect.y * 0.5f;
	}

	return trans;
}

void FTFontRecord::push_vb(FTTextureAtlas* atlas, vertex_t* vb, int vb_cnt)
{
	FontMeshRecord* mesh_rec = getMeshRecord(atlas);

	ENN_ASSERT(mesh_rec);

	int req_count = mesh_rec->vb_count_ + vb_cnt;
	if (req_count > mesh_rec->vb_cap_count_)
	{
		uchar* temp_buff = (uchar*)ENN_MALLOC_ARRAY_T(vertex_t, req_count * 2);

		if (mesh_rec->vb_count_)
		{
			memcpy(temp_buff, mesh_rec->vb_temp_buff_, sizeof(vertex_t) * mesh_rec->vb_count_);
		}
		
		ENN_SAFE_FREE(mesh_rec->vb_temp_buff_);

		mesh_rec->vb_temp_buff_ = temp_buff;
		mesh_rec->vb_cap_count_ = 2 * req_count;
	}

	int s = sizeof(vertex_t);

	uchar* start_pos = mesh_rec->vb_temp_buff_ + sizeof(vertex_t) * mesh_rec->vb_count_;
	memcpy(start_pos, vb, vb_cnt * sizeof(vertex_t));

	mesh_rec->vb_count_ += vb_cnt;
}

void FTFontRecord::push_ib(FTTextureAtlas* atlas, short* ib, int ib_cnt)
{
	FontMeshRecord* mesh_rec = getMeshRecord(atlas);

	ENN_ASSERT(mesh_rec);

	int req_count = mesh_rec->ib_count_ + ib_cnt;
	if (req_count > mesh_rec->ib_cap_count_)
	{
		uchar* temp_buff = (uchar*)ENN_MALLOC_ARRAY_T(short, req_count * 2);

		if (mesh_rec->ib_count_)
		{
			memcpy(temp_buff, mesh_rec->ib_temp_buff_, sizeof(short) * mesh_rec->ib_count_);
		}

		ENN_SAFE_FREE(mesh_rec->ib_temp_buff_);

		mesh_rec->ib_temp_buff_ = temp_buff;
		mesh_rec->ib_cap_count_ = 2 * req_count;
	}

	uchar* start_pos = mesh_rec->ib_temp_buff_ + sizeof(short) * mesh_rec->ib_count_;
	memcpy(start_pos, ib, ib_cnt * sizeof(short));

	mesh_rec->ib_count_ += ib_cnt;
}

FontMeshRecord* FTFontRecord::getMeshRecord(FTTextureAtlas* atlas)
{
	ENN_ASSERT(atlas);

	FontMeshRecordMap::iterator it = mesh_record_list_.find(atlas);

	if (it != mesh_record_list_.end())
	{
		return it->second;
	}

	// create new one
	FontMeshRecord* new_rec = ENN_NEW FontMeshRecord();
	mesh_record_list_[atlas] = new_rec;

	return new_rec;
}

void FTFontRecord::push_vi_done()
{
	ENN_FOR_EACH(FontMeshRecordMap, mesh_record_list_, it)
	{
		FTTextureAtlas* atlas = it->first;
		FontMeshRecord* mesh_rec = it->second;

		updateFontMeshRec(mesh_rec);
	}
}

void FTFontRecord::updateFontMeshRec(FontMeshRecord* mesh_rec)
{
	// we never check if update again.
	if (mesh_rec->vb_count_ <= 0 || mesh_rec->ib_count_ <= 0)
	{
		return;
	}

	VertexBuffer*& vb = mesh_rec->vb_;
	IndexBuffer*& ib = mesh_rec->ib_;

	uint32 vb_size = vb ? vb->getSize() : 0;
	uint32 ib_size = ib ? ib->getSize() : 0;
	uint32 vb_req_size = mesh_rec->vb_count_ * sizeof(vertex_t);
	uint32 ib_req_size = mesh_rec->ib_count_ * sizeof(short);

	if (vb_req_size > vb_size)
	{
		ENN_SAFE_RELEASE(vb);

		vb = VertexBuffer::createInstance();
		vb->setUseMemory(isDynamic());
		vb->createBuff(vb_req_size);
		vb->setElementMask(MASK_P3U2C4);
	}

	if (ib_req_size > ib_size)
	{
		ENN_SAFE_RELEASE(ib);

		ib = IndexBuffer::createInstance();
		ib->setUseMemory(isDynamic());
		ib->createBuff(ib_req_size);
	}

	vb->setData(mesh_rec->vb_temp_buff_);
	ib->setData(mesh_rec->ib_temp_buff_);

	if (!isDynamic())
	{
		ENN_SAFE_FREE(mesh_rec->vb_temp_buff_);
		ENN_SAFE_FREE(mesh_rec->ib_temp_buff_);
		mesh_rec->vb_cap_count_ = 0;
		mesh_rec->ib_cap_count_ = 0;
	}

	// free cpu memory data
	vb->freeData();
	ib->freeData();
}

void FTFontRecord::clearFontMesh()
{
	ENN_FOR_EACH(FontMeshRecordMap, mesh_record_list_, it)
	{
		FTTextureAtlas* atlas = it->first;
		FontMeshRecord* mesh_rec = it->second;

		mesh_rec->vb_count_ = 0;
		mesh_rec->ib_count_ = 0;
	}
}

}