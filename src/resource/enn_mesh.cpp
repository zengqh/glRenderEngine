/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/03
* File: enn_mesh.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_mesh.h"
#include "enn_material.h"
#include "enn_root.h"
#include "enn_gl_render_system.h"

#define DEFAULT_ANI_NAME    "__default_ani"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// BoneNode
BoneNode::BoneNode(const String& name, int id)
: name_(name)
, id_(id)
, parent_(0)
, init_scale_(Vector3::UNIT_SCALE)
, self_dirty_(false)
, parent_dirty_(false)
, mat_offset_matrix_(Matrix4::IDENTITY)
{

}

BoneNode::~BoneNode()
{
	
}

void BoneNode::setParent(BoneNode* bn)
{
	if (parent_ == bn)
	{
		return;
	}

	if (parent_)
	{
		parent_->removeChild(this);
	}

	parent_ = bn;
	bn->addChild(this);

	dirtySelf();
}

void BoneNode::setInitPos(const vec3f& pos)
{
	init_pos_ = pos;
	setCurPos(pos);
}

void BoneNode::setInitRot(const Quaternion& rot)
{
	init_rot_ = rot;
	setCurRot(rot);
}

void BoneNode::setInitScale(const vec3f& sca)
{
	init_scale_ = sca;
	setCurScale(sca);
}

void BoneNode::reset()
{
	setCurPos( init_pos_ );
	setCurRot( init_rot_ );
	setCurScale( init_scale_ );
}

const vec3f& BoneNode::getCurPos() const
{
	return cur_pos_;
}

const Quaternion& BoneNode::getCurRot() const
{
	return cur_rot_;
}

const vec3f& BoneNode::getCurScale() const
{
	return cur_scale_;
}

void BoneNode::setCurPos(const vec3f& pos)
{
	cur_pos_ = pos;
	dirtySelf();
}

void BoneNode::setCurRot(const Quaternion& rot)
{
	cur_rot_ = rot;
	dirtySelf();
}

void BoneNode::setCurScale(const vec3f& sca)
{
	cur_scale_ = sca;
	dirtySelf();
}

void BoneNode::translate(const vec3f& pos)
{
	cur_pos_ += pos;
	dirtySelf();
}

void BoneNode::rotate(const Quaternion& rot)
{
	// 在local空间旋转，而不是toParent空间
	// 也即先乘以当前的逆转到local，然后乘以rot，然后再回到toParent空间乘以当前（乘法顺序是从右开始）
	// m_curRot = m_curRot * rot * inverse( m_curRot ) * m_curRot;
	cur_rot_ = cur_rot_ * rot;
	dirtySelf();
}

void BoneNode::scale(float sca)
{
	cur_scale_ *= sca;
	dirtySelf();
}

const Matrix4& BoneNode::getMatInit() const
{
	const_cast<BoneNode*>(this)->updateMatrix();
	return mat_init_;
}

const Matrix4& BoneNode::getMatToParent() const
{
	const_cast<BoneNode*>(this)->updateMatrix();
	return mat_to_parent_;
}

const Matrix4& BoneNode::getMatFull() const
{
	const_cast<BoneNode*>(this)->updateMatrix();
	return mat_full_;
}

const Matrix4& BoneNode::getMatFullInit() const
{
	const_cast<BoneNode*>(this)->updateMatrix();
	return mat_full_init_;
}

Matrix4& BoneNode::getOffsetMatrix()
{
	return mat_offset_matrix_;
}

void BoneNode::updateReset()
{
	reset();

	ENN_FOR_EACH(BoneNodeList, children_, it)
	{
		BoneNode* child = *it;
		child->updateReset();
	}
}

void BoneNode::updateInitMatrix()
{
	mat_to_parent_.makeTransform(init_pos_, init_scale_, init_rot_);

	if (parent_)
	{
		mat_full_ = parent_->mat_full_ * mat_to_parent_;
	}
	else
	{
		mat_full_ = mat_to_parent_;
	}

	ENN_FOR_EACH(BoneNodeList, children_, it)
	{
		BoneNode* child = *it;
		child->updateInitMatrix();
	}
}

void BoneNode::updateTransform()
{
	updateMatrix();

	ENN_FOR_EACH(BoneNodeList, children_, it)
	{
		BoneNode* child = *it;
		child->updateMatrix();
	}
}

