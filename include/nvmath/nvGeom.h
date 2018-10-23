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
    const T* poly_x;    // 数据地址
    const T* poly_y;
    int      stride;    // 跨度
    int      count;     // 顶点总数
    int_vec  indices;   // 子索引
    int      user_data; // 用户定义数据

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
    // 1 - 交与p1
    // 2 - 交与p2
    // 3 - 交与中间
    // 0 - 不交

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

    if ( fabs(v_phi) < nv::degree_to_rad((T)0.5) ) // 在0.5度误差
    {
        if ( nv::is_between_strict(pt.x, p1.x, p2.x) ||
             nv::is_between_strict(pt.y, p1.y, p2.y) )
            return 3;
    }

    // 不交
    return 0;
}

template<typename T>
inline int xray_intersect_line( const vec2<T>& ro, const vec2<T>& p1, const vec2<T>& p2 )
{
    // x射线和直线交的情况：
    // 1 - 交与纵坐标小者
    // 2 - 交与纵坐标大者
    // 3 - 交与中间
    // 0 - 不相交或者重叠或者平行

    T dy = p2.y - p1.y;
    
    if ( fabs(dy) < NV_FLT_EPSILON )
        return 0;

    T t = (ro.y - p1.y) / dy;
    
    T x = p1.x + t * (p2.x - p1.x);

    if ( !is_less_equal(ro.x, x) ) // 在ro右侧
        return 0;

    if ( fabs(t) < NV_FLT_EPSILON ) // p1
        return p1.y < p2.y ? 1 : 2;

    if ( fabs(t - (T)1) < NV_FLT_EPSILON ) // p2
        return p1.y < p2.y ? 2 : 1;

    if ( t < (T)0 || t > (T)1 ) // 交与p1p2内
        return 0;

    return 3;
}

template<typename T>
inline bool line_intersect_line( const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& q1, const vec2<T>& q2 )
{
    vec2<T> p1b(p1), p2b(p2), q1b(q1), q2b(q2);

    // aabb测试
    sort_point( p1b, p2b );
    sort_point( q1b, q2b );

    if ( !aabb_intersect_aabb_equal( p1b, p2b, q1b, q2b ) )
        return false;
    
    // 跨立测试
    // 点p1、p2是否在直线q1q2两侧
    vec2<T> q2_q1 = q2 - q1;
    vec2<T> p1_q1 = p1 - q1;
    vec2<T> p2_q1 = p2 - q1;

    T v = nv::dot( nv::cross( p1_q1, q2_q1 ), nv::cross( p2_q1, q2_q1 ) );
    if ( v > 0 && fabs(v) > NV_FLT_EPSILON )
        return false;

    // 同样在判断点q1、q2是否在直线p1p2两侧
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
    // 0 不交
    // 1 共线交1点
    // 2 不共线交1点

    if ( !line_intersect_line( p1, p2, q1, q2 ) )
        return 0;

    // 共线
    vec2<T> np = p2 - p1;
    vec2<T> nq = q2 - q1;

    T cd = np.x * nq.y - np.y * nq.x;

    if ( is_zero(cd) )  // 斜率一致
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

        // 无数个交点
        return 0;
    }

    // 返回只有一个交点
    vec2<T> q1_p1 = q1 - p1;
    T tq = (q1_p1.x * np.y - q1_p1.y * np.x) / cd;
    r = q1 + nq * tq;
    return 2;
}

template<typename T>
inline T line_get_t( const vec2<T>& p1, const vec2<T>& p2, const vec2<T>& p )
{
    // 返回p在p1-p2的参数t
    vec2<T> pdir = p2 - p1;

    if ( fabs(pdir.x) > fabs(pdir.y) )
        return (p.x - p1.x) / pdir.x;
    else
        return (p.y - p1.y) / pdir.y;
}

