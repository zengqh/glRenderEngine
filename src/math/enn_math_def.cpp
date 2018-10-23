/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/24
* File: enn_math_def.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_math_def.h"
#include "enn_axis_aligned_box.h"
#include "enn_plane.h"
#include "enn_sphere.h"
#include "enn_ray.h"

namespace enn
{
	const float Math::POS_INFINITY   		=  std::numeric_limits<float>::infinity();
	const float Math::NEG_INFINITY   		= -std::numeric_limits<float>::infinity();
	const float Math::PI             		= float( 4.0 * atan( 1.0 ) );
	const float Math::TWO_PI         		= float( 2.0 * PI );
	const float Math::HALF_PI        		= float( 0.5 * PI );
	const float Math::s_deg2Rad      		= Math::PI / float(180.0);
	const float Math::s_rad2Deg      		= float(180.0) / Math::PI;
	const uint32 Math::ENN_MAX_UNSIGNED	 	= 0xffffffff;
	const int Math::ENN_MAX_SIGNED		 	= 0x7fffffff;
	const int Math::ENN_MIN_SIGNED		 	= 0x80000000;
	const float Math::ENN_EPSILON      		= 0.000001f;

	//////////////////////////////////////////////////////////////////////////
	bool Math::isVector3Zero(const Vector3& v)
	{
		return (Math::isZero(v.x) && Math::isZero(v.y) && Math::isZero(v.z));
	}

	bool Math::isVector3Equal(const Vector3& v0, const Vector3& v1)
	{
		Vector3 v = v0 - v1;
		return isVector3Zero(v);
	}

	//////////////////////////////////////////////////////////////////////////
	void Math::makeTransformLookDir( Matrix4& m, const Vector3& eye, const Vector3& lookDir, const Vector3& upDir )
	{
		Vector3 zdir = -lookDir;
		zdir.normalise();

		Vector3 xdir = upDir.crossProduct(zdir);
		xdir.normalise();

		Vector3 ydir = zdir.crossProduct(xdir);

		m[0][0] = xdir.x; m[0][1] = ydir.x; m[0][2] = zdir.x; m[0][3] = eye.x;
		m[1][0] = xdir.y; m[1][1] = ydir.y; m[1][2] = zdir.y; m[1][3] = eye.y;
		m[2][0] = xdir.z; m[2][1] = ydir.z; m[2][2] = zdir.z; m[2][3] = eye.z;
		m[3][0] = 0.0;    m[3][1] = 0.0;    m[3][2] = 0.0;    m[3][3] = 1.0f;
	}

	void Math::makeTransformLookAt( Matrix4& m, const Vector3& eye, const Vector3& target, const Vector3& upDir )
	{
		makeTransformLookDir( m, eye, target-eye, upDir );
	}

	void Math::makeGLPerspective(Matrix4& m, float fov, float aspect, float z_near, float z_far)
	{
		float f   = 1.0f / Math::tan( fov * 0.5f );
		float n_f = z_near - z_far;

		m[0][0] = f / aspect; m[0][1] = 0; m[0][2] = 0;						   m[0][3] = 0;
		m[1][0] = 0;          m[1][1] = f; m[1][2] = 0;                        m[1][3] = 0;
		m[2][0] = 0;          m[2][1] = 0; m[2][2] = (z_far + z_near) / n_f;   m[2][3] = -1;
		m[3][0] = 0;          m[3][1] = 0; m[3][2] = (2 * z_far * z_near) / n_f; m[3][3] = 0;
	}

	bool Math::intersects(const Sphere& sphere, const AxisAlignedBox& box)
	{
		if (box.isNull()) return false;
		if (box.isInfinite()) return true;

		// Use splitting planes
		const Vector3& center = sphere.getCenter();
		float radius = sphere.getRadius();
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();

		// Arvo's algorithm
		float s, d = 0;
		for (int i = 0; i < 3; ++i)
		{
			if (center.ptr()[i] < min.ptr()[i])
			{
				s = center.ptr()[i] - min.ptr()[i];
				d += s * s; 
			}
			else if(center.ptr()[i] > max.ptr()[i])
			{
				s = center.ptr()[i] - max.ptr()[i];
				d += s * s; 
			}
		}
		return d <= radius * radius;

	}

