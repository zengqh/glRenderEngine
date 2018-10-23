/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/07
* File: enn_mesh_manager.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_mesh_manager.h"
#include "enn_mesh.h"
#include "enn_mesh_manual_creator.h"
#include "enn_mesh_mqo_txt_creator.h"
#include "enn_mesh_fullscreen_creator.h"
#include "enn_mesh_poly_creator.h"
#include "enn_root.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
/** 1.0 version mesh format */
#define CURR_MESH_MAJOR_VERSION		(1)
#define CURR_MESH_VICE_VERSION		(0)


//////////////////////////////////////////////////////////////////////////
static inline const char* _loadAlignedString( const int*& nextdata )
{
	int len = *nextdata;

	ENN_ASSERT((len % 4) == 0);

	++nextdata;
	const char* str = (const char*)nextdata;
	nextdata  = (const int*)((char*)nextdata + len);
	return str;
}

static inline bool assert_mesh_ptr_align(const void* ptr)
{
	return ((uint32)ptr % 4 == 0) ? true : false;
}

// MeshResAutoCreator
MeshResAutoCreator::MeshResAutoCreator()
: temp_mesh_(0)
, curr_temp_sub_mtr_(0)
, curr_temp_sub_mesh_(0)
, curr_temp_sub_mesh_info_(0)
, memory_buff_(0)
, curr_temp_skel_(0)
, curr_temp_anim_(0)
, curr_temp_anim_track_(0)
{
	m_basePath = "mesh";
}

MeshResAutoCreator::~MeshResAutoCreator()
{

}

bool MeshResAutoCreator::buildResource( Resource* res )
{
	temp_mesh_ = static_cast<Mesh*>(res);

	temp_mesh_->makeGPUBuffer();

	temp_mesh_ = 0;

	return true;
}

bool MeshResAutoCreator::prepareResourceImpl( Resource* res, MemoryBuffer* buff )
{
	ENN_ASSERT(buff);

	if (!buff)
	{
		return false;
	}

	temp_mesh_ = static_cast<Mesh*>(res);
	memory_buff_ = buff;

	if (!memory_buff_)
	{
		return false;
	}

	void* data = memory_buff_->GetHandle();

	ENN_HEAD_FILE_INFO* ehfi = static_cast<ENN_HEAD_FILE_INFO*>(data);

	if (strncmp(ehfi->achr_file_flag, "enn", 3) != 0)
	{
		return false;
	}

	uchar ver_first = ehfi->achr_version[0];
	uchar ver_second = ehfi->achr_version[1];

	int total_ver = ver_first * 256 + ver_second;

	onMeshBeginParse(ehfi);

	const int* next_data = (const int*)(ehfi + 1);

	/** parse mtr */
	int mtr_idx_cnt = *next_data;
	++next_data;

	ENN_ASSERT(mtr_idx_cnt > 0);

	Material* mtr = ENN_NEW Material();

	for (int i = 0; i < mtr_idx_cnt; ++i)
	{
		int mtr_id = *next_data;
		++next_data;

		curr_temp_sub_mtr_ = ENN_NEW SubMaterial(mtr);

		mtr->addSubMaterial(i, curr_temp_sub_mtr_);

		onMeshSubMtr(next_data);
	}

	/** parse sub mesh */
	int sub_mesh_count = ehfi->sub_mesh_count;
	ENN_ASSERT(sub_mesh_count > 0);

	for (int i = 0; i < sub_mesh_count; ++i)
	{
		const ENN_SUB_MESH_HEAD_INFO* esmhi = (const ENN_SUB_MESH_HEAD_INFO*)next_data;
		next_data = (const int*)(esmhi + 1);

		ENN_ASSERT(assert_mesh_ptr_align(esmhi));

		onMeshBeginSubMesh(i, esmhi);

		ENN_ASSERT(assert_mesh_ptr_align(next_data));

		/** vertex data */
		int vertex_count = *next_data;
		next_data++;

		curr_temp_sub_mesh_->setVertexCount(vertex_count);
		curr_temp_sub_mesh_->setVertexData((void*)next_data);

		next_data = (const int*)((const char*)next_data + vertex_count * esmhi->vertex_bytes_);

		/** index data */
		ENN_ASSERT(assert_mesh_ptr_align(next_data));
		int index_count = *next_data;
		++next_data;

		curr_temp_sub_mesh_->setIndexCount(index_count);
		curr_temp_sub_mesh_->setIndexData((void*)next_data);
		int indices_bytes = index_count * esmhi->index_bytes_;

		indices_bytes = ENN_ALIGN_4_BYTES(indices_bytes);

		next_data = (const int*)((const char*)next_data + indices_bytes);

		ENN_ASSERT(assert_mesh_ptr_align(next_data));

		/** sub mesh name */
		const char* name = _loadAlignedString( next_data );
		curr_temp_sub_mesh_->setName(name);

		/** sub mesh mtr idx */
		int mtr_idx = -1;
		mtr_idx = *next_data;
		next_data++;

		ENN_ASSERT(mtr_idx != -1);

		curr_temp_sub_mesh_->setMatIdx(mtr_idx);

		onMeshEndSubMesh();
	}

	/** other cmd like skeleton */
	int other_cmd_id = *next_data;
	++next_data;

	while (other_cmd_id != ENN_MESH_CMD::C_END)
	{
		if (other_cmd_id == ENN_MESH_CMD::C_SKELETON)
		{
			loadSkeleton(next_data);
		}

		other_cmd_id = *next_data;
		++next_data;
	}

	temp_mesh_->setMaterial(mtr);

	Root::getSingletonPtr()->get_file_system()->freeFileData(memory_buff_);

	memory_buff_ = 0;
	temp_mesh_ = 0;
	curr_temp_sub_mtr_ = 0;
	curr_temp_sub_mesh_ = 0;
	curr_temp_sub_mesh_info_ = 0;
	curr_temp_skel_ = 0;
	curr_temp_anim_ = 0;
	curr_temp_anim_track_ = 0;

	return true;
}