template<typename T>
inline bool poly2d_has_point( const poly2d<T>& poly, const vec2<T>& pt )
{
    // 过pt做射线+x，与poly交点奇数则内；反之外
    nv::vec2<T> p1, p2;

    int inter_count = 0;

    int poly_count = poly.get_count();

    for ( int i = 0; i < poly_count; ++i )
    {
        // 得到线段
        p1 = poly.get_point(i);
        p2 = poly.get_point(i+1);

        // 点就在该线段上
        if ( point_on_line( pt, p1, p2 ) )
            return true;

        // xray交点情况
        int r = xray_intersect_line( pt, p1, p2 );
        if ( r >= 2 ) // 交于坐标大者或者中间才算一个交点
            ++inter_count;
    }

    return (inter_count % 2) == 1; // 奇数个交点说明在内
}

template<typename T>
inline bool poly2d_intersect_line( const poly2d<T>& poly, const vec2<T>& p1, const vec2<T>& p2 )
{
    // 判断多边形是否和直线相交（直线在内也算）
    // 是否和多边形有边相交
    int cnt = poly.get_count();

    for ( int i = 0; i < cnt; ++i )
    {
        nv::vec2<T> q1 = poly.get_point(i);
        nv::vec2<T> q2 = poly.get_point(i+1);

        if ( line_intersect_line( p1, p2, q1, q2 ) )
            return true;
    }

    // 是否在多边形内，只要测试1点即可
    if ( poly2d_has_point( poly, p1 ) )
        return true;

    return false;
}

template<typename T>
inline bool poly2d_intersect_aabb( const poly2d<T>& poly, const vec2<T>& pmin, const vec2<T>& pmax )
{
    // 多边形与aabb相交

    // 测试poly是否有点在aabb内
    int cnt = poly.get_count();

    for ( int i = 0; i < cnt; ++i )
    {
        vec2<T> pt = poly.get_point(i);
        if ( aabb_has_pt_strict( pmin, pmax, pt ) )
            return true;
    }

    // 测试aabb四条边是否和poly相交
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
    // 检查对角线p1_i,p2_i能否把多边形切成2部分
    p1_i = poly.wrap_index( p1_i );
    p2_i = poly.wrap_index( p2_i );

    sort_point( p1_i, p2_i );

    if ( abs(p1_i - p2_i) <= 1 ) // 不是对角线
        return false;

    int poly_count = poly.get_count();

    if ( abs(p1_i + poly_count - p2_i) <= 1 ) // p1在头，p2在尾的接近处情况
        return false;

    nv::vec2<T> p1 = poly.get_point(p1_i);
    nv::vec2<T> p2 = poly.get_point(p2_i);

    if ( is_zero( nv::length(p1-p2) ) ) // 两点重合
        return false;

    for ( int i = 0; i < poly_count; ++i ) // 检测对角线p1p2是否切割多边形
    {
        int i_n = poly.wrap_index( i + 1 );

        if ( p1_i == i || p1_i == i_n || p2_i == i || p2_i == i_n )
            continue;

        nv::vec2<T> q1 = poly.get_point(i);
        nv::vec2<T> q2 = poly.get_point(i_n);

        if ( line_intersect_line( p1, p2, q1, q2 ) )
            return false;
    }

    // 检测对角线是否在外
    // 将多边形的所有点投影到直线p1,p2上，计算投影点间的中点是否都在内
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

        // 测试中点在内
        nv::vec2<T> p = p1 + pdir * tc;
        if ( !nv::poly2d_has_point( poly, p ) )
            return false;
    }

    return true;
}

