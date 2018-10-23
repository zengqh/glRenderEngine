/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_mesh_manager.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_resource_manager.h"
#include "enn_mesh.h"
#include "enn_mesh_screen_mesh.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
struct ENN_AABB_INFO
{
	float vMin[3];
	float vMax[3];
};

struct ENN_HEAD_FILE_INFO
{
	char achr_file_flag[3];
	char achr_version[4];
	char achr_reversed[21];

	ENN_AABB_INFO aabb;
	int sub_mesh_count;
};

struct ENN_MTR_HEAD_INFO
{
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float emissive[4];
	float shininess;
};

struct ENN_MESH_CMD
{
	enum
	{
		C_DIFF = 0,
		C_DIFF2,
		C_LIGHTING,
		C_NORMALMAP,
		C_SPECMAP,
		C_EMISSIVEMAP,
		C_TEXANI,
		C_CULLMODE,
		C_NAME,
		C_ALPHA_TEST,
		C_SKELETON,
		C_TU_END,
		C_END
	};
};

struct ENN_SUB_MESH_HEAD_INFO
{
	ENN_AABB_INFO aabb;
	int prim_type_;
	int vertex_type_;
	int vertex_bytes_;
	int index_type_;
	int index_bytes_;
};

//////////////////////////////////////////////////////////////////////////
// MeshResAutoCreator
class MeshResAutoCreator : public ResAutoCreatorBase
{
public:
	MeshResAutoCreator();
	~MeshResAutoCreator();

	virtual bool buildResource( Resource* res );

protected:
	virtual bool prepareResourceImpl( Resource* res, MemoryBuffer* buff );

protected:
	void onMeshBeginParse(ENN_HEAD_FILE_INFO* ehfi);
	void onMeshSubMtr(const int*& next_data);
	void onMeshBeginSubMtr(const ENN_MTR_HEAD_INFO* emhi);

	void parseTex(const int*& next_data, int cmd);
	void parseTexAni(const int*& next_data);

	void onMeshBeginSubMesh(int sub_idx, const ENN_SUB_MESH_HEAD_INFO* esmhi);
	void onMeshEndSubMesh();

	void loadSkeleton(const int*& data);
	void onSkelGetBone(const char* name, int id, 
		const vec3f& pos, const Quaternion& quat, const vec3f& sca,
		const Matrix4& offset_matrix);
	void onSkelGetBoneParent(int i_child, int i_parent);

protected:
	MemoryBuffer*					memory_buff_;
	Mesh*							temp_mesh_;
	SubMaterial*					curr_temp_sub_mtr_;
	SubMesh*						curr_temp_sub_mesh_;
	ENN_SUB_MESH_HEAD_INFO*			curr_temp_sub_mesh_info_;
	Skeleton*						curr_temp_skel_;
	Animation*						curr_temp_anim_;
	NodeAniTrack*					curr_temp_anim_track_;
};


//////////////////////////////////////////////////////////////////////////
// MeshManager
class MeshManager : public ResourceManager
{
public:
	MeshManager();
	virtual ~MeshManager();

public:
	virtual void init();

public:
	static Mesh* createCube(const String& name, float size);
	static Mesh* createSphere(const String& name, float size, int segments);
	static Mesh* createPlane(const String& name, float x_len, float z_len, int x_seg, int z_seg);
	static Mesh* createMeshFromMqoTxt(const String& name, const String& sub_dir = "");
	static Mesh* createMeshFromPoly(const String& name, void* user_data);
	static Mesh* createScreenMesh(const String& name, const ScreenRectDesc& screen_rect);
public:
	Mesh* getFullScreenMesh();
	Mesh* getPointLitMesh();
	Mesh* getSpotLitMesh();

public:
	void saveMesh(Mesh* mesh, const String& file_name);
	void saveMesh(Mesh* mesh, File* file);

protected:
	void copyMeshInfo(ENN_HEAD_FILE_INFO* ehfi);
	void saveMeshSubMtr(File* file);
	void copyMtrInfo(ENN_MTR_HEAD_INFO* emhi);
	void copyClr(float dest[4], const Color& clr);
	void copyAABB(ENN_AABB_INFO* aabb, const AxisAlignedBox& aabb_src);
	void saveTexture(File* file, int cmd_id);
	void writeCmd(File* file, int cmd_id);
	void writeAlignedName(File* file, const char* name);
	void writeAlignedBland(File* file, int bytes);
	void copySubMeshInfo(ENN_SUB_MESH_HEAD_INFO* sub_mesh_info);
	void writeSkeleton(File* file, Skeleton* skel);

protected:
	virtual Resource* createImpl();

protected:
	Mesh*			curr_temp_save_mesh_;
	SubMaterial*	curr_temp_save_sub_mtr_;
	SubMesh*		curr_temp_save_sub_mesh_;

	Mesh*			full_screen_mesh_;
	Mesh*			point_lit_mesh_;
	Mesh*			spot_lit_mesh_;
};

}