void MeshResAutoCreator::onMeshBeginParse(ENN_HEAD_FILE_INFO* ehfi)
{
	/** aabb */
	AxisAlignedBox& mesh_aabb = temp_mesh_->getAABB();
	mesh_aabb.setExtents(ehfi->aabb.vMin, ehfi->aabb.vMax);
}

void MeshResAutoCreator::onMeshSubMtr(const int*& next_data)
{
	const ENN_MTR_HEAD_INFO* emhi = (const ENN_MTR_HEAD_INFO*)next_data;
	onMeshBeginSubMtr(emhi);

	next_data = (const int*)((const char*)next_data + sizeof(ENN_MTR_HEAD_INFO));

	int cmd_id = *next_data;
	++next_data;

	while (cmd_id != ENN_MESH_CMD::C_END)
	{
		if (cmd_id == ENN_MESH_CMD::C_DIFF)
		{
			parseTex(next_data, cmd_id);
		}
		else if (cmd_id == ENN_MESH_CMD::C_DIFF2)
		{
			parseTex(next_data, cmd_id);
		}
		else if (cmd_id == ENN_MESH_CMD::C_LIGHTING)
		{
			int en = *next_data;
			++next_data;
			curr_temp_sub_mtr_->enableLighting(en != 0);
		}
		else if (cmd_id == ENN_MESH_CMD::C_NORMALMAP)
		{
			parseTex(next_data, cmd_id);
		}
		else if (cmd_id == ENN_MESH_CMD::C_SPECMAP)
		{
			parseTex(next_data, cmd_id);
		}
		else if (cmd_id == ENN_MESH_CMD::C_EMISSIVEMAP)
		{
			parseTex(next_data, cmd_id);
		}
		else if (cmd_id == ENN_MESH_CMD::C_TEXANI)
		{
			parseTexAni(next_data);
		}
		else if (cmd_id == ENN_MESH_CMD::C_CULLMODE)
		{
			int cm = *next_data;
			++next_data;

			curr_temp_sub_mtr_->setCullMode(cm);
		}
		else if (cmd_id == ENN_MESH_CMD::C_NAME)
		{
			const char* name = _loadAlignedString(next_data);
			curr_temp_sub_mtr_->setName(name);
		}
		else if (cmd_id == ENN_MESH_CMD::C_ALPHA_TEST)
		{
			float alphatest = *(float*)next_data;
			++next_data;

			curr_temp_sub_mtr_->setAlphaTest(alphatest);
		}
		
		cmd_id = *next_data;
		++next_data;
	}
}

void MeshResAutoCreator::onMeshBeginSubMtr(const ENN_MTR_HEAD_INFO* emhi)
{
	curr_temp_sub_mtr_->getDiffuse() = Color(emhi->diffuse);
	curr_temp_sub_mtr_->getSpecularShiness() = Color(emhi->specular);
	curr_temp_sub_mtr_->getEmissive() = Color(emhi->emissive);
	curr_temp_sub_mtr_->getSpecularShiness().a = (emhi->shininess);
}

