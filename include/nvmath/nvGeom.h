#pragma once

#error deprecated by maoyanhong

#include "nvVector.h"
#include "nvMisc.h"
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <assert.h>


namespace nv
{
//////////////////////////////////////////////////////////////////////////

template<typename T>
struct poly2d
{
    typedef std::vector<int> int_vec;

public:
    poly2d() : poly_x(0), poly_y(0), stride(0), count(0), user_data(0) {}

public:
    const T* poly_x;    // ���ݵ�ַ
    const T* poly_y;
    int      stride;    // ���
    int      count;     // ��������
    int_vec  indices;   // ������
    int      user_data; // �û���������

public:
    bool is_valid() const
    {
        return get_count() >= 3;
    }

    int get_count() const
    {
        return indices.empty() ? count : indices.size();
    }

    int wrap_index( int i ) const
    {
        int cnt = get_count();
        i = i % cnt;
        return (i + cnt) % cnt;
    }

    int to_abs_index( int i ) const
    {
        i = wrap_index(i);
        return indices.empty() ? i : indices[i];
    }

    T get_x( int i ) const
    {
        i = to_abs_index(i);
        return *(T*)(((char*)poly_x) + stride * i);
    }

    T get_y( int i ) const
    {
        i = to_abs_index(i);
        return *(T*)(((char*)poly_y) + stride * i);
    }

    vec2<T> get_point( int i ) const
    {
        return vec2<T>( get_x(i), get_y(i) );
    }

public:
    void build_indices()
    {
        if ( indices.empty() )
        {
            for ( int i = 0; i < count; ++i )
                indices.push_back( i );
        }
    }

