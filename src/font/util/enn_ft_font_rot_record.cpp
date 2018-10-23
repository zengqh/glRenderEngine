//
// Copyright (c) 2013 Pateo Software Ltd
//
// This source file is part of Pateo gis engine.
// For latest info, see http://www.pateo.com.cn/
//
// Time: 2013/10/14
// File: enn_ft_font_rot_record.cpp
//

#include "enn_ft_font_rot_record.h"
#include "enn_root.h"

namespace enn
{

FTFontRotRecord::FTFontRotRecord()
: FTFontRecord()
{

}

FTFontRotRecord::~FTFontRotRecord()
{
	destroy();
}

void FTFontRotRecord::render()
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

	Program* eff = 0; // ROOTPTR->get_render_pipe()->getEffManager()->getFontEffect();
	if (!eff)
	{
		ENN_ASSERT(0);
		return;
	}

	rs->setProgram(eff);

	// disable z test
	rs->enableZTest(false);

	// we should disable zwrite and enable blend.
	// this should be done outside.
	// but our render system have check the same render state.
	// so do it here.
	rs->enableZWrite(false);
	rs->enableBlend(true);

	nv::matrix4f mat_proj, mat_world, mat_rot, matWVP;
	vec2f trans;

	trans = getStyleTrans();
	trans.x += position_.x;
	trans.y += position_.y;

	nv::make_ortho<float>(mat_proj, (float)vp_.left, (float)vp_.right, (float)vp_.bottom, (float)vp_.top, -1, 1);
	
	// first scale, then rotate, the last translate.
	// scale
	mat_world.set_scale(nv::vec3f(scale_.x, scale_.y, 1.0f));

	// rotation
	nv::quaternionf quat(rotation_.w, rotation_.x, rotation_.y, rotation_.z);
	quat.get_value(mat_rot);
	mat_world = mat_rot * mat_world;

	// translate
	mat_world.set_translate(nv::vec3f(trans.x, trans.y, 0));

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

void FTFontRotRecord::setRotation(const Quaternion& quat)
{
	rotation_ = quat;
}

IntRectangle& FTFontRotRecord::getDrawRect()
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

}