void MeshResAutoCreator::parseTex(const int*& next_data, int cmd)
{
	int tex_name_len = *next_data;
	ENN_ASSERT((tex_name_len % 4) == 0);

	++next_data;

	const char* tex_name = (const char*)next_data;
	next_data = (const int*)(tex_name + tex_name_len);

	int tex_addr_mode = *next_data;
	++next_data;

	int tex_filter_mode = *next_data;
	++next_data;

	if (cmd == ENN_MESH_CMD::C_DIFF)
	{
		TextureUnit* tu = curr_temp_sub_mtr_->useTexDiff();

		uint32 addr_u = tex_addr_mode & 0x0f;
		uint32 addr_v = (tex_addr_mode >> 8) & 0x0f;
		uint32 min_filter = tex_filter_mode & 0x0f;
		uint32 mag_filter = (tex_filter_mode >> 8) & 0x0f;

		tu->setTexAddr(addr_u, addr_v);
		tu->setTexFilter(min_filter, mag_filter);

		tu->setTexName(tex_name);
	}
	else if (cmd == ENN_MESH_CMD::C_DIFF2)
	{
		TextureUnit* tu = curr_temp_sub_mtr_->useTexDiff2();

		uint32 addr_u = tex_addr_mode & 0x0f;
		uint32 addr_v = (tex_addr_mode >> 8) & 0x0f;
		uint32 min_filter = tex_filter_mode & 0x0f;
		uint32 mag_filter = (tex_filter_mode >> 8) & 0x0f;

		tu->setTexAddr(addr_u, addr_v);
		tu->setTexFilter(min_filter, mag_filter);

		tu->setTexName(tex_name);
	}
	else if (cmd == ENN_MESH_CMD::C_NORMALMAP)
	{
		TextureUnit* tu = curr_temp_sub_mtr_->useTexNormal();

		uint32 addr_u = tex_addr_mode & 0x0f;
		uint32 addr_v = (tex_addr_mode >> 8) & 0x0f;
		uint32 min_filter = tex_filter_mode & 0x0f;
		uint32 mag_filter = (tex_filter_mode >> 8) & 0x0f;

		tu->setTexAddr(addr_u, addr_v);
		tu->setTexFilter(min_filter, mag_filter);

		tu->setTexName(tex_name);
	}
	else if (cmd == ENN_MESH_CMD::C_SPECMAP)
	{
		TextureUnit* tu = curr_temp_sub_mtr_->useTexSpecular();

		uint32 addr_u = tex_addr_mode & 0x0f;
		uint32 addr_v = (tex_addr_mode >> 8) & 0x0f;
		uint32 min_filter = tex_filter_mode & 0x0f;
		uint32 mag_filter = (tex_filter_mode >> 8) & 0x0f;

		tu->setTexAddr(addr_u, addr_v);
		tu->setTexFilter(min_filter, mag_filter);

		tu->setTexName(tex_name);
	}
	else if (cmd == ENN_MESH_CMD::C_EMISSIVEMAP)
	{
		TextureUnit* tu = curr_temp_sub_mtr_->useTexEmissive();

		uint32 addr_u = tex_addr_mode & 0x0f;
		uint32 addr_v = (tex_addr_mode >> 8) & 0x0f;
		uint32 min_filter = tex_filter_mode & 0x0f;
		uint32 mag_filter = (tex_filter_mode >> 8) & 0x0f;

		tu->setTexAddr(addr_u, addr_v);
		tu->setTexFilter(min_filter, mag_filter);

		tu->setTexName(tex_name);
	}
	else
	{
		ENN_ASSERT(0);
	}

	int cmd_id = *next_data;
	++next_data;

	while (cmd_id != ENN_MESH_CMD::C_TU_END)
	{
		cmd_id = *next_data;
		++next_data;
	}

}

void MeshResAutoCreator::parseTexAni(const int*& next_data)
{
	/** not impl now */
	ENN_ASSERT(0);
}

void MeshResAutoCreator::onMeshBeginSubMesh(int sub_idx, 
											const ENN_SUB_MESH_HEAD_INFO* esmhi)
{
	curr_temp_sub_mesh_info_ = const_cast<ENN_SUB_MESH_HEAD_INFO*>(esmhi);
	curr_temp_sub_mesh_ = temp_mesh_->createNewSubMesh(sub_idx);

	AxisAlignedBox& mesh_aabb = curr_temp_sub_mesh_->getAABB();
	mesh_aabb.setExtents(curr_temp_sub_mesh_info_->aabb.vMin, curr_temp_sub_mesh_info_->aabb.vMax);
	curr_temp_sub_mesh_->setPrimType(curr_temp_sub_mesh_info_->prim_type_);
	curr_temp_sub_mesh_->setVertexType(curr_temp_sub_mesh_info_->vertex_type_);
	curr_temp_sub_mesh_->setIdxType(curr_temp_sub_mesh_info_->index_type_);
}

void MeshResAutoCreator::onMeshEndSubMesh()
{
	
}