void BoneNode::addChild(BoneNode* bn)
{
	ENN_ASSERT(std::find(children_.begin(), children_.end(), bn) == children_.end());

	children_.push_back(bn);
}


void BoneNode::removeChild(BoneNode* bn)
{
	BoneNodeList::iterator it = std::remove( children_.begin(), children_.end(), bn );
	ENN_ASSERT( it != children_.end() );
	children_.erase( it, children_.end() );
}

void BoneNode::dirtySelf()
{
	self_dirty_ = true;

	ENN_FOR_EACH(BoneNodeList, children_, it)
	{
		BoneNode* child = *it;
		child->onParentDirty();
	}
}

void BoneNode::onParentDirty()
{
	parent_dirty_ = true;
	
	ENN_FOR_EACH(BoneNodeList, children_, it)
	{
		BoneNode* child = *it;
		child->onParentDirty();
	}
}

void BoneNode::updateMatrix()
{
	if (!(self_dirty_ || parent_dirty_))
	{
		return;
	}

	if (self_dirty_)
	{
		mat_to_parent_.makeTransform(cur_pos_, cur_scale_, cur_rot_);
	}

	if (self_dirty_ || parent_dirty_)
	{
		if (parent_)
		{
			mat_full_ = parent_->getMatFull() * mat_to_parent_;
		}
		else
		{
			mat_full_ = mat_to_parent_;
		}

		mat_full_init_ = mat_full_;
	}

	self_dirty_ = false;
	parent_dirty_ = false;
}

//////////////////////////////////////////////////////////////////////////
// Skeleton
Skeleton::Skeleton()
{
	
}

Skeleton::~Skeleton()
{
	clearAllAnimations();
	clearAllBoneNodes();
	clearAllSubIndices();
}

BoneNode* Skeleton::createBoneNode( const String& name, int id )
{
	if ( id >= (int)bones_.size() )
	{
		bones_.resize( id + 1 );
	}

	BoneNode*& bn = bones_[id];
	if ( !bn )
	{
		bn = new BoneNode( name, id );
	}

	return bn;
}

int       Skeleton::getBoneNodeCount() const
{
	return (int)bones_.size();
}

BoneNode* Skeleton::getBoneNode( int id )
{
	if (id >= 0 && id < (int)bones_.size())
	{
		return bones_[id];
	}
	
	return 0;
}

void      Skeleton::clearAllBoneNodes()
{
	int cnt = (int)bones_.size();

	for (int i = 0; i < cnt; ++i)
	{
		BoneNode* bn = bones_[i];
		ENN_SAFE_DELETE(bn);
	}

	bones_.clear();
}

Animation* Skeleton::createAnimation( const String& name )
{
	AnimationMap::iterator iter = anis_.find(name);
	if (iter != anis_.end())
	{
		ENN_ASSERT(0);
		return iter->second;
	}

	Animation*& ani = anis_[name];
	if (!ani)
	{
		ani = ENN_NEW Animation(-1);
	}

	return ani;
}

void Skeleton::insertAnimation( const String& name, Animation* anim )
{
	Animation*& ani = anis_[name];
	ENN_ASSERT(!ani);

	ani = anim;
}

Animation* Skeleton::getDefaultAnimation()
{
	return getAnimation( DEFAULT_ANI_NAME );
}

Animation* Skeleton::getAnimation( const String& name )
{
	AnimationMap::iterator it = anis_.find( name );
	if ( it != anis_.end() )
	{
		return it->second;
	}

	return 0;
}

Animation* Skeleton::firstAnimation( String* name )
{
	ani_it_ = anis_.begin();
	return nextAnimation( name );
}

Animation* Skeleton::nextAnimation( String* name )
{
	if ( ani_it_ != anis_.end() )
	{
		if ( name )
		{
			*name = ani_it_->first;
		}

		Animation* ani = ani_it_->second;
		++ani_it_;

		return ani;
	}

	return 0;
}

int        Skeleton::getAnimationCount()
{
	return (int)anis_.size();
}

void       Skeleton::clearAllAnimations()
{
	ENN_FOR_EACH(AnimationMap, anis_, it)
	{
		Animation* ani = it->second;
		ENN_SAFE_DELETE(ani);
	}

	anis_.clear();
}

bool Skeleton::hasEnoughIndices() const
{
	return bones_.size() <= MAX_BONES_NUM;
}

