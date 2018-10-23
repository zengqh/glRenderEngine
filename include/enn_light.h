/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/10
* File: enn_light.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_math.h"
#include "enn_node_def.h"
#include "enn_shadow.h"

#define MAX_FORWARD_POINT_LIT_NUM   4
#define MAX_FORWARD_SPOT_LIT_NUM    4

namespace enn
{
//////////////////////////////////////////////////////////////////////////
enum
{
	LT_DIRECTIONAL,
	LT_POINT,
	LT_SPOT,
	LT_NUM
};

class Mesh;
//////////////////////////////////////////////////////////////////////////
class Light : public AllocatedObject, public ShadowInfoWrap
{
public:
	Light();
	virtual ~Light();

public:
	int getLightType() const
	{
		return light_type_;
	}

	template <typename T>
	const T* getLightAs() const
	{
		return static_cast<const T*>(this);
	}

	template <typename T>
	T* getLightAs()
	{
		return static_cast<T*>(this);
	}

	void setPosition(const vec3f& pos);
	void setDirection(const vec3f& dir);
	void setUp(const vec3f& up);

	const vec3f& getPosition() const;
	const vec3f& getDirection() const;
	const vec3f& getUp() const;
	const vec3f& getRight() const;

	void setDiffuse(const Color& clr);
	const Color& getDiffuse() const;

public:
	void setTransform(const Matrix4& mat_world);
	const Matrix4& getTransform() const;

	const AxisAlignedBox& getAABB() const;

	bool intersect(const AxisAlignedBox& aabb) const;
	virtual float squaredDistance(const vec3f& pos) const = 0;

public:
	int getTempId() const
	{
		return temp_id_;
	}

	void setTempId(int id)
	{
		temp_id_ = id;
	}

public:
	virtual Mesh* getVolume() const 
	{ 
		return 0; 
	}

	virtual const Matrix4& getVolumeWorldMatrix() const
	{
		return getTransform();
	}

	virtual const AxisAlignedBox& getVolumeAABB() const
	{
		return getAABB();
	}

protected:
	void update();
	void setPropertyDirty();

protected:
	virtual void checkLightProp();
	virtual void checkAABB() = 0;

protected:
	virtual Light* getHost()
	{
		return this;
	}

protected:
	int				light_type_;
	vec3f			position_;
	vec3f			direction_;
	vec3f			up_;
	vec3f			right_;

	Color			diffuse_;
	Matrix4			mat_world_;
	AxisAlignedBox	world_aabb_;

	Flags32			dirty_flags_;

	int				temp_id_;
};

//////////////////////////////////////////////////////////////////////////
// DirectionLight
class DirectionLight : public Light
{
public:
	DirectionLight();
	virtual ~DirectionLight();

public:
	virtual float squaredDistance(const vec3f& pos) const;

protected:
	virtual void checkAABB();
	virtual ShadowTechBase* createShadowTech(ShadowType type);
};


//////////////////////////////////////////////////////////////////////////
// PointLight
class PointLight : public Light
{
public:
	PointLight();
	virtual ~PointLight();

	void setRange(float range);
	void setFadeDist(float dist);

	float getRange() const;
	float getFadeDist() const;

	float getFadeScale() const;

public:
	virtual float squaredDistance(const vec3f& pos) const;

	virtual Mesh* getVolume() const;
	virtual const Matrix4& getVolumeWorldMatrix() const;
	virtual const AxisAlignedBox& getVolumeAABB() const;

protected:
	virtual void checkLightProp();
	virtual void checkAABB();

protected:
	float			range_;
	float			fade_dist_;
	Matrix4			m_volMatWorld;
	AxisAlignedBox	m_volAABB;
};

//////////////////////////////////////////////////////////////////////////
// SpotLight
class SpotLight : public PointLight
{
public:
	SpotLight();
	virtual ~SpotLight();

	void setInnerCone(float angle);
	void setOuterCone(float angle);

	float getInnerCone() const;
	float getOuterCone() const;

	const Vector2& getSpotAngles() const;

public:
	virtual float squaredDistance(const vec3f& pos) const;
	virtual Mesh* getVolume() const;

protected:
	virtual void checkLightProp();
	virtual void checkAABB();

	void _makeSpotAABB( const Vector3& pos, const Vector3& xdir, const Vector3& ydir, const Vector3& zdir,
		float radius, float range, Vector3& centerSpot, AxisAlignedBox& aabb );

protected:
	float	inner_cone_;
	float	outer_cone_;
	float	outer_radius_;
	vec3f	center_spot_;

	/** x = cos(outerCone) y = 1 / (cos(innerCone) - cos(outerCone)) */
	vec2f	spot_angles_;
};

//////////////////////////////////////////////////////////////////////////
// LightFactory
class LightFactory
{
public:
	static Light* createLight(int type);
};

//////////////////////////////////////////////////////////////////////////
// LightsInfo
class LightsInfo : public AllocatedObject
{
public:
	struct LightItem : public AllocatedObject
	{
		LightItem() : lit(0), dist(0.0f), in_sm(false)
		{

		}

		LightItem(Light* l, float d, bool is_in_sm = false)
			: lit(l), dist(d), in_sm(is_in_sm)
		{

		}

		Light*		lit;
		float		dist;
		bool		in_sm;
	};

	typedef enn::vector<LightItem>::type LightList;

protected:
	static bool sortByDist( const LightItem& lhs, const LightItem& rhs )
	{
		return lhs.dist < rhs.dist;
	}

public:
	LightsInfo();
	~LightsInfo();

public:
	void addLight(Light* lit, float dist, bool is_in_sm = false);
	void clear();
	void sort_n(int light_type, int max_n);

	int getLightsCount(int type) const;
	LightList* getLightList(int type) const;

protected:
	LightList*			lit_list_[LT_NUM];
};
}