void MeshResAutoCreator::loadSkeleton(const int*& data)
{
	if (!data)
	{
		return;
	}

	curr_temp_skel_ = temp_mesh_->createSkeleton();

	MemoryBuffer buff((void*)data, Math::ENN_MAX_UNSIGNED);

	int bone_cnt;
	buff.ReadBin(bone_cnt);

	ENN_ASSERT(bone_cnt > 0);

	/** bone info */
	for (int i = 0; i < bone_cnt; ++i)
	{
		String name = buff.ReadLenStringAlign4();

		int id;
		buff.ReadBin(id);

		vec3f pos;
		buff.ReadBin(pos);

		Quaternion quat;
		buff.ReadBin(quat);

		vec3f sca;
		buff.ReadBin(sca);

		Matrix4 offset_matrix;
		buff.Read(&offset_matrix.m[0][0], sizeof(offset_matrix));

		onSkelGetBone(name.c_str(), id, pos, quat, sca, offset_matrix);
	}

	/** bone parent child relationship */
	for (int i = 0; i < bone_cnt; ++i)
	{
		int i_child, i_parent;

		buff.ReadBin(i_child);
		buff.ReadBin(i_parent);

		onSkelGetBoneParent(i_child, i_parent);
	}

	/** animation info */
	int ani_cnt;
	buff.ReadBin(ani_cnt);

	for (int ia = 0; ia < ani_cnt; ++ia)
	{
		String ani_name = buff.ReadLenStringAlign4();
		float ani_len;
		buff.ReadBin(ani_len);

		curr_temp_anim_ = curr_temp_skel_->createAnimation(ani_name);
		curr_temp_anim_->setLength(ani_len);

		int ani_tracks;
		buff.ReadBin(ani_tracks);

		for (int it = 0; it < ani_tracks; ++it)
		{
			int id_track;
			buff.ReadBin(id_track);

			curr_temp_anim_track_ = curr_temp_anim_->createNodeTrack(id_track);
			int key_cnt;
			buff.ReadBin(key_cnt);

			for (int ik = 0; ik < key_cnt; ++ik)
			{
				float time_pos;
				buff.ReadBin(time_pos);

				vec3f pos;
				buff.ReadBin(pos);

				Quaternion quat;
				buff.ReadBin(quat);

				vec3f sca;
				buff.ReadBin(sca);

				NodeKeyFrame* kf = (NodeKeyFrame*)(curr_temp_anim_track_->createKeyFrame(time_pos));
				kf->setPosition(pos);
				kf->setRotation(quat);
				kf->setScale(sca);
			}
		}
	}

	/** 读取拆分映射表 */
	int sub_map_cnt;
	buff.ReadBin(sub_map_cnt);

	for (int im = 0; im < sub_map_cnt; ++im)
	{
		int idx_cnt;
		buff.ReadBin(idx_cnt);

		Skeleton::IndexMap idx_map(idx_cnt);
		buff.ReadBin(&idx_map[0], idx_cnt);

		curr_temp_skel_->setSubIndices(im, idx_map);
	}

	data = (const int*)((uchar*)buff.GetHandle() + buff.GetPosition());
}

void MeshResAutoCreator::onSkelGetBone(const char* name, int id, 
				   const vec3f& pos, const Quaternion& quat, const vec3f& sca,
				   const Matrix4& offset_matrix)
{
	BoneNode* bn = curr_temp_skel_->createBoneNode(name, id);
	bn->setInitPos(pos);
	bn->setInitRot(quat);
	bn->setInitScale(sca);
	bn->getOffsetMatrix() = offset_matrix;
}

void MeshResAutoCreator::onSkelGetBoneParent(int i_child, int i_parent)
{
	BoneNode* bn_child = curr_temp_skel_->getBoneNode(i_child);
	BoneNode* bn_parent = curr_temp_skel_->getBoneNode(i_parent);

	bn_child->setParent(bn_parent);
}

//////////////////////////////////////////////////////////////////////////
// MeshManager
MeshManager::MeshManager()
: curr_temp_save_mesh_(0)
, curr_temp_save_sub_mtr_(0)
, curr_temp_save_sub_mesh_(0)
, full_screen_mesh_(0)
, point_lit_mesh_(0)
, spot_lit_mesh_(0)
{

}

MeshManager::~MeshManager()
{

}

void MeshManager::init()
{
	ResourceManager::init();

	setAutoCreator(ENN_NEW MeshResAutoCreator());
}

Mesh* MeshManager::createCube(const String& name, float size)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshCubeCreatorImpl(size));
	}

	return mesh;
}

Mesh* MeshManager::createSphere(const String& name, float size, int segments)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshSphereCreatorImpl(size, segments));
	}

	return mesh;
}

Mesh* MeshManager::createPlane(const String& name, float x_len, float z_len, int x_seg, int z_seg)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshPlaneCreatorImpl(x_len, z_len, x_seg, z_seg));
	}

	return mesh;
}

Mesh* MeshManager::createMeshFromMqoTxt(const String& name, const String& sub_dir)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshMqoTxtCreatorImpl(sub_dir));
	}

	return mesh;
}

