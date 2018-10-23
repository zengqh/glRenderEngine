/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_node_def.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

namespace enn
{
	enum NodeDirtyFlags
	{
		/** node */
		SELF_TRANSFORM_DIRTY = 0,
		PARENT_TRANSFORM_DIRTY,
		/** scene node */
		DF_WORLD_AABB,
		DF_OCTREE_POS,
		DF_LIGHT_INFO,
	};

	enum NodeFeatureFlags
	{
		SHADOW_ENABLE_CAST = 0,
		SHADOW_ENABLE_RECEIVE
	};

	enum NodeTypeFlags
	{
		NODE_SCENE = 0,
		NODE_RENDERABLE,
		NODE_CAMERA,
		NODE_LIGHT,
		NODE_AREA,
		NODE_MESH,
		NODE_WATER,
		NODE_WATER_PLANE
	};
}