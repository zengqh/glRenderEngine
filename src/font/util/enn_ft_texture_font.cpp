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
// File: enn_ft_texture_font.cpp
//

#include "enn_ft_texture_font.h"
#include "enn_ft_texture_font_mgr.h"

namespace enn
{

#define OUTLINE_THICK_PACK_FACTOR			(100)

FTTextureFont::FTTextureFont(FTTextureFontMgr* owner)
: font_size_(0)
, font_height_(0)
, linegap_(0)
, ascender_(0)
, descender_(0)
, outline_thickness_(0)
, owner_(owner)
, outline_type_(0)
{

}

FTTextureFont::~FTTextureFont()
{
	destroy();
}

bool FTTextureFont::initalize(const String& name, int size)
{
	FT_Library library = owner_->ft_library_;

	if (!load_face(&library, name.c_str(), (float)size, &face_))
	{
		ENN_ASSERT(0);
		return false;
	}

	const FT_Size_Metrics& metrics = face_->size->metrics; 
	ascender_ = (metrics.ascender >> 6) / 100.0f;
	descender_ = (metrics.descender >> 6) / 100.0f;
	font_height_ = (metrics.height >> 6) / 100.0f;
	linegap_ = font_height_ - ascender_ + descender_;

	return true;
}

bool FTTextureFont::load_glyphs(const wchar_t* charcodes)
{
	size_t							x, y, width, height, w, h;
	FT_Library						library = owner_->ft_library_;
	FT_Error						error;
	FT_Face							face = face_;
	FT_Glyph						ft_glyph;
	FT_GlyphSlot					slot;
	FT_Bitmap						ft_bitmap;
	FT_UInt							glyph_index;
	nv::vec4i						region;
	size_t							missed = 0;

	for (size_t i = 0; i < wcslen(charcodes); ++i)
	{
		FT_Int32	flags = 0;
		int 		ft_bitmap_width = 0;
		int 		ft_bitmap_rows = 0;
		int 		ft_bitmap_pitch = 0;
		int 		ft_glyph_top = 0;
		int 		ft_glyph_left = 0;

		glyph_index = FT_Get_Char_Index(face, charcodes[i]);

		flags |= FT_LOAD_RENDER;
		flags |= FT_LOAD_FORCE_AUTOHINT;

		// error than continue
		error = FT_Load_Glyph(face, glyph_index, flags);
		if (error)
		{
			ENN_ASSERT(0);
			return false;
		}

		if (outline_type_ == 0)
		{
			slot            = face->glyph;
			ft_bitmap       = slot->bitmap;
			ft_bitmap_width = slot->bitmap.width;
			ft_bitmap_rows  = slot->bitmap.rows;
			ft_bitmap_pitch = slot->bitmap.pitch;
			ft_glyph_top    = slot->bitmap_top;
			ft_glyph_left   = slot->bitmap_left;
		}
		else
		{
			FT_Stroker stroker;
			FT_BitmapGlyph ft_bitmap_glyph;
			error = FT_Stroker_New( library, &stroker );

			if (error)
			{
				FT_Stroker_Done( stroker );

				ENN_ASSERT(0);
				return false;
			}

			FT_Stroker_Set( stroker,
				(int)(outline_thickness_ * 64),
				FT_STROKER_LINECAP_ROUND,
				FT_STROKER_LINEJOIN_ROUND,
				0);

			error = FT_Get_Glyph( face->glyph, &ft_glyph);
			if( error )
			{
				FT_Stroker_Done( stroker );

				ENN_ASSERT(0);
				return false;
			}

			if (outline_type_ == 1)
			{
				error = FT_Glyph_Stroke( &ft_glyph, stroker, 1 );
			}
			else if (outline_type_ == 2)
			{
				error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 0, 1 );
			}
			else if (outline_type_ == 3)
			{
				error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 1, 1 );
			}

			if( error )
			{
				FT_Stroker_Done( stroker );

				ENN_ASSERT(0);
				return false;
			}

			error = FT_Glyph_To_Bitmap( &ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
			if( error )
			{
				FT_Stroker_Done( stroker );

				ENN_ASSERT(0);
				return false;
			}

			ft_bitmap_glyph = (FT_BitmapGlyph) ft_glyph;
			ft_bitmap       = ft_bitmap_glyph->bitmap;
			ft_bitmap_width = ft_bitmap.width;
			ft_bitmap_rows  = ft_bitmap.rows;
			ft_bitmap_pitch = ft_bitmap.pitch;
			ft_glyph_top    = ft_bitmap_glyph->top;
			ft_glyph_left   = ft_bitmap_glyph->left;
			FT_Stroker_Done(stroker);
		}

		// We want each glyph to be separated by at least one black pixel
		// (for example for shader used in demo-subpixel.c)
		w = ft_bitmap_width + 1;
		h = ft_bitmap_rows + 1;

		nv::vec4i region;
		FTTextureAtlas* atlas = 0;
		owner_->get_glyph_region(region, atlas, w, h);

		ENN_ASSERT(atlas);

		w = w - 1;
		h = h - 1;
		x = region.x;
		y = region.y;
		width = atlas->w_;
		height = atlas->h_;

		atlas->setRegion(x, y, w, h, ft_bitmap.buffer, ft_bitmap.pitch);

		FTTextureGlyph* glyph = ENN_NEW FTTextureGlyph(this);
		glyph->charcode_ = charcodes[i];
		glyph->width_    = w;
		glyph->height_   = h;
		glyph->outline_thickness_ = outline_thickness_;
		glyph->offset_x_ = ft_glyph_left;
		glyph->offset_y_ = ft_glyph_top;
		glyph->s0_       = x/(float)width;
		glyph->t0_       = y/(float)height;
		glyph->s1_       = (x + glyph->width_)/(float)width;
		glyph->t1_       = (y + glyph->height_)/(float)height;
		glyph->belong_atlas_ = atlas;

		// Discard hinting to get advance
		FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
		slot = face->glyph;
		glyph->advance_x_ = slot->advance.x / 64.0f;
		glyph->advance_y_ = slot->advance.y / 64.0f;

		if (outline_type_ > 0) FT_Done_Glyph( ft_glyph );

		GlyphKey key;
		key = packGlyphKey(outline_type_, outline_thickness_, charcodes[i]);
		glyph_map_.insert(FTTextureGlyphMap::value_type(key, glyph));
	}