Mesh* MeshManager::createMeshFromPoly(const String& name, void* user_data)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW MeshPolyCreatorImpl());
	}

	mesh->setUserData(user_data);

	return mesh;
}

Mesh* MeshManager::createScreenMesh(const String& name, const ScreenRectDesc& screen_rect)
{
	Mesh* mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource(name));
	if (mesh)
	{
		mesh->setResCreator(ENN_NEW ScreenMeshCreatorImpl(screen_rect));
	}

	return mesh;
}

Resource* MeshManager::createImpl()
{
	Resource* res = ENN_NEW Mesh;
	res->setManager(this);

	return res;
}

Mesh* MeshManager::getFullScreenMesh()
{
	if (full_screen_mesh_)
	{
		return full_screen_mesh_;
	}

	full_screen_mesh_ = static_cast<Mesh*>(createResource( "full_screen_mesh" ));

	if (full_screen_mesh_)
	{
		full_screen_mesh_->setResCreator(ENN_NEW FullScreenMeshCreatorImpl);
	}

	full_screen_mesh_->load(false);

	return full_screen_mesh_;
}

Mesh* MeshManager::getPointLitMesh()
{
	if (point_lit_mesh_)
	{
		return point_lit_mesh_;
	}

	point_lit_mesh_ = static_cast<Mesh*>(createResource( "point_lit_mesh" ));

	const float minRadius = 1;
	const int   segments  = 15;

	float radius = minRadius / Math::cos( Math::PI / (segments - 1) );
	float size = radius * 2;

	point_lit_mesh_->setResCreator(ENN_NEW MeshSphereCreatorImpl(size, segments));
	point_lit_mesh_->load(false);

	return point_lit_mesh_;
}

Mesh* MeshManager::getSpotLitMesh()
{
	if (spot_lit_mesh_)
	{
		return spot_lit_mesh_;
	}

	spot_lit_mesh_ = static_cast<Mesh*>(createResource( "spot_lit_mesh" ));

	// 要求最小的半径和切分数
	const float minRadius = 1;
	const float height    = 1;
	const int   wsegments = 20;
	const int   hsegments = 2;

	float radius = minRadius / Math::cos( Math::PI / (wsegments - 1) );
	float width = radius * 2;

	spot_lit_mesh_->setResCreator(ENN_NEW MeshConeCreatorImpl(width, height, wsegments, hsegments, true));
	spot_lit_mesh_->load(false);

	return spot_lit_mesh_;
}

void MeshManager::saveMesh(Mesh* mesh, const String& file_name)
{
	const String& root_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	String full_file_name = root_path + "/mesh/" + file_name;

	File file(full_file_name.c_str(), FILE_WRITE);
	saveMesh(mesh, &file);
}

void MeshManager::saveMesh(Mesh* mesh, File* file)
{
	curr_temp_save_mesh_ = mesh;

	int sub_mesh_count = (int)(mesh->getSubMeshCount());

	ENN_ASSERT(sub_mesh_count > 0);

	/** ENN_HEAD_FILE_INFO */
	ENN_HEAD_FILE_INFO ehfi = {0};
	memcpy(ehfi.achr_file_flag, "enn", 3);
	ehfi.achr_version[0] = CURR_MESH_MAJOR_VERSION;
	ehfi.achr_version[1] = CURR_MESH_VICE_VERSION;
	ehfi.sub_mesh_count = sub_mesh_count;
	copyMeshInfo(&ehfi);

	file->Write(&ehfi, sizeof(ehfi));

	/** write material info */
	Material* mtr = curr_temp_save_mesh_->getMaterial();

	int mtr_cnt = mtr->getSubMtrCount();

	ENN_ASSERT(mtr_cnt > 0);

	file->Write(&mtr_cnt, sizeof(mtr_cnt));

	int curr_mtr_idx = mtr->firstMtrIdx();
	ENN_ASSERT(curr_mtr_idx != -1);

	while (curr_mtr_idx >= 0)
	{
		file->Write(&curr_mtr_idx, sizeof(curr_mtr_idx));

		curr_temp_save_sub_mtr_ = mtr->getSubMaterial(curr_mtr_idx);

		saveMeshSubMtr(file);

		curr_mtr_idx = mtr->nextMtrIdx();
	}

	curr_temp_save_sub_mtr_ = 0;

	/** write sub mesh */
	for (int sub_idx = 0; sub_idx < sub_mesh_count; ++sub_idx)
	{
		curr_temp_save_sub_mesh_ = mesh->getSubMesh(sub_idx);

		ENN_SUB_MESH_HEAD_INFO esmhi = {0};
		
		copySubMeshInfo(&esmhi);

		file->WriteBin(esmhi);

		/** write vertex data */
		int vertex_count = 0;
		void* vertex_data = 0;

		vertex_count = curr_temp_save_sub_mesh_->getVertexCount();
		vertex_data = curr_temp_save_sub_mesh_->getVertexData();

		ENN_ASSERT(vertex_data);

		file->WriteBin(vertex_count);
		file->Write(vertex_data, vertex_count * esmhi.vertex_bytes_);

		/** write index data */
		int index_count = 0;
		void* index_data = 0;

		index_count = curr_temp_save_sub_mesh_->getIndexCount();
		index_data = curr_temp_save_sub_mesh_->getIndexData();

		ENN_ASSERT(index_data);

		file->WriteBin(index_count);
		int indices_bytes = index_count * esmhi.index_bytes_;
		file->Write(index_data, indices_bytes);
		writeAlignedBland(file, indices_bytes);

		/** write sub mesh name */
		const char* sub_mesh_name = curr_temp_save_sub_mesh_->getName().c_str();
		writeAlignedName(file, sub_mesh_name);

		/** write mtr idx */
		int sub_mesh_mtr_idx = curr_temp_save_sub_mesh_->getMatIdx();
		file->WriteBin(sub_mesh_mtr_idx);
	}

	/** write skeleton info */
	if (Skeleton* skel = curr_temp_save_mesh_->getSkeleton())
	{
		writeCmd(file, ENN_MESH_CMD::C_SKELETON);

		writeSkeleton(file, skel);
	}

	writeCmd(file, ENN_MESH_CMD::C_END);

	file->Flush();
}