template<typename T>
inline bool poly2d_find_cut_diagonal( const poly2d<T>& poly, int& p1, int &p2 )
{
    // 找一条对角线使得能把多边形切分2部分
    int poly_count = poly.get_count();
    if ( poly_count <= 3 )
        return false;

    int count_half = poly_count / 2;

    for ( p1 = 0; p1 < poly_count; ++p1 )
    {
        p2 = p1 + count_half; // 从p1的对边开始找

        if ( poly2d_cut_diagonal( poly, p1, p2 ) )
            return true;

        // 两边查找
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
    // 截取多边形从p1_i到p2_i的子部分
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
    // 将多边形细分成三角形列表
    int poly_count = poly.get_count();

    // 至少3顶点
    if ( poly_count < 3 )
        return false;

    // 切成最小三角形那么可以加入了
    if ( poly_count == 3 )
    {
        for ( int i = 0; i < 3; ++i )
            idxs.push_back( poly.to_abs_index(i) );

        return true;
    }

    // 继续细分
    int p1, p2;
    if ( !poly2d_find_cut_diagonal( poly, p1, p2 ) )
        return false;
    
    // 继续递归第1部分
    poly2d<T> poly_sub;
    if ( !poly2d_cut_sub( poly, p1, p2, poly_sub ) )
        return false;

    if ( !poly2d_subdivide( poly_sub, idxs ) )
        return false;

    // 继续递归第2部分
    if ( !poly2d_cut_sub( poly, p2, p1, poly_sub ) )
        return false;

    if ( !poly2d_subdivide( poly_sub, idxs ) )
        return false;

    return true;
}

template<typename T>
inline bool poly2d_check_convex( const poly2d<T>& poly, int pi )
{
    // 检查pi是否是凸点（向量叉乘方法）
    // 假设poly是逆时针的

    vec2<T> cur  = poly.get_point( pi );
    vec2<T> pre  = poly.get_point( pi - 1 );
    vec2<T> next = poly.get_point( pi + 1 );

    T z = nv::cross( cur - pre, next - cur ).z;
    return is_less_equal(z, (T)0); // 纸外
}

template<typename T>
inline bool poly2d_is_ccw( const poly2d<T>& poly, const std::vector<int>& tris )
{
    // poly是否逆时针
    int cnt = (int)tris.size() / 3;

    poly2d<T> polyb = poly;
    polyb.indices.resize(3);

    for ( int i = 0; i < cnt; ++i )
    {
        int s = i * 3;

        polyb.indices[0] = tris[s];
        polyb.indices[1] = tris[s+1];
        polyb.indices[2] = tris[s+2];

        // 有一个不是凸则失败
        if ( !poly2d_check_convex(polyb, 0) )
            return false;
    }

    return true;
}

template<typename T>
inline bool poly2d_is_ccw( const poly2d<T>& poly )
{
    // poly是否逆时针
    std::vector<int> tris;
    
    if ( !nv::poly2d_subdivide( poly, tris ) )
        return false;

    return poly2d_is_ccw( poly, tris );
}

template<typename T>
inline void poly2d_normalize( poly2d<T>& poly )
{
    // poly正规化，符合逆时针排列
    if ( !poly2d_is_ccw(poly) )
        poly.reverse_indices();
}

//////////////////////////////////////////////////////////////////////////
// 多边形减集算法
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

    int         a_edge;     // 交与a的边
    int         b_edge;     // 交与b的边
    vec2<T>     pt;         // 交点
    bool        is_in;      // 入点还是出点
    bool        is_used;    // 是否已经使用
};

template<typename T>
inline void _poly2d_get_inters( const poly2d<T>& poly_a1, const poly2d<T>& poly_b1, std::vector<inter_contex<T> >& ics )
{
    // 求a1对于b1的所有入点出点情况
    // poly_a1和poly_b1必须正规化

    ics.clear();

    int a1_cnt = poly_a1.get_count();
    int b1_cnt = poly_b1.get_count();

    for ( int i = 0; i < a1_cnt; ++i ) // 遍历a的每条边
    {
        vec2<T> a_p1 = poly_a1.get_point(i);
        vec2<T> a_p2 = poly_a1.get_point(i+1);
        vec2<T> na   = a_p2 - a_p1;

        inter_contex<T> ic;
        ic.a_edge = i;

        // 检查和b的每条边情况
        for ( int j = 0; j < b1_cnt; ++j )
        {
            vec2<T> b_p1 = poly_b1.get_point(j);
            vec2<T> b_p2 = poly_b1.get_point(j+1);

            // 不共线相交
            int ret = line_intersect_line( a_p1, a_p2, b_p1, b_p2, ic.pt );
            if ( ret == 2 ) // 不共线交与1点
            {
                // 判断pt是否是入点 a x b指向纸内z > 0
                vec2<T> nb = b_p2 - b_p1;

                T z = nv::cross(na, nb).z;

                ic.is_in  = (z > 0);
                ic.b_edge = j;

                ics.push_back(ic);
            }
        } // 结束b遍历

    } // 结束a遍历
}

