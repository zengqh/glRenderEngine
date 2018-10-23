/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/16
* File: enn_mesh_poly_creator.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_mesh_manager.h"
#include "enn_math.h"

namespace enn
{
struct PolyMeshUserData : public AllocatedObject
{
	uint32	vertex_num;
	void*	vertex_ptr;
	uint32	grade_level;
	uint32  sub_wall_mtr_idx_;
	uint32	sub_top_mtr_idx_;
};
//////////////////////////////////////////////////////////////////////////
// MeshPolyCreatorImpl
class MeshPolyCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	MeshPolyCreatorImpl();
	~MeshPolyCreatorImpl();

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context );

	virtual bool buildResource( Resource* res );

protected:
	virtual void prepare_wall();
	virtual void prepare_top();

protected:
	enn::vector<vec3f>::type			raw_vertex_list_;
	Mesh*								temp_mesh_;
	PolyMeshUserData*					temp_poly_data_;
};
}