int  Skeleton::getSubIndicesCount() const
{
	return (int)indices_map_.size();
}

void Skeleton::setSubIndices( int i, const IndexMap& idxs )
{
	if ( i >= (int)indices_map_.size() )
	{
		indices_map_.resize( i + 1 );
	}

	IndexMap*& im = indices_map_[i];
	if ( !im )
	{
		im = ENN_NEW_T(IndexMap);
	}

	*im = idxs;
}

const Skeleton::IndexMap* Skeleton::getSubIndices( int i ) const
{
	if ( 0 <= i && i < (int)indices_map_.size() )
	{
		return indices_map_[i];
	}

	return 0;
}

void Skeleton::clearAllSubIndices()
{
	for ( size_t i = 0; i < indices_map_.size(); ++i )
	{
		IndexMap* im = indices_map_[i];
		ENN_SAFE_DELETE_T(im);
	}

	indices_map_.clear();
}

//////////////////////////////////////////////////////////////////////////
// SkeletonInstance
SkeletonInstance::SkeletonInstance()
: skel_(0)
, blend_state_(ANIMBLEND_AVERAGE)
, cur_total_weight_(0.0f)
{

}

SkeletonInstance::~SkeletonInstance()
{

}

void SkeletonInstance::init( Skeleton* skel )
{
	skel_ = skel;

	bones_ = &(skel_->getBones());

	mat_bones_ = ENN_NEW_ARRAY_T(Matrix4, bones_->size());

	derived_roots();

	createAnimationStates(skel_->getAnis(), ani_states_);
}

void SkeletonInstance::update( float elapsedTime )
{
	if (Math::realEqual(elapsedTime, 0.0f, 1e-5f))
	{
		return;
	}

	/** set init state */
	update_reset();

	/** update total weight */
	update_total_weight();

	/** update every animation state */
	ENN_FOR_EACH(AnimationStateMap, ani_states_, it)
	{
		AnimationState* state = it->second;

		if (state->isEnable())
		{
			state->addTime(elapsedTime);
		}
	}

	/** update transform */
	update_transform();

	update_matrix();
}

void SkeletonInstance::reset()
{
	ENN_FOR_EACH(AnimationStateMap, ani_states_, it)
	{
		AnimationState* ani_state = it->second;
		ani_state->setTimePos(0);
		//ani_state->setEnable(false);
	}

	update_reset();
	update_transform();
	update_matrix();
}

void SkeletonInstance::setNodeValue( int trackId, NodeKeyFrame* kf )
{
	float weight = 1.0f;
	BoneNode* bn = (*bones_)[trackId];

	vec3f trans(kf->getPosition());
	bn->setCurPos(trans);

	Quaternion rot = kf->getRotation();
	bn->setCurRot(rot);

	vec3f sca(kf->getScale());
	bn->setCurScale(sca);
}

void SkeletonInstance::setBlendState( int state )
{
	blend_state_ = state;
}

bool SkeletonInstance::getSubMatBones(int i, Matrix4* mat_bones, int* cnt) const
{
	const Skeleton::IndexMap* im = skel_->getSubIndices(i);

	if (!im)
	{
		return false;
	}

	int im_cnt = (int)im->size();

	for (int j = 0; j < im_cnt; ++j)
	{
		int idx = im->at(j);
		*mat_bones = mat_bones_[idx];
		++mat_bones;
	}

	*cnt = im_cnt;
	return true;
}

void SkeletonInstance::derived_roots()
{
	Skeleton::BoneNodeArray& bones = *bones_;

	for ( Skeleton::BoneNodeArray::iterator it = bones.begin(), ite = bones.end(); it != ite; ++it )
	{
		BoneNode* bn = *it;
		if ( !bn->getParent() )
		{
			root_bones_.push_back( bn );
		}
	}

	for ( Skeleton::BoneNodeArray::iterator it = root_bones_.begin(), ite = root_bones_.end(); it != ite; ++it )
	{
		BoneNode* bn = *it;
		bn->updateInitMatrix();
	}
}

