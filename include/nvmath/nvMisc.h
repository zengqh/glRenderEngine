#ifndef NV_MISC_H
#define NV_MISC_H

#include "nvVector.h"
#include "nvMatrix.h"
#include "nvConst.h"
#include <math.h>
#include <stdlib.h>

namespace nv
{
    //////////////////////////////////////////////////////////////////////////
    // ����
    template<typename T>
    inline bool is_zero( T a )
    {
        return fabs(a) < NV_FLT_EPSILON;
    }
    
    template<typename T>
    inline T clamp( T x, T m1, T m2 )
    {
        if ( x < m1 )
            x = m1;
        else if ( x > m2 )
            x = m2;
        
        return x;
    }
    
    template<class T>
    inline T safe_acos( T t )
    {
        t = nv::clamp( t, (T)-1, (T)1 );
        return acos(t);
    }

    template<class T>
    inline T safe_asin( T t )
    {
        t = nv::clamp( t, (T)-1, (T)1 );
        return asin(t);
    }

    inline float rand_normal()
    {
        return (float)rand() / (float)RAND_MAX;
    }

    inline float rand_pn_normal()
    {
        return rand_normal() * 2.0f - 1.0f;
    }

    template<typename T>
    inline int meta_1_dim_1_equation( T b, T c, T& r )
    {
        // b = 0�޽�
        if ( is_zero(b) )
            return 0;
    
        r = -c / b;
        return 1;
    }

    template<typename T>
    inline int meta_1_dim_2_equation( T a, T b, T c, T& r1, T& r2 )
    {
        // �˻�Ϊ1�η���
        if ( is_zero(a) )
        {
            int n = meta_1_dim_1_equation( b, c, r1 );
            if ( n > 0 )
                r2 = r1;
            return n;
        }

        // ����η���
        T delta = b * b - (T)4 * a * c;

        if ( is_zero(delta) ) // delta = 0, 1����
        {
            
            r1 = r2 = -b / ((T)2 * a);
            return 1;
        }

        if ( delta > 0 ) // delta > 0, 2����
        {
            delta = sqrt(delta);

            T a_2 = (T)2 * a;

            r1 = (-b + delta) / a_2;
            r2 = (-b - delta) / a_2;

            return 2;
        }

        // delta < 0 �޽�
        return 0;
    }

    template<typename T>
    T get_equal_moving( const nv::vec3<T>& lo, const nv::vec3<T>& ldir, const nv::vec3<T>& p1, const nv::vec3<T>& p2 )
    {
        // ����l�����ltʹ��lt��p1��p2�������
        nv::vec3<T> m1 = lo - p1;
        nv::vec3<T> m2 = lo - p2;

        // t = (m2^2 - m1^2) / (2 * (m1 - m2) * d)
        T td = nv::dot(p2 - p1, ldir);
        if ( nv::is_zero(td) )
            return (T)0;

        T tu = nv::dot(m2, m2) - nv::dot(m1, m1);
        T t = tu / td * (T)0.5;
        return t;
    }

    //////////////////////////////////////////////////////////////////////////
    // ��ֵ
    template<typename T>
    inline T lerp( T t1, T t2, T s )
    {
        return (t2 - t1) * s + t1;
    }

    template<typename T>
    inline T lerp2( T v1, T v2, T v3, T v4, T sx, T sy )
    {
        //    v1 --- v2
        //    |      |
        //    v3 --- v4

        T v12 = lerp( v1, v2, sx );
        T v34 = lerp( v3, v4, sx );

        return lerp( v12, v34, sy );
    }

    //////////////////////////////////////////////////////////////////////////
    // ��Χ
    template<typename T>
    inline void make_swap( T& x, T& y )
    {
        T t = y;
        y = x;
        x = t;
    }

    template<typename T>
    inline void sort_point( T& p1, T& p2 )
    {
        if ( p1 > p2 )
            make_swap( p1, p2 );
    }

    template<typename T>
    inline void sort_point( vec2<T>& p1, vec2<T>& p2 )
    {
        sort_point( p1.x, p2.x );
        sort_point( p1.y, p2.y );
    }

    template<typename T>
    inline void sort_point( vec3<T>& p1, vec3<T>& p2 )
    {
        sort_point( p1.x, p2.x );
        sort_point( p1.y, p2.y );
        sort_point( p1.z, p2.z );
    }

    template<typename T>
    inline T get_min( T a, T b )
    {
        return a < b ? a : b;
    }

    template<typename T>
    inline void get_min( vec3<T>&r, const vec3<T>& a, const vec3<T>& b )
    {
        r.x = get_min( a.x, b.x );
        r.y = get_min( a.y, b.y );
        r.z = get_min( a.z, b.z );
    }

    template<typename T>
    inline T get_max( T a, T b )
    {
        return a < b ? b : a;
    }

    template<typename T>
    inline void get_max( vec3<T>&r, const vec3<T>& a, const vec3<T>& b )
    {
        r.x = get_max( a.x, b.x );
        r.y = get_max( a.y, b.y );
        r.z = get_max( a.z, b.z );
    }

    template<typename T>
    inline bool is_clamp( T x, T m1, T m2 )
    {
        if ( x < m1 )
            return false;
        else if ( x > m2 )
            return false;

        return true;
    }

    template<typename T>
    inline bool is_equal( T a, T b )
    {
        return fabs(a - b) < NV_FLT_EPSILON;
    }

    template<typename T>
    inline bool is_equal( const vec2<T>& a, const vec2<T>& b )
    {
        return is_equal(a.x, b.x) && is_equal(a.y, b.y);
    }

	template<typename T>
	inline bool is_equal( const vec3<T>& a, const vec3<T>& b )
	{
		return is_equal(a.x, b.x) && is_equal(a.y, b.y) && is_equal(a.z, b.z);
	}

    template<typename T>
    inline bool is_less_equal( T a, T b )
    {
        return (a < b) || is_equal(a, b);
    }

    template<typename T>
    inline bool is_less_strict( T a, T b )
    {
        return (a < b) && !is_equal(a, b);
    }

    template<typename T>
    inline bool is_greater_equal( T a, T b )
    {
        return (a > b) || is_equal(a, b);
    }

    template<typename T>
    inline bool is_greater_strict( T a, T b )
    {
        return (a > b) && !is_equal(a, b);
    }

    template<typename T>
    inline bool is_between_strict( T x, T m1, T m2 )
    {
        sort_point( m1, m2 );
        return is_less_equal(m1, x) && is_less_equal(x, m2);
    }

    template<typename T>
    inline bool is_between_strict( const vec2<T>& x, const vec2<T>& m1, const vec2<T>& m2 )
    {
        return is_between_strict( x.x, m1.x, m2.x ) && is_between_strict( x.y, m1.y, m2.y );
    }

