/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/15
* File: enn_mesh_fullscreen_creator.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_mesh_manager.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// FullScreenMeshCreatorImpl
class FullScreenMeshCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	FullScreenMeshCreatorImpl()
	{

	}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context )
	{
		VertexP3N3U2 vts[4] =
		{
			/** left top */
			-1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f,

			/** right top */
			1.0f, 1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 0.0f,

			/** left bottom */
			-1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 1.0f,

			/** right bottom */
			1.0f, -1.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			1.0f, 1.0f
		};

		ushort ids[6] = 
		{
			0, 2, 1,
			1, 2, 3
		};


		Mesh* mesh = static_cast<Mesh*>(res);
		SubMesh* sub_mesh = mesh->createNewSubMesh(0);

		sub_mesh->setPrimType(PRIM_TRIANGLELIST);
		sub_mesh->setVertexType(MASK_P3N3U2);
		sub_mesh->setVertexCount(ENN_ARRAY_SIZE(vts));

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(ENN_ARRAY_SIZE(ids));

		sub_mesh->setVertexData(vts);
		sub_mesh->setIndexData(ids);

		return true;
	}

	virtual bool buildResource( Resource* res )
	{
		Mesh* mesh = static_cast<Mesh*>(res);
		mesh->makeGPUBuffer();

		return true;
	}
};

}