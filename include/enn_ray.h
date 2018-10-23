/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_ray.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_vector3.h"
#include "enn_plane_bounded_volume.h"

namespace enn
{
	class Ray
    {
    protected:
        Vector3 m_origin;
        Vector3 m_direction;

    public:
        Ray():m_origin(Vector3::ZERO), m_direction(Vector3::UNIT_Z) {}
        Ray(const Vector3& origin, const Vector3& direction)
            :m_origin(origin), m_direction(direction) {}

    public:
        /** Sets the origin of the ray. */
        void setOrigin(const Vector3& origin) {m_origin = origin;} 

        /** Gets the origin of the ray. */
        const Vector3& getOrigin(void) const {return m_origin;} 

        /** Sets the direction of the ray. */
        void setDirection(const Vector3& dir) {m_direction = dir;} 
        /** Gets the direction of the ray. */
        const Vector3& getDirection(void) const {return m_direction;} 

		/** Gets the position of a point t units along the ray. */
		Vector3 getPoint(float t) const { 
			return Vector3(m_origin + (m_direction * t));
		}
		
		/** Gets the position of a point t units along the ray. */
		Vector3 operator*(float t) const { 
			return getPoint(t);
		}

    public:
		/** Tests whether this ray intersects the given plane. 
		    return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Plane& p) const
		{
			return Math::intersects(*this, p);
		}

        /** Tests whether this ray intersects the given plane bounded volume. 
        return A pair structure where the first element indicates whether
        an intersection occurs, and if true, the second element will
        indicate the distance along the ray at which it intersects. 
        This can be converted to a point in space by calling getPoint().
        */
        std::pair<bool, float> intersects(const PlaneBoundedVolume& p) const
        {
            return Math::intersects(*this, p.planes, p.outside == Plane::POSITIVE_SIDE);
        }

		/** Tests whether this ray intersects the given sphere. 
		    return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const Sphere& s) const
		{
			return Math::intersects(*this, s);
		}

		/** Tests whether this ray intersects the given box. 
		    return A pair structure where the first element indicates whether
			an intersection occurs, and if true, the second element will
			indicate the distance along the ray at which it intersects. 
			This can be converted to a point in space by calling getPoint().
		*/
		std::pair<bool, float> intersects(const AxisAlignedBox& box) const
		{
			return Math::intersects(*this, box);
		}
    };
}