void MeshManager::copyMeshInfo(ENN_HEAD_FILE_INFO* ehfi)
{
	const AxisAlignedBox& aabb = curr_temp_save_mesh_->getAABB();
	const vec3f& vmin = aabb.getMinimum();
	const vec3f& vmax = aabb.getMaximum();

	ehfi->aabb.vMin[0] = vmin.x;
	ehfi->aabb.vMin[1] = vmin.y;
	ehfi->aabb.vMin[2] = vmin.z;

	ehfi->aabb.vMax[0] = vmax.x;
	ehfi->aabb.vMax[1] = vmax.y;
	ehfi->aabb.vMax[2] = vmax.z;
}

void MeshManager::saveMeshSubMtr(File* file)
{
	ENN_MTR_HEAD_INFO emhi = {0};
	int cmd_id = 0;

	copyMtrInfo(&emhi);

	file->Write(&emhi, sizeof(emhi));

	saveTexture(file, ENN_MESH_CMD::C_DIFF);
	saveTexture(file, ENN_MESH_CMD::C_DIFF2);
	saveTexture(file, ENN_MESH_CMD::C_NORMALMAP);
	saveTexture(file, ENN_MESH_CMD::C_SPECMAP);
	saveTexture(file, ENN_MESH_CMD::C_EMISSIVEMAP);

	/** is lighting enable */
	writeCmd(file, ENN_MESH_CMD::C_LIGHTING);
	int is_lighting_enable = curr_temp_save_sub_mtr_->isEnableLighting();
	file->WriteBin(is_lighting_enable);

	/** tex ani not impl */

	/** cull mode */
	writeCmd(file, ENN_MESH_CMD::C_CULLMODE);
	int cull_mode = (int)(curr_temp_save_sub_mtr_->getCullMode());
	file->WriteBin(cull_mode);

	/** mtr name */
	writeCmd(file, ENN_MESH_CMD::C_NAME);
	writeAlignedName(file, curr_temp_save_sub_mtr_->getName().c_str());

	/** alpha test */
	writeCmd(file, ENN_MESH_CMD::C_ALPHA_TEST);
	float ref_value = curr_temp_save_sub_mtr_->getAlphaTest();
	file->WriteBin(ref_value);

	/** end cmd */
	writeCmd(file, ENN_MESH_CMD::C_END);
}

void MeshManager::copyMtrInfo(ENN_MTR_HEAD_INFO* emhi)
{
	copyClr(emhi->diffuse, curr_temp_save_sub_mtr_->getDiffuse());
	copyClr(emhi->specular, curr_temp_save_sub_mtr_->getSpecularShiness());
	copyClr(emhi->emissive, curr_temp_save_sub_mtr_->getEmissive());
}

void MeshManager::copyClr(float dest[4], const Color& clr)
{
	memcpy(&dest[0], &clr.r, sizeof(clr));
}

void MeshManager::copyAABB(ENN_AABB_INFO* aabb, const AxisAlignedBox& aabb_src)
{
	const vec3f& vmin = aabb_src.getMinimum();
	const vec3f& vmax = aabb_src.getMaximum();

	aabb->vMin[0] = vmin.x;
	aabb->vMin[1] = vmin.y;
	aabb->vMin[2] = vmin.z;

	aabb->vMax[0] = vmax.x;
	aabb->vMax[1] = vmax.y;
	aabb->vMax[2] = vmax.z;
}

