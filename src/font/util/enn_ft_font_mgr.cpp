/**
* Copyright (c) 2013 Pateo Software Ltd
*
* This source file is part of Pateo gis engine.
* For latest info, see http://www.pateo.com.cn/
*
* Time: 2013/09/06
* File: enn_ft_font_mgr.cpp
**/

#include "enn_ft_font_mgr.h"

namespace enn
{

#define IPAD2_DPI					(160)
#define ANDROID_DPI					(130)

FTFontManager::FTFontManager(Root* owner)
: owner_(owner)
, ft_mgr_(0)
{
	
}

FTFontManager::~FTFontManager()
{

}

bool FTFontManager::initalize(int w, int h)
{
	buffer_w_ = w;
	buffer_h_ = h;

	ft_mgr_ = ENN_NEW FTTextureFontMgr();
	ft_mgr_->initalize(buffer_w_, buffer_h_);

	registerDefaultFamilyFont();

	return true;
}

void FTFontManager::finalize()
{
	ENN_SAFE_DELETE(ft_mgr_);
}


FTTextureFont* FTFontManager::getFontByName(const String& name, int size)
{
	FTTextureFont* tex_font = 0;

	String font_name;
	FamilyFontMap::iterator it = family_font_map_.find(name);
	if (it != family_font_map_.end())
	{
		font_name = it->second;
	}
	else
	{
		font_name = default_family_name_;
	}

	ENN_ASSERT(size > 0);
	ENN_ASSERT(!font_name.empty());

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	HDC hdesk = GetDC(0);
	size = MulDiv(size, GetDeviceCaps(hdesk, LOGPIXELSY),72);
#elif ENN_PLATFORM == ENN_PLATFORM_ANDROID
	size = size * ANDROID_DPI / 72;
#elif ENN_PLATFORM == ENN_PLATFORM_IOS
	size = size * IPAD2_DPI / 72;
#else
	ENN_ASSERT(0);
#endif

	tex_font = ft_mgr_->getFromFileName(font_name.c_str(), size);

	ENN_ASSERT(tex_font);

	return tex_font;
}

void FTFontManager::registerFamilyFont(const String& name, const String& ttf_name)
{
	ENN_ASSERT(family_font_map_.find(name) == family_font_map_.end());

	family_font_map_[name] = ttf_name;
}

void FTFontManager::registerDefaultFamilyFont()
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	family_font_map_["ËÎÌå"] = "C:/Windows/Fonts/simsun.ttc";
	family_font_map_["Arial"] = "C:/Windows/Fonts/arial.ttf";
	family_font_map_["ÐÂËÎÌå"] = "C:/Windows/Fonts/simsun.ttc";
	family_font_map_["Ó×Ô²"] = "C:/Windows/Fonts/SIMYOU.TTF";
	family_font_map_["Î¢ÈíÑÅºÚ"] = "C:/Windows/Fonts/msyh.ttf";
	family_font_map_["Courier New"] = "C:/Windows/Fonts/cour.ttf";
	family_font_map_["Consolas"] = "C:/Windows/Fonts/consola.ttf";
	default_family_name_ = "C:/Windows/Fonts/msyh.TTF";

#elif ENN_PLATFORM == ENN_PLATFORM_ANDROID
	default_family_name_ = "/system/fonts/DroidSansFallback.ttf";

#elif ENN_PLATFORM == ENN_PLATFORM_IOS
	default_family_name_ = "/System/Library/Fonts/Cache/STHeiti-Light.ttc";

#else
	ENN_ASSERT(0);
#endif
}

}