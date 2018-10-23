/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/03
* File: enn_mesh.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_resource.h"
#include "enn_gl_vb.h"
#include "enn_gl_ib.h"
#include "enn_material.h"
#include "enn_animation.h"

namespace enn
{
class SubMesh;
class Skeleton;
class SubMaterial;
class Mesh;

#define MAX_BONES_NUM       60

enum SkeletonAnimationBlendMode 
{
	ANIMBLEND_AVERAGE       = 0,
	ANIMBLEND_CUMULATIVE    = 1
};

//////////////////////////////////////////////////////////////////////////
// BoneNode
class BoneNode : public AllocatedObject, public Noncopyable
{
	typedef enn::vector<BoneNode*>::type BoneNodeList;
public:
	BoneNode(const String& name, int id);
	~BoneNode();

public:
	const String& getName() const
	{
		return name_;
	}

	int getId() const
	{
		return id_;
	}

public:
	const BoneNode* getParent() const
	{
		return parent_;
	}

	BoneNode* getParent()
	{
		return parent_;
	}

	void setParent(BoneNode* bn);

public:
	vec3f& getInitPos()
	{
		return init_pos_;
	}

	Quaternion& getInitRot()
	{
		return init_rot_;
	}

	vec3f& getInitScale()
	{
		return init_scale_;
	}

	void setInitPos(const vec3f& pos);
	void setInitRot(const Quaternion& rot);
	void setInitScale(const vec3f& sca);
	void reset();

public:
	const vec3f& getCurPos() const;
	const Quaternion& getCurRot() const;
	const vec3f& getCurScale() const;

	void setCurPos(const vec3f& pos);
	void setCurRot(const Quaternion& rot);
	void setCurScale(const vec3f& sca);

public:
	void translate(const vec3f& pos);
	void rotate(const Quaternion& rot);
	void scale(float sca);

public:
	const Matrix4& getMatInit() const;
	const Matrix4& getMatToParent() const;
	const Matrix4& getMatFull() const;
	const Matrix4& getMatFullInit() const;
	Matrix4& getOffsetMatrix();

public:
	void updateReset();
	void updateInitMatrix();
	void updateTransform();

public:
	void addChild(BoneNode* bn);
	void removeChild(BoneNode* bn);

protected:
	void dirtySelf();
	void onParentDirty();
	void updateMatrix();

protected:
	String				name_;
	int					id_;

	vec3f				init_pos_;
	Quaternion			init_rot_;
	vec3f				init_scale_;

	vec3f				cur_pos_;
	Quaternion			cur_rot_;
	vec3f				cur_scale_;

	Matrix4				mat_init_;
	Matrix4				mat_to_parent_;
	Matrix4				mat_full_;
	Matrix4				mat_full_init_;
	Matrix4				mat_offset_matrix_;

	BoneNode*			parent_;
	BoneNodeList		children_;

	bool				self_dirty_;
	bool				parent_dirty_;
};

//////////////////////////////////////////////////////////////////////////
// Skeleton
class Skeleton : public AllocatedObject, public Noncopyable
{
public:
	typedef enn::vector<BoneNode*>::type              BoneNodeArray;
	typedef enn::map<std::string, Animation*>::type   AnimationMap;
	typedef enn::vector<int>::type                    IndexMap;
	typedef enn::vector<IndexMap*>::type              IndicesMap;

public:
	Skeleton();
	~Skeleton();

public:
	BoneNode* createBoneNode( const String& name, int id );
	int       getBoneNodeCount() const;
	BoneNode* getBoneNode( int id );
	void      clearAllBoneNodes();

public:
	Animation* createAnimation( const String& name );
	void	   insertAnimation( const String& name, Animation* anim );
	Animation* getDefaultAnimation();
	Animation* getAnimation( const String& name );
	Animation* firstAnimation( String* name = 0 );
	Animation* nextAnimation( String* name = 0 );
	int        getAnimationCount();
	void       clearAllAnimations();

public:
	BoneNodeArray& getBones() 
	{ 
		return bones_; 
	}

	AnimationMap& getAnis() 
	{ 
		return anis_; 
	}

public:
	bool hasEnoughIndices() const;
	int  getSubIndicesCount() const;
	void setSubIndices( int i, const IndexMap& idxs );
	const IndexMap* getSubIndices( int i ) const;
	void clearAllSubIndices();

private:
	BoneNodeArray 				bones_;
	AnimationMap  				anis_;
	IndicesMap    				indices_map_;
	AnimationMap::iterator		ani_it_;
};

//////////////////////////////////////////////////////////////////////////
// SkeletonInstance
class SkeletonInstance : public IAniCtrlObjSet, public AllocatedObject
{
	typedef enn::map<std::string, AnimationState*>::type    AnimationStateMap;
	typedef enn::map<std::string, Animation*>::type			AnimationMap;

public:
	SkeletonInstance();
	~SkeletonInstance();

public:
	void init( Skeleton* skel );
	void update( float elapsedTime );
	void reset();

	virtual void setNodeValue( int trackId, NodeKeyFrame* kf );

public:
	Skeleton* getSkeleton() const 
	{ 
		return skel_; 
	}

	const Matrix4* getMatBones() const
	{
		return mat_bones_;
	}

	bool getSubMatBones(int i, Matrix4* mat_bones, int* cnt) const;

public:
	void setBlendState( int state );

