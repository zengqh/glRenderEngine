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
// File: enn_ft_texture_font.h
//

#pragma once

#include "enn_ft_texture_atlas.h"
#include "enn_ft_texture_glyph.h"

namespace enn
{

class FTTextureFontMgr;
class FTTextureFont : public AllocatedObject
{
	typedef enn::uint32 GlyphKey;
	typedef enn::map<GlyphKey, FTTextureGlyph*>::type FTTextureGlyphMap;

public:
	FTTextureFont(FTTextureFontMgr* owner);
	virtual ~FTTextureFont();

public:
	bool initalize(const String& name, int size);

public:
	bool load_glyphs(const wchar_t* charcodes);

	bool load_glyph(wchar_t charcode, FTTextureGlyph*& glyph_out);

	FTTextureGlyph* getGlyph(wchar_t charcode);

protected:
	bool load_face(FT_Library * library, 
		const char * filename,
		const float size,
		FT_Face * face);

	void destroy();

protected:
	bool insert_glphys(FTTextureGlyph* tex_glyph);
	ushort packOutlineThickness(float thickness);
	float unpackOutlineThickness(ushort thickness);
	GlyphKey packGlyphKey(int outline_type, float thickness, wchar_t charcode);
	void unpackGlyphKey(GlyphKey key, int& outline_type, float& thickness, wchar_t& charcode);

public:
	String			font_name_;
	int				font_size_;

    // This field is simply used to compute a default line spacing (i.e., the
    // baseline-to-baseline distance) when writing text with this font. Note
    // that it usually is larger than the sum of the ascender and descender
    // taken as absolute values. There is also no guarantee that no glyphs
    // extend above or below subsequent baselines when using this distance.
    float			font_height_;

	 
     // This field is the distance that must be placed between two lines of
     // text. The baseline-to-baseline distance should be computed as:
     // ascender - descender + linegap
    float			linegap_;

	
    // The ascender is the vertical distance from the horizontal baseline to
    // the highest 'character' coordinate in a font face. Unfortunately, font
    // formats define the ascender differently. For some, it represents the
    // ascent of all capital latin characters (without accents), for others it
    // is the ascent of the highest accented character, and finally, other
    // formats define it as being equal to bbox.yMax.
    float			ascender_;

    
    // The descender is the vertical distance from the horizontal baseline to
    // the lowest 'character' coordinate in a font face. Unfortunately, font
    // formats define the descender differently. For some, it represents the
    // descent of all capital latin characters (without accents), for others it
    // is the ascent of the lowest accented character, and finally, other
    // formats define it as being equal to bbox.yMin. This field is negative
    // for values below the baseline.
    float			descender_;

    // Outline thickness
    float			outline_thickness_;

	int				outline_type_;

protected:
	FTTextureFontMgr*		owner_;
	FT_Face					face_;
	FTTextureGlyphMap		glyph_map_;
};	 

}