    void reverse_indices()
    {
        build_indices();
        std::reverse( indices.begin(), indices.end() );
    }
};

template<typename T>
inline int point_on_line( const vec2<T>& pt, const vec2<T>& p1, const vec2<T>& p2 )
{
    // 1 - ����p1
    // 2 - ����p2
    // 3 - �����м�
    // 0 - ����

    vec2<T> pt_p1 = pt - p1;
    T pt_p1_len = nv::length(pt_p1);
    if ( nv::is_zero(pt_p1_len) )
        return 1;

    vec2<T> p2_pt = p2 - pt;
    T p2_pt_len = nv::length(p2_pt);
    if ( nv::is_zero(p2_pt_len) )
        return 2;
   
    T v_dot = nv::dot( pt_p1, p2_pt );
    T v_cos = v_dot / (pt_p1_len * p2_pt_len);
    T v_phi = safe_acos(v_cos);

    if ( fabs(v_phi) < nv::degree_to_rad((T)0.5) ) // ��0.5�����
    {
        if ( nv::is_between_strict(pt.x, p1.x, p2.x) ||
             nv::is_between_strict(pt.y, p1.y, p2.y) )
            return 3;
    }

    // ����
    return 0;
}

template<typename T>
inline int xray_intersect_line( const vec2<T>& ro, const vec2<T>& p1, const vec2<T>& p2 )
{
    // x���ߺ�ֱ�߽��������
    // 1 - ����������С��
    // 2 - �������������
    // 3 - �����м�
    // 0 - ���ཻ�����ص�����ƽ��

    T dy = p2.y - p1.y;
    
    if ( fabs(dy) < NV_FLT_EPSILON )
        return 0;

    T t = (ro.y - p1.y) / dy;
    
    T x = p1.x + t * (p2.x - p1.x);

    if ( !is_less_equal(ro.x, x) ) // ��ro�Ҳ�
        return 0;

    if ( fabs(t) < NV_FLT_EPSILON ) // p1
        return p1.y < p2.y ? 1 : 2;

    if ( fabs(t - (T)1) < NV_FLT_EPSILON ) // p2
        return p1.y < p2.y ? 2 : 1;

    if ( t < (T)0 || t > (T)1 ) // ����p1p2��
        return 0;

    return 3;
}

template<typename T>
inline bool line_intersect_line( const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& q1, const vec2<T>& q2 )
{
    vec2<T> p1b(p1), p2b(p2), q1b(q1), q2b(q2);

    // aabb����
    sort_point( p1b, p2b );
    sort_point( q1b, q2b );

    if ( !aabb_intersect_aabb_equal( p1b, p2b, q1b, q2b ) )
        return false;
    
    // ��������
    // ��p1��p2�Ƿ���ֱ��q1q2����
    vec2<T> q2_q1 = q2 - q1;
    vec2<T> p1_q1 = p1 - q1;
    vec2<T> p2_q1 = p2 - q1;

    T v = nv::dot( nv::cross( p1_q1, q2_q1 ), nv::cross( p2_q1, q2_q1 ) );
    if ( v > 0 && fabs(v) > NV_FLT_EPSILON )
        return false;

    // ͬ�����жϵ�q1��q2�Ƿ���ֱ��p1p2����
    vec2<T> p2_p1 = p2 - p1;
    vec2<T> q1_p1 = q1 - p1;
    vec2<T> q2_p1 = q2 - p1;

    v = nv::dot( nv::cross( q1_p1, p2_p1 ), nv::cross( q2_p1, p2_p1 ) );
    if ( v > 0 && fabs(v) > NV_FLT_EPSILON )
        return false;

    return true;
}

template<typename T>
inline int line_intersect_line( const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& q1, const vec2<T>& q2, vec2<T>& r )
{
    // 0 ����
    // 1 ���߽�1��
    // 2 �����߽�1��

    if ( !line_intersect_line( p1, p2, q1, q2 ) )
        return 0;

    // ����
    vec2<T> np = p2 - p1;
    vec2<T> nq = q2 - q1;

    T cd = np.x * nq.y - np.y * nq.x;

    if ( is_zero(cd) )  // б��һ��
    {
        if ( is_equal(p1, q1) || is_equal(p1, q2) )
        {
            r = p1;
            return 1;
        }

        if ( is_equal(p2, q1) || is_equal(p2, q2) )
        {
            r = p2;
            return 1;
        }

        // ����������
        return 0;
    }

    // ����ֻ��һ������
    vec2<T> q1_p1 = q1 - p1;
    T tq = (q1_p1.x * np.y - q1_p1.y * np.x) / cd;
    r = q1 + nq * tq;
    return 2;
}

template<typename T>
inline T line_get_t( const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& p )
{
    // ����p��p1-p2�Ĳ���t
    vec2<T> pdir = p2 - p1;

    if ( fabs(pdir.x) > fabs(pdir.y) )
        return (p.x - p1.x) / pdir.x;
    else
        return (p.y - p1.y) / pdir.y;
}

template<typename T>
inline bool poly2d_has_point( const poly2d<T>& poly, const vec2<T>& pt )
{
    // ��pt������+x����poly�����������ڣ���֮��
    nv::vec2<T> p1, p2;

    int inter_count = 0;

    int poly_count = poly.get_count();

    for ( int i = 0; i < poly_count; ++i )
    {
        // �õ��߶�
        p1 = poly.get_point(i);
        p2 = poly.get_point(i+1);

        // ����ڸ��߶���
        if ( point_on_line( pt, p1, p2 ) )
            return true;

        // xray�������
        int r = xray_intersect_line( pt, p1, p2 );
        if ( r >= 2 ) // ����������߻����м����һ������
            ++inter_count;
    }

    return (inter_count % 2) == 1; // ����������˵������
}

template<typename T>
inline bool poly2d_intersect_line( const poly2d<T>& poly, const vec2<T>& p1, const vec2<T>& p2 )
{
    // �ж϶�����Ƿ��ֱ���ཻ��ֱ������Ҳ�㣩
    // �Ƿ�Ͷ�����б��ཻ
    int cnt = poly.get_count();

    for ( int i = 0; i < cnt; ++i )
    {
        nv::vec2<T> q1 = poly.get_point(i);
        nv::vec2<T> q2 = poly.get_point(i+1);

        if ( line_intersect_line( p1, p2, q1, q2 ) )
            return true;
    }

    // �Ƿ��ڶ�����ڣ�ֻҪ����1�㼴��
    if ( poly2d_has_point( poly, p1 ) )
        return true;

    return false;
}

template<typename T>
inline bool poly2d_intersect_aabb( const poly2d<T>& poly, const vec2<T>& pmin, const vec2<T>& pmax )
{
    // �������aabb�ཻ

    // ����poly�Ƿ��е���aabb��
    int cnt = poly.get_count();

    for ( int i = 0; i < cnt; ++i )
    {
        vec2<T> pt = poly.get_point(i);
        if ( aabb_has_pt_strict( pmin, pmax, pt ) )
            return true;
    }

    // ����aabb�������Ƿ��poly�ཻ
    vec2<T> rt(pmax.x, pmin.y);
    vec2<T> lb(pmin.x, pmax.y);

    if ( poly2d_intersect_line( poly, pmin, rt ) )
        return true;

    if ( poly2d_intersect_line( poly, pmin, lb ) )
        return true;

    if ( poly2d_intersect_line( poly, pmax, rt ) )
        return true;

    if ( poly2d_intersect_line( poly, pmax, lb ) )
        return true;

    return false;
}

template<typename T>
inline bool poly2d_cut_diagonal( const poly2d<T>& poly, int p1_i, int p2_i )
{
    // ���Խ���p1_i,p2_i�ܷ�Ѷ�����г�2����
    p1_i = poly.wrap_index( p1_i );
    p2_i = poly.wrap_index( p2_i );

    sort_point( p1_i, p2_i );

    if ( abs(p1_i - p2_i) <= 1 ) // ���ǶԽ���
        return false;

    int poly_count = poly.get_count();

    if ( abs(p1_i + poly_count - p2_i) <= 1 ) // p1��ͷ��p2��β�Ľӽ������
        return false;

    nv::vec2<T> p1 = poly.get_point(p1_i);
    nv::vec2<T> p2 = poly.get_point(p2_i);

    if ( is_zero( nv::length(p1-p2) ) ) // �����غ�
        return false;

    for ( int i = 0; i < poly_count; ++i ) // ���Խ���p1p2�Ƿ��и�����
    {
        int i_n = poly.wrap_index( i + 1 );

        if ( p1_i == i || p1_i == i_n || p2_i == i || p2_i == i_n )
            continue;

        nv::vec2<T> q1 = poly.get_point(i);
        nv::vec2<T> q2 = poly.get_point(i_n);

        if ( line_intersect_line( p1, p2, q1, q2 ) )
            return false;
    }

    // ���Խ����Ƿ�����
    // ������ε����е�ͶӰ��ֱ��p1,p2�ϣ�����ͶӰ�����е��Ƿ�����
    nv::vec2<T> pdir = nv::normalize( p2 - p1 );

    std::set<T> proj_ts;
    proj_ts.insert((T)0);
    proj_ts.insert((T)1);

    for ( int i = 0; i < poly_count; ++i )
    {
        if ( i == p1_i || i == p2_i )
            continue;

        nv::vec2<T> q = poly.get_point(i);
        
        T t = 0;
        nv::point_proj_ray( p1, pdir, q, t );
        
        if ( (T)0 < t && t < (T)1 )
            proj_ts.insert( t );
    }

    std::set<T>::iterator it = proj_ts.begin();

    while ( true )
    {
        T t1 = *it;
        ++it;

        if ( it == proj_ts.end() )
            break;

        T t2 = *it;
        T tc = (t1 + t2) * (T)0.5;

        // �����е�����
        nv::vec2<T> p = p1 + pdir * tc;
        if ( !nv::poly2d_has_point( poly, p ) )
            return false;
    }

    return true;
}

template<typename T>
inline bool poly2d_find_cut_diagonal( const poly2d<T>& poly, int& p1, int &p2 )
{
    // ��һ���Խ���ʹ���ܰѶ�����з�2����
    int poly_count = poly.get_count();
    if ( poly_count <= 3 )
        return false;

    int count_half = poly_count / 2;

    for ( p1 = 0; p1 < poly_count; ++p1 )
    {
        p2 = p1 + count_half; // ��p1�ĶԱ߿�ʼ��

        if ( poly2d_cut_diagonal( poly, p1, p2 ) )
            return true;

        // ���߲���
        for ( int i = 1; i < count_half; ++i )
        {
            if ( poly2d_cut_diagonal( poly, p1, p2-i ) )
            {
                p2 = p2-i;
                return true;
            }

            if ( poly2d_cut_diagonal( poly, p1, p2+i ) )
            {
                p2 = p2+i;
                return true;
            }
        }
    }

    p1 = 0;
    p2 = 2;
    return false;
}

template<typename T>
inline bool poly2d_cut_sub( const poly2d<T>& poly, int p1_i, int p2_i, poly2d<T>& poly_sub )
{
    // ��ȡ����δ�p1_i��p2_i���Ӳ���
    poly_sub.poly_x = poly.poly_x;
    poly_sub.poly_y = poly.poly_y;
    poly_sub.stride = poly.stride;
    poly_sub.count  = poly.count;

    poly_sub.indices.clear();

    if ( p1_i == p2_i )
        return false;

    if ( p2_i < p1_i )
        p2_i += poly.get_count();

    for ( int i = p1_i; i <= p2_i; ++i )
    {
        int idx = poly.to_abs_index( i );
        poly_sub.indices.push_back( idx );
    }

    return true;
}

template<typename T>
inline bool poly2d_subdivide( const poly2d<T>& poly, std::vector<int>& idxs )
{
    // �������ϸ�ֳ��������б�
    int poly_count = poly.get_count();

    // ����3����
    if ( poly_count < 3 )
        return false;

    // �г���С��������ô���Լ�����
    if ( poly_count == 3 )
    {
        for ( int i = 0; i < 3; ++i )
            idxs.push_back( poly.to_abs_index(i) );

        return true;
    }

    // ����ϸ��
    int p1, p2;
    if ( !poly2d_find_cut_diagonal( poly, p1, p2 ) )
        return false;
    
    // �����ݹ��1����
    poly2d<T> poly_sub;
    if ( !poly2d_cut_sub( poly, p1, p2, poly_sub ) )
        return false;

    if ( !poly2d_subdivide( poly_sub, idxs ) )
        return false;

    // �����ݹ��2����
    if ( !poly2d_cut_sub( poly, p2, p1, poly_sub ) )
        return false;

    if ( !poly2d_subdivide( poly_sub, idxs ) )
        return false;

    return true;
}

template<typename T>
inline bool poly2d_check_convex( const poly2d<T>& poly, int pi )
{
    // ���pi�Ƿ���͹�㣨������˷�����
    // ����poly����ʱ���

    vec2<T> cur  = poly.get_point( pi );
    vec2<T> pre  = poly.get_point( pi - 1 );
    vec2<T> next = poly.get_point( pi + 1 );

    T z = nv::cross( cur - pre, next - cur ).z;
    return is_less_equal(z, (T)0); // ֽ��
}

template<typename T>
inline bool poly2d_is_ccw( const poly2d<T>& poly, const std::vector<int>& tris )
{
    // poly�Ƿ���ʱ��
    int cnt = (int)tris.size() / 3;

    poly2d<T> polyb = poly;
    polyb.indices.resize(3);

    for ( int i = 0; i < cnt; ++i )
    {
        int s = i * 3;

        polyb.indices[0] = tris[s];
        polyb.indices[1] = tris[s+1];
        polyb.indices[2] = tris[s+2];

        // ��һ������͹��ʧ��
        if ( !poly2d_check_convex(polyb, 0) )
            return false;
    }

    return true;
}

template<typename T>
inline bool poly2d_is_ccw( const poly2d<T>& poly )
{
    // poly�Ƿ���ʱ��
    std::vector<int> tris;
    
    if ( !nv::poly2d_subdivide( poly, tris ) )
        return false;

    return poly2d_is_ccw( poly, tris );
}

template<typename T>
inline void poly2d_normalize( poly2d<T>& poly )
{
    // poly���滯��������ʱ������
    if ( !poly2d_is_ccw(poly) )
        poly.reverse_indices();
}

//////////////////////////////////////////////////////////////////////////
// ����μ����㷨
template<typename T>
struct poly2d_sub_poly2d_callback
{
    virtual void on_begin_sub_poly() = 0;

