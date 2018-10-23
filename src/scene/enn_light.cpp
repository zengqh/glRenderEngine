/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/10
* File: enn_light.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_light.h"
#include "enn_mesh.h"
#include "enn_root.h"

namespace enn
{
Light::Light()
: light_type_(LT_DIRECTIONAL)
, position_(Vector3::ZERO)
, direction_(Vector3::NEGATIVE_UNIT_Y)
, up_(Vector3::NEGATIVE_UNIT_Z)
, right_(Vector3::UNIT_X)
, diffuse_(Color::WHITE)
, temp_id_(0)
{

}

Light::~Light()
{

}

void Light::setPosition(const vec3f& pos)
{
	position_ = pos;

	setPropertyDirty();
}

void Light::setDirection(const vec3f& dir)
{
	direction_ = dir;

	setPropertyDirty();
}

void Light::setUp(const vec3f& up)
{
	up_ = up;

	setPropertyDirty();
}

const vec3f& Light::getPosition() const
{
	const_cast<Light*>(this)->update();
	return position_;
}

const vec3f& Light::getDirection() const
{
	const_cast<Light*>(this)->update();
	return direction_;
}

const vec3f& Light::getUp() const
{
	const_cast<Light*>(this)->update();
	return up_;
}

const vec3f& Light::getRight() const
{
	const_cast<Light*>(this)->update();
	return right_;
}

void Light::setDiffuse(const Color& clr)
{
	diffuse_ = clr;
}

const Color& Light::getDiffuse() const
{
	return diffuse_;
}

void Light::setTransform(const Matrix4& mat_world)
{
	setPosition(mat_world.getTrans());
	setDirection(-mat_world.getAxisZ());
	setUp(mat_world.getAxisY());
}

const Matrix4& Light::getTransform() const
{
	const_cast<Light*>(this)->update();
	return mat_world_;
}

const AxisAlignedBox& Light::getAABB() const
{
	const_cast<Light*>(this)->update();
	return world_aabb_;
}

bool Light::intersect(const AxisAlignedBox& aabb) const
{
	return getAABB().intersects(aabb);
}

void Light::update()
{
	if (dirty_flags_.test_flag(DF_LIGHT_INFO))
	{
		checkLightProp();
		checkAABB();
		dirty_flags_.reset_flag(DF_LIGHT_INFO);
	}
}

void Light::setPropertyDirty()
{
	dirty_flags_.set_flag(DF_LIGHT_INFO);
}

void Light::checkLightProp()
{
	direction_.normalise();

	right_ = direction_.crossProduct(up_);

	right_.normalise();

	up_ = right_.crossProduct(direction_);

	mat_world_.makeTransform(position_, Vector3::UNIT_SCALE, Quaternion(right_, up_, -direction_));
}

//////////////////////////////////////////////////////////////////////////
DirectionLight::DirectionLight()
{
	light_type_ = LT_DIRECTIONAL;
}

DirectionLight::~DirectionLight()
{

}

float DirectionLight::squaredDistance(const vec3f& pos) const
{
	return 0;
}

void DirectionLight::checkAABB()
{
	world_aabb_.setInfinite();
}

ShadowTechBase* DirectionLight::createShadowTech(ShadowType type)
{
	switch (type)
	{
	case ST_SM:
	case ST_PSSM:
		return ENN_NEW ShadowTechDirect();
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
PointLight::PointLight()
: fade_dist_(1.0f)
{
	light_type_ = LT_POINT;
}

PointLight::~PointLight()
{

}

void PointLight::setRange(float range)
{
	range_ = range;
	setPropertyDirty();
}

void PointLight::setFadeDist(float dist)
{
	fade_dist_ = dist;
	setPropertyDirty();
}

float PointLight::getRange() const
{
	const_cast<PointLight*>(this)->update();
	return range_;
}

float PointLight::getFadeDist() const
{
	const_cast<PointLight*>(this)->update();
	return fade_dist_;
}

float PointLight::getFadeScale() const
{
	return Math::PI * fade_dist_ / range_;
}

float PointLight::squaredDistance(const vec3f& pos) const
{
	return position_.squaredDistance(pos);
}

Mesh* PointLight::getVolume() const
{
	return MESH_MGR->getPointLitMesh();
}

const Matrix4& PointLight::getVolumeWorldMatrix() const
{
	const_cast<PointLight*>(this)->update();
	return m_volMatWorld;
}

const AxisAlignedBox& PointLight::getVolumeAABB() const
{
	const_cast<PointLight*>(this)->update();
	return m_volAABB;
}

void PointLight::checkLightProp()
{
	Light::checkLightProp();

	if (range_ < 0)
	{
		range_ = 0;
	}

	if (fade_dist_ < 0)
	{
		fade_dist_ = 0;
	}

	m_volMatWorld.makeTransform( position_, Vector3(range_), Quaternion::IDENTITY );
}

void PointLight::checkAABB()
{
	Vector3 size(range_, range_, range_);
	world_aabb_.setExtents( position_ - size, position_ + size );

	Vector3 volRadius = getVolume()->getAABB().getHalfSize() * range_;
	m_volAABB.setExtents( position_ - volRadius, position_ + volRadius );
}

//////////////////////////////////////////////////////////////////////////
// SpotLight
SpotLight::SpotLight()
: inner_cone_(0)
, outer_cone_(0)
, outer_radius_(0)
{
	light_type_ = LT_SPOT;
}

SpotLight::~SpotLight()
{

}

void SpotLight::setInnerCone(float angle)
{
	inner_cone_ = angle;
	setPropertyDirty();
}

void SpotLight::setOuterCone(float angle)
{
	outer_cone_ = angle;
	setPropertyDirty();
}

float SpotLight::getInnerCone() const
{
	const_cast<SpotLight*>(this)->update();
	return inner_cone_;
}

float SpotLight::getOuterCone() const
{
	const_cast<SpotLight*>(this)->update();
	return outer_cone_;
}

const Vector2& SpotLight::getSpotAngles() const
{
	return spot_angles_;
}

float SpotLight::squaredDistance(const vec3f& pos) const
{
	return center_spot_.squaredDistance(pos);
}

Mesh* SpotLight::getVolume() const
{
	return MESH_MGR->getSpotLitMesh();
}

void SpotLight::checkLightProp()
{
	PointLight::checkLightProp();

	/** 由于是半角，最大90 */
	outer_cone_ = Math::clamp(outer_cone_, 0.0f, Math::HALF_PI);
	/** 内角不能超过外角 */
	inner_cone_ = Math::clamp(inner_cone_, 0.0f, outer_cone_);
	/** 外圆的半径 */
	outer_radius_ = range_ * Math::tan(outer_cone_);

	spot_angles_.x = Math::cos(outer_cone_);
	spot_angles_.y = 1.0f / (Math::cos(inner_cone_) - spot_angles_.x);

	m_volMatWorld.makeTransform( position_, Vector3(outer_radius_, outer_radius_, range_), 
		Quaternion(right_, up_, -direction_) );
}

void SpotLight::checkAABB()
{
	Vector3  zdir = -direction_;
	Vector3& ydir = up_;
	Vector3& xdir = right_;

	_makeSpotAABB( position_, xdir, ydir, zdir, outer_radius_, range_, center_spot_, world_aabb_);

	// 这里准确计算volaabb
	const AxisAlignedBox& volMeshAABB = getVolume()->getAABB();
	Vector3 volHalfSize = volMeshAABB.getHalfSize();
	float volRadius = Math::maxVal(volHalfSize.x, volHalfSize.y) * outer_radius_;
	float volRange  = volMeshAABB.getSize().z * range_;
	Vector3 volCenterSpot;
	_makeSpotAABB( position_, xdir, ydir, zdir, volRadius, volRange, volCenterSpot, m_volAABB );
}

void SpotLight::_makeSpotAABB( const Vector3& pos, const Vector3& xdir, const Vector3& ydir, const Vector3& zdir,
				   float radius, float range, Vector3& centerSpot, AxisAlignedBox& aabb )
{
	float halfRange = range * 0.5f;
	centerSpot = pos - zdir * halfRange;
	Vector3 halfSize(radius, radius, halfRange);

	Vector3 corners[8];
	ObjectAlignedBox::getCorners( centerSpot, xdir, ydir, zdir, halfSize, corners );

	aabb.setNull();
	aabb.merge( corners, 8, sizeof(Vector3) );
}

//////////////////////////////////////////////////////////////////////////
// LightFactory
Light* LightFactory::createLight(int type)
{
	switch (type)
	{
	case LT_DIRECTIONAL:
		return ENN_NEW DirectionLight;

	case LT_POINT:
		return ENN_NEW PointLight;

	case LT_SPOT:
		return ENN_NEW SpotLight;

	default:
		ENN_ASSERT(0);
		return 0;
	}
}

//////////////////////////////////////////////////////////////////////////
// LightsInfo
LightsInfo::LightsInfo()
{
	for (int i = 0; i < LT_NUM; ++i)
	{
		lit_list_[i] = 0;
	}
}

LightsInfo::~LightsInfo()
{
	for (int i = 0; i < LT_NUM; ++i)
	{
		if (LightList* ll = lit_list_[i])
		{
			ll->clear();
			ENN_SAFE_DELETE_T(ll);
		}
	}
}

void LightsInfo::addLight(Light* lit, float dist, bool is_in_sm)
{
	LightList*& ll = lit_list_[lit->getLightType()];

	if (!ll)
	{
		ll = ENN_NEW_T(LightsInfo::LightList);
	}

	ll->push_back(LightsInfo::LightItem(lit, dist, is_in_sm));
}

void LightsInfo::clear()
{
	for (int i = 0; i < LT_NUM; ++i)
	{
		if (LightList* ll = lit_list_[i])
		{
			ll->clear();
		}
	}
}

void LightsInfo::sort_n(int light_type, int max_n)
{
	ENN_ASSERT(light_type >= 0 && light_type < LT_NUM);
	ENN_ASSERT(max_n >= 0 && max_n <= 4);

	LightList* ll = lit_list_[light_type];
	if (!ll)
	{
		return;
	}

	std::sort(ll->begin(), ll->end(), sortByDist);
}

int LightsInfo::getLightsCount(int type) const
{
	ENN_ASSERT(type >= 0 && type < LT_NUM);

	LightList* ll = lit_list_[type];
	if (!ll)
	{
		return 0;
	}

	return ll->size();
}

LightsInfo::LightList* LightsInfo::getLightList(int type) const
{
	ENN_ASSERT(type >= 0 && type < LT_NUM);

	return lit_list_[type];
}

}