/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_math_def.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"

namespace enn
{
	class AxisAlignedBox;
	class Plane;
	class Sphere;
	class Ray;
	class Vector3;
	class Vector2;
	class Matrix4;

	class Math
	{
	public:
		// 常数
		static const float POS_INFINITY;
		static const float NEG_INFINITY;
		static const float PI;
		static const float TWO_PI;
		static const float HALF_PI;
		static const uint32 ENN_MAX_UNSIGNED;
		static const int ENN_MAX_SIGNED;
		static const int ENN_MIN_SIGNED;
		static const float ENN_EPSILON;

	public:
		static const float s_deg2Rad;
		static const float s_rad2Deg;

	public:
		// 基本数值运算
		static bool isNaN( float f )
		{
			return f != f;
		}

		static int iabs( int x )
		{
			return x >= 0 ? x : -x;
		}

		static float fabs( float x )
		{
			return ::fabs( x );
		}

		static float sqrt( float x )
		{
			return ::sqrt( x );
		}

		static float sqr( float fValue )
		{
			return fValue * fValue;
		}

		static float invSqrt( float x )
		{
			return (float)1.0 / ::sqrt( x );
		}

		static float pow( float x, float y )
		{
			return ::pow( x, y );
		}

		static float toRadians( float degrees )
		{
			return degrees * s_deg2Rad; 
		}

		static float toDegrees( float radians )
		{
			return radians * s_rad2Deg; 
		}

		static bool realEqual( float a, float b, float tolerance = Math::ENN_EPSILON )
		{
			if ( fabs(b-a) <= tolerance )
				return true;
			else
				return false;
		}

		static bool isZero(float a)
		{
			return realEqual(a, 0.0f);
		}

		static bool isVector3Zero(const Vector3& v);

		static bool isVector3Equal(const Vector3& v0, const Vector3& v1);

		template<typename T>
		static T maxVal( T a, T b )
		{
			return a > b ? a : b;
		}

		template<typename T>
		static T minVal( T a, T b )
		{
			return a < b ? a : b;
		}  

		template <typename T>
		static T clamp( T val, T minval, T maxval )
		{
			ENN_ASSERT( minval <= maxval );
			return maxVal(minVal(val, maxval), minval);
		}

		static float floor( float t )
		{
			return ::floorf( t );
		}

		static float ceil( float t )
		{
			return ::ceilf( t );
		}

		// 随机数
		static float unitRandom()
		{
			// [0, 1]
			return ::rand() / (float)RAND_MAX;
		}

		static float symmetricRandom()
		{
			// [-1, 1]
			return (float)2.0 * unitRandom() - (float)1.0;
		}

		static float rangeRandom( float low, float high )
		{
			// [low, high]
			return (high - low) * unitRandom() + low;
		}

		// 三角函数
		static float sin( float x )
		{
			return ::sin(x);
		}

		static float cos( float x )
		{
			return ::cos(x);
		}

		static float tan( float x )
		{
			return ::tan( x );
		}

		static float asin( float x )
		{
			if ( (float)-1.0 < x )
			{
				if ( x < (float)1.0 )
					return ::asin(x);
				else
					return HALF_PI;
			}
			else
			{
				return -HALF_PI;
			}
		}

		static float acos( float x )
		{
			if ( (float)-1.0 < x )
			{
				if ( x < (float)1.0 )
					return ::acos(x);
				else
					return (float)0.0;
			}
			else
			{
				return PI;
			}
		}

		static float atan( float x )
		{
			return ::atan( x );
		}

		static float atan2( float y, float x )
		{
			return ::atan2( y, x );
		}

	public:
		// 矩阵函数
		static void makeTransformLookDir( Matrix4& m, const Vector3& eye, const Vector3& lookDir, const Vector3& upDir );
		static void makeTransformLookAt( Matrix4& m, const Vector3& eye, const Vector3& target, const Vector3& upDir );
		static void makeGLPerspective(Matrix4& m, float fov, float aspect, float z_near, float z_far);
		
	public:
		// 相交测试
		static bool intersects( const Sphere& sphere, const AxisAlignedBox& box );
		static bool intersects( const Plane& plane, const AxisAlignedBox& box );
		static bool intersects( const Sphere& sphere, const Plane& plane );

		static std::pair<bool, float> intersects( const Ray& ray, const Plane& plane );
		static std::pair<bool, float> intersects( const Ray& ray, const AxisAlignedBox& box );
		static std::pair<bool, float> intersects( const Ray& ray, const Sphere& sphere, bool discardInside = true );
		static std::pair<bool, float> intersects( const Ray& ray, const vector<Plane>::type& planeList, bool normalIsOutside );
		static std::pair<bool, float> intersects( const Ray& ray, const list<Plane>::type& planeList, bool normalIsOutside );

	public:
		static bool inverse_matrix2(float v[4]);

		/** ref http://www.terathon.com/code/tangent.html */
		static void cal_tangent(const Vector3& va, const Vector3& vd, const Vector3& vc,
			const Vector2& uva, const Vector2& uvd, const Vector2& uvc,
			Vector3& tangent, Vector3& binormal);

		/** ref http://www.terathon.com/code/tangent.html */
		static void gramSchmidtOrthogonalize(Vector3& t, const Vector3& b, const Vector3& n );

	public:
		static void screen_to_cube_space(Vector3& v, int screenWidth, int screenHeight, int cursorX, int cursorY, float z = -1.0f);
	};
}