void SkeletonInstance::createAnimationStates( SkeletonInstance::AnimationMap& anis, 
											 SkeletonInstance::AnimationStateMap& aniStats )
{
	cur_total_weight_ = (float)anis.size();

	ENN_FOR_EACH(SkeletonInstance::AnimationMap, anis, it)
	{
		const String& name = it->first;
		Animation* ani = it->second;
		AnimationState* stat = ENN_NEW AnimationState;

		stat->setAnimation( ani );
		stat->setAniCtrlObjSet(this);
		stat->setLoop( true );
		stat->setEnable( true );
		stat->setTimePos( 0 );

		aniStats[name] = stat;
	}

	reset();


}

void SkeletonInstance::update_reset()
{
	ENN_FOR_EACH(Skeleton::BoneNodeArray, root_bones_, it)
	{
		BoneNode* bn = *it;
		bn->updateReset();
	}
}

void SkeletonInstance::update_total_weight()
{
	if (blend_state_ == ANIMBLEND_AVERAGE)
	{
		cur_total_weight_ = 0.0f;
		ENN_FOR_EACH(AnimationStateMap, ani_states_, it)
		{
			AnimationState* stat = it->second;
			if (stat->isEnable())
			{
				cur_total_weight_ += stat->getWeight();
			}
		}
	}
	else
	{
		cur_total_weight_ = 1.0f;
	}
}

void SkeletonInstance::update_transform()
{
	ENN_FOR_EACH(Skeleton::BoneNodeArray, root_bones_, it)
	{
		BoneNode* bn = *it;
		bn->updateTransform();
	}
}

void SkeletonInstance::update_matrix()
{
	Skeleton::BoneNodeArray& bones = *bones_;

	int cnt = (int)bones.size();

	for (int i = 0; i < cnt; ++i)
	{
		BoneNode* bn = bones[i];

		Matrix4& mat = mat_bones_[i];
		mat = bn->getMatFull();
		mat = mat * bn->getOffsetMatrix();
		mat = mat.transpose();
	}
}

//////////////////////////////////////////////////////////////////////////
// SubMesh
SubMesh::SubMesh(Mesh* parent)
: parent_(parent)
, vb_(0)
, ib_(0)
, use_memory_(0)
, prim_type_(PRIM_TRIANGLELIST)
, vb_type_(0)
, ib_type_(IDX_16)
, vb_count_(0)
, ib_count_(0)
, stride_(0)
, mat_idx_(-1)
, vertex_data_(0)
, index_data_(0)
{

}

SubMesh::~SubMesh()
{
	ENN_SAFE_RELEASE(vb_);
	ENN_SAFE_RELEASE(ib_);
	ENN_SAFE_FREE(vertex_data_);
	ENN_SAFE_FREE(index_data_);
}

void SubMesh::setVertexCount(uint32 vertex_count)
{
	/** could not change the vb size. */
	if (vb_)
	{
		ENN_ASSERT(0);
		return;
	}

	ENN_ASSERT(vb_type_ > 0);
	ENN_ASSERT(vertex_count > 0);

	vb_count_ = vertex_count;

	stride_ = RenderSystem::GetVertexSize(vb_type_);
}

void SubMesh::setIndexCount(uint32 idx_count)
{
	if (ib_)
	{
		ENN_ASSERT(0);
		return;
	}

	ENN_ASSERT(idx_count > 0);
	ib_count_ = idx_count;
}

void SubMesh::setVertexData(void* data)
{
	uint32 vertex_buffer_size = stride_ * getVertexCount();

	if (!vertex_data_)
	{
		vertex_data_ = ENN_MALLOC(vertex_buffer_size);
	}

	memcpy(vertex_data_, data, vertex_buffer_size);
}

void SubMesh::setIndexData(void* data)
{
	uint32 index_buffer_size = getIdxSize() * getIndexCount();
	if (!index_data_)
	{
		index_data_ = ENN_MALLOC(index_buffer_size);
	}

	memcpy(index_data_, data, index_buffer_size);
}

void* SubMesh::getVertexData()
{
	if (!vertex_data_)
	{
		uint32 vertex_buffer_size = stride_ * getVertexCount();
		vertex_data_ = ENN_MALLOC(vertex_buffer_size);
	}

	return vertex_data_;
}


void* SubMesh::getIndexData()
{
	if (!index_data_)
	{
		uint32 index_buffer_size = getIdxSize() * getIndexCount();
		index_data_ = ENN_MALLOC(index_buffer_size);
	}

	return index_data_;
}

