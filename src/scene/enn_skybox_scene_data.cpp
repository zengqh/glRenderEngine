/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/01
* File: enn_skybox_scene_data.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_skybox_scene_data.h"
#include "enn_gl_frame_buffer.h"
#include "enn_gl_texture_obj.h"
#include "enn_gl_render_buffer.h"

namespace enn
{
SkyboxSceneData::SkyboxSceneData()
{

}

SkyboxSceneData::~SkyboxSceneData()
{

}

void SkyboxSceneData::setBackTex(const String& tex)
{
	cubemap_tex_[FACE_POSITIVE_Z] = tex;
}

void SkyboxSceneData::setBottomTex(const String& tex)
{
	cubemap_tex_[FACE_NEGATIVE_Y] = tex;
}

void SkyboxSceneData::setFrontTex(const String& tex)
{
	cubemap_tex_[FACE_NEGATIVE_Z] = tex;
}

void SkyboxSceneData::setLeftTex(const String& tex)
{
	cubemap_tex_[FACE_NEGATIVE_X] = tex;
}

void SkyboxSceneData::setRightTex(const String& tex)
{
	cubemap_tex_[FACE_POSITIVE_X] = tex;
}

void SkyboxSceneData::setTopTex(const String& tex)
{
	cubemap_tex_[FACE_POSITIVE_Y] = tex;
}
}