void MeshManager::saveTexture(File* file, int cmd_id)
{
	const char* tex_name = 0;
	int tex_addr_mode = 0;
	int tex_filter_mode = 0;

	if (cmd_id == ENN_MESH_CMD::C_DIFF)
	{
		TextureUnit* tu = curr_temp_save_sub_mtr_->getTexDiff();

		if (!tu)
		{
			return;
		}

		uint32 add_u = tu->getTexAddrU();
		uint32 add_v = tu->getTexAddrV();
		uint32 min_filter = tu->getTexFilterMin();
		uint32 mag_filter = tu->getTexFilterMag();
		const String& tu_name = tu->getTexName();

		tex_name = tu_name.c_str();
		tex_addr_mode = (add_u | (add_v << 8));
		tex_filter_mode = (min_filter | (mag_filter << 8));
	}
	else if (cmd_id == ENN_MESH_CMD::C_DIFF2)
	{
		TextureUnit* tu = curr_temp_save_sub_mtr_->getTexDiff2();

		if (!tu)
		{
			return;
		}

		uint32 add_u = tu->getTexAddrU();
		uint32 add_v = tu->getTexAddrV();
		uint32 min_filter = tu->getTexFilterMin();
		uint32 mag_filter = tu->getTexFilterMag();
		const String& tu_name = tu->getTexName();

		tex_name = tu_name.c_str();
		tex_addr_mode = (add_u | (add_v << 8));
		tex_filter_mode = (min_filter | (mag_filter << 8));
	}
	else if (cmd_id == ENN_MESH_CMD::C_NORMALMAP)
	{
		TextureUnit* tu = curr_temp_save_sub_mtr_->getTexNormal();

		if (!tu)
		{
			return;
		}

		uint32 add_u = tu->getTexAddrU();
		uint32 add_v = tu->getTexAddrV();
		uint32 min_filter = tu->getTexFilterMin();
		uint32 mag_filter = tu->getTexFilterMag();
		const String& tu_name = tu->getTexName();

		tex_name = tu_name.c_str();
		tex_addr_mode = (add_u | (add_v << 8));
		tex_filter_mode = (min_filter | (mag_filter << 8));
	}
	else if (cmd_id == ENN_MESH_CMD::C_SPECMAP)
	{
		TextureUnit* tu = curr_temp_save_sub_mtr_->getTexSpecular();

		if (!tu)
		{
			return;
		}

		uint32 add_u = tu->getTexAddrU();
		uint32 add_v = tu->getTexAddrV();
		uint32 min_filter = tu->getTexFilterMin();
		uint32 mag_filter = tu->getTexFilterMag();
		const String& tu_name = tu->getTexName();

		tex_name = tu_name.c_str();
		tex_addr_mode = (add_u | (add_v << 8));
		tex_filter_mode = (min_filter | (mag_filter << 8));
	}
	else if (cmd_id == ENN_MESH_CMD::C_EMISSIVEMAP)
	{
		TextureUnit* tu = curr_temp_save_sub_mtr_->getTexEmissive();

		if (!tu)
		{
			return;
		}

		uint32 add_u = tu->getTexAddrU();
		uint32 add_v = tu->getTexAddrV();
		uint32 min_filter = tu->getTexFilterMin();
		uint32 mag_filter = tu->getTexFilterMag();
		const String& tu_name = tu->getTexName();

		tex_name = tu_name.c_str();
		tex_addr_mode = (add_u | (add_v << 8));
		tex_filter_mode = (min_filter | (mag_filter << 8));
	}
	else
	{
		ENN_ASSERT(0);
		return;
	}

	/** first write cmd id */
	file->Write(&cmd_id, sizeof(cmd_id));
	
	int tex_name_len = (int)strlen(tex_name) + 1;
	int tex_name_adj = ENN_ALIGN_4_BYTES(tex_name_len);
	int tex_name_len_pad = tex_name_adj - tex_name_len;

	/** write tex name length */
	file->Write(&tex_name_adj, sizeof(tex_name_adj));

	/** write tex name */
	file->Write(tex_name, tex_name_len);
	
	if (tex_name_len_pad > 0)
	{
		char dummy[4] = {0};
		file->Write(dummy, tex_name_len_pad);
	}

	file->Write(&tex_addr_mode, sizeof(tex_addr_mode));
	file->Write(&tex_filter_mode, sizeof(tex_filter_mode));

	/** tu end cmd id */
	int cmd_end = ENN_MESH_CMD::C_TU_END;
	file->Write(&cmd_end, sizeof(cmd_end));
}

void MeshManager::writeCmd(File* file, int cmd_id)
{
	file->Write(&cmd_id, sizeof(cmd_id));
}