template<typename T>
inline int _poly2d_find_next_pt( const poly2d<T>& poly_a, const poly2d<T>& poly_b, int& edge_i, vec2<T>& pt,
    const std::vector<inter_contex<T> >& ics, bool& round_b )
{
    // 按照指定顺序找到下个点
    // -1 - 结果是个顶点
    // >=0 - 结果是相交点，返回在ics中索引

    // 首先找方向上的最近交点
    const poly2d<T>& poly = round_b ? poly_b : poly_a;

    vec2<T> edge_p0 = poly.get_point(edge_i); // 当前边
    vec2<T> edge_p1 = poly.get_point(edge_i + 1);

    T pt_t  = line_get_t( edge_p0, edge_p1, pt ); // 该点在边的t
    T cur_t = round_b ? (T)-1 : (T)2; // 该点下一个t
    int idx = -1;

    for ( int i = 0; i < (int)ics.size(); ++i )
    {
        const inter_contex<T>& ic = ics[i];

        if ( round_b ) // b回合找出点
        {
            if ( edge_i == ic.b_edge && !ic.is_in ) // 该边有交点,b回合找出点
            {
                T t = line_get_t( edge_p0, edge_p1, ic.pt );

                // 找的t必须小于pt_t且为最大的
                if ( is_less_equal(t, pt_t) && t > cur_t )
                {
                    cur_t = t;
                    idx = i;
                }
            }
        }
        else // a回合找入点
        {
            if ( edge_i == ic.a_edge && ic.is_in ) // 该边有交点,a回合找入点
            {
                T t = line_get_t( edge_p0, edge_p1, ic.pt );

                // 找的t必须大于pt_t且为最小的
                if ( is_greater_strict(t, pt_t) && t < cur_t )
                {
                    cur_t = t;
                    idx = i;
                }
            }
        }
    }

    if ( idx >= 0 ) // 找到出点或者入点
    {
        pt = ics[idx].pt;

        // 转到别人的回合
        round_b = !round_b; 
        edge_i = round_b ? ics[idx].b_edge : ics[idx].a_edge;
        return idx;
    }

    // 改不存在交点了，直接进军下一条边
    if ( round_b ) // b回合逆向找
    {
        pt = poly.get_point( edge_i ); // 该边的起点也就是上条边的终点
        edge_i = poly.wrap_index( edge_i - 1 ); // 上条边
    }
    else // a回合正向找
    {
        edge_i = poly.wrap_index( edge_i + 1 ); // 下条边
        pt = poly.get_point( edge_i ); // 下条边起点
    }

    return -1;
}

