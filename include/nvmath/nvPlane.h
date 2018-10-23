/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2012/10/23
* File: nvPlane.h
**/

#ifndef nvPlane_h__
#define nvPlane_h__

#include "nvVector.h"
#include "nvMatrix.h"
#include "nvQuaternion.h"
#include "nvMisc.h"

namespace nv
{
	template <typename T>
	class Plane
	{
	public:
		Plane()
		{
			_d = 0.0;
		}

		Plane(const Plane& rhs)
		{
			_normal = rhs._normal;
			_d = rhs._d;
		}

		Plane(const vec3<T>& normal, T dis)
		{
			_normal = normal;
			_d = -dis;
		}

		Plane(T a, T b, T c, T d)
			: _normal(a, b, c), _d(d)
		{

		}
		Plane(const vec3<T>& v0, const vec3<T>& v1, const vec3<T>& v2)
		{
			vec3<T> kEdge1 = v1 - v0;
			vec3<T> kEdge2 = v2 - v0;
			_normal = cross(kEdge1, kEdge2);
			_normal = normalize(_normal);
			_d = -dot(_normal, v0);
		}

		enum Side
		{
			NO_SIDE,
			POSITIVE_SIDE,
			NEGATIVE_SIDE,
			BOTH_SIDE
		};

		Side getSide (const vec3<T>& rkPoint) const
		{
			T dis = getDistance(rkPoint);
			if (dis < 0.0)
			{
				return Plane::NEGATIVE_SIDE;
			}
			if (dis > 0.0)
			{
				return Plane::POSITIVE_SIDE;
			}

			return Plane::NO_SIDE;
		}

		Side getSide (const vec3<T>& centre, const vec3<T>& halfSize) const
		{
			T dist = getDistance(centre);

			T maxAbsDist = fabs(_normal.x * halfSize.x) + 
				fabs(_normal.y * halfSize.y) + 
				fabs(_normal.z * halfSize.z);

			if (nv::is_less_strict(dist, -maxAbsDist))
				return Plane::NEGATIVE_SIDE;

			if (nv::is_greater_strict(dist, maxAbsDist))
				return Plane::POSITIVE_SIDE;

			return Plane::BOTH_SIDE;
		}

		T getDistance (const vec3<T>& rkPoint) const
		{
			return dot(_normal, rkPoint) + _d;
		}

		T normalize()
		{
			T length = nv::length(_normal);

			if (length > 0.0)
			{
				T inv_length = 1.0 / length;
				_normal *= inv_length;
				_d *= inv_length;
			}

			return length;
		}

	protected:
		vec3<T> _normal;
		T		_d;
	};
}

#endif    /* nvPlane_h__ */