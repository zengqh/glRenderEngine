/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_mesh_node.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_mesh_node.h"
#include "enn_mesh.h"
#include "enn_root.h"
#include "enn_mesh_manager.h"
#include "enn_render_pass_base.h"
#include "enn_light.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// SubEntity
void SubEntity::addToRenderer(IAddRenderable* ad)
{
	ad->addRenderable(this);
}

void SubEntity::render()
{
	SubMesh* sm = mesh_node_->getMesh()->getSubMesh(sub_idx_);

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->drawIndexedPrimitives(sm->getPrimType(), 0, sm->getIndexCount(), sm->getIdxType() == IDX_16 ? true : false);

	checkGLError();
}

const Matrix4& SubEntity::getWorldMatrix() const
{
	return mesh_node_->getDerivedMatrix();
}

const AxisAlignedBox& SubEntity::getAABBWorld() const
{
	if (aabb_world_dirty_)
	{
		SubEntity* pthis = const_cast<SubEntity*>(this);

		pthis->world_aabb_ = mesh_node_->getMesh()->getSubMesh(sub_idx_)->getAABB();

		pthis->world_aabb_.transformAffine(getWorldMatrix());

		pthis->aabb_world_dirty_ = false;
	}

	return world_aabb_;
}

SubMaterial* SubEntity::getMaterial() const
{
	return mtr_;
}

VertexBuffer* SubEntity::getVB() const
{
	SubMesh* sm = mesh_node_->getMesh()->getSubMesh(sub_idx_);
	return sm->getVB();
}

IndexBuffer* SubEntity::getIB() const
{
	SubMesh* sm = mesh_node_->getMesh()->getSubMesh(sub_idx_);
	return sm->getIB();
}

LightsInfo* SubEntity::getLightsInfo() const
{
	return mesh_node_->getLightsInfo();
}

//////////////////////////////////////////////////////////////////////////
// MeshNode
MeshNode::MeshNode()
: mesh_(0)
, skel_inst_(0)
{
	type_flag_.set_flag(NODE_MESH);
}

MeshNode::~MeshNode()
{
	clearSubEntity();

	if (mesh_)
	{
		mesh_->removeListener(this);
	}

	ENN_SAFE_RELEASE(mesh_);
}

void MeshNode::setMesh(const String& name)
{
	clearSubEntity();
	if (mesh_)
	{
		mesh_->removeListener(this);
		ENN_SAFE_RELEASE(mesh_);
	}

	if (!name.empty())
	{
		mesh_ = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->getOrCreateResource(name));
		mesh_->addListener(this);

		mesh_->checkLoad();
	}
}

void MeshNode::setMesh(Mesh* mesh)
{
	clearSubEntity();
	if (mesh_)
	{
		mesh_->removeListener(this);
		ENN_SAFE_RELEASE(mesh_);
	}

	mesh_ = mesh;
	mesh_->addRef();

	mesh_->addListener(this);
	mesh_->checkLoad();
}

Mesh* MeshNode::getMesh() const
{
	return mesh_;
}

void MeshNode::addToRender(IAddRenderable* ad)
{
	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		(*it)->addToRenderer(ad);
	}
}

void MeshNode::setWorldAABBDirty()
{
	SceneNode::setWorldAABBDirty();

	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		(*it)->setAABBWorldDirty();
	}
}

void MeshNode::clearSubEntity()
{
	ENN_FOR_EACH(SubEntityList, sub_entity_list_, it)
	{
		SubEntity* entity = (*it);
		ENN_SAFE_DELETE(entity);
	}

	sub_entity_list_.clear();
}

void MeshNode::initSubEntity()
{
	ENN_ASSERT(sub_entity_list_.empty());

	Mesh::SubMeshList& sub_mesh_list = mesh_->getSubMeshList();

	ENN_FOR_EACH(Mesh::SubMeshList, sub_mesh_list, it)
	{
		int sub_mesh_idx = it->first;

		SubMesh* sub_mesh = mesh_->getSubMesh(sub_mesh_idx);
		SubEntity* sub_entity = ENN_NEW SubEntity;

		sub_entity->setMeshNode(this);
		sub_entity->setSubIndex(sub_mesh_idx);
		sub_entity->setSkelInst(skel_inst_);

		sub_entity->setMaterial(sub_mesh->getMaterial());

		sub_entity_list_.push_back(sub_entity);
	}
}

void MeshNode::clearSkeleton()
{
	ENN_SAFE_DELETE(skel_inst_);
}

void MeshNode::initSkeleton()
{
	ENN_ASSERT(mesh_);

	Skeleton* skel = mesh_->getSkeleton();

	if (skel)
	{
		skel_inst_ = ENN_NEW SkeletonInstance();
		skel_inst_->init(skel);
	}
}

void MeshNode::onResourceLoaded( Resource* res )
{
	initSkeleton();
	initSubEntity();
	setWorldAABBDirty();
}

void MeshNode::onResourceUpdate( Resource* res )
{
	clearSkeleton();
	initSkeleton();
	clearSubEntity();
	initSubEntity();
	setWorldAABBDirty();
}

const AxisAlignedBox& MeshNode::getObjectAABB() const
{
	if (mesh_)
	{
		object_aabb_ = mesh_->getAABB();
	}
	else
	{
		object_aabb_ = AxisAlignedBox(vec3f(-0.5f, -0.5f, -0.5f), vec3f(0.5f, 0.5f, 0.5f));
	}

	return object_aabb_;
}

void MeshNode::update()
{
	float elpased_time = Root::getSingletonPtr()->get_time_system()->getElapsedTime();

	if (skel_inst_)
	{
		skel_inst_->update(elpased_time);
	}

	RenderableNode::update();
}

}