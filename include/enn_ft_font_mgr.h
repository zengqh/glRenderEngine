/**
* Copyright (c) 2013 Pateo Software Ltd
*
* This source file is part of Pateo gis engine.
* For latest info, see http://www.pateo.com.cn/
*
* Time: 2013/09/06
* File: enn_ft_font_mgr.h
**/
 
#pragma once

#include "enn_ft_texture_font_mgr.h"

namespace enn
{

class Root;
class FTFontManager : public AllocatedObject, public Noncopyable
{
	typedef enn::map<String, String>::type			FamilyFontMap;
	typedef enn::set<FTTextureFont*>::type			TextureFontSet;

public:
	FTFontManager(Root* owner);
	virtual ~FTFontManager();

public:
	bool initalize(int w, int h);

	void finalize();

	FTTextureFont* getFontByName(const String& name, int size);

public:
	void registerFamilyFont(const String& name, const String& ttf_name);
	void registerDefaultFamilyFont();

public:

protected:
	FTTextureFontMgr*			ft_mgr_;
	FamilyFontMap				family_font_map_;
	TextureFontSet				tex_font_set_;
	int							buffer_w_, buffer_h_;
	Root*						owner_;
	String						default_family_name_;
};

}