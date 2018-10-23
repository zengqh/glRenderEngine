/**
* Copyright (c) 2013 Pateo Software Ltd
*
* This source file is part of Pateo gis engine.
* For latest info, see http://www.pateo.com.cn/
*
* Time: 2013/09/26
* File: enn_physics_utils.h
**/
 
#pragma once

#include "enn_math.h"

#include <LinearMath/btVector3.h>
#include <LinearMath/btQuaternion.h>

namespace enn
{

inline btVector3 ToBtVector3(const Vector3& vector)
{
	return btVector3(vector.x, vector.y, vector.z);
}

inline btQuaternion ToBtQuaternion(const Quaternion& quaternion)
{
	return btQuaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
}

inline Vector3 ToVector3(const btVector3& vector)
{
	return Vector3(vector.x(), vector.y(), vector.z());
}

inline Quaternion ToQuaternion(const btQuaternion& quaternion)
{
	return Quaternion(quaternion.w(), quaternion.x(), quaternion.y(), quaternion.z());
}

inline bool HasWorldScaleChanged(const Vector3& oldWorldScale, const Vector3& newWorldScale)
{
	Vector3 delta = newWorldScale - oldWorldScale;
	float dot = delta.dotProduct(delta);
	return dot > 0.01f;
}

}