/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/16
* File: enn_mesh_poly_creator.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_mesh_poly_creator.h"

#define POLY_WINDOW_WIDTH			(3.0f)
#define POLY_WINDOW_HEIGHT			(3.0f)

namespace enn
{
MeshPolyCreatorImpl::MeshPolyCreatorImpl()
	: temp_mesh_(0)
	, temp_poly_data_(0)
{

}

MeshPolyCreatorImpl::~MeshPolyCreatorImpl()
{

}

bool MeshPolyCreatorImpl::prepareResource( Resource* res, void* context )
{
	temp_poly_data_ = (PolyMeshUserData*)(res->getUserData());

	if (!temp_poly_data_)
	{
		ENN_ASSERT(0);
		return false;
	}

	if (temp_poly_data_->vertex_num < 3)
	{
		ENN_ASSERT(0);
		return false;
	}

	ENN_ASSERT(raw_vertex_list_.empty());

	MemoryBuffer buffer(temp_poly_data_->vertex_ptr, Math::ENN_MAX_UNSIGNED);

	for (int i = 0; i <= (int)temp_poly_data_->vertex_num - 1; ++i)
	{
		vec3f pos;

		buffer.ReadBin(pos.x);
		pos.y = 0.0f;

		/** cuz the north is y axis, but in opengl, z is toward the south */
		buffer.ReadBin(pos.z);
		pos.z = -pos.z;

		if (raw_vertex_list_.size() > 0)
		{
			const vec3f& pre_pos = raw_vertex_list_[raw_vertex_list_.size() - 1];

			vec3f del_pos = pos - pre_pos;

			if (!Math::isVector3Zero(del_pos))
			{
				raw_vertex_list_.push_back(pos);
			}
		}
		else
		{
			raw_vertex_list_.push_back(pos);
		}
	}

	/** check out the first and the last point can not be same */
	const vec3f& v_first = raw_vertex_list_[0];
	const vec3f& v_last = raw_vertex_list_[raw_vertex_list_.size() - 1];

	/** delete last point */
	if (Math::isVector3Equal(v_first, v_last))
	{
		raw_vertex_list_.pop_back();
	}

	if (raw_vertex_list_.size() < 3)
	{
		ENN_ASSERT(0);
		return false;
	}

	std::reverse(raw_vertex_list_.begin(), raw_vertex_list_.end());

	temp_mesh_ = static_cast<Mesh*>(res);

	prepare_wall();
	prepare_top();

	raw_vertex_list_.clear();

	temp_mesh_->calcNormal();
	temp_mesh_->updateAABB(true);

	temp_mesh_ = 0;

	return true;
}

bool MeshPolyCreatorImpl::buildResource( Resource* res )
{
	Mesh* mesh = static_cast<Mesh*>(res);

	mesh->makeGPUBuffer();

	return true;
}

void MeshPolyCreatorImpl::prepare_wall()
{
	enn::vector<VertexP3N3U2>::type vts;
	enn::vector<ushort>::type ids;

	for (int i = 0; i < (int)raw_vertex_list_.size(); ++i)
	{
		const vec3f& pre = raw_vertex_list_[i];

		int i_next = i + 1;
		if (i_next >= (int)raw_vertex_list_.size())
		{
			i_next = 0;
		}

		const vec3f& next = raw_vertex_list_[i_next];

		float len = (next - pre).length();

		float u = static_cast<float>((int)(len / (POLY_WINDOW_WIDTH) + 0.5f));

		VertexP3N3U2 v0, v1, v2, v3;

		v0.x = pre.x;
		v0.y = pre.y;
		v0.z = pre.z;
		v0.u = 0.0f;
		v0.v = 1.0f;

		v1.x = next.x;
		v1.y = next.y;
		v1.z = next.z;
		v1.u = u;
		v1.v = 1.0f;

		v2.x = next.x;
		v2.y = temp_poly_data_->grade_level * POLY_WINDOW_HEIGHT;
		v2.z = next.z;
		v2.u = u;
		v2.v = 1.0f - temp_poly_data_->grade_level * 0.1f;

		v3.x = pre.x;
		v3.y = temp_poly_data_->grade_level * POLY_WINDOW_HEIGHT;
		v3.z = pre.z;
		v3.u = 0.0f;
		v3.v = 1.0f - temp_poly_data_->grade_level * 0.1f;

		ushort index_start = vts.size();

		vts.push_back(v0);
		vts.push_back(v1);
		vts.push_back(v2);
		vts.push_back(v3);

		ids.push_back(index_start);
		ids.push_back(index_start + 1);
		ids.push_back(index_start + 2);

		ids.push_back(index_start + 2);
		ids.push_back(index_start + 3);
		ids.push_back(index_start);
	}

	ENN_ASSERT(vts.size() < 65536);

	SubMesh* sub_mesh = temp_mesh_->createNewSubMesh(0);

	sub_mesh->setPrimType(PRIM_TRIANGLELIST);
	sub_mesh->setVertexType(MASK_P3N3U2);
	sub_mesh->setVertexCount(vts.size());
	
	sub_mesh->setIdxType(IDX_16);
	sub_mesh->setIndexCount(ids.size());

	sub_mesh->setVertexData(&vts[0]);
	sub_mesh->setIndexData(&ids[0]);

	sub_mesh->setMatIdx(temp_poly_data_->sub_wall_mtr_idx_);
}

void MeshPolyCreatorImpl::prepare_top()
{
	/** we use std vector here */
	std::vector<p2t::Point*> poly;
	for (int i = 0; i < (int)raw_vertex_list_.size(); ++i)
	{
		p2t::Point* v = new p2t::Point();
		v->x = raw_vertex_list_[i].x;
		v->y = raw_vertex_list_[i].z;

		poly.push_back(v);
	}

	/** we could not use enn's malloc here !!! */
	/** make sure memory can not be leak !!! */
	p2t::CDT* cdt = new p2t::CDT(poly);

	/** do not support hole now. */

	cdt->Triangulate();

	std::vector<p2t::Triangle*>& triangles = cdt->GetTriangles();

	enn::vector<VertexP3N3>::type vts;
	enn::vector<ushort>::type ids;
	ushort curr_idx = 0;

	for (int i = 0; i < (int)triangles.size(); ++i)
	{
		p2t::Triangle* tri = triangles[i];

		for (int j = 0; j < 3; ++j)
		{
			VertexP3N3 vtx;
			p2t::Point* p = tri->GetPoint(j);

			vtx.x = (float)p->x;
			vtx.y = temp_poly_data_->grade_level * POLY_WINDOW_HEIGHT;
			vtx.z = (float)p->y;
			vtx.nx = 0.0f;
			vtx.ny = 1.0f;
			vtx.nz = 0.0f;

			vts.push_back(vtx);
			ids.push_back(curr_idx++);
		}
	}

	ENN_ASSERT(vts.size() < 65536);

	SubMesh* sub_mesh = temp_mesh_->createNewSubMesh(1);

	sub_mesh->setPrimType(PRIM_TRIANGLELIST);
	sub_mesh->setVertexType(MASK_P3N3);
	sub_mesh->setVertexCount(vts.size());

	sub_mesh->setIdxType(IDX_16);
	sub_mesh->setIndexCount(ids.size());

	sub_mesh->setVertexData(&vts[0]);
	sub_mesh->setIndexData(&ids[0]);

	sub_mesh->setMatIdx(temp_poly_data_->sub_top_mtr_idx_);

	for (int i = 0; i < (int)poly.size(); ++i)
	{
		p2t::Point* p = poly[i];
		delete p;
	}

	delete cdt;
	cdt = 0;
}

}