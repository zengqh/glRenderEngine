/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_scene_manager.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_scene_node.h"
#include "enn_camera_node.h"
#include "enn_mesh_node.h"
#include "enn_water_plane_node.h"
#include "enn_skybox_scene_data.h"
#include "enn_light_node.h"
#include "enn_skybox_scene_data.h"
#include "enn_scene_find.h"
#include "enn_physics_world.h"

namespace enn
{

struct SceneFindResult
{
	typedef enn::vector<SceneNode*>::type Objs;

	SceneFindResult()
	{
		
	}

	void insert_finded(SceneNode* node)
	{
		obj_list_.push_back(node);
	}

	void clear()
	{
		obj_list_.clear();
	}

	Objs	obj_list_;
};

class SceneManager : public AllocatedObject, public Noncopyable
{
public:
	typedef enn::map<enn::String, SceneNode*>::type		NodeMap;
	typedef enn::vector<SceneNode*>::type				NodeList;
	typedef SceneNode*									(*CreatorType)();
	typedef enn::map<ClassType, CreatorType>::type		CreatorMap;
	typedef enn::list<MeshNode*>::type					UserMeshList;

public:
	SceneManager();
	~SceneManager();

	void init();
	void uinit();

public:
	SceneNode* createSceneNode(ClassType type, const enn::String& name);
	template<class T> T* createSceneNode(const enn::String& name)
	{
		return static_cast<T*>(createSceneNode(ENN_CLASS_TYPE(T), name));
	}

	void destroySceneNode(SceneNode* sn);
	void destroyAllSceneNode();

	SceneNode* getRootNode() const;

public:
	void update();
	void find(SceneFindOption& sfo);

public:
	void setGlobalAmb(const Color& clr);
	const Color& getGlobalAmb() const;

public:
	void setSkyboxData(SkyboxSceneData* skybox);
	SkyboxSceneData* getSkyboxData();

public:
	PhysicsWorld* getPhysicsWorld() const;

protected:
	void findByCamera(SceneFindOption& sfo);

protected:
	CreatorType findCreator( ClassType type ) const;

	template<class T>
	void registerCreator();

	template<class T>
	static SceneNode* _createSceneNode();

	SceneNode* getSceneNode(const enn::String& name) const;

protected:
	CreatorMap						creator_map_;
	NodeMap							node_list_;
	SceneNode*						root_node_;
	Color							global_amb_;
	SkyboxSceneData*				skybox_scene_data_;
	PhysicsWorld*					physics_world_;
};

}