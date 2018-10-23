#pragma once
#include "nvVector.h"
#include "nvMisc.h"
#include <vector>
//#include <list>
#include <set>
#include <algorithm>
#include <assert.h>


namespace nv
{
//////////////////////////////////////////////////////////////////////////

template<typename T>
struct poly2d
{
    //////////////////////////////////////////////////////////////////////////
    // 这里定义poly的默认坐标系x右，y下
    typedef poly2d<T>               class_type;
    typedef nv::vec2<T>             vec2_type;
    typedef std::vector<vec2_type>  vec2_array_type;

private:
    // 误差定义
    static T        _eps_comm()  { return (T)1e-5; }   // 普通项
    static T        _eps_point() { return (T)1e-4; }   // 坐标点
    static double   _eps_area()  { return 1e-8;    }   // 面积
    static T        _eps_cost()  { return (T)1e-4; }   // 角度误差

    template<typename U>
    static bool _is_zero( U v, U e )
    {
        return fabs(v) < e;
    }

    template<typename U>
    static bool _is_equal( U v1, U v2, U e )
    {
        return fabs(v1-v2) < e;
    }

    template<typename U>
    static bool _is_less_equal( U v1, U v2, U e )
    {
        return (v1 < v2) || _is_equal( v1, v2, e );
    }

    template<typename U>
    static bool _is_less_strict( U v1, U v2, U e )
    {
        return (v1 < v2) && !_is_equal( v1, v2, e );
    }

    static bool _is_equal_point( const vec2_type& v1, const vec2_type& v2 )
    {
        T e = _eps_point();
        return _is_equal(v1.x, v2.x, e) && _is_equal(v1.y, v2.y, e);
    }

    // 数据
public:
    vec2_array_type pts;

//////////////////////////////////////////////////////////////////////////
// 基础方法
public:
    bool is_valid() const
    {
        // 至少3点合法
        return (int)pts.size() >= 3;
    }

    int get_count() const
    {
        return (int)pts.size();
    }

    int wrap_index( int i ) const
    {
        // 允许wrap索引
        if ( pts.empty() )
        {
            return 0;
        }
        else
        {
            int cnt = (int)pts.size();
            i = i % cnt;
            return (i + cnt) % cnt;
        }
    }

    vec2_type& get_point( int i )
    {
        assert( 0 <= i && i < (int)pts.size() );
        return pts[i];
    }

    const vec2_type& get_point( int i ) const
    {
        return const_cast<class_type*>(this)->get_point(i);
    }

    vec2_type& get_wrap_point( int i )
    {
        return get_point( wrap_index(i) );
    }

    const vec2_type& get_wrap_point( int i ) const
    {
        return const_cast<class_type*>(this)->get_wrap_point(i);
    }

    void add_point( const vec2_type& pt )
    {
        pts.push_back(pt);
    }

    void clear()
    {
        pts.clear();
    }

//////////////////////////////////////////////////////////////////////////
// 高级方法
public:
    // 获取面积
    double get_area() const
    {
        if ( !is_valid() )
            return 0;

        double area = 0;
        double s    = 0;

        // s = ∫f(x)dx (x: x1->x2) = (x2 - x1)(y1 + y2) / 2;
        int cnt = get_count();

        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& pt1 = get_point( i );
            const vec2_type& pt2 = get_wrap_point( i+1 );

            s = ((double)pt2.x - (double)pt1.x) * ((double)pt1.y + (double)pt2.y);
            area += s;
        }

        area *= 0.5;
        return area;
    }

    // 面积是存在
    bool has_area() const
    {
        // 面积不为0
        return !_is_zero( get_area(), _eps_area() );
    }

    // 是否是逆时针，规定逆时针为正向
    bool is_ccw() const
    {
        if ( !is_valid() )
            return false;

        double area = get_area();
        return !_is_less_equal( area, 0.0, _eps_area() ); // 面积>0为正
    }

    // 调整顺序为正向
    void normalize()
    {
        if ( !is_valid() )
            return;

        if ( is_ccw() )
            return;

        // 颠倒顺序
        std::reverse( pts.begin(), pts.end() );
    }

public:
    static T _point_proj_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& pt )
    {
        // 计算pt在p1p2上的投影
        // t = (pt - p1).p2p1 / p2p1^2
        vec2_type p2_p1 = p2 - p1;
        vec2_type pt_p1 = pt - p1;
        T p2_p1_dot = nv::dot( p2_p1, p2_p1 );
        T t = nv::dot(pt_p1, p2_p1) / p2_p1_dot;
        return t;
    }

    static bool _point_on_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& pt )
    {
        // 1 - 交
        // 0 - 不交

        const T e_comm = _eps_comm();
        const T e_pt   = _eps_point();

        if ( _is_equal_point(p1, pt) || _is_equal_point(p2, pt) ) // 和其中端点重合算交
            return true;

        // 计算pt在p1p2上的投影
        T t = _point_proj_line( p1, p2, pt );

        if ( _is_less_equal((T)0, t, e_comm) && _is_less_equal(t, (T)1, e_comm) ) // t在线段内
        {
            vec2_type pt_proj = p1 + (p2 - p1) * t;
            T dist = nv::length(pt - pt_proj);
            return _is_zero(dist, e_pt); // 投影距离很接近线段
        }
        
        return false;
    }

    static bool _xray_intersect_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& ro )
    {
        // x射线和直线交的情况：
        // 1 - 交与纵坐标大者或者中间
        // 0 - 其他情况

        const T e_comm = _eps_comm();
        const T e_pt   = _eps_point();

        T dy = p2.y - p1.y;
        if ( _is_zero(dy, e_pt) ) // 平行线忽略
            return false;

        T t = (ro.y - p1.y) / dy;
        T x = p1.x + t * (p2.x - p1.x);
        if ( !_is_less_equal(ro.x, x, e_pt) ) // ro在交点右侧
            return false;

        if ( _is_zero(t, e_comm) ) // 射线穿过p1
            return p1.y < p2.y ? false : true; // 选大者

        if ( _is_equal(t, (T)1, e_comm) ) // 射线穿过p2
            return p1.y < p2.y ? true : false; // 选大者

        if ( t < (T)0 || t > (T)1 ) // 交与p1p2外
            return false;

        return true;
    }

