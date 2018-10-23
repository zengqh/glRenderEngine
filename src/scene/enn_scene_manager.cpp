/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_scene_manager.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_scene_manager.h"
#include "enn_root.h"

namespace enn
{
SceneManager::SceneManager()
: root_node_(0)
, skybox_scene_data_(0)
, physics_world_(0)
{
	
}

SceneManager::~SceneManager()
{
	
}

void SceneManager::init()
{
	registerCreator<SceneNode>();
	registerCreator<MeshNode>();
	registerCreator<CameraNode>();
	registerCreator<LightNode>();
	registerCreator<WaterPlaneNode>();

	root_node_ = createSceneNode<SceneNode>("root");

	physics_world_ = ENN_NEW PhysicsWorld();
}

void SceneManager::uinit()
{
	destroyAllSceneNode();

	ENN_SAFE_DELETE(skybox_scene_data_);
}

SceneNode* SceneManager::createSceneNode(ClassType type, const enn::String& name)
{
	SceneNode* sn = 0;

	if (getSceneNode(name))
	{
		ENN_ASSERT(0);
		return sn;
	}

	CreatorType fn = findCreator( type );
	if ( fn )
	{
		sn = fn();
		sn->setName(name);
	}

	node_list_.insert(NodeMap::value_type(sn->getName(), sn));
	sn->setSceneManager(this);

	return sn;
}

void SceneManager::destroySceneNode(SceneNode* sn)
{
	ENN_SAFE_DELETE(sn);
}

void SceneManager::destroyAllSceneNode()
{
	ENN_FOR_EACH(NodeMap, node_list_, it)
	{
		SceneNode* sn = it->second;
		destroySceneNode(sn);
	}

	node_list_.clear();

	/** destroy root node */
	root_node_ = 0;
}

SceneNode* SceneManager::getRootNode() const
{
	return root_node_;
}

void SceneManager::update()
{
	root_node_->updateDerived();

	physics_world_->update(ROOTPTR->get_time_system()->getElapsedTime());
}

void SceneManager::find(SceneFindOption& sfo)
{
	if (sfo.option_ == SceneFindOption::OP_CAMERA_FULL)
	{
		findByCamera(sfo);
	}
}

void SceneManager::findByCamera(SceneFindOption& sfo)
{
	FindByCamera finder(this, &sfo);

	ENN_FOR_EACH(NodeMap, node_list_, it)
	{
		SceneNode* node = it->second;

		finder.onTestObject(node);
	}
}

void SceneManager::setGlobalAmb(const Color& clr)
{
	global_amb_ = clr;
}

const Color& SceneManager::getGlobalAmb() const
{
	return global_amb_;
}

void SceneManager::setSkyboxData(SkyboxSceneData* skybox)
{
	skybox_scene_data_ = skybox;
}

SkyboxSceneData* SceneManager::getSkyboxData()
{
	return skybox_scene_data_;
}

PhysicsWorld* SceneManager::getPhysicsWorld() const
{
	return physics_world_;
}

SceneManager::CreatorType SceneManager::findCreator( ClassType type ) const
{
	CreatorMap::const_iterator it = creator_map_.find(type);
	if ( it != creator_map_.end() )
	{
		return it->second;
	}

	return 0;
}

template<class T>
void SceneManager::registerCreator()
{
	if ( findCreator(ENN_CLASS_TYPE(T)) )
	{
		ENN_ASSERT(0);
		return;
	}

	creator_map_.insert(CreatorMap::value_type(ENN_CLASS_TYPE(T), _createSceneNode<T>));
}

template<class T>
SceneNode* SceneManager::_createSceneNode()
{
	return ENN_NEW T();
}

SceneNode* SceneManager::getSceneNode(const enn::String& name) const
{
	NodeMap::const_iterator it = node_list_.find(name);
	if (it != node_list_.end())
	{
		return it->second;
	}

	return 0;
}

}