    template<typename T>
    inline bool is_between_strict( const vec3<T>& x, const vec3<T>& m1, const vec3<T>& m2 )
    {
        return is_between_strict( x.x, m1.x, m2.x ) && is_between_strict( x.y, m1.y, m2.y ) && 
            is_between_strict( x.z, m1.z, m2.z );
    }

    template<typename T>
    void aabb_clear( vec3<T>& vmin, vec3<T>& vmax )
    {
        vmin = vec3<T>( 100, 100, 100 );
        vmax = vec3<T>( -100, -100, -100 );
    }

    template<typename T>
    inline bool is_aabb_empty( const vec3<T>& vmin, const vec3<T>& vmax )
    {
        return is_greater_strict( vmin.x, vmax.x ) || 
            is_greater_strict( vmin.y, vmax.y ) || 
            is_greater_strict( vmin.z, vmax.z );
    }

    template<typename T>
    inline void merge_aabb( vec3<T>& vmin_a, vec3<T>& vmax_a, const vec3<T>& vmin_b, const vec3<T>& vmax_b )
    {
        // a��
        if ( is_aabb_empty( vmin_a, vmax_a ) )
        {
            vmin_a = vmin_b;
            vmax_a = vmax_b;
            return;
        }

        // b��
        if ( is_aabb_empty( vmin_b, vmax_b ) )
            return;

        get_min( vmin_a, vmin_a, vmin_b );
        get_max( vmax_a, vmax_a, vmax_b );
    }

    template<typename T>
    inline void calc_aabb( const vec3<T>* v, int n, int stride, vec3<T>& minpt, vec3<T>& maxpt )
    {
        if ( n == 0 )
        {
            aabb_clear( minpt, maxpt );
            return;
        }

        minpt = maxpt = *v;

        for ( int i = 1; i < n; ++i )
        {
            const vec3<T>& pt = *(vec3<T>*)((char*)v + stride * i);

            if ( pt.x < minpt.x )
                minpt.x = pt.x;
            else if ( pt.x > maxpt.x )
                maxpt.x = pt.x;

            if ( pt.y < minpt.y )
                minpt.y = pt.y;
            else if ( pt.y > maxpt.y )
                maxpt.y = pt.y;

            if ( pt.z < minpt.z )
                minpt.z = pt.z;
            else if ( pt.z > maxpt.z )
                maxpt.z = pt.z;
        }
    }

    template<typename T>
    inline void calc_aabb( const vec3<T>* v, int n, vec3<T>& minpt, vec3<T>& maxpt )
    {
        calc_aabb( v, n, sizeof(vec3<T>), minpt, maxpt );
    }

    template<typename T>
    inline void calc_obb( const vec3<T>* v, int n, const vec3<T>& xdir, const vec3<T>& ydir, const vec3<T>& zdir,
        const vec3<T>& orig, vec2<T>& xrange, vec2<T>& yrange, vec2<T>& zrange )
    {
        // ����obb
        if ( n <= 0 )
        {
            xrange = yrange = zrange = vec2<T>(0, 0);
            return;
        }

        const vec3<T>* dirs[3] = { &xdir, &ydir, &zdir }; // �涨x,y,z�Ѿ����滯
        vec2<T>* ranges[3] = { &xrange, &yrange, &zrange }; // �����Ϸ�Χ

        T t = 0;

        for ( int i = 0; i < 3; ++i )
        {
            const vec3<T>& cur_dir = *(dirs[i]);
            vec2<T>& cur_range = *(ranges[i]);

            // ��һ����
            point_proj_ray( orig, cur_dir, v[0], t );
            cur_range.x = t;
            cur_range.y = t;

            // ������
            for ( int vi = 1; vi < n; ++vi )
            {
                point_proj_ray( orig, cur_dir, v[vi], t );

                if ( t < cur_range.x )
                    cur_range.x = t;
                else if ( t > cur_range.y )
                    cur_range.y = t;
            }
        }
    }

    template<typename T>
    inline void calc_obb_by_ball( const vec3<T>& pv, T radius, const vec3<T>& xdir, const vec3<T>& ydir, const vec3<T>& zdir,
        const vec3<T>& orig, vec2<T>& xrange, vec2<T>& yrange, vec2<T>& zrange )
    {
        // ����obbͨ��������
        const vec3<T>* dirs[3] = { &xdir, &ydir, &zdir }; // �涨x,y,z�Ѿ����滯
        vec2<T>* ranges[3] = { &xrange, &yrange, &zrange }; // �����Ϸ�Χ

        T t = 0;

        for ( int i = 0; i < 3; ++i )
        {
            const vec3<T>& cur_dir = *(dirs[i]);
            vec2<T>& cur_range = *(ranges[i]);

            // ͶӰ����
            point_proj_ray( orig, cur_dir, pv, t );

            // ��С���Χ
            cur_range.x = t - radius;
            cur_range.y = t + radius;
        }
    }

    template<typename T>
    inline bool obb_has_point( const matrix4<T>& obbWorld, 
        const vec2<T>& xrange, const vec2<T>& yrange, const vec2<T>& zrange, 
        const vec3<T>& pt )
    {
        const nv::vec3<T>& xdir = *(const nv::vec3<T>*)&obbWorld._11;
        const nv::vec3<T>& ydir = *(const nv::vec3<T>*)&obbWorld._21;
        const nv::vec3<T>& zdir = *(const nv::vec3<T>*)&obbWorld._31;
        const nv::vec3<T>& opos = *(const nv::vec3<T>*)&obbWorld._41;

        T t = 0;
        point_proj_ray( opos, xdir, pt, t );
        if ( !is_clamp(t, xdir.x, xdir.y) )
            return false;

        point_proj_ray( opos, ydir, pt, t );
        if ( !is_clamp(t, ydir.x, ydir.y) )
            return false;

        point_proj_ray( opos, zdir, pt, t );
        if ( !is_clamp(t, zdir.x, zdir.y) )
            return false;

        return true;
    }