void SubMesh::makeGPUBuffer(bool free_cpu_data)
{
	/** vb */
	if (vertex_data_)
	{
		if (!vb_)
		{
			vb_ = VertexBuffer::createInstance();
			vb_->setUseMemory(use_memory_);
			vb_->createBuff(vb_count_ * stride_);
			vb_->setElementMask(vb_type_);
		}

		vb_->setData(vertex_data_);
	}
	
	if (index_data_)
	{
		if (!ib_)
		{
			ib_ = IndexBuffer::createInstance();
			ib_->setUseMemory(use_memory_);
			ib_->createBuff(ib_count_ * getIdxSize());
		}

		ib_->setData(index_data_);
	}
	

	ENN_SAFE_FREE(vertex_data_);
	ENN_SAFE_FREE(index_data_);

	if (free_cpu_data)
	{
		vb_->freeData();
		ib_->freeData();
	}
}

void SubMesh::freeCPUBuffer()
{
	ENN_ASSERT(vb_ && ib_);
	if (vb_) vb_->freeData();
	if (ib_) ib_->freeData();
}

void SubMesh::setName(const String& name)
{
	name_ = name;
}

void SubMesh::setPrimType(uint32 prim_type)
{
	/** 
	 if we have created vb and ib, we
	 should not change the type.
	 */
	ENN_ASSERT(!vb_);
	ENN_ASSERT(!ib_);

	prim_type_ = prim_type;
}

void SubMesh::setVertexType(uint32 vb_type)
{
	/** 
	 if we have created vb and ib, we
	 should not change the type.
	 */
	ENN_ASSERT(!vb_);

	vb_type_ = vb_type;
}

void SubMesh::setIdxType(uint32 ib_type)
{
	/** 
	 if we have created vb and ib, we
	 should not change the type.
	 */
	ENN_ASSERT(!ib_);

	ib_type_ = ib_type;
}

uint32 SubMesh::getVertexCount() const
{
	return vb_count_;
}

uint32 SubMesh::getIndexCount() const
{
	return ib_count_;
}

const String& SubMesh::getName() const
{
	return name_;
}

uint32 SubMesh::getPrimType() const
{
	return prim_type_;
}

uint32 SubMesh::getVertexType() const
{
	return vb_type_;
}

uint32 SubMesh::getIdxType() const
{
	return ib_type_;
}

uint32 SubMesh::getVertexSize() const
{
	return stride_;
}

uint32 SubMesh::getIdxSize() const
{
	if (ib_type_ == IDX_16)
	{
		return sizeof(uint16);
	}
	else if (ib_type_ == IDX_32)
	{
		return sizeof(int);
	}
	else
	{
		ENN_ASSERT(0);
		return 0;
	}
}

VertexBuffer* SubMesh::getVB()
{
	return vb_;
}

IndexBuffer* SubMesh::getIB()
{
	return ib_;
}

const VertexBuffer* SubMesh::getVB() const
{
	return vb_;
}

const IndexBuffer* SubMesh::getIB() const
{
	return ib_;
}

void SubMesh::setUseMemory(bool b)
{
	/** 
	 if we have created vb and ib, we
	 should not change the type.
	 */
	ENN_ASSERT(!vb_);
	ENN_ASSERT(!ib_);

	use_memory_ = b;
}

bool SubMesh::isUseMemory() const
{
	return use_memory_;
}

const AxisAlignedBox& SubMesh::getAABB() const
{
	return aabb_;
}

AxisAlignedBox& SubMesh::getAABB()
{
	return aabb_;
}

void SubMesh::updateAABB()
{
	ENN_ASSERT(vb_count_ > 0);

	if (!vb_count_)
	{
		return;
	}

	
	void* data = 0;

	/** vb is not created */
	if (!vb_)
	{
		if (!vertex_data_)
		{
			return;
		}
		else
		{
			data = vertex_data_;
		}
	}
	else
	{
		data = vb_->getData();
	}

	
	if (!data)
	{
		return;
	}

	aabb_.setNull();

	aabb_.merge((vec3f*)data, vb_count_, stride_);
}

void SubMesh::setMatIdx(int sub_idx)
{
	mat_idx_ = sub_idx;
}

int SubMesh::getMatIdx() const
{
	return mat_idx_;
}

SubMaterial* SubMesh::getMaterial() const
{
	return parent_->getMaterial()->getSubMaterial(mat_idx_);
}

