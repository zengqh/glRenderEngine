/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/01
* File: enn_skybox_scene_data.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"

namespace enn
{

class SkyboxSceneData : public AllocatedObject
{
public:
	SkyboxSceneData();
	~SkyboxSceneData();

public:
	void setBackTex(const String& tex);
	void setBottomTex(const String& tex);
	void setFrontTex(const String& tex);
	void setLeftTex(const String& tex);
	void setRightTex(const String& tex);
	void setTopTex(const String& tex);

	String		cubemap_tex_[6];
};
}