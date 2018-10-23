/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_renderable.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_material.h"
#include "enn_gl_vb.h"
#include "enn_gl_ib.h"

namespace enn
{
class Material;
class SkeletonInstance;
class LightsInfo;
//////////////////////////////////////////////////////////////////////////
class Renderable : public AllocatedObject
{
public:
	virtual ~Renderable() {}

public:
	virtual const Matrix4& getWorldMatrix() const = 0;
	virtual const AxisAlignedBox& getAABBWorld() const = 0;
	virtual SubMaterial* getMaterial() const = 0;

	virtual VertexBuffer* getVB() const = 0;
	virtual IndexBuffer* getIB() const = 0;

	/** skeleton about */
	virtual int getSubIdx() const { return 0; }
	virtual SkeletonInstance* getSkelInst() const { return 0; }

	/** light info */
	virtual LightsInfo* getLightsInfo() const { return 0; };

	virtual void render() = 0;
};

}