uint32 SubMesh::getPrimCount() const
{
	uint32 idx_count = getIndexCount();

	return idx_count / getPrimVertexCount();
}

uint32 SubMesh::getPrimVertexCount() const
{
	switch (prim_type_)
	{
	case PRIM_TRIANGLELIST:
		return 3;
	case PRIM_LINELIST:
		return 2;
	case PRIM_POINTLIST:
		return 1;
	default:
		ENN_ASSERT(0);
		return 0;
	}
}

uint32 SubMesh::getPrimVertexIndex(uint32 prim, uint32 vtx) const
{
	uint32 idx = prim * getPrimVertexCount() + vtx;
	return getIndex(idx);
}

uint32 SubMesh::getIndex( uint32 i ) const
{
	const void* data = 0;

	if (ib_)
	{
		data = getIB()->getData();
	}
	else
	{
		data = index_data_;
	}

	ENN_ASSERT(data);

	const uchar* udata = static_cast<const uchar*>(data);

	const uchar* i_curr_data = udata + i * getIdxSize();

	if (ib_type_ == IDX_16)
	{
		return *((const ushort*)i_curr_data);
	}
	else if (ib_type_ == IDX_32)
	{
		return *((const uint32*)i_curr_data);
	}
	else
	{
		ENN_ASSERT(0);
		return 0;
	}
}

const void* SubMesh::getVertex(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getVertex(i);
}

void* SubMesh::getVertex(uint32 i)
{
	uchar* data = 0;
	if (vb_)
	{
		data = static_cast<uchar*>(vb_->getData());
	}
	else
	{
		data = static_cast<uchar*>(vertex_data_);
	}

	ENN_ASSERT(data);

	return data + i * getVertexSize();
}

const vec3f* SubMesh::getPosition(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getPosition(i);
}

vec3f* SubMesh::getPosition(uint32 i)
{
	/** off set must be zero */
	return (vec3f*)(getVertex(i));
}

const vec3f* SubMesh::getNormal(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getNormal(i);
}

vec3f* SubMesh::getNormal(uint32 i)
{
	uint32 normal_offset = RenderSystem::GetElementOffset(vb_type_, ELEMENT_NORMAL);

	return (vec3f*)((uchar*)getVertex(i) + normal_offset);
}

const vec3f* SubMesh::getTangent(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getTangent(i);
}

vec3f* SubMesh::getTangent(uint32 i)
{
	uint32 tangent_offset = RenderSystem::GetElementOffset(vb_type_, ELEMENT_TANGENT);
	return (vec3f*)((uchar*)getVertex(i) + tangent_offset);
}

const vec2f* SubMesh::getUV1(uint32 i) const
{
	return const_cast<SubMesh*>(this)->getUV1(i);
}

vec2f* SubMesh::getUV1(uint32 i)
{
	uint32 uv1_offset = RenderSystem::GetElementOffset(vb_type_, ELEMENT_TEXCOORD1);
	return (vec2f*)((uchar*)getVertex(i) + uv1_offset);
}

const void* SubMesh::getElement(uint32 i, int element) const
{
	return const_cast<SubMesh*>(this)->getElement(i, element);
}

void* SubMesh::getElement(uint32 i, int element)
{
	uint32 offset = RenderSystem::GetElementOffset(vb_type_, (VertexElement)element);
	return ((uchar*)getVertex(i) + offset);
}

vec3f SubMesh::getNormalByFace( uint32 i ) const
{
	uint32 i0 = getPrimVertexIndex(i, 0);
	uint32 i1 = getPrimVertexIndex(i, 1);
	uint32 i2 = getPrimVertexIndex(i, 2);

	const vec3f& v0 = *getPosition(i0);
	const vec3f& v1 = *getPosition(i1);
	const vec3f& v2 = *getPosition(i2);

	vec3f va = v1 - v0;
	vec3f vb = v2 - v1;
	vec3f n = va.crossProduct(vb);

	/** 考虑到面积因素，不作单位化 */
	return n;
}

