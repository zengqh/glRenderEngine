/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/11
* File: Node.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_node.h"

namespace enn
{
Node::Node()
: scale_(vec3f::UNIT_SCALE)
, parent_(0)
{
	dirty_flag_.set_flag(SELF_TRANSFORM_DIRTY);
	dirty_flag_.set_flag(PARENT_TRANSFORM_DIRTY);
}

Node::~Node()
{

}

void Node::addChild(Node* node)
{
	NodeList::iterator iter = std::find(child_list_.begin(), child_list_.end(), node);

	if (iter != child_list_.end())
	{
		ENN_ASSERT(0);
		return;
	}

	node->parent_ = this;
	child_list_.push_back(node);

	node->updateDerivedByAdd();
}

void Node::removeChild(Node* node)
{
	NodeList::iterator iter = std::find(child_list_.begin(), child_list_.end(), node);

	if (iter == child_list_.end())
	{
		ENN_ASSERT(0);
		return;
	}

	node->parent_ = 0;
	child_list_.erase(iter);

	node->updateDerivedByRemove();
}

void Node::removeAllChild()
{
	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->parent_ = 0;
		node->updateDerivedByRemove();
	}
}

void Node::updateDerivedByAdd()
{
	onSelfAdd();

	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->updateDerivedByAdd();
	}
}

void Node::updateDerivedByRemove()
{
	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->updateDerivedByRemove();
	}

	onSelfRemove();
}

void Node::onSelfAdd()
{
	setParentTransformDirty();
}

void Node::onSelfRemove()
{
	setParentTransformDirty();
}

void Node::setDerivedSelfTransformDirty()
{
	/** 设置自己脏标记 */
	setSelfTransformDirty();

	/** 同步通知孩子，告之我脏了 */
	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->setDerivedParentTransformDirty();
	}
}

void Node::setDerivedParentTransformDirty()
{
	/** 孩子收到父脏后，设置标记，并告之孩子的孩子 */
	setParentTransformDirty();

	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->setDerivedParentTransformDirty();
	}
}

void Node::setSelfTransformDirty()
{
	dirty_flag_.set_flag(SELF_TRANSFORM_DIRTY);
}

void Node::setParentTransformDirty()
{
	dirty_flag_.set_flag(PARENT_TRANSFORM_DIRTY);
}

void Node::setPosition(const vec3f& pos)
{
	position_ = pos;
	setDerivedSelfTransformDirty();
}

void Node::setPosition(float x, float y, float z)
{
	setPosition(vec3f(x, y, z));
}

void Node::setWorldPosition(const vec3f& pos)
{
	setPosition(!parent_ ? pos : parent_->getDerivedMatrix().inverse() * pos);
}

void Node::setWorldPosition(float x, float y, float z)
{
	setWorldPosition(Vector3(x, y, z));
}

void Node::setOrientation(const Quaternion& ori)
{
	orientation_ = ori;
	orientation_.normalise();

	setDerivedSelfTransformDirty();
}

void Node::resetOrientation()
{
	setOrientation(Quaternion::IDENTITY);
}

void Node::setScale(const vec3f& sca)
{
	scale_ = sca;

	setDerivedSelfTransformDirty();
}

void Node::setScale(float x, float y, float z)
{
	setScale(vec3f(x, y, z));
}

void Node::translate(const vec3f& d, TransformSpace ts)
{
	switch (ts)
	{
	case TS_LOCAL:
		position_ += orientation_ * d;
		break;
	case TS_PARENT:
		position_ += d;
		break;
	case TS_WORLD:
		/** 把平移向量d从世界空间转换到父空间(m_position在父空间定义) */
		if ( parent_ )
			position_ += parent_->getDerivedMatrix().inverse().transformAffine(d);
		else
			position_ += d;
		break;
	}
	
	setDerivedSelfTransformDirty();
}

void Node::translate(float x, float y, float z, TransformSpace ts)
{
	translate(vec3f(x, y, z), ts);
}