	return true;
}

bool FTTextureFont::load_glyph(wchar_t charcode, FTTextureGlyph*& glyph_out)
{
	size_t							x, y, width, height, w, h;
	FT_Library						library = owner_->ft_library_;
	FT_Error						error;
	FT_Glyph						ft_glyph;
	FT_GlyphSlot					slot;
	FT_Bitmap						ft_bitmap;
	FT_UInt							glyph_index;
	nv::vec4i						region;
	size_t							missed = 0;

	FT_Int32						flags = 0;
	int 							ft_bitmap_width = 0;
	int 							ft_bitmap_rows = 0;
	int 							ft_bitmap_pitch = 0;
	int 							ft_glyph_top = 0;
	int 							ft_glyph_left = 0;

	glyph_out = 0;

	glyph_index = FT_Get_Char_Index(face_, charcode);

	flags |= FT_LOAD_RENDER;
	flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;;

	// error than continue
	error = FT_Load_Glyph(face_, glyph_index, flags);
	if (error)
	{
		ENN_ASSERT(0);
		return false;
	}

	if (outline_type_ == 0)
	{
		slot            = face_->glyph;
		ft_bitmap       = slot->bitmap;
		ft_bitmap_width = slot->bitmap.width;
		ft_bitmap_rows  = slot->bitmap.rows;
		ft_bitmap_pitch = slot->bitmap.pitch;
		ft_glyph_top    = slot->bitmap_top;
		ft_glyph_left   = slot->bitmap_left;
	}
	else
	{
		FT_Stroker stroker;
		FT_BitmapGlyph ft_bitmap_glyph;
		error = FT_Stroker_New( library, &stroker );

		if (error)
		{
			FT_Stroker_Done( stroker );

			ENN_ASSERT(0);
			return false;
		}

		FT_Stroker_Set( stroker,
			(int)(outline_thickness_ * 64),
			FT_STROKER_LINECAP_ROUND,
			FT_STROKER_LINEJOIN_ROUND,
			0);

		error = FT_Get_Glyph( face_->glyph, &ft_glyph);
		if( error )
		{
			FT_Stroker_Done( stroker );

			ENN_ASSERT(0);
			return false;
		}

		if (outline_type_ == 1)
		{
			error = FT_Glyph_Stroke( &ft_glyph, stroker, 1 );
		}
		else if (outline_type_ == 2)
		{
			error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 0, 1 );
		}
		else if (outline_type_ == 3)
		{
			error = FT_Glyph_StrokeBorder( &ft_glyph, stroker, 1, 1 );
		}

		if( error )
		{
			FT_Stroker_Done( stroker );

			ENN_ASSERT(0);
			return false;
		}

		error = FT_Glyph_To_Bitmap( &ft_glyph, FT_RENDER_MODE_NORMAL, 0, 1);
		if( error )
		{
			FT_Stroker_Done( stroker );

			ENN_ASSERT(0);
			return false;
		}

		ft_bitmap_glyph = (FT_BitmapGlyph) ft_glyph;
		ft_bitmap       = ft_bitmap_glyph->bitmap;
		ft_bitmap_width = ft_bitmap.width;
		ft_bitmap_rows  = ft_bitmap.rows;
		ft_bitmap_pitch = ft_bitmap.pitch;
		ft_glyph_top    = ft_bitmap_glyph->top;
		ft_glyph_left   = ft_bitmap_glyph->left;
		FT_Stroker_Done(stroker);
	}

	// We want each glyph to be separated by at least one black pixel
	// (for example for shader used in demo-subpixel.c)
	w = ft_bitmap_width + 1;
	h = ft_bitmap_rows + 1;

	FTTextureAtlas* atlas = 0;
	owner_->get_glyph_region(region, atlas, w, h);

	ENN_ASSERT(atlas);

	w = w - 1;
	h = h - 1;
	x = region.x;
	y = region.y;
	width = atlas->w_;
	height = atlas->h_;

	atlas->setRegion(x, y, w, h, ft_bitmap.buffer, ft_bitmap.pitch);

	FTTextureGlyph* glyph = ENN_NEW FTTextureGlyph(this);
	glyph->charcode_ = charcode;
	glyph->width_    = w;
	glyph->height_   = h;
	glyph->outline_thickness_ = outline_thickness_;
	glyph->offset_x_ = ft_glyph_left;
	glyph->offset_y_ = ft_glyph_top;
	glyph->s0_       = x/(float)width;
	glyph->t0_       = y/(float)height;
	glyph->s1_       = (x + glyph->width_)/(float)width;
	glyph->t1_       = (y + glyph->height_)/(float)height;
	glyph->belong_atlas_ = atlas;

	// Discard hinting to get advance
	FT_Load_Glyph( face_, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
	slot = face_->glyph;
	glyph->advance_x_ = slot->advance.x / 64.0f;
	glyph->advance_y_ = slot->advance.y / 64.0f;

	if (outline_type_ > 0) FT_Done_Glyph( ft_glyph );

	GlyphKey key;
	key = packGlyphKey(outline_type_, outline_thickness_, charcode);
	glyph_map_.insert(FTTextureGlyphMap::value_type(key, glyph));

	glyph_out = glyph;

	return true;
}