bool SubMesh::calcNormal()
{
	if (prim_type_ != PRIM_TRIANGLELIST)
	{
		return false;
	}

	if (!(vb_type_ & MASK_NORMAL))
	{
		return false;
	}

	if (getPrimCount() <= 0)
	{
		return false;
	}

	IntListList adjList;
	getVertexAdjList(adjList);

	uint32 vtx_cnt = getVertexCount();
	for (uint32 i = 0; i < vtx_cnt; ++i)
	{
		vec3f n(0.0f, 0.0f, 0.0f);

		const IntList& adjs = adjList[i];
		for (uint32 j = 0; j < adjs.size(); ++j)
		{
			uint32 face_idx = adjs[j];
			n += getNormalByFace(face_idx);
		}

		vec3f normalized_n = n.normalisedCopy();

		if (Math::isVector3Zero(normalized_n))
		{
			normalized_n = Vector3::UNIT_Y;
		}

		*getNormal(i) = normalized_n;
	}

	if (vb_) vb_->updateGPUData();

	return true;
}

bool SubMesh::calcTangent()
{
	if (prim_type_ != PRIM_TRIANGLELIST)
	{
		return false;
	}

	if (!(vb_type_ & MASK_TANGENT))
	{
		return false;
	}

	uint32 prim_count = getPrimCount();
	if (prim_count <= 0)
	{
		return false;
	}

	uint32 vtx_cnt = getVertexCount();

	enn::vector<vec3f>::type face_tangents(prim_count);
	enn::vector<vec3f>::type face_binormals(prim_count);
	enn::vector<IntList>::type vtx_faces(vtx_cnt);

	for (uint32 i = 0; i < prim_count; ++i)
	{
		uint32 i0 = getPrimVertexIndex( i, 0 );
		uint32 i1 = getPrimVertexIndex( i, 1 );
		uint32 i2 = getPrimVertexIndex( i, 2 );

		const vec3f& v0 = *getPosition( i0 );
		const vec3f& v1 = *getPosition( i1 );
		const vec3f& v2 = *getPosition( i2 );

		const vec2f& uv0 = *getUV1( i0 );
		const vec2f& uv1 = *getUV1( i1 );
		const vec2f& uv2 = *getUV1( i2 );

		Math::cal_tangent(v0, v1, v2, uv0, uv1, uv2, face_tangents[i], face_binormals[i]);

		vtx_faces[i0].push_back(i);
		vtx_faces[i1].push_back(i);
		vtx_faces[i2].push_back(i);
	}

	for (uint32 i = 0; i < vtx_cnt; ++i)
	{
		enn::vector<uint32>::type& faces = vtx_faces[i];

		const vec3f& normal = *getNormal(i);

		vec3f tanget(0,0,0);
		vec3f binormal(0,0,0);

		for ( size_t j = 0; j < faces.size(); ++j )
		{
			uint32 fi = faces[j];
			tanget   += face_tangents[fi];
			binormal += face_binormals[fi];
		}

		Math::gramSchmidtOrthogonalize(tanget, binormal, normal);
		*getTangent(i) = tanget;
	}

	vb_->updateGPUData();

	return true;
}

void SubMesh::getVertexAdjList(SubMesh::IntListList& adjList)
{
	adjList.resize(getVertexCount());

	uint32 prim_count = getPrimCount();
	for (uint32 i = 0; i < prim_count; ++i)
	{
		uint32 i0 = getPrimVertexIndex(i, 0);
		uint32 i1 = getPrimVertexIndex(i, 1);
		uint32 i2 = getPrimVertexIndex(i, 2);

		adjList[i0].push_back(i);
		adjList[i1].push_back(i);
		adjList[i2].push_back(i);
	}
}

bool SubMesh::rebuildMesh(uint32 new_mask)
{
	uint32 old_mask = getVertexType();
	uint32 vtx_cnt = getVertexCount();

	uint32 new_vertex_bytes = RenderSystem::GetVertexSize(new_mask);

	if (new_vertex_bytes == 0)
	{
		ENN_ASSERT(0);
		return false;
	}

	uchar* new_data = (uchar*)ENN_MALLOC(vtx_cnt * new_vertex_bytes);

	for (uint32 iv = 0; iv < vtx_cnt; ++iv)
	{
		uchar* curr_vtx_begin = new_data + iv * new_vertex_bytes;
		uchar* curr_vtx_element = curr_vtx_begin;

		for (uint32 i = 0; i < ELEMENT_NUM; ++i)
		{
			uint32 element_size = RenderSystem::elementSize[i];

			if (new_mask & BITS(i))
			{
				if (old_mask & BITS(i))
				{
					void* old_element_ptr = getElement(iv, i);

					memcpy(curr_vtx_element, old_element_ptr, element_size);
				}
				
				curr_vtx_element += element_size;
			}
		}
	}
	

	ENN_SAFE_RELEASE(vb_);

	setVertexType(new_mask);
	setVertexCount(vb_count_);
	setVertexData(new_data);
	makeGPUBuffer();

	ENN_SAFE_FREE(new_data);

	return true;
}