    template<typename T>
    inline void get_ofrustum_planes( vec4<T>* planes, const matrix4<T>& obbWorld, 
        const vec2<T>& xrange, const vec2<T>& yrange, const vec2<T>& zrange )
    {
        const nv::vec3<T>& xdir = *(const nv::vec3<T>*)&obbWorld._11;
        const nv::vec3<T>& ydir = *(const nv::vec3<T>*)&obbWorld._21;
        const nv::vec3<T>& zdir = *(const nv::vec3<T>*)&obbWorld._31;
        const nv::vec3<T>& opos = *(const nv::vec3<T>*)&obbWorld._41;

        // x+ ����ָ��׶��
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[0].x;
            dir = -xdir;
            nv::vec3<T> pt = opos + xdir * xrange.y;
            planes[0].w = -nv::dot( dir, pt );
        }
        // x-
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[1].x;
            dir = xdir;
            nv::vec3<T> pt = opos + xdir * xrange.x;
            planes[1].w = -nv::dot( dir, pt );
        }
        // y+
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[2].x;
            dir = -ydir;
            nv::vec3<T> pt = opos + ydir * yrange.y;
            planes[2].w = -nv::dot( dir, pt );
        }
        // y-
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[3].x;
            dir = ydir;
            nv::vec3<T> pt = opos + ydir * yrange.x;
            planes[3].w = -nv::dot( dir, pt );
        }
        // z+
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[4].x;
            dir = -zdir;
            nv::vec3<T> pt = opos + zdir * zrange.y;
            planes[4].w = -nv::dot( dir, pt );
        }
        // z-
        {
            nv::vec3<T>& dir = *(nv::vec3<T>*)&planes[5].x;
            dir = zdir;
            nv::vec3<T> pt = opos + zdir * zrange.x;
            planes[5].w = -nv::dot( dir, pt );
        }
    }

    template<typename T>
    inline T get_min_dist2_aabb_to_pt( const vec3<T>& bmin, const vec3<T>& bmax, const vec3<T>& pt )
    {
        vec3<T> v[8];

        v[0] = vec3<T>( bmin.x, bmin.y, bmin.z );
        v[1] = vec3<T>( bmin.x, bmin.y, bmax.z );
        v[2] = vec3<T>( bmin.x, bmax.y, bmin.z );
        v[3] = vec3<T>( bmin.x, bmax.y, bmax.z );
        v[4] = vec3<T>( bmax.x, bmin.y, bmin.z );
        v[5] = vec3<T>( bmax.x, bmin.y, bmax.z );
        v[6] = vec3<T>( bmax.x, bmax.y, bmin.z );
        v[7] = vec3<T>( bmax.x, bmax.y, bmax.z );
        
        T dist2 = nv::square_norm( v[0] - pt );
        
        for ( int i = 1; i < 8; ++i )
        {
            T cur = nv::square_norm( v[i] - pt );
            if ( cur < dist2 )
                dist2 = cur;
        }

        return dist2;
    }

    //////////////////////////////////////////////////////////////////////////
    // �ǶȻ���
    template<typename T>
    inline T degree_to_rad( T d )
    {
        return NV_PI * d / 180;
    }

    template<typename T>
    inline T rad_to_degree( T r )
    {
        return 180 * r / NV_PI;
    }

    //////////////////////////////////////////////////////////////////////////
    // ��ͼͶӰ
    template<typename T>
    inline void make_cam_world_by_lookat( matrix4<T>& mat, const vec3<T>& eyePos, const vec3<T>& atPos, const vec3<T>& upDir )
    {
        // ����ͬD3DXMatrixLookAtLH�㷨
        // zaxis = normal(At - Eye)
        // xaxis = normal(cross(Up, zaxis))
        // yaxis = cross(zaxis, xaxis)

        nv::vec3<T>& xdir = *(nv::vec3<T>*)&mat._11;
        nv::vec3<T>& ydir = *(nv::vec3<T>*)&mat._21;
        nv::vec3<T>& zdir = *(nv::vec3<T>*)&mat._31;
        nv::vec3<T>& opos = *(nv::vec3<T>*)&mat._41;

        mat._14 = 0;
        mat._24 = 0;
        mat._34 = 0;
        mat._44 = 1;

        opos = eyePos;

        zdir = eyePos - atPos; // opengl -z���򳡾�
        zdir = nv::normalize( zdir );

        xdir = nv::cross( upDir, zdir );
        xdir = nv::normalize( xdir );

        ydir = nv::cross( zdir, xdir );
    }

    template<typename T>
    inline void make_ortho( matrix4<T>& mat, T left, T right, T bottom, T top, T nearVal, T farVal )
    {
        // ע��mat�����ȴ��(_�к��к�)�����Ժ�gl�ĵ��ߵ�
        T tx = - (right + left) / (right - left);
        T ty = - (top + bottom) / (top - bottom);
        T tz = - (farVal + nearVal) / (farVal - nearVal);

        mat._11 = 2 / (right - left); mat._12 = 0;                  mat._13 = 0;                       mat._14 = 0;
        mat._21 = 0;                  mat._22 = 2 / (top - bottom); mat._23 = 0;                       mat._24 = 0;
        mat._31 = 0;                  mat._32 = 0;                  mat._33 = -2 / (farVal - nearVal); mat._34 = 0;
        mat._41 = tx;                 mat._42 = ty;                 mat._43 = tz;                      mat._44 = 1;
    }

    template<typename T>
    inline void make_ortho( matrix4<T>& mat, T width, T height, T nearVal, T farVal )
    {
        // ��dx�汾ͬ
        T tz = - (farVal + nearVal) / (farVal - nearVal);

        mat._11 = 2 / width; mat._12 = 0;           mat._13 = 0;                       mat._14 = 0;
        mat._21 = 0;         mat._22 = 2 / height;  mat._23 = 0;                       mat._24 = 0;
        mat._31 = 0;         mat._32 = 0;           mat._33 = -2 / (farVal - nearVal); mat._34 = 0;
        mat._41 = 0;         mat._42 = 0;           mat._43 = tz;                      mat._44 = 1;
    }

    template<typename T>
    inline void make_perspective( matrix4<T>& mat, T fovy, T aspect, T zNear, T zFar )
    {
        T f   = (T)1.0 / tan( fovy * (T)0.5 );
        T n_f = zNear - zFar;

        mat._11 = f / aspect; mat._12 = 0; mat._13 = 0;                        mat._14 = 0;
        mat._21 = 0;          mat._22 = f; mat._23 = 0;                        mat._24 = 0;
        mat._31 = 0;          mat._32 = 0; mat._33 = (zFar + zNear) / n_f;     mat._34 = -1;
        mat._41 = 0;          mat._42 = 0; mat._43 = (2 * zFar * zNear) / n_f; mat._44 = 0;
    }

    template<typename T>
    inline void make_perspective( matrix4<T>& mat, T left, T right, T bottom, T top, T nearVal, T farVal )
    {
        T X = 2 * nearVal / (right - left);
        T Y = 2 * nearVal / (top - bottom);

        T A = (right + left) / (right - left);
        T B = (top + bottom) / (top - bottom);
        T C = -(farVal + nearVal) / (farVal - nearVal);
        T D = -2 * farVal * nearVal / (farVal - nearVal);

        mat._11 = X; mat._12 = 0; mat._13 = 0; mat._14 = 0;
        mat._21 = 0; mat._22 = Y; mat._23 = 0; mat._24 = 0;
        mat._31 = A; mat._32 = B; mat._33 = C; mat._34 = -1;
        mat._41 = 0; mat._42 = 0; mat._43 = D; mat._44 = 0;
    }

    template<typename T>
    inline void make_perspective_by_far( matrix4<T>& mat, T left, T right, T bottom, T top, T nearVal, T farVal )
    {
        T scale = nearVal / farVal;

        left   *= scale;
        right  *= scale;
        bottom *= scale;
        top    *= scale;

        make_perspective( mat, left, right, bottom, top, nearVal, farVal );
    }

    template<typename T>
    inline void get_perspective_pts( vec3<T>* vts, T fovy, T aspect, T nearPlane, T farPlane )
    {
        // �õ���׶��8����
        // ע��fov����y-zƽ���ϵ�
        float tanFovY  = tan(fovy * (T)0.5);

        float yFarRad  = farPlane * tanFovY;
        float xFarRad  = yFarRad  * aspect;  // xrad / yrad = aspect = w / h

        float yNearRad = nearPlane * tanFovY;
        float xNearRad = yNearRad  * aspect;

        farPlane  = -farPlane; // gl����
        nearPlane = -nearPlane;

        vts[0].x = -xFarRad; vts[0].y = -yFarRad; vts[0].z = farPlane;
        vts[1].x = -xFarRad; vts[1].y =  yFarRad; vts[1].z = farPlane;
        vts[2].x =  xFarRad; vts[2].y = -yFarRad; vts[2].z = farPlane;
        vts[3].x =  xFarRad; vts[3].y =  yFarRad; vts[3].z = farPlane;

        vts[4].x = -xNearRad; vts[4].y = -yNearRad; vts[4].z = nearPlane;
        vts[5].x = -xNearRad; vts[5].y =  yNearRad; vts[5].z = nearPlane;
        vts[6].x =  xNearRad; vts[6].y = -yNearRad; vts[6].z = nearPlane;
        vts[7].x =  xNearRad; vts[7].y =  yNearRad; vts[7].z = nearPlane;
    }

    template<typename T>
    inline T get_znear_from_perspective( const matrix4<T>& mat )
    {
        return mat._43 / (mat._33 - 1);
    }

    template<typename T>
    inline T get_zfar_from_perspective( const matrix4<T>& mat )
    {
        return mat._43 / (mat._33 + 1);
    }

    template<typename T>
    inline T get_cothf_from_perspective( const matrix4<T>& mat )
    {
        return mat._22;
    }

    inline void proj_to_window( float xInProj, float yInProj, int screenWidth, int screenHeight, int& xInW, int& yInW )
    {
        xInW = int((xInProj + 1.0f) / 2.0f * screenWidth);
        yInW = int((1.0f - yInProj) / 2.0f * screenHeight);
    }

    template<typename T>
    inline void screen_to_cube_space( vec3<T>& v, int screenWidth, int screenHeight, int cursorX, int cursorY )
    {
        // �Ƚ���Ļ����任��������ռ䣬��ʹ���ӿڱ任����任(�������Ͻ�ΪX=0, Y=0)
        v.x = (T)(2) * cursorX / (T)(screenWidth) - (T)(1);
        v.y = (T)(1) - (T)(2) * cursorY / (T)(screenHeight); // gl����
        v.z = 0;
    }

    template<typename T>
    inline void screen_to_cube_space( vec3<T>& v, int screenWidth, int screenHeight, T cursorX, T cursorY, T z )
    {
        // �Ƚ���Ļ����任��������ռ䣬��ʹ���ӿڱ任����任(�������Ͻ�ΪX=0, Y=0)
        v.x = (T)(2) * cursorX / (T)(screenWidth) - (T)(1);
        v.y = (T)(1) - (T)(2) * cursorY / (T)(screenHeight); // gl����
        v.z = z;
    }

    template<typename T>
    inline void cube_to_view_space( vec3<T>& v, const matrix4<T>& matProj )
    {
        // dx proj 
        // x' = x * cot(fov / 2) / asp
        // y' = y * cot(fov / 2)
        // z' = Zf * (z - Zn) / (Zf - Zn)
        // w' = z

        // gl proj 
        // x' = x * cot(fov / 2) / asp
        // y' = y * cot(fov / 2)
        // z' = (z * (Zf + Zn) + 2 * Zf * Zn) / (Zn - Zf)
        // w' = -z

        // �ٽ�ͶӰ�ռ�����任��������ռ�z=1(gl:z=-1)����ȥ
        v.x /=  matProj._11;    // xscale = cot(fov/2) / asp
        v.y /=  matProj._22;    // yscale = cot(fov/2)
        v.z = -1;               // gl -z
    }

    template<typename T>
    inline void view_to_world_space( vec3<T>& v, const matrix4<T>& matCam )
    {
        // ������������ռ䣬�����λ�ü�ԭ��(0,0,0)����ô�õ���ʰȡ���߾���v - 0 = v
        // ���ԭ������ߴ�������ռ�任����������ȥ

        // ����ʹ�ú���D3DXVec3TransformNormal�任����(��w=0)��D3DXVec3TransformCoord(��w=1)���任ԭ��
        // �����ֶ���ɼ��㣬��ʵ����1*4�����4*4�������

        // ��v.w����0
        vec3<T> vk;

        vk.x = v.x * matCam._11 + v.y * matCam._21 + v.z * matCam._31 /*+ 0 * matCam._41*/;
        vk.y = v.x * matCam._12 + v.y * matCam._22 + v.z * matCam._32 /*+ 0 * matCam._42*/;
        vk.z = v.x * matCam._13 + v.y * matCam._23 + v.z * matCam._33 /*+ 0 * matCam._43*/;

        v = vk;
    }

    template<typename T>
    inline void get_view_world_pos( vec3<T>& v, const matrix4<T>& matCam )
    {
        // ԭ��(0,0,0,1)
        v.x = /*0 * matCam._11 + 0 * matCam._21 + 0 * matCam._31 + 1 **/ matCam._41;
        v.y = /*0 * matCam._12 + 0 * matCam._22 + 0 * matCam._32 + 1 **/ matCam._42;
        v.z = /*0 * matCam._13 + 0 * matCam._23 + 0 * matCam._33 + 1 **/ matCam._43;
    }

    template<typename T>
    inline void get_pick_ray( vec3<T>& pickRayOrig, vec3<T>& pickRayDir, 
        const matrix4<T>& matCam, const matrix4<T>& matProj, int screenWidth, int screenHeight,
        int cursorX, int cursorY )
    {
        // �Ƚ���Ļ����任��������ռ䣬��ʹ���ӿڱ任����任(�������Ͻ�ΪX=0, Y=0)
        screen_to_cube_space( pickRayDir, screenWidth, screenHeight, cursorX, cursorY );

        // �ٽ�ͶӰ�ռ�����任��������ռ�z=1(gl:z=-1)����ȥ
        cube_to_view_space( pickRayDir, matProj );

        // ������������ռ䣬�����λ�ü�ԭ��(0,0,0)����ô�õ���ʰȡ���߾���v - 0 = v
        // ���ԭ������ߴ�������ռ�任����������ȥ
        view_to_world_space( pickRayDir, matCam );

        // �������
        pickRayDir = normalize( pickRayDir );

        // ԭ��(0,0,0,1)
        get_view_world_pos( pickRayOrig, matCam );
    }

    template<typename T>
    inline void get_pick_ray_comm( vec3<T>& pickRayOrig, vec3<T>& pickRayDir, 
        const matrix4<T>& matView, const matrix4<T>& matProj, int screenWidth, int screenHeight,
        int cursorX, int cursorY )
    {
        // �Ƚ���Ļ����任��������ռ䣬��ʹ���ӿڱ任����任(�������Ͻ�ΪX=0, Y=0)
        vec4<T> v;
        screen_to_cube_space( *(vec3<T>*)(&v), screenWidth, screenHeight, cursorX, cursorY );
        v.z = 0.0f;
        v.w = 1.0f;

        // ͶӰ�����view space
        matrix4<T> matProjInv = inverse( matProj );
        v = matProjInv * v;
        v /= v.w;

        // �������ֱ�ӵ�world space
        matrix4<T> matViewInv = inverse( matView );
        v = matViewInv * v;

        // ԭ��(0,0,0,1)
        get_view_world_pos( pickRayOrig, matViewInv );

        // �õ�����
        pickRayDir = *(vec3<T>*)(&v) - pickRayOrig;
        pickRayDir = normalize( pickRayDir );
    }

    template<typename T>
    inline void normalize_plane( vec4<T>& plane )
    {
        T mag = sqrt( plane.x * plane.x + plane.y * plane.y + plane.z * plane.z );

        if ( mag != (T)0 )
        {
            plane.x /= mag;
            plane.y /= mag;
            plane.z /= mag;
            plane.w /= mag;
        }
    }

    template<typename T>
    inline void get_frustum_from_matvp( vec4<T>* planes, const matrix4<T>& matvp )
    {
        // Left clipping plane
        planes[0].x = matvp._14 + matvp._11;
        planes[0].y = matvp._24 + matvp._21;
        planes[0].z = matvp._34 + matvp._31;
        planes[0].w = matvp._44 + matvp._41;

        // Right clipping plane
        planes[1].x = matvp._14 - matvp._11;
        planes[1].y = matvp._24 - matvp._21;
        planes[1].z = matvp._34 - matvp._31;
        planes[1].w = matvp._44 - matvp._41;

        // Top clipping plane
        planes[2].x = matvp._14 - matvp._12;
        planes[2].y = matvp._24 - matvp._22;
        planes[2].z = matvp._34 - matvp._32;
        planes[2].w = matvp._44 - matvp._42;

        // Bottom clipping plane
        planes[3].x = matvp._14 + matvp._12;
        planes[3].y = matvp._24 + matvp._22;
        planes[3].z = matvp._34 + matvp._32;
        planes[3].w = matvp._44 + matvp._42;

        // Near clipping plane
        planes[4].x = matvp._14 + matvp._13;
        planes[4].y = matvp._24 + matvp._23;
        planes[4].z = matvp._34 + matvp._33;
        planes[4].w = matvp._44 + matvp._43;

        // Far clipping plane
        planes[5].x = matvp._14 - matvp._13;
        planes[5].y = matvp._24 - matvp._23;
        planes[5].z = matvp._34 - matvp._33;
        planes[5].w = matvp._44 - matvp._43;

        // Normalize the plane equations
        normalize_plane(planes[0]);
        normalize_plane(planes[1]);
        normalize_plane(planes[2]);
        normalize_plane(planes[3]);
        normalize_plane(planes[4]);
        normalize_plane(planes[5]);
    }

    template<typename T>
    inline T plane_dot_coord( const vec4<T>& plane, const vec3<T>& pt )
    {
        T n = plane.x * pt.x + plane.y * pt.y + plane.z * pt.z + plane.w;
        return n;
    }

    template<typename T>
    bool frustum_has_point( const vec4<T>* planes, const vec3<T>& pt )
    {
        for ( int i=0; i<6; ++i )
        {
            T x = plane_dot_coord( planes[i], pt );

            // A.x + B.y + C.z + D < 0 ���ڸ���ռ���
            if ( x < 0 )
                return false;
        }

        // ������6��ƽ�������ռ��ڣ������ڽ�׶���ڲ�
        return true;
    }

    template<typename T>
    inline bool frustum_cull_aabb( const vec4<T>* planes, const vec3<T>& bmin, const vec3<T>& bmax )
    {
        // ����aabb�ų��㷨������AABB���ж���ȫ���ڽ�׶������һ��ƽ���⣬��ôAABB�϶����ų����⡣
        // ��ע�⣬�����������ཻ��������Ǵ���ģ����������ڽ�׶���ĸ���ǵ������
        // ������Σ��㷨����ʧ�ܣ�û�ų���ֻ�����Ժ�໨һЩʱ�䴦����ѡ�

        vec3<T> /*vmin,*/ vmax;

        for ( int k=0; k<6; ++k )
        {
            const vec4<T>& plane = planes[k];

            // ���뵱ǰ�淨����ƥ���AABB�еĶԽ���
            // ��ôֻҪ����������vmin,vmax��ƽ�������Լ��ɣ�����8��������ȫ�����ԣ�
            // ��vmax - vmin�õ���������ƽ�淨����ƥ��
            for ( int i=0; i<3; ++i )
            {
                if ( plane[i] >= 0 )
                {
                    //vmin[i] = bmin[i];  // ���ڲ����ཻ�Ͱ������ԣ����Բ���Ҫvmin
                    vmax[i] = bmax[i]; 
                }
                else
                {
                    //vmin[i] = bmax[i];
                    vmax[i] = bmin[i];
                }
            }

            // ������ĵ���ڵ�ǰ�����棬��ô���е㶼�����棬Ҳ����aabb�϶��ڽ�׶����
            T x = plane_dot_coord( plane, vmax );
            if ( x < 0 )
                return true; // ��ʾ�϶�Ҫ���ü���
        }

        // ��ʾ��һ�����ü���
        return false;
    }

    //////////////////////////////////////////////////////////////////////////
    // �ཻ����
    template<typename T>
    inline bool aabb_intersect_aabb( const vec3<T>& min_a, const vec3<T>& max_a, 
        const vec3<T>& min_b, const vec3<T>& max_b )
    {
        if ( min_a.x > max_b.x || min_b.x > max_a.x )
            return false;

        if ( min_a.y > max_b.y || min_b.y > max_a.y )
            return false;

        if ( min_a.z > max_b.z || min_b.z > max_a.z )
            return false;

        return true;
    }

    template<typename T>
    inline bool aabb_intersect_aabb_equal( const vec2<T>& min_a, const vec2<T>& max_a, 
        const vec2<T>& min_b, const vec2<T>& max_b )
    {
        if ( !is_less_equal( min_a.x, max_b.x ) || !is_less_equal( min_b.x, max_a.x ) )
            return false;

        if ( !is_less_equal( min_a.y, max_b.y ) || !is_less_equal( min_b.y, max_a.y ) )
            return false;

        return true;
    }

    template<typename T>
    inline bool aabb_intersect_aabb_strict( const vec2<T>& min_a, const vec2<T>& max_a, 
        const vec2<T>& min_b, const vec2<T>& max_b )
    {
        if ( !is_less_strict( min_a.x, max_b.x ) || !is_less_strict( min_b.x, max_a.x ) )
            return false;

        if ( !is_less_strict( min_a.y, max_b.y ) || !is_less_strict( min_b.y, max_a.y ) )
            return false;

        return true;
    }

    template<typename T>
    inline bool aabb_intersect_aabb_equal( const vec3<T>& min_a, const vec3<T>& max_a, const vec3<T>& min_b, const vec3<T>& max_b )
    {
        if ( !is_less_equal( min_a.x, max_b.x ) || !is_less_equal( min_b.x, max_a.x ) )
            return false;

        if ( !is_less_equal( min_a.y, max_b.y ) || !is_less_equal( min_b.y, max_a.y ) )
            return false;

        if ( !is_less_equal( min_a.z, max_b.z ) || !is_less_equal( min_b.z, max_a.z ) )
            return false;

        return true;
    }

    template<typename T>
    inline bool aabb_intersect_aabb_strict( const vec3<T>& min_a, const vec3<T>& max_a, const vec3<T>& min_b, const vec3<T>& max_b )
    {
        if ( !is_less_strict( min_a.x, max_b.x ) || !is_less_strict( min_b.x, max_a.x ) )
            return false;

        if ( !is_less_strict( min_a.y, max_b.y ) || !is_less_strict( min_b.y, max_a.y ) )
            return false;

        if ( !is_less_strict( min_a.z, max_b.z ) || !is_less_strict( min_b.z, max_a.z ) )
            return false;

        return true;
    }

    template<typename T>
    inline bool aabb_has_pt_equal( const vec2<T>& min_a, const vec2<T>& max_a, const vec2<T>& pt )
    {
        return is_less_equal(min_a.x, pt.x) && is_less_equal(pt.x, max_a.x) &&
               is_less_equal(min_a.y, pt.y) && is_less_equal(pt.y, max_a.y);
    }

    template<typename T>
    inline bool aabb_has_pt_strict( const vec2<T>& min_a, const vec2<T>& max_a, const vec2<T>& pt )
    {
        return is_less_strict(min_a.x, pt.x) && is_less_strict(pt.x, max_a.x) &&
            is_less_strict(min_a.y, pt.y) && is_less_strict(pt.y, max_a.y);
    }

    template<typename T>
    inline bool ray_intersect_plane( const vec3<T>& rayOrig, const vec3<T>& rayDir, const vec4<T>& plane, T& t )
    {
        // ray vs plane
        // ray = p0 + tu ���� plane = (n, d)
        // t = (-d - n.p0) / (n.u)

        T n_u = plane.x * rayDir.x + plane.y * rayDir.y + plane.z * rayDir.z;

        if ( fabs(n_u) < NV_FLT_EPSILON )
        {
            t = NV_FLT_MAX;
            return false; // ƽ��
        }

        T n_p0 = plane.x * rayOrig.x + plane.y * rayOrig.y + plane.z * rayOrig.z;

        t = (-plane.w - n_p0) / n_u;

        return true; // �ཻ
    }

    template<class T>
    inline bool ray_intersect_aabb( const vec3<T>& rayOrig, const vec3<T>& rayDir, const vec3<T>& minPt, const vec3<T>& maxPt, T* s = 0 )
    {
        // ray vs aabb
        if ( s ) *s = -1;

        // ���Ⱦ������ĸ������ƽ��
        bool inside = true;

        T xt;
        if ( rayOrig.x < minPt.x ) // ��min���
        {
            xt = minPt.x - rayOrig.x;
            xt /= rayDir.x; // ͶӰ��
            inside = false;
        }
        else if ( rayOrig.x > maxPt.x ) // ��max�Ҳ�
        {
            xt = maxPt.x - rayOrig.x;
            xt /= rayDir.x;
            inside = false;
        }
        else // ����ԭ����aabb�ڲ�
        {
            xt = -1.0f;
        }

        T yt;
        if ( rayOrig.y < minPt.y ) // ��min���
        {
            yt = minPt.y - rayOrig.y;
            yt /= rayDir.y; // ͶӰ��
            inside = false;
        }
        else if ( rayOrig.y > maxPt.y ) // ��max�Ҳ�
        {
            yt = maxPt.y - rayOrig.y;
            yt /= rayDir.y;
            inside = false;
        }
        else // ����ԭ����aabb�ڲ�
        {
            yt = -1.0f;
        }

        T zt;
        if ( rayOrig.z < minPt.z ) // ��min���
        {
            zt = minPt.z - rayOrig.z;
            zt /= rayDir.z; // ͶӰ��
            inside = false;
        }
        else if ( rayOrig.z > maxPt.z ) // ��max�Ҳ�
        {
            zt = maxPt.z - rayOrig.z;
            zt /= rayDir.z;
            inside = false;
        }
        else // ����ԭ����aabb�ڲ�
        {
            zt = -1.0f;
        }

        // �������ڲ�
        if ( inside )
            return true;

        // ѡ�������3��ǰ��֮һ
        int which = 0;
        T t = xt;
        if ( yt > t ) // ˭�ıȴ����˭
        {
            which = 1;
            t = yt;
        }

        if ( zt > t )
        {
            which = 2;
            t = zt;
        }

        switch ( which )
        {
        case 0: // x������󣬾���yzƽ��
            {
                T y = rayOrig.y + rayDir.y * t;
                if ( y < minPt.y || y > maxPt.y )
                    return false;
                T z = rayOrig.z + rayDir.z * t;
                if ( z < minPt.z || z > maxPt.z )
                    return false;
            }
            break;

        case 1: // y������󣬾���xzƽ��
            {
                T x = rayOrig.x + rayDir.x * t;
                if ( x < minPt.x || x > maxPt.x )
                    return false;
                T z = rayOrig.z + rayDir.z * t;
                if ( z < minPt.z || z > maxPt.z )
                    return false;
            }
            break;

        case 2: // z������󣬾���xyƽ��
            {
                T x = rayOrig.x + rayDir.x * t;
                if ( x < minPt.x || x > maxPt.x )
                    return false;
                T y = rayOrig.y + rayDir.y * t;
                if ( y < minPt.y || y > maxPt.y )
                    return false;
            }
            break;
        }

        if ( s ) *s = t;
        return true;
    }

    template<class T>
    inline bool ray_intersect_sphere( const vec3<T>& rayOrig, const vec3<T>& rayDir, const vec3<T>& spCenter, T spRadius, T* s = 0 )
    {
        vec3<T> p = rayOrig - spCenter;

        T a = nv::dot( rayDir, rayDir );
        T b = (T)2 * nv::dot( p, rayDir );
        T c = nv::dot( p, p ) - spRadius * spRadius;

        T r1, r2;
        if ( meta_1_dim_2_equation( a, b, c, r1, r2 ) == 0 )
            return false;

        sort_point( r1, r2 );

        if ( s )
            *s = r1;

        return true;
    }

    template<class T>
    inline bool ray_intersect_triangle( const vec3<T>& orig, const vec3<T>& dir,
        const vec3<T>& v0, const vec3<T>& v1, const vec3<T>& v2,
        float* pt = 0, float* pu = 0, float* pv = 0 )
    {
        // ͨ������(orig, dir)��������(v0,v1,v2)�󽻵����������������(u,v)��������ԭ��orig������ľ���t
        // ��ν�������������꼴��(u,v)����ô��Ӧ�������ڣ�Ҳ�������⣩�ĵ�p
        // ��p = (1 - u - v) * v0 + u * v1 + v * v2 ��һ�ּ�Ȩ��﷽ʽ
        // ��ô�������������ν���p����
        // p = (1 - u - v) * v0 + u * v1 + v * v2
        // �� p = orig + t * dir �����߷��̣�
        // ���� (1 - u - v) * v0 + u * v1 + v * v2 = orig + t * dir
        // ����ã� -dir * t + (v1 -v0) * u + (v2 - v0) * v = (orig - v0)
        // �� e1 = v1 - v0, e2 = v2 - v0, q = orig - v0, nd = -dir
        // �� nd * t + e1 * u + e2 * v = q
        // ���ݿ���ķ����ⷽ�̣�
        // ��������ʽ D = | nd e1 e2 |������һ������ʽ��ֵ������ת������ʽֵ��ͬ������
        //          | nd |
        // D = D' = | e1 | = (e1 �� e2)��nd ����ʵ�����Ȱ�nd����(i,j, k)�������õ�������nd�ٵ�ˣ�
        //          | e2 |
        // ���D != 0 ��ô�����н⣺
        // t = Dt / D = | q   e1  e2  | / D = (e1 �� e2)��q   / D
        // u = Du / D = | nd  q   e2  | / D = (q  �� e2)��nd  / D
        // v = Dv / D = | nd  e1  q   | / D = (e1 ��  q)��nd  / D
        // �� u,v��[0, 1]��t >= 0��Ϸ�

        vec3<T> e1 = v1 - v0;
        vec3<T> e2 = v2 - v0;
        vec3<T> nd = -dir;

        // ��D
        vec3<T> e1_e2 = nv::cross( e1, e2 );
        T       D     = nv::dot( e1_e2, nd );

        if ( fabs(D) < NV_FLT_EPSILON )
            return false;

        // ��Dt, t
        vec3<T> q  = orig - v0;
        T       Dt = nv::dot( e1_e2, q );
        T       t  = Dt / D;

        if ( t < (T)0 )
            return false;

        // ��Du, u
        vec3<T> q_e2 = nv::cross( q, e2 );
        T       Du   = nv::dot( q_e2, nd );
        T       u    = Du / D;

        if ( u < (T)0 || u > (T)1 )
            return false;

        // ��Dv, v
        vec3<T> e1_q = nv::cross( e1, q );
        T       Dv   = nv::dot( e1_q, nd );
        T       v    = Dv / D;

        if ( v < (T)0 || v > (T)1 )
            return false;

        // ���w
        T w = (T)1 - u - v;
        if ( w < (T)0 || w > (T)1 )
            return false;

        if ( pt ) *pt = t;
        if ( pu ) *pu = u;
        if ( pv ) *pv = v;

        return true;
    }

    template<typename T>
    inline bool aabb_vs_ball( const vec3<T>& bmin, const vec3<T>& bmax, const vec3<T>& pos, T radius )
    {
        for ( int i = 0; i < 3; ++i )
        {
            T lpos = pos[i];
            T lmin = bmin[i] - radius;
            T lmax = bmax[i] + radius;

            if ( lpos < lmin || lpos > lmax )
                return false;
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    // ͶӰ
    template<typename T>
    inline void point_proj_plane( const vec3<T>& pt, const vec4<T>& plane, vec3<T>& ptProj )
    {
        // ����pt��plane��ͶӰptProj
        
        // pt2 = pt - Ln ���� p.n+d = 0 �õ� L = p.n + d
        T L = nv::dot( pt, *(const vec3<T>*)(&plane) ) + plane.w;

        // ����pt2
        ptProj = pt - (*(const vec3<T>*)(&plane)) * L;
    }

    template<typename T>
    inline void vec_proj_plane( const vec3<T>& v, const vec4<T>& plane, vec3<T>& vProj )
    {
        // ��������v��plane��ͶӰ
        vec3<T> p1, p2;
        point_proj_plane( vec3<T>(0,0,0), plane, p1 );
        point_proj_plane( v, plane, p2 );
        vProj = p2 - p1;
    }

    template<typename T>
    inline void point_proj_ray( const vec3<T>& rayOrig, const vec3<T>& rayDir, const vec3<T>& pt, T& t )
    {
        // ����pt��ray�ϵ�ͶӰ
        // t = (p - o).n
        vec3<T> p_o = pt - rayOrig;
        t = nv::dot(p_o, rayDir);
    }

    template<typename T>
    inline void point_proj_ray( const vec2<T>& rayOrig, const vec2<T>& rayDir, const vec2<T>& pt, T& t )
    {
        vec2<T> p_o = pt - rayOrig;
        t = nv::dot(p_o, rayDir);
    }

    template<typename T>
    inline void point_proj_ray( const vec3<T>& rayOrig, const vec3<T>& rayDir, const vec3<T>& pt, vec3<T>& ptProj )
    {
        T t = 0;
        point_proj_ray( rayOrig, rayDir, pt, t );
        ptProj = rayOrig + rayDir * t;
    }

    template<typename T>
    inline void point_proj_line( const vec3<T>& pt1, const vec3<T>& pt2, const vec3<T>& pt, vec3<T>& ptProj )
    {
        vec3<T> pdir = nv::normalize( pt2 - pt1 );

        T t = 0;
        point_proj_ray( pt1, pdir, pt, t );

        ptProj = pt1 + pdir * t;
    }

    //////////////////////////////////////////////////////////////////////////
    // ����
    template<typename T>
    inline void make_plane_by_ptn( vec4<T>& pla, const vec3<T>& pt, const vec3<T>& n )
    {
        // ��֪��ͷ��ߣ���ƽ��
        // n.p + d = 0 => d = -n.p
        T d = -nv::dot( pt, n );
        
        pla.x = n.x;
        pla.y = n.y;
        pla.z = n.z;
        pla.w = d;
    }

    template<typename T>
    inline void make_orth_basis( vec3<T>& zdir, vec3<T>& ydir, vec3<T>& xdir )
    {
        // ����zdir����ydir��xdir
        zdir = nv::normalize(zdir);

        ydir.x = (T)0.0023; ydir.y = (T)0.9020; ydir.z = (T)0.4317299;
        xdir = nv::cross( ydir, zdir );
        xdir = nv::normalize(xdir);

        ydir = nv::cross(zdir, xdir);
        ydir = nv::normalize(ydir);
    }

    template<typename T>
    inline void make_orth_basis_by_up( vec3<T>& zdir, vec3<T>& ydir, vec3<T>& xdir )
    {
        // ����zdir����ydir��xdir
        zdir = nv::normalize(zdir);

        //ydir.x = (T)0; ydir.y = (T)1; ydir.z = (T)0;
        xdir = nv::cross( ydir, zdir );
        xdir = nv::normalize(xdir);

        ydir = nv::cross(zdir, xdir);
        ydir = nv::normalize(ydir);
    }

    //////////////////////////////////////////////////////////////////////////
    // ��ֵ
    template<typename T>
    inline vec3<T> slerp( const vec3<T>& v1, const vec3<T>& v2, T s )
    {
        // �õ���ת��
        vec3<T> v1n = nv::normalize(v1);
        vec3<T> v2n = nv::normalize(v2);
        vec3<T> rd  = nv::cross( v1, v2 );
        
        // �õ���ֵ��ת�Ƕ�
        T theta = nv::dot( v1n, v2n );
        theta = (T)safe_acos( theta );
        theta *= s;

        // ��ת
        nv::quaternion<T> rot( rd, theta );
        vec3<T> vt;
        rot.mult_vec( v1n, vt );

        // ��ֵ����
        T len1 = nv::length(v1);
        T len2 = nv::length(v2);
        T len  = (len2 - len1) * s + len1;

        // ���
        vt *= len;
        return vt;
    }

    //////////////////////////////////////////////////////////////////////////
    // ����
    template<typename T>
    void calc_tangent( const nv::vec3<T>& va, const nv::vec3<T>& vd, const nv::vec3<T>& vc, 
        const nv::vec2<T>& uva, const nv::vec2<T>& uvd, const nv::vec2<T>& uvc,
        nv::vec3<T>& tangent, nv::vec3<T>& binormal )
    {
        // tri ADC
        //| T | = | Uad  Vad | -1 | AD |  
        //| B |   | Uac  Vac |    | AC |

        T matUV[4] = 
        {
            uvd.x-uva.x,  uvd.y-uva.y,
            uvc.x-uva.x,  uvc.y-uva.y
        };

        nv::inverse_mat2( matUV );

        nv::vec3<T> ad = vd - va;
        nv::vec3<T> ac = vc - va;

        tangent  = ad * matUV[0] + ac * matUV[1];
        binormal = ad * matUV[2] + ac * matUV[3];
    }

	template <typename T>
	vec3<T> vert_move_distance(const vec3<T>& v, const vec3<T>& dir, T dis)
	{
		nv::vec3<T> ndir = nv::normalize(dir);

		return v + dis * ndir;
	}

    template<typename T>
    void decomposeMatrix( const matrix4<T>& mat, vec3<T>& pos, vec3<T>& scale, matrix4<T>& rot )
    {
        rot = mat;

        // translate
        pos = &rot._41;

        // scale
        vec3<T>& xdir = *(vec3<T>*)(&rot._11);
        vec3<T>& ydir = *(vec3<T>*)(&rot._21);
        vec3<T>& zdir = *(vec3<T>*)(&rot._31);

        scale.x = nv::length( xdir );
        scale.y = nv::length( ydir );
        scale.z = nv::length( zdir );

        // normalize
        if ( scale.x != 0 )
            xdir /= scale.x;

        if ( scale.y != 0 )
            ydir /= scale.y;

        if ( scale.z != 0 )
            zdir /= scale.z;

        // no trans
        rot._41 = rot._42 = rot._43 = 0;
    }

    template<typename T>
    bool toEulerAngles( const matrix4<T>& mat, T& rfYAngle, T& rfPAngle, T& rfRAngle )
    {
        // rot =  cy*cz          -cy*sz           sy
        //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
        //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

        rfPAngle = safe_asin(mat(0,2));

        if ( rfPAngle < NV_PI_2 )
        {
            if ( rfPAngle > -NV_PI_2 )
            {
                rfYAngle = atan2(-mat(1,2), mat(2,2));
                rfRAngle = atan2(-mat(0,1), mat(0,0));
                return true;
            }
            else
            {
                // WARNING.  Not a unique solution.
                T fRmY = atan2(mat(1,0), mat(1,1));
                rfRAngle = 0;  // any angle works
                rfYAngle = rfRAngle - fRmY;
                return false;
            }
        }
        else
        {
            // WARNING.  Not a unique solution.
            T fRpY = atan2(mat(1,0),mat(1,1));
            rfRAngle = 0;  // any angle works
            rfYAngle = fRpY - rfRAngle;
            return false;
        }
    }

    template<typename T>
    void fromEulerAngles( matrix4<T>& mat, T x, T y, T z )
    {
        // ִ��ŷ����ת
        nv::quaternion<T> qrx, qry, qrz, qr;

        qrx.set_value(nv::vec3<T>(1.0, 0.0, 0.0), x);
        qry.set_value(nv::vec3<T>(0.0, 1.0, 0.0), y);
        qrz.set_value(nv::vec3<T>(0.0, 0.0, 1.0), z);

        qr = qrx * qry * qrz;
        qr.get_value(mat);
    }
}

#endif