void MeshManager::writeAlignedName(File* file, const char* name)
{
	if (!name)
	{
		ENN_ASSERT(0);
		name = "";
	}

	int len = strlen(name) + 1;
	int adj_len = ENN_ALIGN_4_BYTES(len);
	int pad = adj_len - len;

	file->WriteBin(adj_len);
	file->Write(name, len);

	if (pad > 0)
	{
		char dummy[4] = {0};
		file->Write(dummy, pad);
	}
}

void MeshManager::writeAlignedBland(File* file, int bytes)
{
	int align_size = ENN_ALIGN_4_BYTES(bytes);
	int pad = align_size - bytes;

	if (pad)
	{
		char dummy[4] = {0};
		file->Write(dummy, pad);
	}
}

void MeshManager::copySubMeshInfo(ENN_SUB_MESH_HEAD_INFO* sub_mesh_info)
{
	copyAABB(&sub_mesh_info->aabb, curr_temp_save_sub_mesh_->getAABB());
	sub_mesh_info->prim_type_ = curr_temp_save_sub_mesh_->getPrimType();
	sub_mesh_info->vertex_type_ = curr_temp_save_sub_mesh_->getVertexType();
	sub_mesh_info->index_type_ = curr_temp_save_sub_mesh_->getIdxType();
	sub_mesh_info->vertex_bytes_ = curr_temp_save_sub_mesh_->getVertexSize();
	sub_mesh_info->index_bytes_ = curr_temp_save_sub_mesh_->getIdxSize();
}

void MeshManager::writeSkeleton(File* file, Skeleton* skel)
{
	int bone_cnt = skel->getBoneNodeCount();

	file->WriteBin(bone_cnt);

	for (int id = 0; id < bone_cnt; ++id)
	{
		vec3f pos;
		Quaternion quat;
		vec3f sca;

		BoneNode* bn = skel->getBoneNode(id);
		pos = bn->getInitPos();
		quat = bn->getInitRot();
		sca = bn->getInitScale();
		Matrix4 offset_matrix = bn->getOffsetMatrix();

		const String& str = bn->getName();

		file->WriteLenStringAlign4(str);
		file->WriteBin(id);
		file->WriteBin(pos);
		file->WriteBin(quat);
		file->WriteBin(sca);
		file->Write(&offset_matrix.m[0][0], sizeof(offset_matrix));
	}

	/** 其次写bone继承关系 */
	for (int i_child = 0; i_child < bone_cnt; ++i_child)
	{
		int i_parent;

		BoneNode* bn = skel->getBoneNode(i_child);

		i_parent = bn->getParent() ? bn->getParent()->getId() : -1;

		file->WriteBin(i_child);
		file->WriteBin(i_parent);
	}

	/** 写动画数据 */
	int ani_cnt = skel->getAnimationCount();
	file->WriteBin(ani_cnt);

	String ani_name;
	float ani_len;
	int ani_tracks;

	Animation* curr_ani = skel->firstAnimation(&ani_name);
	
	while (curr_ani)
	{
		ani_len = curr_ani->getLength();
		ani_tracks = curr_ani->getNumNodeTracks();

		file->WriteLenStringAlign4(ani_name);
		file->WriteBin(ani_len);
		file->WriteBin(ani_tracks);

		/** 写每个骨骼动画轨迹 */
		for (int ib = 0; ib < bone_cnt; ++ib)
		{
			int key_cnt;

			NodeAniTrack* track = curr_ani->getNodeTrack(ib);
			key_cnt = track->getKeyFrameCount();
			if (key_cnt <= 0)
			{
				continue;
			}

			file->WriteBin(ib);
			file->WriteBin(key_cnt);

			/** 写每个轨迹的每个关键帧 */
			for (int ik = 0; ik < key_cnt; ++ik)
			{
				float time_pos;
				vec3f pos;
				Quaternion quat;
				vec3f sca;

				NodeKeyFrame* kf = (NodeKeyFrame*)track->getKeyFrame(ik);

				ENN_ASSERT(kf);

				time_pos = kf->getTime();
				pos = kf->getPosition();
				quat = kf->getRotation();
				sca = kf->getScale();

				file->WriteBin(time_pos);
				file->WriteBin(pos);
				file->WriteBin(quat);
				file->WriteBin(sca);
			}
		}

		curr_ani = skel->nextAnimation(&ani_name);
	}

	/** 写拆分映射表 */
	int sub_map_cnt = skel->getSubIndicesCount();
	file->WriteBin(sub_map_cnt);
	for (int im = 0; im < sub_map_cnt; ++im)
	{
		const int* idx;
		int idx_cnt;

		const Skeleton::IndexMap* idx_map = skel->getSubIndices(im);

		idx = &(*idx_map)[0];
		idx_cnt = (int)idx_map->size();

		file->WriteBin(idx_cnt);
		file->WriteBin(idx, idx_cnt);
	}
}

}