//////////////////////////////////////////////////////////////////////////
Mesh::Mesh()
: material_(0)
, skel_(0)
{
	setLoadPriority(MODEL_LOAD_PRIORITY);
}

Mesh::~Mesh()
{
	clear();

	ENN_SAFE_RELEASE(material_);
}

uint32 Mesh::getSubMeshCount() const
{
	return sub_mesh_list_.size();
}

const SubMesh* Mesh::getSubMesh(int i) const
{
	SubMeshList::const_iterator iter = sub_mesh_list_.find(i);

	ENN_ASSERT(iter != sub_mesh_list_.end());

	return iter->second;
}

SubMesh* Mesh::getSubMesh(int i)
{
	SubMeshList::iterator iter = sub_mesh_list_.find(i);

	ENN_ASSERT(iter != sub_mesh_list_.end());

	return iter->second;
}

const SubMesh* Mesh::getSubMeshByName(const String& name) const
{
	ENN_FOR_EACH_CONST(SubMeshList, sub_mesh_list_, it)
	{
		if (it->second->getName() == name)
		{
			return it->second;
		}
	}

	return 0;
}

SubMesh* Mesh::getSubMeshByName(const String& name)
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		if (it->second->getName() == name)
		{
			return it->second;
		}
	}

	return 0;
}

const Mesh::SubMeshList& Mesh::getSubMeshList() const
{
	return sub_mesh_list_;
}

Mesh::SubMeshList& Mesh::getSubMeshList()
{
	return sub_mesh_list_;
}

void Mesh::makeGPUBuffer(bool free_cpu_data)
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->makeGPUBuffer(free_cpu_data);
	}
}

void Mesh::freeCPUBuffer()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->freeCPUBuffer();
	}
}

SubMesh* Mesh::createNewSubMesh(int i)
{
	SubMesh* sub_mesh = ENN_NEW SubMesh(this);

	sub_mesh_list_.insert(SubMeshList::value_type(i, sub_mesh));

	return sub_mesh;
}

void Mesh::destorySubMesh(int i)
{
	SubMeshList::iterator it = sub_mesh_list_.find(i);
	if (it != sub_mesh_list_.end())
	{
		SubMesh* sub_mesh = it->second;
		ENN_SAFE_DELETE(sub_mesh);

		sub_mesh_list_.erase(it);
	}
}

void Mesh::clear()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		ENN_SAFE_DELETE(sub_mesh);
	}

	sub_mesh_list_.clear();
}

void Mesh::updateAABB(bool update_sub)
{
	int cnt = getSubMeshCount();
	if (cnt <= 0)
	{
		aabb_.setNull();
	}

	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;

		ENN_ASSERT(sub_mesh);
		
		if (update_sub)
		{
			sub_mesh->updateAABB();
		}

		const AxisAlignedBox& sub_aabb = sub_mesh->getAABB();
		aabb_.merge(sub_aabb);
	}

}

const AxisAlignedBox& Mesh::getAABB() const
{
	return aabb_;
}

AxisAlignedBox& Mesh::getAABB()
{
	return aabb_;
}

void Mesh::setMaterial(Material* mat)
{
	material_ = mat;

	material_->addRef();

}

Material* Mesh::getMaterial() const
{
	return material_;
}

Skeleton* Mesh::createSkeleton()
{
	ENN_ASSERT(!skel_);

	skel_ = ENN_NEW Skeleton;
	return skel_;
}

const Skeleton* Mesh::getSkeleton() const
{
	return skel_;
}

Skeleton* Mesh::getSkeleton()
{
	return skel_;
}

void Mesh::calcNormal()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->calcNormal();
	}
}

void Mesh::calcTangent()
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->calcTangent();
	}
}

void Mesh::rebuildMesh(uint32 new_mask)
{
	ENN_FOR_EACH(SubMeshList, sub_mesh_list_, it)
	{
		SubMesh* sub_mesh = it->second;
		sub_mesh->rebuildMesh(new_mask);
	}
}

}