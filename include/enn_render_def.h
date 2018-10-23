/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_render_def.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include <enn_platform_headers.h>

namespace enn
{

enum { MAX_TEXTURE_UNITS = 10 };
enum { MAX_FRAME_BUFFER_CHANNEL = 4 };
enum CmpFunc
{
	CMP_NEVER,
	CMP_LESS,
	CMP_EQUAL,
	CMP_LESSEQUAL,
	CMP_GREATER,
	CMP_NOTEQUAL,
	CMP_GREATEREQUAL,
	CMP_ALWAYS,
	CF_NUM
};

enum CullMode
{
	CULL_NONE,
	CULL_CW,
	CULL_CCW,
	CM_NUM
};

enum TextureFilter
{
	TEXF_NONE,         
	TEXF_POINT,
	TEXF_LINEAR,
	TEXF_MIP_LINEAR,
	TF_NUM
};

enum TextureAddress
{
	TEXADDR_NONE,
	TEXADDR_WRAP,
	TEXADDR_CLAMP,
	TA_NUM
};

enum TextureType
{
	TEXTURE_2D,
	TEXTURE_CUBE,
	TEX_NUM
};

enum PixelFormat
{
	U1,
	U3_RGB,
	U3_BGR,
	U4_RGBA,
	U4_BGRA,
	U16_1,
	U16_2,
	U16_4,
	D16,
	D24,
	D32,
	DXT1,
	DXT3,
	DXT5,
	F16_1,
	F16_2,
	F16_4,
	F32_1,
	F32_2,
	F32_4,
	PIXEL_NUM
};


enum PrimType
{
	PRIM_LINELIST = 0,
	PRIM_TRIANGLELIST,
	PRIM_POINTLIST,
	PRIM_NUM
};

enum VertexElement
{
	ELEMENT_POSITION = 0,
	ELEMENT_NORMAL,
	ELEMENT_COLOR,
	ELEMENT_TEXCOORD1,
	ELEMENT_TEXCOORD2,
	ELEMENT_CUBETEXCOORD1,
	ELEMENT_CUBETEXCOORD2,
	ELEMENT_TANGENT,
	ELEMENT_BLENDWEIGHTS,
	ELEMENT_BLENDINDICES,
	ELEMENT_NUM
};

enum IndexType
{
	IDX_16,
	IDX_32,
	IDX_NUM
};

enum RenderState
{
	RS_WIREFRAME = 0,
	RS_FRONTCCW,
	RS_CULLFACE,
	RS_CULLFACEEN,
	RS_ZTEST,
	RS_ZWRITE,
	RS_BLEND,
	RS_NUM
};

enum CubeMapFaces
{
	FACE_POSITIVE_X = 0,
	FACE_NEGATIVE_X,
	FACE_POSITIVE_Y,
	FACE_NEGATIVE_Y,
	FACE_POSITIVE_Z,
	FACE_NEGATIVE_Z,
	MAX_CUBEMAP_FACES
};


static const unsigned MASK_POSITION = 0x1;
static const unsigned MASK_NORMAL = 0x2;
static const unsigned MASK_COLOR = 0x4;
static const unsigned MASK_TEXCOORD1 = 0x8;
static const unsigned MASK_TEXCOORD2 = 0x10;
static const unsigned MASK_CUBETEXCOORD1 = 0x20;
static const unsigned MASK_CUBETEXCOORD2 = 0x40;
static const unsigned MASK_TANGENT = 0x80;
static const unsigned MASK_BLENDWEIGHTS = 0x100;
static const unsigned MASK_BLENDINDICES = 0x200;

static const unsigned MASK_P3N3U2 = (MASK_POSITION | MASK_NORMAL | MASK_TEXCOORD1);
static const unsigned MASK_P3N3 = (MASK_POSITION | MASK_NORMAL);
static const unsigned MASK_P3U2 = (MASK_POSITION | MASK_TEXCOORD1);
static const unsigned MASK_P3U2C4 = (MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1);

static const unsigned MASK_P3U2U2 = (MASK_P3U2 | MASK_TEXCOORD2);

}