    virtual void on_begin_poly( poly2d<T>& poly_new ) = 0;

    virtual void on_add_point_from_new( poly2d<T>& poly_new, const vec2<T>& pt_new ) = 0;

    virtual void on_end_poly( poly2d<T>& poly_new, bool ok ) = 0;

    virtual void on_get_result( int ret, poly2d<T>& poly ) = 0;

    virtual void on_end_sub_poly() = 0;
};

template<typename T>
struct inter_contex
{
    inter_contex() : a_edge(0), b_edge(0), is_in(false), is_used(false) {}

    int         a_edge;     // ����a�ı�
    int         b_edge;     // ����b�ı�
    vec2<T>     pt;         // ����
    bool        is_in;      // ��㻹�ǳ���
    bool        is_used;    // �Ƿ��Ѿ�ʹ��
};

template<typename T>
inline void _poly2d_get_inters( const poly2d<T>& poly_a1, const poly2d<T>& poly_b1, std::vector<inter_contex<T> >& ics )
{
    // ��a1����b1���������������
    // poly_a1��poly_b1�������滯

    ics.clear();

    int a1_cnt = poly_a1.get_count();
    int b1_cnt = poly_b1.get_count();

    for ( int i = 0; i < a1_cnt; ++i ) // ����a��ÿ����
    {
        vec2<T> a_p1 = poly_a1.get_point(i);
        vec2<T> a_p2 = poly_a1.get_point(i+1);
        vec2<T> na   = a_p2 - a_p1;

        inter_contex<T> ic;
        ic.a_edge = i;

        // ����b��ÿ�������
        for ( int j = 0; j < b1_cnt; ++j )
        {
            vec2<T> b_p1 = poly_b1.get_point(j);
            vec2<T> b_p2 = poly_b1.get_point(j+1);

            // �������ཻ
            int ret = line_intersect_line( a_p1, a_p2, b_p1, b_p2, ic.pt );
            if ( ret == 2 ) // �����߽���1��
            {
                // �ж�pt�Ƿ������ a x bָ��ֽ��z > 0
                vec2<T> nb = b_p2 - b_p1;

                T z = nv::cross(na, nb).z;

                ic.is_in  = (z > 0);
                ic.b_edge = j;

                ics.push_back(ic);
            }
        } // ����b����

    } // ����a����
}

template<typename T>
inline int _poly2d_find_next_pt( const poly2d<T>& poly_a, const poly2d<T>& poly_b, int& edge_i, vec2<T>& pt,
    const std::vector<inter_contex<T> >& ics, bool& round_b )
{
    // ����ָ��˳���ҵ��¸���
    // -1 - ����Ǹ�����
    // >=0 - ������ཻ�㣬������ics������

    // �����ҷ����ϵ��������
    const poly2d<T>& poly = round_b ? poly_b : poly_a;

    vec2<T> edge_p0 = poly.get_point(edge_i); // ��ǰ��
    vec2<T> edge_p1 = poly.get_point(edge_i + 1);

    T pt_t  = line_get_t( edge_p0, edge_p1, pt ); // �õ��ڱߵ�t
    T cur_t = round_b ? (T)-1 : (T)2; // �õ���һ��t
    int idx = -1;

    for ( int i = 0; i < (int)ics.size(); ++i )
    {
        const inter_contex<T>& ic = ics[i];

        if ( round_b ) // b�غ��ҳ���
        {
            if ( edge_i == ic.b_edge && !ic.is_in ) // �ñ��н���,b�غ��ҳ���
            {
                T t = line_get_t( edge_p0, edge_p1, ic.pt );

                // �ҵ�t����С��pt_t��Ϊ����
                if ( is_less_equal(t, pt_t) && t > cur_t )
                {
                    cur_t = t;
                    idx = i;
                }
            }
        }
        else // a�غ������
        {
            if ( edge_i == ic.a_edge && ic.is_in ) // �ñ��н���,a�غ������
            {
                T t = line_get_t( edge_p0, edge_p1, ic.pt );

                // �ҵ�t�������pt_t��Ϊ��С��
                if ( is_greater_strict(t, pt_t) && t < cur_t )
                {
                    cur_t = t;
                    idx = i;
                }
            }
        }
    }

    if ( idx >= 0 ) // �ҵ�����������
    {
        pt = ics[idx].pt;

        // ת�����˵Ļغ�
        round_b = !round_b; 
        edge_i = round_b ? ics[idx].b_edge : ics[idx].a_edge;
        return idx;
    }

    // �Ĳ����ڽ����ˣ�ֱ�ӽ�����һ����
    if ( round_b ) // b�غ�������
    {
        pt = poly.get_point( edge_i ); // �ñߵ����Ҳ���������ߵ��յ�
        edge_i = poly.wrap_index( edge_i - 1 ); // ������
    }
    else // a�غ�������
    {
        edge_i = poly.wrap_index( edge_i + 1 ); // ������
        pt = poly.get_point( edge_i ); // ���������
    }

    return -1;
}

template<typename T>
inline bool _poly2d_contain_poly2d_pts( const poly2d<T>& poly_a, const poly2d<T>& poly_b )
{
    // ���Զ����a�Ƿ���������b�ĵ㼯��
    int cnt = poly_b.get_count();

    for ( int i = 0; i < cnt; ++i )
    {
        if ( !poly2d_has_point( poly_a, poly_b.get_point(i) ) )
            return false;
    }

    return true;
}

template<typename T>
inline bool _poly2d_cut_hole( const poly2d<T>& poly_b1, const vec2<T>& a_p, const vec2<T>& b_p, int b_i )
{
    // �ж�ab�Ƿ��и�polyb��ÿ����
    int b1_cnt = poly_b1.get_count();

    for ( int k = 0; k < b1_cnt; ++k )
    {
        if ( k == b_i ) // �������ĵ������ 
            continue;

        if ( poly_b1.wrap_index( k + 1 ) == b_i )
            continue;

        if ( line_intersect_line( a_p, b_p, poly_b1.get_point(k), poly_b1.get_point(k+1) ) )
        {
            return false;
        }
    }

    return true;
}

template<typename T>
inline bool _poly2d_hole_normal( const poly2d<T>& poly_a1, const poly2d<T>& poly_b1, int& a_i, int& b_i )
{
    // ��a����b���Ķ��������һ��ab�ߣ�ʹ֮��Ϊ�򵥶����
    // ���ش�a_i����ʼ��ƫ��

    int a1_cnt = poly_a1.get_count();
    int b1_cnt = poly_b1.get_count();

    for ( /*a_i = 0*/; a_i < a1_cnt; ++a_i )
    {
        vec2<T> a_p = poly_a1.get_point(a_i);

        for ( b_i = 0; b_i < b1_cnt; ++b_i )
        {
            vec2<T> b_p = poly_b1.get_point(b_i);

            // �ж�ab�Ƿ��и�polyb��ÿ����
            if ( _poly2d_cut_hole(poly_b1, a_p, b_p, b_i) && _poly2d_cut_hole(poly_a1, a_p, b_p, a_i) ) // �����ཻ���ɹ�
                return true;
        }
    }

    return false;
}

template<typename T>
inline int poly2d_sub_poly2d( const poly2d<T>& poly_a, const poly2d<T>& poly_b, poly2d_sub_poly2d_callback<T>* ev )
{
    // ����a-b
    // -1���Ϸ�
    // 0 ���Ϊ�ռ�
    // 1 ���a
    // 2 ���a����b
    // 3 ���Ϊ��ͨ�ཻ

    // �㷨��ʼ
    ev->on_begin_sub_poly();

    // û���ཻ���Ϊa
    if ( !poly_a.is_valid() || !poly_b.is_valid() )
    {
        ev->on_end_sub_poly();
        return -1;
    }

    // �������滯
    poly2d<T> poly_a1 = poly_a;
    poly2d<T> poly_b1 = poly_b;

    poly2d_normalize( poly_a1 );
    poly2d_normalize( poly_b1 );

    // ����a��b�Ľ������
    std::vector<inter_contex<T> > ics;
    _poly2d_get_inters( poly_a1, poly_b1, ics );
   
    // ����㣬��ÿ����㿪ʼ�Ҽ���
    if ( !ics.empty() ) 
    {
        // ��ÿ�����
        for ( int i = 0; i < (int)ics.size(); ++i )
        {
            inter_contex<T>& ic = ics[i];
            if ( !ic.is_in || ic.is_used )
                continue;

            ic.is_used = true; // ���Ϊʹ��

            // ��һ��δʹ�õ���㿪ʼ����һ�������
            poly2d<T> poly_new1;
            ev->on_begin_poly( poly_new1 );

            // �ȼ������
            ev->on_add_point_from_new( poly_new1, ic.pt );

            int        edge_i   = ic.b_edge;
            vec2<T>    pt       = ic.pt;
            bool       round_b  = true;
            bool       ok       = false;

            while ( true )
            {
                // ���¸���
                int fd = _poly2d_find_next_pt( poly_a1, poly_b1, edge_i, pt, ics, round_b );

                if ( fd >= 0 ) // �Ǹ�����
                {
                    if ( fd == i ) // �ֵ�����㣬һȦ����
                    {
                        ok = true;
                        break;
                    }

                    if ( ics[fd].is_used ) // �����Ѿ�ʹ�ù��ˣ��ظ���
                        break;

                    ics[fd].is_used = true; // ��������
                }
                else // �Ǹ�����
                {
                    if ( !round_b && nv::is_equal( ic.pt, pt ) ) // ��������Ҷ��������
                    {
                        ok = true;
                        break;
                    }
                }

                // ��������ĵ�
                ev->on_add_point_from_new( poly_new1, pt );
            }

            if ( !poly_new1.is_valid() )
                ok = false;

            ev->on_end_poly( poly_new1, ok );

            if ( ok )
                ev->on_get_result( 3, poly_new1 );
        }

        ev->on_end_sub_poly();
        return 3;
    }

    //////////////////////////////////////////////////////////////////////////
    // û�н������
    // ���b����a
    if ( _poly2d_contain_poly2d_pts( poly_b1, poly_a1 ) )
    {
        // ���Ϊ�ռ���
        //poly2d<T> poly_empty;
        //ev->on_get_result( 0, poly_empty );
        ev->on_end_sub_poly();
        return 0;
    }

    // ���a����b
    if ( _poly2d_contain_poly2d_pts( poly_a1, poly_b1 ) )
    {
        // Ҳ��a�д��˸��������������abһ���ߣ�ʹ��ɸ��򵥶����
        int a_i_1 = 0, b_i_1 = 0;

        if ( _poly2d_hole_normal( poly_a1, poly_b1, a_i_1, b_i_1 ) )
        {
            int a1_cnt = poly_a1.get_count();
            int b1_cnt = poly_b1.get_count();

            poly2d<T> poly_new1;
            ev->on_begin_poly( poly_new1 );

            for ( int i = 0; i <= a1_cnt; ++i ) // �������������
                ev->on_add_point_from_new( poly_new1, poly_a1.get_point(a_i_1 + i) );

            for ( int i = 0; i <= b1_cnt; ++i ) // �������������
                ev->on_add_point_from_new( poly_new1, poly_b1.get_point(b_i_1 - i) );

            // ����
            bool ok = poly_new1.is_valid();
            ev->on_end_poly( poly_new1, ok );

            if ( ok )
                ev->on_get_result( 2, poly_new1 );

            ev->on_end_sub_poly();
            return 2;
        }
    }

    // û���ཻ���Ϊa
    poly2d<T> poly_new1;
    ev->on_begin_poly( poly_new1 );
    
    int a1_cnt = poly_a1.get_count();
    for ( int i = 0; i < a1_cnt; ++i )
        ev->on_add_point_from_new( poly_new1, poly_a1.get_point(i) );

    ev->on_end_poly( poly_new1, true );
    ev->on_get_result( 1, poly_new1 );
    ev->on_end_sub_poly();
    return 1;
}

//////////////////////////////////////////////////////////////////////////
}

