/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_plane.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_vector3.h"

namespace enn
{
	class AxisAlignedBox;

	class Plane
	{
	public:
		Plane();
		Plane(const Plane& rhs);
		Plane(const Vector3& rkNormal, float fConstant);
		Plane(float a, float b, float c, float d);
		Plane(const Vector3& rkNormal, const Vector3& rkPoint);
		Plane(const Vector3& rkPoint0, const Vector3& rkPoint1,
			const Vector3& rkPoint2);

	public:
		enum Side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE,
			BOTH_SIDE
		};

	public:
		Side getSide(const Vector3& rkPoint) const;
		Side getSide(const AxisAlignedBox& rkBox) const;
		Side getSide(const Vector3& centre, const Vector3& halfSize) const;

		float getDistance(const Vector3& rkPoint) const;

		void redefine(const Vector3& rkPoint0, const Vector3& rkPoint1,
			const Vector3& rkPoint2);
		void redefine(const Vector3& rkNormal, const Vector3& rkPoint);

		Vector3 projectVector(const Vector3& v) const;

		float normalise();

		Vector3 reflect(const Vector3& v) const;

	public:
		Vector3 normal;
		float d;

	public:
		bool operator==(const Plane& rhs) const
		{
			return (rhs.d == d && rhs.normal == normal);
		}
		bool operator!=(const Plane& rhs) const
		{
			return (rhs.d != d || rhs.normal != normal);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	typedef vector<Plane>::type PlaneList;
}