template<typename T>
inline bool _poly2d_contain_poly2d_pts( const poly2d<T>& poly_a, const poly2d<T>& poly_b )
{
    // 测试多边形a是否包含多边形b的点集合
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
    // 判断ab是否切割polyb的每条边
    int b1_cnt = poly_b1.get_count();

    for ( int k = 0; k < b1_cnt; ++k )
    {
        if ( k == b_i ) // 不包括改点的两边 
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
    // 将a中有b洞的多边形连接一条ab线，使之变为简单多边形
    // 返回从a_i处起始的偏移

    int a1_cnt = poly_a1.get_count();
    int b1_cnt = poly_b1.get_count();

    for ( /*a_i = 0*/; a_i < a1_cnt; ++a_i )
    {
        vec2<T> a_p = poly_a1.get_point(a_i);

        for ( b_i = 0; b_i < b1_cnt; ++b_i )
        {
            vec2<T> b_p = poly_b1.get_point(b_i);

            // 判断ab是否切割polyb的每条边
            if ( _poly2d_cut_hole(poly_b1, a_p, b_p, b_i) && _poly2d_cut_hole(poly_a1, a_p, b_p, a_i) ) // 都不相交，成功
                return true;
        }
    }

    return false;
}

template<typename T>
inline int poly2d_sub_poly2d( const poly2d<T>& poly_a, const poly2d<T>& poly_b, poly2d_sub_poly2d_callback<T>* ev )
{
    // 计算a-b
    // -1不合法
    // 0 结果为空集
    // 1 结果a
    // 2 结果a包含b
    // 3 结果为普通相交

    // 算法开始
    ev->on_begin_sub_poly();

    // 没有相交结果为a
    if ( !poly_a.is_valid() || !poly_b.is_valid() )
    {
        ev->on_end_sub_poly();
        return -1;
    }

    // 首先正规化
    poly2d<T> poly_a1 = poly_a;
    poly2d<T> poly_b1 = poly_b;

    poly2d_normalize( poly_a1 );
    poly2d_normalize( poly_b1 );

    // 计算a与b的交点情况
    std::vector<inter_contex<T> > ics;
    _poly2d_get_inters( poly_a1, poly_b1, ics );
   
    // 有入点，从每个入点开始找减集
    if ( !ics.empty() ) 
    {
        // 找每个入点
        for ( int i = 0; i < (int)ics.size(); ++i )
        {
            inter_contex<T>& ic = ics[i];
            if ( !ic.is_in || ic.is_used )
                continue;

            ic.is_used = true; // 标记为使用

            // 从一个未使用的入点开始创建一个多边形
            poly2d<T> poly_new1;
            ev->on_begin_poly( poly_new1 );

            // 先加上入点
            ev->on_add_point_from_new( poly_new1, ic.pt );

            int        edge_i   = ic.b_edge;
            vec2<T>    pt       = ic.pt;
            bool       round_b  = true;
            bool       ok       = false;

            while ( true )
            {
                // 找下个点
                int fd = _poly2d_find_next_pt( poly_a1, poly_b1, edge_i, pt, ics, round_b );

                if ( fd >= 0 ) // 是个交点
                {
                    if ( fd == i ) // 又到了入点，一圈兜完
                    {
                        ok = true;
                        break;
                    }

                    if ( ics[fd].is_used ) // 其他已经使用过了，重复？
                        break;

                    ics[fd].is_used = true; // 遍历过了
                }
                else // 是个顶点
                {
                    if ( !round_b && nv::is_equal( ic.pt, pt ) ) // 在找入点且顶点是入点
                    {
                        ok = true;
                        break;
                    }
                }

                // 加入遍历的点
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
    // 没有交点情况
    // 如果b包含a
    if ( _poly2d_contain_poly2d_pts( poly_b1, poly_a1 ) )
    {
        // 结果为空集合
        //poly2d<T> poly_empty;
        //ev->on_get_result( 0, poly_empty );
        ev->on_end_sub_poly();
        return 0;
    }

    // 如果a包含b
    if ( _poly2d_contain_poly2d_pts( poly_a1, poly_b1 ) )
    {
        // 也即a中打了个洞，该情况连接ab一条边，使变成个简单多边形
        int a_i_1 = 0, b_i_1 = 0;

        if ( _poly2d_hole_normal( poly_a1, poly_b1, a_i_1, b_i_1 ) )
        {
            int a1_cnt = poly_a1.get_count();
            int b1_cnt = poly_b1.get_count();

            poly2d<T> poly_new1;
            ev->on_begin_poly( poly_new1 );

            for ( int i = 0; i <= a1_cnt; ++i ) // 包括自身点两次
                ev->on_add_point_from_new( poly_new1, poly_a1.get_point(a_i_1 + i) );

            for ( int i = 0; i <= b1_cnt; ++i ) // 包括自身点两次
                ev->on_add_point_from_new( poly_new1, poly_b1.get_point(b_i_1 - i) );

            // 返回
            bool ok = poly_new1.is_valid();
            ev->on_end_poly( poly_new1, ok );

            if ( ok )
                ev->on_get_result( 2, poly_new1 );

            ev->on_end_sub_poly();
            return 2;
        }
    }

    // 没有相交结果为a
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

