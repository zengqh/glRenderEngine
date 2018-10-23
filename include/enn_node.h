/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/11
* File: Node.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_node_def.h"

namespace enn
{
class Node : public Noncopyable, public AllocatedObject
{
	ENN_DECLARE_RTTI(Node)
public:
	typedef enn::vector<Node*>::type NodeList;

	enum TransformSpace
	{
		TS_LOCAL,
		TS_PARENT,
		TS_WORLD
	};

public:
	Node();
	virtual ~Node();

public:
	void setName(const enn::String& name)
	{
		name_ = name;
	}

	const enn::String& getName() const
	{
		return name_;
	}

	virtual void addChild(Node* node);
	virtual void removeChild(Node* node);
	virtual void removeAllChild();

	Node* getParent() const
	{
		return parent_;
	}

public:
	void setPosition(const vec3f& pos);
	void setPosition(float x, float y, float z);

	void setWorldPosition(const vec3f& pos);
	void setWorldPosition(float x, float y, float z);

	void setOrientation(const Quaternion& ori);
	void resetOrientation();

	void setScale(const vec3f& sca);
	void setScale(float x, float y, float z);

	const vec3f& getPosition() const
	{
		return position_;
	}

	const Quaternion& getQuaternion() const
	{
		return orientation_;
	}

	const vec3f& getScale() const
	{
		return scale_;
	}

public:
	void translate(const vec3f& d, TransformSpace ts = TS_PARENT);
	void translate(float x, float y, float z, TransformSpace ts = TS_PARENT);

	void rotate(const Quaternion& q, TransformSpace ts = TS_LOCAL);
	void rotate(const vec3f& axis, float angle, TransformSpace ts = TS_LOCAL);

	void yaw(float angle, TransformSpace ts = TS_LOCAL);
	void pitch(float angle, TransformSpace ts = TS_LOCAL);
	void roll(float angle, TransformSpace ts = TS_LOCAL);

	void scale(const vec3f& sca);
	void scale(float x, float y, float z);

	void look_at(const vec3f& eye, const vec3f& target, const vec3f& upDir);
	void look_dir(const vec3f& eye, const vec3f& look_direction, const vec3f& upDir);

public:
	const Matrix4& getMatrix();
	const Matrix4& getDerivedMatrix();

	virtual const Flags64& getTypeFlag() const
	{
		return type_flag_;
	}

	virtual bool isCameraNode() const;
	virtual bool isLightNode() const;
	virtual bool isMeshNode() const;
	virtual bool isWaterNode() const;
	virtual bool isRenderableNode() const;
	virtual bool isWaterPlaneNode() const;

protected:

	/**
	���ڵ㱻��ӵ�����ͼ�л��Ƴ�������ͼ��ʱ��
	ִ��������_onSelfAdd/_onSelfRemove 
	*/
	void updateDerivedByAdd();
	void updateDerivedByRemove();

	/**
	 ���ڵ���߸��׽ڵ㱻��ӵ�����ͼ�л��Ƴ�������ͼ��ʱ��
	 ������������ÿ���ڵ�Ĵ���Ĭ��ִ�����ñ��ڵ㸸������
	*/
	virtual void onSelfAdd();
	virtual void onSelfRemove();

	/** �����Լ����ǣ�ͬʱ֪ͨ���� */
	void setDerivedSelfTransformDirty();

	/** �����Լ��������ǣ�ͬʱ֪ͨ���� */
	void setDerivedParentTransformDirty();

	/** ���ýڵ���Լ���/�������� */
	virtual void setSelfTransformDirty();
	virtual void setParentTransformDirty();

	virtual void update();

	virtual bool checkUpdateTransform();

public:
	void updateDerived();

protected:
	enn::String					name_;
	Node*						parent_;
	vec3f						position_;
	vec3f						scale_;
	Quaternion					orientation_;
	NodeList					child_list_;

	Matrix4						local_matrix_;
	Matrix4						derived_matrix_;

	Flags64						dirty_flag_;
	Flags64						features_flag_;
	Flags64						type_flag_;
};
}