FTTextureGlyph* FTTextureFont::getGlyph(wchar_t charcode)
{
	GlyphKey key = packGlyphKey(outline_type_, outline_thickness_, charcode);

	FTTextureGlyphMap::iterator it = glyph_map_.find(key);

	if (it != glyph_map_.end())
	{
		return it->second;
	}

	FTTextureGlyph* glyph_out = 0;

	if (load_glyph(charcode, glyph_out))
	{
		return glyph_out;
	}

	return 0;
}

bool FTTextureFont::load_face(FT_Library * library, 
	const char * filename,
	const float size,
	FT_Face * face)
{
	size_t hres = 64;
	FT_Error error;
	FT_Matrix matrix = { (int)((1.0/hres) * 0x10000L),
		(int)((0.0)      * 0x10000L),
		(int)((0.0)      * 0x10000L),
		(int)((1.0)      * 0x10000L) };

	ENN_ASSERT( library );
	ENN_ASSERT( filename );

	error = FT_New_Face( *library, filename, 0, face );
	if (error)
	{
		ENN_ASSERT(0);
		return false;
	}

	// Select charmap
	error = FT_Select_Charmap( *face, FT_ENCODING_UNICODE );
	if( error )
	{
		ENN_ASSERT(0);
		return false;
	}

	// Set char size
	error = FT_Set_Char_Size( *face, (int)(size*64), 0, 72*hres, 72 );
	if (error)
	{
		ENN_ASSERT(0);
		return false;
	}

	FT_Set_Transform( *face, &matrix, 0 );

	return true;
}

void FTTextureFont::destroy()
{
	ENN_FOR_EACH(FTTextureGlyphMap, glyph_map_, it)
	{
		FTTextureGlyph* tex_glyph = it->second;
		ENN_SAFE_DELETE(tex_glyph);
	}

	glyph_map_.clear();

	FT_Done_Face(face_);
}

bool FTTextureFont::insert_glphys(FTTextureGlyph* tex_glyph)
{
	if (!tex_glyph)
	{
		return false;
	}

	float thickness = tex_glyph->outline_thickness_;
	ushort charcode = tex_glyph->charcode_;

	GlyphKey key = packGlyphKey(outline_type_, thickness, charcode);

	ENN_ASSERT(glyph_map_.find(key) == glyph_map_.end());

	glyph_map_[key] = tex_glyph;

	return true;
}

ushort FTTextureFont::packOutlineThickness(float thickness)
{
	return static_cast<ushort>(thickness * OUTLINE_THICK_PACK_FACTOR);
}

float FTTextureFont::unpackOutlineThickness(ushort thickness)
{
	return thickness / (float)OUTLINE_THICK_PACK_FACTOR;
}

FTTextureFont::GlyphKey FTTextureFont::packGlyphKey(int outline_type, float thickness, wchar_t charcode)
{
	ushort pack_thick = packOutlineThickness(thickness);
	GlyphKey key = (charcode | (outline_type << 16) | (pack_thick << 18));

	return key;
}

void FTTextureFont::unpackGlyphKey(GlyphKey key, int& outline_type, float& thickness, wchar_t& charcode)
{
	charcode = (key & 0xffff);
	ushort pack_thick = ((key >> 18) & 0xffff);
	outline_type = ((key >> 16) & 0x3);

	thickness = unpackOutlineThickness(pack_thick);
}

}