void Node::rotate(const Quaternion& q, TransformSpace ts)
{
	Quaternion q1 = q;

	q1.normalise();

	switch (ts)
	{
	case TS_LOCAL:
		// 新的父空间R = R * q * inv(R) * R = R * q
		// T * R * S => T * (R * q) * S
		// T,S无变化
		orientation_ = orientation_ * q1;
		break;
	case TS_PARENT:
		// T * R * S => T * (q * R) * S
		// T,S无变化
		orientation_ = q * orientation_;
		break;
	case TS_WORLD:
		// 新的父空间R = R * inv(Rp-w * R) * q * (Rp-w * R) = R * inv(Rw) * q * Rw
		// m_Orientation = m_orientation * _getDerivedOrientation().inverse() * q * _getDerivedOrientation(); 
		// 暂时不支持了，要额外算累计的旋转量。或者从矩阵提取，计算量大
		ENN_ASSERT(0);
	}

	setDerivedSelfTransformDirty();
}

void Node::rotate(const vec3f& axis, float angle, TransformSpace ts)
{
	rotate(Quaternion(angle, axis), ts);
}

void Node::yaw(float angle, TransformSpace ts)
{
	rotate(vec3f::UNIT_Y, angle, ts);
}

void Node::pitch(float angle, TransformSpace ts)
{
	rotate(vec3f::UNIT_X, angle, ts);
}

void Node::roll(float angle, TransformSpace ts)
{
	rotate(vec3f::UNIT_Z, angle, ts);
}

void Node::scale(const vec3f& sca)
{
	scale_ *= sca;
	setDerivedSelfTransformDirty();
}

void Node::scale(float x, float y, float z)
{
	scale(vec3f(x, y, z));
}

void Node::look_at(const vec3f& eye, const vec3f& target, const vec3f& upDir)
{
	// look at
	Matrix4 mat;
	Math::makeTransformLookAt( mat, eye, target, upDir );

	// 分解
	Vector3 pos;
	Vector3 sca;
	Quaternion rot;
	mat.decomposition( pos, sca, rot );

	// 赋值
	setPosition( pos );
	setScale( sca );
	setOrientation( rot );
}

void Node::look_dir(const vec3f& eye, const vec3f& look_direction, const vec3f& upDir)
{
	// look at
	Matrix4 mat;
	Math::makeTransformLookDir( mat, eye, look_direction, upDir );

	// 分解
	Vector3 pos;
	Vector3 sca;
	Quaternion rot;
	mat.decomposition( pos, sca, rot );

	// 赋值
	setPosition( pos );
	setScale( sca );
	setOrientation( rot );
}

const Matrix4& Node::getMatrix()
{
	checkUpdateTransform();

	return local_matrix_;
}

const Matrix4& Node::getDerivedMatrix()
{
	checkUpdateTransform();

	return derived_matrix_;
}

void Node::update()
{
	checkUpdateTransform();
}

void Node::updateDerived()
{
	update();

	for (NodeList::iterator iter = child_list_.begin(); iter != child_list_.end(); ++iter)
	{
		Node* node = *iter;
		node->updateDerived();
	}
}

bool Node::checkUpdateTransform()
{
	bool needUpdateDerivedTransform = false;

	/** 自己变换更新 */
	if (dirty_flag_.test_flag(SELF_TRANSFORM_DIRTY))
	{
		local_matrix_.makeTransform(position_, scale_, orientation_);

		needUpdateDerivedTransform = true;

		dirty_flag_.reset_flag(SELF_TRANSFORM_DIRTY);
	}

	/** 父亲变换有更新 */
	if (dirty_flag_.test_flag(PARENT_TRANSFORM_DIRTY))
	{
		needUpdateDerivedTransform = true;

		dirty_flag_.reset_flag(PARENT_TRANSFORM_DIRTY);
	}

	if (needUpdateDerivedTransform)
	{
		if (parent_)
		{
			derived_matrix_ = parent_->getDerivedMatrix() * local_matrix_;
		}	
		else
		{
			derived_matrix_ = local_matrix_;
		}
	}

	return needUpdateDerivedTransform;
}

bool Node::isCameraNode() const
{
	return type_flag_.test_flag(NODE_CAMERA);
}

bool Node::isLightNode() const
{
	return type_flag_.test_flag(NODE_LIGHT);
}

bool Node::isMeshNode() const
{
	return type_flag_.test_flag(NODE_MESH);
}

bool Node::isWaterNode() const
{
	return type_flag_.test_flag(NODE_WATER);
}

bool Node::isRenderableNode() const
{
	return type_flag_.test_flag(NODE_RENDERABLE);
}

bool Node::isWaterPlaneNode() const
{
	return type_flag_.test_flag(NODE_WATER_PLANE);
}

}