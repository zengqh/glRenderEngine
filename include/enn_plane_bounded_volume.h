/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_plane_bounded_volume.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_axis_aligned_box.h"
#include "enn_sphere.h"
#include "enn_math_def.h"
#include "enn_plane.h"

namespace enn
{
	class PlaneBoundedVolume
    {
    public:
        typedef vector<Plane>::type PlaneList;

    public:  
        PlaneList   planes;
        Plane::Side outside;

    public:
        PlaneBoundedVolume() : outside(Plane::NEGATIVE_SIDE) {}
        PlaneBoundedVolume(Plane::Side theOutside) : outside(theOutside) {}

    public:
        /** Intersection test with AABB
         May return false positives but will never miss an intersection.
        */
        bool intersects(const AxisAlignedBox& box) const
        {
            if (box.isNull()) return false;
            if (box.isInfinite()) return true;

            // Get centre of the box
            Vector3 centre = box.getCenter();
            // Get the half-size of the box
            Vector3 halfSize = box.getHalfSize();
            
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                Plane::Side side = plane.getSide(centre, halfSize);
                if (side == outside)
                {
                    // Found a splitting plane therefore return not intersecting
                    return false;
                }
            }

            // couldn't find a splitting plane, assume intersecting
            return true;

        }
        /** Intersection test with Sphere
         May return false positives but will never miss an intersection.
        */
        bool intersects(const Sphere& sphere) const
        {
            PlaneList::const_iterator i, iend;
            iend = planes.end();
            for (i = planes.begin(); i != iend; ++i)
            {
                const Plane& plane = *i;

                // Test which side of the plane the sphere is
                float d = plane.getDistance(sphere.getCenter());
                // Negate d if planes point inwards
                if (outside == Plane::NEGATIVE_SIDE) d = -d;

                if ( (d - sphere.getRadius()) > 0)
                    return false;
            }

            return true;

        }

        /** Intersection test with a Ray
         return std::pair of hit (bool) and distance
         May return false positives but will never miss an intersection.
        */
        std::pair<bool, float> intersects(const Ray& ray)
        {
            return Math::intersects(ray, planes, outside == Plane::POSITIVE_SIDE);
        }
    };

    //////////////////////////////////////////////////////////////////////////
    typedef vector<PlaneBoundedVolume>::type PlaneBoundedVolumeList;
}