/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_mesh_node.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_renderable.h"
#include "enn_scene_node.h"
#include "enn_resource.h"
#include "enn_material.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// SubEntity
class MeshNode;
class Mesh;
class SkeletonInstance;
class SubEntity : public Renderable
{
public:
	SubEntity()
		: mesh_node_(0)
		, sub_idx_(0)
		, aabb_world_dirty_(true)
		, skel_inst_(0)
	{

	}

	virtual ~SubEntity() {}

public:
	void setMeshNode(MeshNode* node)
	{
		mesh_node_ = node;
	}

	void setSubIndex(int idx)
	{
		sub_idx_ = idx;
	}

	virtual int getSubIdx() const 
	{
		return sub_idx_;
	}

	void setSkelInst(SkeletonInstance* skel_inst)
	{
		skel_inst_ = skel_inst;
	}

	virtual SkeletonInstance* getSkelInst() const
	{ 
		return skel_inst_; 
	}

	virtual LightsInfo* getLightsInfo() const;

	void setAABBWorldDirty()
	{
		aabb_world_dirty_ = true;
	}

	void setMaterial(SubMaterial* mtr)
	{
		mtr_ = mtr;
	}

	virtual const Matrix4& getWorldMatrix() const;

	virtual const AxisAlignedBox& getAABBWorld() const;

	void addToRenderer(IAddRenderable* ad);

	virtual SubMaterial* getMaterial() const;
	virtual VertexBuffer* getVB() const;
	virtual IndexBuffer* getIB() const;

	virtual void render();

protected:
	MeshNode*				mesh_node_;
	int						sub_idx_;
	SubMaterial*			mtr_;
	AxisAlignedBox			world_aabb_;
	bool					aabb_world_dirty_;
	SkeletonInstance*		skel_inst_;
};

//////////////////////////////////////////////////////////////////////////
// Mesh node
class MeshNode : public RenderableNode, public IResourceListener
{
	ENN_DECLARE_RTTI(MeshNode)
public:
	typedef enn::vector<SubEntity*>::type SubEntityList;

public:
	MeshNode();
	virtual ~MeshNode();

public:
	void setMesh(const String& name);
	void setMesh(Mesh* mesh);
	Mesh* getMesh() const;

protected:
	virtual void addToRender(IAddRenderable* ra);

	virtual void setWorldAABBDirty();

	void clearSubEntity();
	void initSubEntity();
	void clearSkeleton();
	void initSkeleton();

public:
	virtual void onResourceLoaded( Resource* res );
	virtual void onResourceUpdate( Resource* res );

public:
	virtual const AxisAlignedBox& getObjectAABB() const;
	virtual void update();

protected:
	Mesh*					mesh_;
	SubEntityList			sub_entity_list_;
	SkeletonInstance*		skel_inst_;
};

}