public:
    bool has_point( const vec2_type& pt, bool include_edge ) const
    {
        // 判断多边形包含点
        // 算法对多边形无要求
        // 过pt做射线+x，与poly交点奇数则内；反之外
        int inter_count = 0;
        int poly_count  = get_count();

        for ( int i = 0; i < poly_count; ++i )
        {
            // 得到线段
            const vec2_type& p1 = get_point(i);
            const vec2_type& p2 = get_wrap_point(i+1);

            // 点就在该线段上
            if ( _point_on_line( p1, p2, pt ) )
            {
                return include_edge;
            }

            // xray交点情况
            if ( _xray_intersect_line( p1, p2, pt ) ) // 交于坐标大者或者中间才算一个交点
                ++inter_count;
        }

        return (inter_count % 2) == 1; // 奇数个交点说明在内
    }

    bool is_concave_point( int i ) const
    {
        // 判断是否凹点
        // 必须事先正规化才能用
        const vec2_type& pt  = get_wrap_point(i);
        const vec2_type& pt1 = get_wrap_point(i-1);
        const vec2_type& pt2 = get_wrap_point(i+1);

        vec2_type vn = pt2 - pt;
        vec2_type vp = pt - pt1;

        nv::vec3<T> vt = nv::cross( vn, vp );

        T e = _eps_comm();
        if ( _is_zero(vt.z, e) ) // 平行可以不算凹
            return false;

        return vt.z < (T)0; // 是凹
    }

    bool intersect_line( const vec2_type& p1, const vec2_type& p2 ) const
    {
        // 判断多边形是否和直线相交（直线在内也算，有一点在多边形边上也算）

        // 是否和多边形有边相交
        int cnt = get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& q1 = get_point(i);
            const vec2_type& q2 = get_wrap_point(i+1);
            vec2_type v;
            T tp, tq;
            if ( _line_vs_line( p1, p2, q1, q2, v, tp, tq ) ) // 有交
                return true;
        }

        // 无交，要么整个在外部，要么整个在内部
        // 是否在多边形内，只要测试1点即可
        if ( has_point( p1, true ) )
            return true;

        return false;
    }

    bool intersect_aabb( const vec2_type& pmin, const vec2_type& pmax ) const
    {
        // 多边形与aabb相交

        // 这一步不是必须得，只是为了加速处理
        // 测试poly是否有点在aabb内
        T e_cmm = _eps_comm();
        int cnt = get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& pt = get_point(i);
            if ( _is_less_equal(pmin.x, pt.x, e_cmm) && _is_less_equal(pt.x, pmax.x, e_cmm) &&
                _is_less_equal(pmin.y, pt.y, e_cmm) && _is_less_equal(pt.y, pmax.y, e_cmm) ) // 有则相交
                return true;
        }

        // 测试aabb四条边是否和poly相交
        vec2_type rt(pmax.x, pmin.y);
        vec2_type lb(pmin.x, pmax.y);

        if ( intersect_line( pmin, rt ) )
            return true;

        if ( intersect_line( pmin, lb ) )
            return true;

        if ( intersect_line( pmax, rt ) )
            return true;

        if ( intersect_line( pmax, lb ) )
            return true;

        return false;
    }