	bool Math::intersects(const Plane& plane, const AxisAlignedBox& box)
	{
		return (plane.getSide(box) == Plane::BOTH_SIDE);
	}

	bool Math::intersects(const Sphere& sphere, const Plane& plane)
	{
		return (
			Math::fabs(plane.getDistance(sphere.getCenter()))
			<= sphere.getRadius() );
	}

	std::pair<bool, float> Math::intersects(const Ray& ray, const Plane& plane)
	{
		float denom = plane.normal.dotProduct(ray.getDirection());
		if (Math::fabs(denom) < std::numeric_limits<float>::epsilon())
		{
			// Parallel
			return std::pair<bool, float>(false, (float)0);
		}
		else
		{
			float nom = plane.normal.dotProduct(ray.getOrigin()) + plane.d;
			float t = -(nom/denom);
			return std::pair<bool, float>(t >= 0, t);
		}
	}

	std::pair<bool, float> Math::intersects(const Ray& ray, const AxisAlignedBox& box)
	{
		if (box.isNull()) return std::pair<bool, float>(false, (float)0);
		if (box.isInfinite()) return std::pair<bool, float>(true, (float)0);

		float lowt = 0.0f;
		float t;
		bool hit = false;
		Vector3 hitpoint;
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();
		const Vector3& rayorig = ray.getOrigin();
		const Vector3& raydir = ray.getDirection();

		// Check origin inside first
		if ( rayorig > min && rayorig < max )
		{
			return std::pair<bool, float>(true, (float)0);
		}

		// Check each face in turn, only check closest 3
		// Min x
		if (rayorig.x <= min.x && raydir.x > 0)
		{
			t = (min.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max x
		if (rayorig.x >= max.x && raydir.x < 0)
		{
			t = (max.x - rayorig.x) / raydir.x;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.y >= min.y && hitpoint.y <= max.y &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min y
		if (rayorig.y <= min.y && raydir.y > 0)
		{
			t = (min.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max y
		if (rayorig.y >= max.y && raydir.y < 0)
		{
			t = (max.y - rayorig.y) / raydir.y;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.z >= min.z && hitpoint.z <= max.z &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Min z
		if (rayorig.z <= min.z && raydir.z > 0)
		{
			t = (min.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}
		// Max z
		if (rayorig.z >= max.z && raydir.z < 0)
		{
			t = (max.z - rayorig.z) / raydir.z;
			if (t >= 0)
			{
				// Substitute t back into ray and check bounds and dist
				hitpoint = rayorig + raydir * t;
				if (hitpoint.x >= min.x && hitpoint.x <= max.x &&
					hitpoint.y >= min.y && hitpoint.y <= max.y &&
					(!hit || t < lowt))
				{
					hit = true;
					lowt = t;
				}
			}
		}

		return std::pair<bool, float>(hit, lowt);
	} 

	std::pair<bool, float> Math::intersects(const Ray& ray, const Sphere& sphere, 
		bool discardInside)
	{
		const Vector3& raydir = ray.getDirection();
		// Adjust ray origin relative to sphere center
		const Vector3& rayorig = ray.getOrigin() - sphere.getCenter();
		float radius = sphere.getRadius();

		// Check origin inside first
		if (rayorig.squaredLength() <= radius*radius && discardInside)
		{
			return std::pair<bool, float>(true, (float)0);
		}

		// Mmm, quadratics
		// Build coeffs which can be used with std quadratic solver
		// ie t = (-b +/- sqrt(b*b + 4ac)) / 2a
		float a = raydir.dotProduct(raydir);
		float b = 2 * rayorig.dotProduct(raydir);
		float c = rayorig.dotProduct(rayorig) - radius*radius;

		// Calc determinant
		float d = (b*b) - (4 * a * c);
		if (d < 0)
		{
			// No intersection
			return std::pair<bool, float>(false, (float)0);
		}
		else
		{
			// BTW, if d=0 there is one intersection, if d > 0 there are 2
			// But we only want the closest one, so that's ok, just use the 
			// '-' version of the solver
			float t = ( -b - Math::sqrt(d) ) / (2 * a);
			if (t < 0)
				t = ( -b + Math::sqrt(d) ) / (2 * a);
			return std::pair<bool, float>(true, t);
		}
	}

	template<class T>
	std::pair<bool, float> _intersects(const Ray& ray, const T& planes, bool normalIsOutside)
	{
		typename T::const_iterator planeit, planeitend;
		planeitend = planes.end();
		bool allInside = true;
		std::pair<bool, float> ret;
		std::pair<bool, float> end;
		ret.first = false;
		ret.second = 0.0f;
		end.first = false;
		end.second = 0;


		// derive side
		// NB we don't pass directly since that would require Plane::Side in 
		// interface, which results in recursive includes since Math is so fundamental
		Plane::Side outside = normalIsOutside ? Plane::POSITIVE_SIDE : Plane::NEGATIVE_SIDE;

		for (planeit = planes.begin(); planeit != planeitend; ++planeit)
		{
			const Plane& plane = *planeit;
			// is origin outside?
			if (plane.getSide(ray.getOrigin()) == outside)
			{
				allInside = false;
				// Test single plane
				std::pair<bool, float> planeRes = 
					ray.intersects(plane);
				if (planeRes.first)
				{
					// Ok, we intersected
					ret.first = true;
					// Use the most distant result since convex volume
					ret.second = std::max(ret.second, planeRes.second);
				}
				else
				{
					ret.first =false;
					ret.second=0.0f;
					return ret;
				}
			}
			else
			{
				std::pair<bool, float> planeRes = 
					ray.intersects(plane);
				if (planeRes.first)
				{
					if( !end.first )
					{
						end.first = true;
						end.second = planeRes.second;
					}
					else
					{
						end.second = std::min( planeRes.second, end.second );
					}

				}

			}
		}

		if (allInside)
		{
			// Intersecting at 0 distance since inside the volume!
			ret.first = true;
			ret.second = 0.0f;
			return ret;
		}

		if( end.first )
		{
			if( end.second < ret.second )
			{
				ret.first = false;
				return ret;
			}
		}
		return ret;
	}

	std::pair<bool, float> Math::intersects( const Ray& ray, const vector<Plane>::type& planeList, bool normalIsOutside )
	{
		return _intersects( ray, planeList, normalIsOutside );
	}

	std::pair<bool, float> Math::intersects( const Ray& ray, const list<Plane>::type& planeList, bool normalIsOutside )
	{
		return _intersects( ray, planeList, normalIsOutside );
	}

	bool Math::inverse_matrix2(float v[4])
	{
		// º∆À„2x2ƒÊæÿ’Û
		float a = v[0];
		float b = v[1];
		float c = v[2];
		float d = v[3];

		float m_t = a * d - b * c;

		v[0] = d;
		v[1] = -b;
		v[2] = -c;
		v[3] = a;

		if ( m_t != 0.0f )
		{
			v[0] /= m_t;
			v[1] /= m_t;
			v[2] /= m_t;
			v[3] /= m_t;
			return true;
		}

		return false; // ∆Ê“Ïæÿ’Û
	}

	void Math::cal_tangent(const Vector3& va, const Vector3& vd, const Vector3& vc,
		const Vector2& uva, const Vector2& uvd, const Vector2& uvc,
		Vector3& tangent, Vector3& binormal)
	{
		// tri ADC
		//| T | = | Uad  Vad | -1 | AD |  
		//| B |   | Uac  Vac |    | AC |

		float matUV[4] = 
		{
			uvd.x-uva.x,  uvd.y-uva.y,
			uvc.x-uva.x,  uvc.y-uva.y
		};

		inverse_matrix2(matUV);

		vec3f ad = vd - va;
		vec3f ac = vc - va;

		tangent  = ad * matUV[0] + ac * matUV[1];
		binormal = ad * matUV[2] + ac * matUV[3];
	}
	
	void Math::gramSchmidtOrthogonalize(Vector3& t, const Vector3& b, const Vector3& n )
	{
		// Gram-Schmidt orthogonalize
		t = t - n * n.dotProduct(t);
		t.normalise();

		// Calculate handedness
		if ( n.crossProduct(t).dotProduct(b) < 0.0f )
		{
			t = t * -1.0f;
		}
	}

	void Math::screen_to_cube_space(Vector3& v, int screenWidth, int screenHeight, int cursorX, int cursorY, float z)
	{
		v.x = 2.0f * cursorX / (float)screenWidth - 1.0f;
		v.y = 1.0f - 2.0f * cursorY / (float)screenHeight;
		v.z = z;
	}
}