	int  getBlendState() const 
	{
		return blend_state_; 
	}

protected:
	void derived_roots();
	void createAnimationStates( AnimationMap& anis, AnimationStateMap& aniStats );

	void update_reset();
	void update_total_weight();
	void update_transform();
	void update_matrix();

protected:
	Skeleton*						skel_;
	int								blend_state_;
	Skeleton::BoneNodeArray*		bones_;
	Skeleton::BoneNodeArray			root_bones_;
	float							cur_total_weight_;
	AnimationStateMap				ani_states_;
	Matrix4*						mat_bones_;
};

//////////////////////////////////////////////////////////////////////////
// sub mesh
class SubMesh : public AllocatedObject
{
	typedef enn::vector<uint32>::type		IntList;
	typedef enn::vector<IntList>::type		IntListList;

public:
	SubMesh(Mesh* parent);
	virtual ~SubMesh();

public:
	/** set */
	void setVertexCount(uint32 vertex_count);
	
	void setIndexCount(uint32 idx_count);

	void setVertexData(void* data);

	void setIndexData(void* data);

	void* getVertexData();

	void* getIndexData();

	void makeGPUBuffer(bool free_cpu_data = false);

	void freeCPUBuffer();

	void setName(const String& name);

	void setPrimType(uint32 prim_type);

	void setVertexType(uint32 vb_type);

	void setIdxType(uint32 ib_type);

	/** get */
	uint32 getVertexCount() const;

	uint32 getIndexCount() const;

	const String& getName() const;

	uint32 getPrimType() const;

	uint32 getVertexType() const;

	uint32 getIdxType() const;

	uint32 getVertexSize() const;

	uint32 getIdxSize() const;

	VertexBuffer* getVB();
	IndexBuffer* getIB();

	const VertexBuffer* getVB() const;
	const IndexBuffer* getIB() const;

	void setUseMemory(bool b);
	bool isUseMemory() const;

	const AxisAlignedBox& getAABB() const;
	AxisAlignedBox& getAABB();
	void updateAABB();

	void setMatIdx(int sub_idx);
	int getMatIdx() const;
	SubMaterial* getMaterial() const;

public:
	uint32 getPrimCount() const;
	uint32 getPrimVertexCount() const;
	uint32 getPrimVertexIndex(uint32 prim, uint32 vtx) const;

	uint32 getIndex(uint32 i) const;

	const void* getVertex(uint32 i) const;
	void* getVertex(uint32 i);

	const vec3f* getPosition(uint32 i) const;
	vec3f* getPosition(uint32 i);

	const vec3f* getNormal(uint32 i) const;
	vec3f* getNormal(uint32 i);

	const vec3f* getTangent(uint32 i) const;
	vec3f* getTangent(uint32 i);

	const vec2f* getUV1(uint32 i) const;
	vec2f* getUV1(uint32 i);

	const void* getElement(uint32 i, int element) const;
	void* getElement(uint32 i, int element);

	vec3f getNormalByFace( uint32 i ) const;

public:
	bool calcNormal();
	bool calcTangent();

public:
	bool rebuildMesh(uint32 new_mask);

protected:
	void getVertexAdjList(IntListList& adjList);

protected:
	Mesh*				parent_;
	String				name_;
	AxisAlignedBox		aabb_;
	VertexBuffer*		vb_;
	IndexBuffer*		ib_;
	uint32				vb_count_;
	uint32				ib_count_;
	uint32				stride_;
	bool				use_memory_;

	String				mat_name_;
	int					mat_idx_;

	uint32				prim_type_;
	uint32				vb_type_;
	uint32				ib_type_;

	void*				vertex_data_;
	void*				index_data_;
};

//////////////////////////////////////////////////////////////////////////
// mesh
class Mesh : public Resource
{
	ENN_DECLARE_RTTI(Mesh)

	enum {MODEL_LOAD_PRIORITY = 0};
public:
	typedef enn::map<int, SubMesh*>::type SubMeshList;

public:
	Mesh();
	virtual ~Mesh();

	uint32 getSubMeshCount() const;

	const SubMesh* getSubMesh(int i) const;
	SubMesh* getSubMesh(int i);

	const SubMesh* getSubMeshByName(const String& name) const;
	SubMesh* getSubMeshByName(const String& name);

	const SubMeshList& getSubMeshList() const;
	SubMeshList& getSubMeshList();

	void makeGPUBuffer(bool free_cpu_data = false);
	void freeCPUBuffer();

public:
	SubMesh* createNewSubMesh(int i);
	void destorySubMesh(int i);
	void clear();

public:
	void updateAABB(bool update_sub = false);
	const AxisAlignedBox& getAABB() const;
	AxisAlignedBox& getAABB();

public:
	void setMaterial(Material* mat);
	Material* getMaterial() const;

public:
	Skeleton* createSkeleton();
	const Skeleton* getSkeleton() const;
	Skeleton* getSkeleton();

public:
	void calcNormal();
	void calcTangent();

public:
	void rebuildMesh(uint32 new_mask);

protected:
	SubMeshList			sub_mesh_list_;
	AxisAlignedBox		aabb_;
	Material*			material_;
	Skeleton*			skel_;
};

typedef ResPtr<Mesh> MeshPtr;

}