//////////////////////////////////////////////////////////////////////////
// 复杂算法
public:
    // 计算a - b的裁剪后多边形（结果可能为多个）
    struct inter_contex
    {
        inter_contex() : a_edge(0), b_edge(0), a_len(0), b_len(0), a_cos(0), b_cos(0), a_vtx(0), b_vtx(0), type(0) {}

        vec2_type   pt;         // 交点
        int         a_edge;     // 交与a的边
        int         b_edge;     // 交与b的边
        T           a_len;      // a边的距离
        T           b_len;      // b边的距离
        T           a_cos;      // a边的角cos
        T           b_cos;      // b边的角cos
        int         a_vtx;      // 假如刚好是顶点，a的顶点号
        int         b_vtx;      // 假如刚好是顶点，b的顶点号
        int         type;       // 0普通交点 1入点 2出点 3入出点
    };

    struct visitor_context
    {
        visitor_context() : port(0), type(0) {}

        vec2_type pt;   // 点
        int       port; // 转去对方的位置
        int       type; // 0普通点 1入点 2出点 3入出点
    };

    static bool _line_vs_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& q1, const vec2_type& q2, 
        vec2_type& v, T& tp, T& tq )
    {
        // 返回值
        // 0 平行无解  false
        // 1 非平行无解 false
        // 2 相交有解 true

        // p2_p1 * tp + q1_q2 * tq = q1_p1
        // a * tp + b * tq = c

        vec2_type a = p2 - p1;
        vec2_type b = q1 - q2;

        T d_a = a.x * b.y - b.x * a.y;

        const T e = _eps_comm();

        if ( _is_zero( d_a, e ) ) // 平行无解
        {
            return false;
        }

        // 误差平行判断
        {
            T e_cos = _eps_cost();
            vec2_type a1 = nv::normalize(a);
            vec2_type b1 = nv::normalize(b);
            T cos_ab = nv::dot(a1, b1);
            if ( _is_equal(cos_ab, (T)1, e_cos) || _is_equal(cos_ab, (T)-1, e_cos) )
                return false;
        }

        // 不平行
        vec2_type c = q1 - p1;

        tp = (c.x * b.y - b.x * c.y) / d_a;

        if ( _is_less_equal((T)0, tp, e) && _is_less_equal(tp, (T)1, e) ) // 在p1p2内
        {
            tq = (a.x * c.y - c.x * a.y) / d_a;

            if ( _is_less_equal((T)0, tq, e) && _is_less_equal(tq, (T)1, e) ) // 在q1q2内
            {
                v = p1 + a * tp; // 有解
                return true;
            }
        }

        // 无解
        return false;
    }

    static T _get_cos_t( const vec2_type& v1, const vec2_type& v2, const vec2_type& vt, T ic_len )
    {
        // 计算 v1.v2 = |v1|v2| cost

        vec2_type v1n = nv::normalize(v1);
        vec2_type v2n = nv::normalize((ic_len < (T)0.5) ? v2 : -v2 ); // <0.5就是起点，>0.5终点要反向
        T cos_t = nv::dot( v1n, v2n );
        return cos_t;
    }

    static int _get_vtx_idx( int edge_idx, int cnt, T ic_len )
    {
        const T e = _eps_comm();
        if ( _is_zero(ic_len, e) ) // 是边上的开头
            return edge_idx;
        if ( _is_equal(ic_len, (T)1.0, e) ) // 是边上的尾，那就是下个顶点号
            return (edge_idx+1) % cnt; // 有可能最后点跑回起点
        return -1; // 非顶点
    }

    static void _get_inters( const poly2d& poly_a, const poly2d& poly_b, std::vector<inter_contex>& ics )
    {
        // 求a对于b的所有入点出点情况
        // b进入a称为入点，b从a出去称为出点
        // poly_a1和poly_b1必须正规化

        const T e = _eps_comm();

        ics.clear();

        int a_cnt = poly_a.get_count();
        int b_cnt = poly_b.get_count();

        for ( int i = 0; i < a_cnt; ++i ) // 遍历a的每条边
        {
            const vec2_type& a_p1 = poly_a.get_point(i);
            const vec2_type& a_p2 = poly_a.get_wrap_point(i+1);

            // 检查和b的每条边情况
            for ( int j = 0; j < b_cnt; ++j )
            {
                const vec2_type& b_p1 = poly_b.get_point(j);
                const vec2_type& b_p2 = poly_b.get_wrap_point(j+1);

                inter_contex ic;

                if ( _line_vs_line( a_p1, a_p2, b_p1, b_p2, ic.pt, ic.a_len, ic.b_len ) ) // 无交点，平行之类都不算
                {
                    // 判断pt是否是入点 b x a指向纸内
                    vec2_type na = a_p2 - a_p1;
                    vec2_type nb = b_p2 - b_p1;

                    T z = nv::cross(nb, na).z;

                    if ( _is_zero(z, e) )
                    {
                        ic.type = 0; // 平行？未知
                    }
                    else if ( z > 0 ) // 是入点
                    {
                        ic.type = 1;
                    }
                    else if ( z < 0 ) // 是出点
                    {
                        ic.type = 2;
                    }

                    // 检查交点是否是顶点
                    ic.a_vtx = _get_vtx_idx( i, a_cnt, ic.a_len );
                    ic.b_vtx = _get_vtx_idx( j, b_cnt, ic.b_len );

                    // 记录cos值备比较排序用
                    ic.a_cos = _get_cos_t( na, nb, ic.pt, ic.b_len );
                    ic.b_cos = _get_cos_t( nb, na, ic.pt, ic.a_len );

                    if ( ic.type > 0 ) // 有效交点
                    {
                        ic.a_edge = i;
                        ic.b_edge = j;
                        ics.push_back( ic );
                    }
                }
            } // 结束b遍历
        } // 结束a遍历
    }

    struct _check_flag
    {
        bool operator()( const inter_contex& ic ) const { return ic.type <= 0; }
    };
    
    static void _merge_inters( std::vector<inter_contex>& ics, int side_cnt, bool side_a )
    {
        // 合并交点
        for ( int ei = 0; ei < side_cnt; ++ei )
        {
            // 搜索包含该顶点的所有交点
            std::vector<int> icidx;
            int ics_cnt = (int)ics.size();
            for ( int i = 0; i < ics_cnt; ++i )
            {
                inter_contex& ic = ics[i];
                if ( ic.type > 0 )
                {
                    int vidx = side_a ? ic.a_vtx : ic.b_vtx;
                    if ( vidx == ei ) // 是顶点
                        icidx.push_back(i);
                }
            }

            // 对搜索到的顶点合并处理
            int icidx_cnt = (int)icidx.size();
            if ( icidx_cnt > 1 ) // 至少2个交点
            {
                // 统计入出类型
                int in_count = 0;
                int out_count = 0;

                for ( int i = 0; i < icidx_cnt; ++i )
                {
                    inter_contex& ic = ics[icidx[i]];
                    if ( ic.type == 1 )
                        ++in_count;
                    else if ( ic.type == 2 )
                        ++out_count;
                    else
                        assert(0);
                }

                // 谁的类型多就是谁，因为+-说明是反射线抵消，++说明是入射，--说明出射
                int type = 0;
                if ( in_count > out_count )
                    type = 1;
                else if ( in_count < out_count )
                    type = 2;

                inter_contex& icfirst = ics[icidx[0]];

                if ( type == 0 ) // 可以取消的反射点，当它不存在
                {
                    icfirst.type = -100; // 删除标记
                }
                else // 修正合并后类型
                {
                    icfirst.type = type;
                }

                // 删除多余其他节点
                for ( int i = 1; i < icidx_cnt; ++i )
                {
                    ics[ icidx[i] ].type = -100; // 删除标记
                }
            } // end of if ( icidx_cnt > 1 ) 
        } // end of 搜索每个顶点

        // 结束后删除标记-100的点
       
        typename std::vector<inter_contex>::iterator it = std::remove_if( ics.begin(), ics.end(), _check_flag() );
        if ( it != ics.end() )
            ics.erase( it, ics.end() );
    }
    
    struct sort_by_dist
    {
        const std::vector<inter_contex>& _ic;
        bool _side_a;
        
        sort_by_dist( const std::vector<inter_contex>& ic, bool s ) : _ic(ic), _side_a(s) {}
        
        bool operator()( int l, int r ) const
        {
            T l_len = _side_a ? _ic[l].a_len : _ic[l].b_len;
            T r_len = _side_a ? _ic[r].a_len : _ic[r].b_len;
            
            // 两点在该直线相同位置
            if ( _is_equal( l_len, r_len, _eps_comm() ) )
            {
                // 在比较cost数值，小者在前
                T l_cos = _side_a ? _ic[l].a_cos : _ic[l].b_cos;
                T r_cos = _side_a ? _ic[r].a_cos : _ic[r].b_cos;
                
                if ( _is_equal( l_cos, r_cos, _eps_comm() ) )
                {
                    // 尼玛，还一样，
                    // 对于a边-前+后，对于b边+前-后（+表示入口，-表示出口）
                    if ( _side_a )
                        return -_ic[l].type < -_ic[r].type;
                    else
                        return _ic[l].type < _ic[r].type;
                }
                
                return l_cos < r_cos;
            }
            
            return l_len < r_len;
        }
    };

    static void _get_edge_pts( const std::vector<inter_contex>& ics, std::vector<int>& ics_side, int& side_vtx,
        int side, bool side_a )
    {
        // 返回某边上的交点情况
        ics_side.clear();

        side_vtx = -1; // 该边起始顶点是否是个交点

        int cnt = (int)ics.size();

        // 遍历找到该边的交点
        for ( int i = 0; i < cnt; ++i )
        {
            const inter_contex& ic = ics[i];

            int vtx = side_a ? ic.a_vtx : ic.b_vtx;
            if ( vtx != -1 ) // 是个顶点交点
            {
                if ( vtx == side ) // 是该边顶点
                {
                    assert(side_vtx == -1);
                    side_vtx = i;
                }
            }
            else // 是个边交点
            {
                int edge = side_a ? ic.a_edge : ic.b_edge;
                if ( edge == side ) // 是这个边
                {
                    ics_side.push_back(i);
                }
            }
        }

        // 根据距离边起点的距离排序
        

        std::sort( ics_side.begin(), ics_side.end(), sort_by_dist(ics, side_a) );
    }

    static void _get_visitor_side( const poly2d& poly, const std::vector<inter_contex>& ics,
        std::vector<visitor_context>& vcs, bool side_a )
    {
        // poly必须正规化
        // 求遍历表
        vcs.clear();
        visitor_context vc;

        // 扫描各边压入
        int cnt = poly.get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            // 获取该边的交点
            std::vector<int> ics_side;
            int side_vtx = -1;
            _get_edge_pts( ics, ics_side, side_vtx, i, side_a );

            // 顶点先压入
            vc.pt   = poly.get_point(i);
            vc.type = 0;
            vc.port = -1;

            if ( side_vtx != -1 ) // 判断顶点是否是交点
            {
                const inter_contex& ic = ics[side_vtx];
                vc.type = ic.type | 0x80;
                vc.port = side_vtx;
            }

            vcs.push_back( vc );

            // 然后按顺序压入该边交点
            int ics_side_cnt = (int)ics_side.size();
            for ( int is_it = 0; is_it < ics_side_cnt; ++is_it )
            {
                int idx = ics_side[is_it];
                const inter_contex& ic = ics[idx];
                vc.pt   = ic.pt;
                vc.type = ic.type | 0x80; // 一个标记表示没有被处理
                vc.port = idx; // 这里先标记ics索引，以被之后连接
                vcs.push_back( vc );
            } // end of每个边上每个交点
        } // end of 每边
    }

    static bool _set_visitor_port( std::vector<visitor_context>& avc, std::vector<visitor_context>& bvc )
    {
        bool has_in_port  = false;
        bool has_out_port = false;

        // 设置入口
        int a_cnt = (int)avc.size();

        for ( int i = 0; i < a_cnt; ++i )
        {
            visitor_context& vc_a = avc[i];
            if ( vc_a.type == 0 ) // 是个普通顶点
                continue;

            // 查找该点在bvc的位置
            assert( (vc_a.type & 0x80) && (vc_a.port >= 0) );
            int i_b = 0;
            int b_cnt = (int)bvc.size();
            for ( ; i_b < b_cnt; ++i_b )
            {
                const visitor_context& vc_b = bvc[i_b];
                if ( (vc_b.port == vc_a.port) && (vc_b.type & 0x80) ) // port就是ids的索引，他们应当相同
                    break;
            }

            if ( i_b < b_cnt )
            {
                vc_a.port = i_b; // 双方跳转设置
                visitor_context& vc_b = bvc[i_b];
                assert( vc_b.type == vc_a.type ); // 对家应当也是出或者入点和vc_a相同
                vc_a.type = vc_b.type = (vc_a.type & 0x3);
                vc_b.port = i;

                if ( vc_a.type == 1 )
                    has_in_port = true; // 至少有一个入点
                else if ( vc_a.type == 2 )
                    has_out_port = true; // 至少有一个出点
            }
            else
            {
                assert(0);
                return false; // 返回失败，可能bug
            }
        }

        // 检查b的多余情况
        int b_cnt = (int)bvc.size();
        for ( int i = 0; i < b_cnt; ++i )
        {
            visitor_context& vc_b = bvc[i];
            if ( vc_b.type & 0x80 )
            {
                assert(0);     // 应当不可能
                return false; // 可能bug，返回失败
            }
        }

        // 是否有至少一个入出口
        if ( !has_in_port )
            return false;
        if ( !has_out_port )
            return false;
        return true;
    }

    static bool _sub_poly_comm( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // 执行a - b 要求必须有交
        // 算法本对凹凸多边形都有效，多边形不能有共点，但为了保证这一情况只能处理凸多边形
        // 多边形必须是正规化的且为凸

        //DebugBreak();

        // 获取交点情况
        std::vector<inter_contex> ics;
        _get_inters( poly_a, poly_b, ics );
        if ( ics.empty() )
            return false;

        // 合并多个交点
        _merge_inters( ics, poly_a.get_count(), true );
        _merge_inters( ics, poly_b.get_count(), false );
        if ( ics.empty() )
            return false;

        // 插入交点并排序
        std::vector<visitor_context> avc, bvc;
        _get_visitor_side( poly_a, ics, avc, true );
        _get_visitor_side( poly_b, ics, bvc, false );

        // 设置切换入口
        if ( !_set_visitor_port( avc, bvc ) )
            return false;

        // 开始遍历
        int avc_cnt = (int)avc.size();
        //int bvc_cnt = (int)bvc.size();

        while ( true )
        {
            // a上找一个入点开始
            int it_a = 0;
            for (; it_a < avc_cnt; ++it_a )
            {
                const visitor_context& vc = avc[it_a];
                if ( vc.type == 1 ) // 入点
                    break;
            }

            // 没有入点了，结束算法
            if ( it_a == avc_cnt )
                break;

            // 避免死循环
            std::set<visitor_context*> cut_logs;

            // 从入点开始遍历
            poly_ret.push_back( class_type() );
            class_type& new_poly = poly_ret.back();
            new_poly.add_point( avc[it_a].pt );
            avc[it_a].type = -1; // 去入标记
            cut_logs.insert( &avc[it_a] ); // 打日志

            // 找下个点
            int it = (it_a + 1) % avc_cnt;
            std::vector<visitor_context>* vcs = &avc;
            int flag = 2;
            int step = 1;

            while( true )
            {
                // 判别当前点
                visitor_context& vc = (*vcs)[it];

                if ( !cut_logs.insert(&vc).second ) // 判断是否将会加入重复的点
                {
                    assert(0);
                    break;
                }

                // 检查是否存在连续碰到++（当入射反射对某边时，入射反射其一为平行认为交了，其实不交）
				
                if ( flag == 2 && (vc.type == 1 || vc.type == -1) ) // 需求是-(即上个关键是+)，又碰到+（或历史+）了
                {
                    if ( vc.type == 1 )
                    {
                        // 清空上次，以这个为新起点
                        new_poly.clear();
                        cut_logs.clear();
                        cut_logs.insert(&vc);
                        it_a = it;
                        vc.type = -1;
                    }
                    else
                    {
                        // 该点作废
                        new_poly.clear();
                        break;
                    }
                }
				

                // 是需要的转折点
                if ( vc.type == flag ) 
                {
                    bool last_is_avc = (vcs == &avc); // 此次是a方

                    // 翻转跳转
                    it = vc.port; 
                    vcs = (last_is_avc ? &bvc : &avc);
                    flag = (~flag) & 0x3;

					// 沿着A的相反方向加入点
                    step *= -1;

                    // 只清除a方的入标记,避免重复
                    if ( last_is_avc )
                    {
                        if ( vc.type == 1 )
                            vc.type = -1;
                    }
                    else
                    {
                        if ( (*vcs)[it].type == 1 )
                            (*vcs)[it].type = -1;
                    }

                    if ( it == it_a && vcs == &avc ) // 跳转后回到起点结束
                        break;
                }

                if ( (new_poly.get_count() > 0) && _is_equal_point(new_poly.pts.back(), vc.pt) )
                    ; // 添加点和上次一样，不重复
                else
                    new_poly.add_point( vc.pt ); // 可以加入这个点

                it += step;
                if ( it < 0 )
                    it = (int)vcs->size() - 1;
                else if ( it >= (int)vcs->size() )
                    it = 0;
            } // 结束一个新子多边形遍历

            // 检查获取新的合法性
            if ( !poly_ret.empty() )
            {
                double poly_area = poly_ret.back().get_area();
                if ( _is_less_equal( poly_area, 0.0, _eps_area() ) ) // 面积<=0不合法
                    poly_ret.pop_back();
            }

        } // 结束所有入点遍历

        return true;
    }

	static bool _intersect_poly_comm( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
	{
		// 执行a intersect b 要求必须有交
		// 算法本对凹凸多边形都有效，多边形不能有共点，但为了保证这一情况只能处理凸多边形
		// 多边形必须是正规化的且为凸

		//DebugBreak();

		// 获取交点情况
		std::vector<inter_contex> ics;
		_get_inters( poly_a, poly_b, ics );
		if ( ics.empty() )
			return false;

		// 合并多个交点
		_merge_inters( ics, poly_a.get_count(), true );
		_merge_inters( ics, poly_b.get_count(), false );
		if ( ics.empty() )
			return false;

		// 插入交点并排序
		std::vector<visitor_context> avc, bvc;
		_get_visitor_side( poly_a, ics, avc, true );
		_get_visitor_side( poly_b, ics, bvc, false );

		// 设置切换入口
		if ( !_set_visitor_port( avc, bvc ) )
			return false;

		// 开始遍历
		int avc_cnt = (int)avc.size();
		int bvc_cnt = (int)bvc.size();

		while ( true )
		{
			// a上找一个入点开始
			int it_a = 0;
			for (; it_a < avc_cnt; ++it_a )
			{
				const visitor_context& vc = avc[it_a];
				if ( vc.type == 1 ) // 入点
					break;
			}

			// 没有入点了，结束算法
			if ( it_a == avc_cnt )
				break;

			// 避免死循环
			std::set<visitor_context*> cut_logs;

			// 从入点开始遍历
			poly_ret.push_back( class_type() );
			class_type& new_poly = poly_ret.back();
			new_poly.add_point( avc[it_a].pt );
			avc[it_a].type = -1; // 去入标记
			cut_logs.insert( &avc[it_a] ); // 打日志

			// 找b当前点开始
			int it = avc[it_a].port;
			std::vector<visitor_context>* vcs = &bvc;
			int flag = 2;
			int step = 1;

			while( true )
			{
				// 判别当前点
				visitor_context& vc = (*vcs)[it];

				if ( !cut_logs.insert(&vc).second ) // 判断是否将会加入重复的点
				{
					break;
				}

				// 检查是否存在连续碰到++（当入射反射对某边时，入射反射其一为平行认为交了，其实不交）
				/*
				if ( flag == 2 && (vc.type == 1 || vc.type == -1) ) // 需求是-(即上个关键是+)，又碰到+（或历史+）了
				{
					if ( vc.type == 1 )
					{
						// 清空上次，以这个为新起点
						new_poly.clear();
						cut_logs.clear();
						cut_logs.insert(&vc);
						it_a = it;
						vc.type = -1;
					}
					else
					{
						// 该点作废
						new_poly.clear();
						break;
					}
				}
				*/

				// 是需要的转折点
				if ( vc.type == flag ) 
				{
					bool last_is_avc = (vcs == &avc); // 此次是a方

					// 翻转跳转
					it = vc.port; 
					vcs = (last_is_avc ? &bvc : &avc);
					flag = (~flag) & 0x3;

					// 沿着A的方向加入点
					step *= 1;

					// 只清除a方的入标记,避免重复
					if ( last_is_avc )
					{
						if ( vc.type == 1 )
							vc.type = -1;
					}
					else
					{
						if ( (*vcs)[it].type == 1 )
							(*vcs)[it].type = -1;
					}

					if ( it == it_a && vcs == &avc ) // 跳转后回到起点结束
						break;
				}

				if ( (new_poly.get_count() > 0) && _is_equal_point(new_poly.pts.back(), vc.pt) )
					; // 添加点和上次一样，不重复
				else
					new_poly.add_point( vc.pt ); // 可以加入这个点

				it += step;
				if ( it < 0 )
					it = (int)vcs->size() - 1;
				else if ( it >= (int)vcs->size() )
					it = 0;
			} // 结束一个新子多边形遍历

			// 检查获取新的合法性
			if ( !poly_ret.empty() )
			{
				double poly_area = poly_ret.back().get_area();
				if ( _is_less_equal( poly_area, 0.0, _eps_area() ) ) // 面积<=0不合法
					poly_ret.pop_back();
			}

		} // 结束所有入点遍历

		return true;
	}

    static bool _poly2d_contain_poly2d_pts( const poly2d& poly_a, const poly2d& poly_b, bool include_edge )
    {
        // 测试多边形a是否包含多边形b的点集合
        int cnt = poly_b.get_count();

        for ( int i = 0; i < cnt; ++i )
        {
            if ( !poly_a.has_point( poly_b.get_point(i), include_edge ) )
                return false;
        }

        return true;
    }

    static bool _sub_poly_full_cut( const poly2d& poly_a, const poly2d& poly_b )
    {
        // 多边形a,b必须为正规化且凸
        // 判断a - b = 0
        return _poly2d_contain_poly2d_pts( poly_b, poly_a, true ); // a都在b内，全部被干掉
    }

    /*static bool _sub_poly_zero_cut( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // 多边形a,b必须为正规化且凸
        // 判断a - b = a

        int cnt = poly_b.get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            if ( poly_a.has_point( poly_b.get_point(i), false ) ) // b有一点是否在a内（不含边）
                return false; // 有，那么需要裁减
        }

        // 没有也不能立即排除，检查b是否有边切割a
        int a_cnt = poly_a.get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& b1 = poly_b.get_point(i);
            const vec2_type& b2 = poly_b.get_wrap_point(i+1);
            for ( int j = 0; j < a_cnt; ++j )
            {
                const vec2_type& a1 = poly_a.get_point(j);
                const vec2_type& a2 = poly_a.get_wrap_point(j+1);
                vec2_type v;
                T tp, tq;
                if ( _line_vs_line( b1, b2, a1, a2, v, tp, tq) )
                    return false; // 有切割
            }
        }

        poly_ret.push_back(poly_a);
        return true; // a完整保留
    }*/

    static bool _is_cut_hole( const poly2d& poly_b, const vec2_type& a_p, const vec2_type& b_p, int b_i )
    {
        // 判断ab是否切割polyb的每条边
        int b_cnt = poly_b.get_count();

        for ( int k = 0; k < b_cnt; ++k )
        {
            if ( k == b_i ) // 不包括该点的两边 
                continue;

            if ( poly_b.wrap_index( k + 1 ) == b_i )
                continue;

            vec2_type v;
            T tp, tq;
            if ( _line_vs_line( a_p, b_p, poly_b.get_point(k), poly_b.get_wrap_point(k+1), v, tp, tq ) )
            {
                return false;
            }
        }

        return true;
    }

    static bool _hole_normal( const poly2d& poly_a, const poly2d& poly_b, int& a_i, int& b_i )
    {
        // 将a中有b洞的多边形连接一条ab线，使之变为简单多边形

        int a_cnt = poly_a.get_count();
        int b_cnt = poly_b.get_count();

        for ( a_i = 0; a_i < a_cnt; ++a_i )
        {
            const vec2_type& a_p = poly_a.get_point(a_i); // 选择a的某点

            for ( b_i = 0; b_i < b_cnt; ++b_i ) // 选择b的某点
            {
                const vec2_type& b_p = poly_b.get_point(b_i);

                // 判断ab是否切割polyb的每条边，必须保证a,b是凸的，那么不用判断连线是否切割a
                if ( _is_cut_hole(poly_b, a_p, b_p, b_i) /*&& _poly2d_cut_hole(poly_a, a_p, b_p, a_i)*/ )
                    return true;
            }
        }

        return false;
    }

    static bool _sub_poly_by_hole( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // 多边形a,b必须为正规化且凸
        // 判断a是否包含b全部，如是则分解

        // b是a内的洞，分解
        if ( _poly2d_contain_poly2d_pts( poly_a, poly_b, true ) )
        {
            // 找一个分解的边
            int a_i = 0, b_i = 0;
            if ( _hole_normal( poly_a, poly_b, a_i, b_i ) )
            {
                // 分解
                int a_cnt = poly_a.get_count();
                int b_cnt = poly_b.get_count();

                class_type poly_new;

                for ( int i = 0; i <= a_cnt; ++i ) // 包括自身点两次
                    poly_new.add_point( poly_a.get_wrap_point(a_i + i) );

                for ( int i = 0; i <= b_cnt; ++i ) // 包括自身点两次
                    poly_new.add_point( poly_b.get_wrap_point(b_i - i) );

                poly_ret.push_back( poly_new );
            }
            else // 分解失败？理论不可能
            {
                // 无论如何，这里保留原始多边形
                assert(0);
                poly_ret.push_back( poly_a );
            }

            return true;
        }

        // b不是a内洞
        return false;
    }

    static int _sub_poly_once( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // 执行a - b
        // 多边形必须是正规化且凸

        // 判断a-b=0的情况
        if ( _sub_poly_full_cut( poly_a, poly_b ) )
            return 0;

        // 判断a-b是个洞的情况
        if ( _sub_poly_by_hole( poly_a, poly_b, poly_ret ) )
            return 1;

        //// 判断a-b=a的情况
        //if ( _sub_poly_zero_cut( poly_a, poly_b, poly_ret ) )
        //    return 2;

        // 正常情况
        if ( _sub_poly_comm( poly_a, poly_b, poly_ret ) )
            return 1;

        // 可能是a-b=a情况。或者未能判断情况，理论不可能
        //assert(0);
        poly_ret.push_back( poly_a );
        return 2;
    }

	static int intersect_poly_once(const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret)
	{
		// 执行a - b
		// 多边形必须是正规化且凸

		// 判断a-b=0的情况
		if ( _sub_poly_full_cut( poly_a, poly_b ) )
		{
			poly_ret.push_back(poly_a);

			return 2;
		}

		if (_sub_poly_full_cut(poly_b, poly_a))
		{
			poly_ret.push_back(poly_b);

			return 2;
		}

		// 正常情况
		if ( _intersect_poly_comm( poly_a, poly_b, poly_ret ) )
			return 1;

		// 可能是a-b=a情况。或者未能判断情况，理论不可能
		//assert(0);
		return 0;
	}

    static void _make_sub( const poly2d& poly_src, int i1, int i2, poly2d& poly_dest )
    {
        poly_dest.clear();

        if ( i2 < i1 )
            i2 += poly_src.get_count();

        for ( int i = i1; i <= i2; ++i )
        {
            poly_dest.add_point( poly_src.get_wrap_point(i) );
        }
    }

    static bool _can_split( const poly2d& poly_src, int i1, int i2, poly2d& poly_src1, poly2d& poly_src2 )
    {
        poly_src1.clear();
        poly_src2.clear();

        // 首先检查是否切割边
        const vec2_type& q1 = poly_src.get_point(i1);
        const vec2_type& q2 = poly_src.get_point(i2);

        int cnt = poly_src.get_count();
        
        for ( int i = 0; i < cnt; ++i )
        {
            int i_n = poly_src.wrap_index( i + 1 );
            if ( i1 == i || i1 == i_n || i2 == i || i2 == i_n ) // 不包括对角线所在四条边
                continue;

            const vec2_type& p1 = poly_src.get_point(i);
            const vec2_type& p2 = poly_src.get_point(i_n);

            vec2_type v;
            T tp, tq;
            if ( _line_vs_line( p1, p2, q1, q2, v, tp, tq ) )
            {
                // 有交，但假如交点就在对角线两顶点上，说明本来就如此，认为不交
                T e_cmm = _eps_comm();
                if ( _is_zero(tq, e_cmm) || _is_equal(tq, (T)1, e_cmm) )
                    continue;

                // 真的相交了
                return false;
            }
        }

        // 不切割，然后判断，对角线是否在多边形内
        // 部分1： i1 - i2
        // 部分2： i2 - i1
        _make_sub( poly_src, i1, i2, poly_src1 );
        _make_sub( poly_src, i2, i1, poly_src2 );

        double e = _eps_area();

        if ( _is_less_strict( poly_src1.get_area(), 0.0, e ) ) // 切割后应当正向
            return false;

        if ( _is_less_strict( poly_src2.get_area(), 0.0, e ) ) // 切割后应当正向
            return false;

        return true;
    }

    static void _subdiv_to_convex( const poly2d& poly_a, std::vector<poly2d>& poly_ret )
    {
        // 执行多边形a的正规化，同时切分成多个凸的
        
        // 无面积忽略
        if ( !poly_a.has_area() )
            return;

        // 正规化
        poly2d poly_src = poly_a;
        poly_src.normalize();

        // 先找一个凹点
        int i = 0;
        int cnt = poly_src.get_count();

_REPEAT_DO:
        for ( ; i < cnt; ++i )
        {
            if ( poly_src.is_concave_point(i) )
                break;
        }

        if ( i == cnt ) // 都是凸的
        {
            assert( poly_src.is_ccw() );
            poly_ret.push_back( poly_src );
            return;
        }

        // 找第二个点，连线可以切分多边形
        class_type poly_src1, poly_src2;

        for ( int k = 2; k < cnt - 1; ++k ) // 不含当前相邻两点
        {
            int i2 = poly_src.wrap_index( i + k );
            
            if ( _can_split( poly_src, i, i2, poly_src1, poly_src2 ) ) // 能切分
            {
                _subdiv_to_convex( poly_src1, poly_ret ); // 继续递归切分
                _subdiv_to_convex( poly_src2, poly_ret );
                return;
            }
        }

        // 不能剖分，理论不可能，但可能刚好切到点上，这时可以换个凹点
        assert(0);
        ++i;
        goto _REPEAT_DO;
    }

    static void _subdiv_to_tris( const poly2d& poly, std::vector<int>& idxs )
    {
        // 将一个凸多边形分为多个三角形

        int cnt = poly.get_count();

        for ( int i = 2; i < cnt; ++i )
        {
            poly2d tri;
            tri.add_point( poly.get_point(0) );
            tri.add_point( poly.get_point(i-1) );
            tri.add_point( poly.get_point(i) );

            if ( tri.has_area() )
            {
                idxs.push_back( 0 );
                idxs.push_back( i-1 );
                idxs.push_back( i );
            }
        }
    }

public:
    static int sub_poly( const poly2d& poly_a, const std::vector<poly2d>& poly_b_list, std::vector<poly2d>& poly_ret )
    {
        // 执行a - b
        // 结果返回多个凸多边形
        // a,b无要求
        // 返回
        // 0 - 全部被裁剪光
        // 1 - 部分裁剪
        // 2 - 整个保留

        // 正规化转凸多边形
        std::vector<class_type> poly_src_list;
        _subdiv_to_convex( poly_a, poly_src_list );
        if ( poly_src_list.empty() ) // 没有面积
            return 0; // 可以认为被裁光

        // 对每一个裁减执行
        // 裁减多边形是否存在面积
        std::vector<class_type> poly_dest_list;
        for ( int i = 0; i < (int)poly_b_list.size(); ++i )
        {
            const class_type& poly_b = poly_b_list[i];
            _subdiv_to_convex( poly_b, poly_dest_list ); // 转凸
        }

        if ( poly_dest_list.empty() ) // 裁剪多边形没有面积不用裁
        {
            poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );
            return 2; // 可以认为整个保留，但还是返回正规化后的原始多边形
        }

        // 执行裁减
        std::vector<class_type> tmp_result;
        bool full_vis = true; // 整个可见
        for ( int j = 0; j < (int)poly_dest_list.size(); ++j ) // 对于每个洞
        {
            const class_type& poly_dest = poly_dest_list[j];
            tmp_result.clear();

            // 对每个源做裁减，结果存到tmp_result
            for ( int i = 0; i < (int)poly_src_list.size(); ++i ) 
            {
                if ( _sub_poly_once( poly_src_list[i], poly_dest, tmp_result ) != 2 )
                    full_vis = false;
            }

            // ok了，那么下次tmp_result就是源了，这里将中间结果正规凸化
            poly_src_list.clear();
            for ( int k = 0; k < (int)tmp_result.size(); ++k )
                _subdiv_to_convex( tmp_result[k], poly_src_list );
        }

        // 返回
        poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );

        if ( full_vis ) // 执行过程都是整个保留
            return 2;
        else 
            return poly_src_list.empty() ? 0 : 1; // 根据最终裁剪结果列表判断是否全部或部分被裁
    }

	static int intersect_poly( const poly2d& poly_a, const std::vector<poly2d>& poly_b_list, std::vector<poly2d>& poly_ret )
	{
		// 执行a intersect b
		// 结果返回多个凸多边形
		// a,b无要求
		// 返回
		// 0 - 全部被裁剪光
		// 1 - 部分裁剪
		// 2 - 整个保留

		// 正规化转凸多边形
		std::vector<class_type> poly_src_list;
		_subdiv_to_convex( poly_a, poly_src_list );
		if ( poly_src_list.empty() ) // 没有面积
			return 0; // 可以认为被裁光

		// 对每一个裁减执行
		// 裁减多边形是否存在面积
		std::vector<class_type> poly_dest_list;
		for ( int i = 0; i < (int)poly_b_list.size(); ++i )
		{
			const class_type& poly_b = poly_b_list[i];
			_subdiv_to_convex( poly_b, poly_dest_list ); // 转凸
		}

		if ( poly_dest_list.empty() ) // 裁剪多边形没有面积不用裁
		{
			poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );
			return 2; // 可以认为整个保留，但还是返回正规化后的原始多边形
		}

		// 执行裁减
		std::vector<class_type> tmp_result;
		bool full_vis = true; // 整个可见
		for ( int j = 0; j < (int)poly_dest_list.size(); ++j ) // 对于每个洞
		{
			const class_type& poly_dest = poly_dest_list[j];
			tmp_result.clear();

			// 对每个源做裁减，结果存到tmp_result
			for ( int i = 0; i < (int)poly_src_list.size(); ++i ) 
			{
				if ( intersect_poly_once( poly_src_list[i], poly_dest, tmp_result ) != 2 )
					full_vis = false;
			}

			// ok了，那么下次tmp_result就是源了，这里将中间结果正规凸化
			poly_src_list.clear();
			for ( int k = 0; k < (int)tmp_result.size(); ++k )
				_subdiv_to_convex( tmp_result[k], poly_src_list );
		}

		// 返回
		poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );

		if ( full_vis ) // 执行过程都是整个保留
			return 2;
		else 
			return poly_src_list.empty() ? 0 : 1; // 根据最终裁剪结果列表判断是否全部或部分被裁
	}

    static void polies_to_tris( std::vector<poly2d>& poly_list, std::vector<int>& tri_idxs )
    {
        // poly_list要求为凸多边形
        // 结果返回细分后三角形的索引，索引为相对list的全局表示

        int begOff = 0;

        int list_cnt = (int)poly_list.size();
        for ( int i = 0; i < list_cnt; ++i )
        {
            std::vector<int> idxs;
            _subdiv_to_tris( poly_list[i], idxs );

            int idx_cnt = (int)idxs.size();
            for ( int j = 0; j < idx_cnt; ++j )
            {
                tri_idxs.push_back( begOff + idxs[j] );
            }

            begOff += (int)poly_list[i].get_count(); // 下一个vb偏移位置
        }
    }
};

typedef poly2d<float>  poly2df;
typedef poly2d<double> poly2dd;

//////////////////////////////////////////////////////////////////////////
}

