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
    // ���ﶨ��poly��Ĭ������ϵx�ң�y��
    typedef poly2d<T>               class_type;
    typedef nv::vec2<T>             vec2_type;
    typedef std::vector<vec2_type>  vec2_array_type;

private:
    // ����
    static T        _eps_comm()  { return (T)1e-5; }   // ��ͨ��
    static T        _eps_point() { return (T)1e-4; }   // �����
    static double   _eps_area()  { return 1e-8;    }   // ���
    static T        _eps_cost()  { return (T)1e-4; }   // �Ƕ����

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

    // ����
public:
    vec2_array_type pts;

//////////////////////////////////////////////////////////////////////////
// ��������
public:
    bool is_valid() const
    {
        // ����3��Ϸ�
        return (int)pts.size() >= 3;
    }

    int get_count() const
    {
        return (int)pts.size();
    }

    int wrap_index( int i ) const
    {
        // ����wrap����
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
// �߼�����
public:
    // ��ȡ���
    double get_area() const
    {
        if ( !is_valid() )
            return 0;

        double area = 0;
        double s    = 0;

        // s = ��f(x)dx (x: x1->x2) = (x2 - x1)(y1 + y2) / 2;
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

    // ����Ǵ���
    bool has_area() const
    {
        // �����Ϊ0
        return !_is_zero( get_area(), _eps_area() );
    }

    // �Ƿ�����ʱ�룬�涨��ʱ��Ϊ����
    bool is_ccw() const
    {
        if ( !is_valid() )
            return false;

        double area = get_area();
        return !_is_less_equal( area, 0.0, _eps_area() ); // ���>0Ϊ��
    }

    // ����˳��Ϊ����
    void normalize()
    {
        if ( !is_valid() )
            return;

        if ( is_ccw() )
            return;

        // �ߵ�˳��
        std::reverse( pts.begin(), pts.end() );
    }

public:
    static T _point_proj_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& pt )
    {
        // ����pt��p1p2�ϵ�ͶӰ
        // t = (pt - p1).p2p1 / p2p1^2
        vec2_type p2_p1 = p2 - p1;
        vec2_type pt_p1 = pt - p1;
        T p2_p1_dot = nv::dot( p2_p1, p2_p1 );
        T t = nv::dot(pt_p1, p2_p1) / p2_p1_dot;
        return t;
    }

    static bool _point_on_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& pt )
    {
        // 1 - ��
        // 0 - ����

        const T e_comm = _eps_comm();
        const T e_pt   = _eps_point();

        if ( _is_equal_point(p1, pt) || _is_equal_point(p2, pt) ) // �����ж˵��غ��㽻
            return true;

        // ����pt��p1p2�ϵ�ͶӰ
        T t = _point_proj_line( p1, p2, pt );

        if ( _is_less_equal((T)0, t, e_comm) && _is_less_equal(t, (T)1, e_comm) ) // t���߶���
        {
            vec2_type pt_proj = p1 + (p2 - p1) * t;
            T dist = nv::length(pt - pt_proj);
            return _is_zero(dist, e_pt); // ͶӰ����ܽӽ��߶�
        }
        
        return false;
    }

    static bool _xray_intersect_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& ro )
    {
        // x���ߺ�ֱ�߽��������
        // 1 - ������������߻����м�
        // 0 - �������

        const T e_comm = _eps_comm();
        const T e_pt   = _eps_point();

        T dy = p2.y - p1.y;
        if ( _is_zero(dy, e_pt) ) // ƽ���ߺ���
            return false;

        T t = (ro.y - p1.y) / dy;
        T x = p1.x + t * (p2.x - p1.x);
        if ( !_is_less_equal(ro.x, x, e_pt) ) // ro�ڽ����Ҳ�
            return false;

        if ( _is_zero(t, e_comm) ) // ���ߴ���p1
            return p1.y < p2.y ? false : true; // ѡ����

        if ( _is_equal(t, (T)1, e_comm) ) // ���ߴ���p2
            return p1.y < p2.y ? true : false; // ѡ����

        if ( t < (T)0 || t > (T)1 ) // ����p1p2��
            return false;

        return true;
    }

public:
    bool has_point( const vec2_type& pt, bool include_edge ) const
    {
        // �ж϶���ΰ�����
        // �㷨�Զ������Ҫ��
        // ��pt������+x����poly�����������ڣ���֮��
        int inter_count = 0;
        int poly_count  = get_count();

        for ( int i = 0; i < poly_count; ++i )
        {
            // �õ��߶�
            const vec2_type& p1 = get_point(i);
            const vec2_type& p2 = get_wrap_point(i+1);

            // ����ڸ��߶���
            if ( _point_on_line( p1, p2, pt ) )
            {
                return include_edge;
            }

            // xray�������
            if ( _xray_intersect_line( p1, p2, pt ) ) // ����������߻����м����һ������
                ++inter_count;
        }

        return (inter_count % 2) == 1; // ����������˵������
    }

    bool is_concave_point( int i ) const
    {
        // �ж��Ƿ񰼵�
        // �����������滯������
        const vec2_type& pt  = get_wrap_point(i);
        const vec2_type& pt1 = get_wrap_point(i-1);
        const vec2_type& pt2 = get_wrap_point(i+1);

        vec2_type vn = pt2 - pt;
        vec2_type vp = pt - pt1;

        nv::vec3<T> vt = nv::cross( vn, vp );

        T e = _eps_comm();
        if ( _is_zero(vt.z, e) ) // ƽ�п��Բ��㰼
            return false;

        return vt.z < (T)0; // �ǰ�
    }

    bool intersect_line( const vec2_type& p1, const vec2_type& p2 ) const
    {
        // �ж϶�����Ƿ��ֱ���ཻ��ֱ������Ҳ�㣬��һ���ڶ���α���Ҳ�㣩

        // �Ƿ�Ͷ�����б��ཻ
        int cnt = get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& q1 = get_point(i);
            const vec2_type& q2 = get_wrap_point(i+1);
            vec2_type v;
            T tp, tq;
            if ( _line_vs_line( p1, p2, q1, q2, v, tp, tq ) ) // �н�
                return true;
        }

        // �޽���Ҫô�������ⲿ��Ҫô�������ڲ�
        // �Ƿ��ڶ�����ڣ�ֻҪ����1�㼴��
        if ( has_point( p1, true ) )
            return true;

        return false;
    }

    bool intersect_aabb( const vec2_type& pmin, const vec2_type& pmax ) const
    {
        // �������aabb�ཻ

        // ��һ�����Ǳ���ã�ֻ��Ϊ�˼��ٴ���
        // ����poly�Ƿ��е���aabb��
        T e_cmm = _eps_comm();
        int cnt = get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            const vec2_type& pt = get_point(i);
            if ( _is_less_equal(pmin.x, pt.x, e_cmm) && _is_less_equal(pt.x, pmax.x, e_cmm) &&
                _is_less_equal(pmin.y, pt.y, e_cmm) && _is_less_equal(pt.y, pmax.y, e_cmm) ) // �����ཻ
                return true;
        }

        // ����aabb�������Ƿ��poly�ཻ
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
// �����㷨
public:
    // ����a - b�Ĳü������Σ��������Ϊ�����
    struct inter_contex
    {
        inter_contex() : a_edge(0), b_edge(0), a_len(0), b_len(0), a_cos(0), b_cos(0), a_vtx(0), b_vtx(0), type(0) {}

        vec2_type   pt;         // ����
        int         a_edge;     // ����a�ı�
        int         b_edge;     // ����b�ı�
        T           a_len;      // a�ߵľ���
        T           b_len;      // b�ߵľ���
        T           a_cos;      // a�ߵĽ�cos
        T           b_cos;      // b�ߵĽ�cos
        int         a_vtx;      // ����պ��Ƕ��㣬a�Ķ����
        int         b_vtx;      // ����պ��Ƕ��㣬b�Ķ����
        int         type;       // 0��ͨ���� 1��� 2���� 3�����
    };

    struct visitor_context
    {
        visitor_context() : port(0), type(0) {}

        vec2_type pt;   // ��
        int       port; // תȥ�Է���λ��
        int       type; // 0��ͨ�� 1��� 2���� 3�����
    };

    static bool _line_vs_line( const vec2_type& p1, const vec2_type& p2, const vec2_type& q1, const vec2_type& q2, 
        vec2_type& v, T& tp, T& tq )
    {
        // ����ֵ
        // 0 ƽ���޽�  false
        // 1 ��ƽ���޽� false
        // 2 �ཻ�н� true

        // p2_p1 * tp + q1_q2 * tq = q1_p1
        // a * tp + b * tq = c

        vec2_type a = p2 - p1;
        vec2_type b = q1 - q2;

        T d_a = a.x * b.y - b.x * a.y;

        const T e = _eps_comm();

        if ( _is_zero( d_a, e ) ) // ƽ���޽�
        {
            return false;
        }

        // ���ƽ���ж�
        {
            T e_cos = _eps_cost();
            vec2_type a1 = nv::normalize(a);
            vec2_type b1 = nv::normalize(b);
            T cos_ab = nv::dot(a1, b1);
            if ( _is_equal(cos_ab, (T)1, e_cos) || _is_equal(cos_ab, (T)-1, e_cos) )
                return false;
        }

        // ��ƽ��
        vec2_type c = q1 - p1;

        tp = (c.x * b.y - b.x * c.y) / d_a;

        if ( _is_less_equal((T)0, tp, e) && _is_less_equal(tp, (T)1, e) ) // ��p1p2��
        {
            tq = (a.x * c.y - c.x * a.y) / d_a;

            if ( _is_less_equal((T)0, tq, e) && _is_less_equal(tq, (T)1, e) ) // ��q1q2��
            {
                v = p1 + a * tp; // �н�
                return true;
            }
        }

        // �޽�
        return false;
    }

    static T _get_cos_t( const vec2_type& v1, const vec2_type& v2, const vec2_type& vt, T ic_len )
    {
        // ���� v1.v2 = |v1|v2| cost

        vec2_type v1n = nv::normalize(v1);
        vec2_type v2n = nv::normalize((ic_len < (T)0.5) ? v2 : -v2 ); // <0.5������㣬>0.5�յ�Ҫ����
        T cos_t = nv::dot( v1n, v2n );
        return cos_t;
    }

    static int _get_vtx_idx( int edge_idx, int cnt, T ic_len )
    {
        const T e = _eps_comm();
        if ( _is_zero(ic_len, e) ) // �Ǳ��ϵĿ�ͷ
            return edge_idx;
        if ( _is_equal(ic_len, (T)1.0, e) ) // �Ǳ��ϵ�β���Ǿ����¸������
            return (edge_idx+1) % cnt; // �п��������ܻ����
        return -1; // �Ƕ���
    }

    static void _get_inters( const poly2d& poly_a, const poly2d& poly_b, std::vector<inter_contex>& ics )
    {
        // ��a����b���������������
        // b����a��Ϊ��㣬b��a��ȥ��Ϊ����
        // poly_a1��poly_b1�������滯

        const T e = _eps_comm();

        ics.clear();

        int a_cnt = poly_a.get_count();
        int b_cnt = poly_b.get_count();

        for ( int i = 0; i < a_cnt; ++i ) // ����a��ÿ����
        {
            const vec2_type& a_p1 = poly_a.get_point(i);
            const vec2_type& a_p2 = poly_a.get_wrap_point(i+1);

            // ����b��ÿ�������
            for ( int j = 0; j < b_cnt; ++j )
            {
                const vec2_type& b_p1 = poly_b.get_point(j);
                const vec2_type& b_p2 = poly_b.get_wrap_point(j+1);

                inter_contex ic;

                if ( _line_vs_line( a_p1, a_p2, b_p1, b_p2, ic.pt, ic.a_len, ic.b_len ) ) // �޽��㣬ƽ��֮�඼����
                {
                    // �ж�pt�Ƿ������ b x aָ��ֽ��
                    vec2_type na = a_p2 - a_p1;
                    vec2_type nb = b_p2 - b_p1;

                    T z = nv::cross(nb, na).z;

                    if ( _is_zero(z, e) )
                    {
                        ic.type = 0; // ƽ�У�δ֪
                    }
                    else if ( z > 0 ) // �����
                    {
                        ic.type = 1;
                    }
                    else if ( z < 0 ) // �ǳ���
                    {
                        ic.type = 2;
                    }

                    // ��齻���Ƿ��Ƕ���
                    ic.a_vtx = _get_vtx_idx( i, a_cnt, ic.a_len );
                    ic.b_vtx = _get_vtx_idx( j, b_cnt, ic.b_len );

                    // ��¼cosֵ���Ƚ�������
                    ic.a_cos = _get_cos_t( na, nb, ic.pt, ic.b_len );
                    ic.b_cos = _get_cos_t( nb, na, ic.pt, ic.a_len );

                    if ( ic.type > 0 ) // ��Ч����
                    {
                        ic.a_edge = i;
                        ic.b_edge = j;
                        ics.push_back( ic );
                    }
                }
            } // ����b����
        } // ����a����
    }

    struct _check_flag
    {
        bool operator()( const inter_contex& ic ) const { return ic.type <= 0; }
    };
    
    static void _merge_inters( std::vector<inter_contex>& ics, int side_cnt, bool side_a )
    {
        // �ϲ�����
        for ( int ei = 0; ei < side_cnt; ++ei )
        {
            // ���������ö�������н���
            std::vector<int> icidx;
            int ics_cnt = (int)ics.size();
            for ( int i = 0; i < ics_cnt; ++i )
            {
                inter_contex& ic = ics[i];
                if ( ic.type > 0 )
                {
                    int vidx = side_a ? ic.a_vtx : ic.b_vtx;
                    if ( vidx == ei ) // �Ƕ���
                        icidx.push_back(i);
                }
            }

            // ���������Ķ���ϲ�����
            int icidx_cnt = (int)icidx.size();
            if ( icidx_cnt > 1 ) // ����2������
            {
                // ͳ���������
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

                // ˭�����Ͷ����˭����Ϊ+-˵���Ƿ����ߵ�����++˵�������䣬--˵������
                int type = 0;
                if ( in_count > out_count )
                    type = 1;
                else if ( in_count < out_count )
                    type = 2;

                inter_contex& icfirst = ics[icidx[0]];

                if ( type == 0 ) // ����ȡ���ķ���㣬����������
                {
                    icfirst.type = -100; // ɾ�����
                }
                else // �����ϲ�������
                {
                    icfirst.type = type;
                }

                // ɾ�����������ڵ�
                for ( int i = 1; i < icidx_cnt; ++i )
                {
                    ics[ icidx[i] ].type = -100; // ɾ�����
                }
            } // end of if ( icidx_cnt > 1 ) 
        } // end of ����ÿ������

        // ������ɾ�����-100�ĵ�
       
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
            
            // �����ڸ�ֱ����ͬλ��
            if ( _is_equal( l_len, r_len, _eps_comm() ) )
            {
                // �ڱȽ�cost��ֵ��С����ǰ
                T l_cos = _side_a ? _ic[l].a_cos : _ic[l].b_cos;
                T r_cos = _side_a ? _ic[r].a_cos : _ic[r].b_cos;
                
                if ( _is_equal( l_cos, r_cos, _eps_comm() ) )
                {
                    // ���꣬��һ����
                    // ����a��-ǰ+�󣬶���b��+ǰ-��+��ʾ��ڣ�-��ʾ���ڣ�
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
        // ����ĳ���ϵĽ������
        ics_side.clear();

        side_vtx = -1; // �ñ���ʼ�����Ƿ��Ǹ�����

        int cnt = (int)ics.size();

        // �����ҵ��ñߵĽ���
        for ( int i = 0; i < cnt; ++i )
        {
            const inter_contex& ic = ics[i];

            int vtx = side_a ? ic.a_vtx : ic.b_vtx;
            if ( vtx != -1 ) // �Ǹ����㽻��
            {
                if ( vtx == side ) // �Ǹñ߶���
                {
                    assert(side_vtx == -1);
                    side_vtx = i;
                }
            }
            else // �Ǹ��߽���
            {
                int edge = side_a ? ic.a_edge : ic.b_edge;
                if ( edge == side ) // �������
                {
                    ics_side.push_back(i);
                }
            }
        }

        // ���ݾ�������ľ�������
        

        std::sort( ics_side.begin(), ics_side.end(), sort_by_dist(ics, side_a) );
    }

    static void _get_visitor_side( const poly2d& poly, const std::vector<inter_contex>& ics,
        std::vector<visitor_context>& vcs, bool side_a )
    {
        // poly�������滯
        // �������
        vcs.clear();
        visitor_context vc;

        // ɨ�����ѹ��
        int cnt = poly.get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            // ��ȡ�ñߵĽ���
            std::vector<int> ics_side;
            int side_vtx = -1;
            _get_edge_pts( ics, ics_side, side_vtx, i, side_a );

            // ������ѹ��
            vc.pt   = poly.get_point(i);
            vc.type = 0;
            vc.port = -1;

            if ( side_vtx != -1 ) // �ж϶����Ƿ��ǽ���
            {
                const inter_contex& ic = ics[side_vtx];
                vc.type = ic.type | 0x80;
                vc.port = side_vtx;
            }

            vcs.push_back( vc );

            // Ȼ��˳��ѹ��ñ߽���
            int ics_side_cnt = (int)ics_side.size();
            for ( int is_it = 0; is_it < ics_side_cnt; ++is_it )
            {
                int idx = ics_side[is_it];
                const inter_contex& ic = ics[idx];
                vc.pt   = ic.pt;
                vc.type = ic.type | 0x80; // һ����Ǳ�ʾû�б�����
                vc.port = idx; // �����ȱ��ics�������Ա�֮������
                vcs.push_back( vc );
            } // end ofÿ������ÿ������
        } // end of ÿ��
    }

    static bool _set_visitor_port( std::vector<visitor_context>& avc, std::vector<visitor_context>& bvc )
    {
        bool has_in_port  = false;
        bool has_out_port = false;

        // �������
        int a_cnt = (int)avc.size();

        for ( int i = 0; i < a_cnt; ++i )
        {
            visitor_context& vc_a = avc[i];
            if ( vc_a.type == 0 ) // �Ǹ���ͨ����
                continue;

            // ���Ҹõ���bvc��λ��
            assert( (vc_a.type & 0x80) && (vc_a.port >= 0) );
            int i_b = 0;
            int b_cnt = (int)bvc.size();
            for ( ; i_b < b_cnt; ++i_b )
            {
                const visitor_context& vc_b = bvc[i_b];
                if ( (vc_b.port == vc_a.port) && (vc_b.type & 0x80) ) // port����ids������������Ӧ����ͬ
                    break;
            }

            if ( i_b < b_cnt )
            {
                vc_a.port = i_b; // ˫����ת����
                visitor_context& vc_b = bvc[i_b];
                assert( vc_b.type == vc_a.type ); // �Լ�Ӧ��Ҳ�ǳ���������vc_a��ͬ
                vc_a.type = vc_b.type = (vc_a.type & 0x3);
                vc_b.port = i;

                if ( vc_a.type == 1 )
                    has_in_port = true; // ������һ�����
                else if ( vc_a.type == 2 )
                    has_out_port = true; // ������һ������
            }
            else
            {
                assert(0);
                return false; // ����ʧ�ܣ�����bug
            }
        }

        // ���b�Ķ������
        int b_cnt = (int)bvc.size();
        for ( int i = 0; i < b_cnt; ++i )
        {
            visitor_context& vc_b = bvc[i];
            if ( vc_b.type & 0x80 )
            {
                assert(0);     // Ӧ��������
                return false; // ����bug������ʧ��
            }
        }

        // �Ƿ�������һ�������
        if ( !has_in_port )
            return false;
        if ( !has_out_port )
            return false;
        return true;
    }

    static bool _sub_poly_comm( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // ִ��a - b Ҫ������н�
        // �㷨���԰�͹����ζ���Ч������β����й��㣬��Ϊ�˱�֤��һ���ֻ�ܴ���͹�����
        // ����α��������滯����Ϊ͹

        //DebugBreak();

        // ��ȡ�������
        std::vector<inter_contex> ics;
        _get_inters( poly_a, poly_b, ics );
        if ( ics.empty() )
            return false;

        // �ϲ��������
        _merge_inters( ics, poly_a.get_count(), true );
        _merge_inters( ics, poly_b.get_count(), false );
        if ( ics.empty() )
            return false;

        // ���뽻�㲢����
        std::vector<visitor_context> avc, bvc;
        _get_visitor_side( poly_a, ics, avc, true );
        _get_visitor_side( poly_b, ics, bvc, false );

        // �����л����
        if ( !_set_visitor_port( avc, bvc ) )
            return false;

        // ��ʼ����
        int avc_cnt = (int)avc.size();
        //int bvc_cnt = (int)bvc.size();

        while ( true )
        {
            // a����һ����㿪ʼ
            int it_a = 0;
            for (; it_a < avc_cnt; ++it_a )
            {
                const visitor_context& vc = avc[it_a];
                if ( vc.type == 1 ) // ���
                    break;
            }

            // û������ˣ������㷨
            if ( it_a == avc_cnt )
                break;

            // ������ѭ��
            std::set<visitor_context*> cut_logs;

            // ����㿪ʼ����
            poly_ret.push_back( class_type() );
            class_type& new_poly = poly_ret.back();
            new_poly.add_point( avc[it_a].pt );
            avc[it_a].type = -1; // ȥ����
            cut_logs.insert( &avc[it_a] ); // ����־

            // ���¸���
            int it = (it_a + 1) % avc_cnt;
            std::vector<visitor_context>* vcs = &avc;
            int flag = 2;
            int step = 1;

            while( true )
            {
                // �б�ǰ��
                visitor_context& vc = (*vcs)[it];

                if ( !cut_logs.insert(&vc).second ) // �ж��Ƿ񽫻�����ظ��ĵ�
                {
                    assert(0);
                    break;
                }

                // ����Ƿ������������++�������䷴���ĳ��ʱ�����䷴����һΪƽ����Ϊ���ˣ���ʵ������
				
                if ( flag == 2 && (vc.type == 1 || vc.type == -1) ) // ������-(���ϸ��ؼ���+)��������+������ʷ+����
                {
                    if ( vc.type == 1 )
                    {
                        // ����ϴΣ������Ϊ�����
                        new_poly.clear();
                        cut_logs.clear();
                        cut_logs.insert(&vc);
                        it_a = it;
                        vc.type = -1;
                    }
                    else
                    {
                        // �õ�����
                        new_poly.clear();
                        break;
                    }
                }
				

                // ����Ҫ��ת�۵�
                if ( vc.type == flag ) 
                {
                    bool last_is_avc = (vcs == &avc); // �˴���a��

                    // ��ת��ת
                    it = vc.port; 
                    vcs = (last_is_avc ? &bvc : &avc);
                    flag = (~flag) & 0x3;

					// ����A���෴��������
                    step *= -1;

                    // ֻ���a��������,�����ظ�
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

                    if ( it == it_a && vcs == &avc ) // ��ת��ص�������
                        break;
                }

                if ( (new_poly.get_count() > 0) && _is_equal_point(new_poly.pts.back(), vc.pt) )
                    ; // ��ӵ���ϴ�һ�������ظ�
                else
                    new_poly.add_point( vc.pt ); // ���Լ��������

                it += step;
                if ( it < 0 )
                    it = (int)vcs->size() - 1;
                else if ( it >= (int)vcs->size() )
                    it = 0;
            } // ����һ�����Ӷ���α���

            // ����ȡ�µĺϷ���
            if ( !poly_ret.empty() )
            {
                double poly_area = poly_ret.back().get_area();
                if ( _is_less_equal( poly_area, 0.0, _eps_area() ) ) // ���<=0���Ϸ�
                    poly_ret.pop_back();
            }

        } // ��������������

        return true;
    }

	static bool _intersect_poly_comm( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
	{
		// ִ��a intersect b Ҫ������н�
		// �㷨���԰�͹����ζ���Ч������β����й��㣬��Ϊ�˱�֤��һ���ֻ�ܴ���͹�����
		// ����α��������滯����Ϊ͹

		//DebugBreak();

		// ��ȡ�������
		std::vector<inter_contex> ics;
		_get_inters( poly_a, poly_b, ics );
		if ( ics.empty() )
			return false;

		// �ϲ��������
		_merge_inters( ics, poly_a.get_count(), true );
		_merge_inters( ics, poly_b.get_count(), false );
		if ( ics.empty() )
			return false;

		// ���뽻�㲢����
		std::vector<visitor_context> avc, bvc;
		_get_visitor_side( poly_a, ics, avc, true );
		_get_visitor_side( poly_b, ics, bvc, false );

		// �����л����
		if ( !_set_visitor_port( avc, bvc ) )
			return false;

		// ��ʼ����
		int avc_cnt = (int)avc.size();
		int bvc_cnt = (int)bvc.size();

		while ( true )
		{
			// a����һ����㿪ʼ
			int it_a = 0;
			for (; it_a < avc_cnt; ++it_a )
			{
				const visitor_context& vc = avc[it_a];
				if ( vc.type == 1 ) // ���
					break;
			}

			// û������ˣ������㷨
			if ( it_a == avc_cnt )
				break;

			// ������ѭ��
			std::set<visitor_context*> cut_logs;

			// ����㿪ʼ����
			poly_ret.push_back( class_type() );
			class_type& new_poly = poly_ret.back();
			new_poly.add_point( avc[it_a].pt );
			avc[it_a].type = -1; // ȥ����
			cut_logs.insert( &avc[it_a] ); // ����־

			// ��b��ǰ�㿪ʼ
			int it = avc[it_a].port;
			std::vector<visitor_context>* vcs = &bvc;
			int flag = 2;
			int step = 1;

			while( true )
			{
				// �б�ǰ��
				visitor_context& vc = (*vcs)[it];

				if ( !cut_logs.insert(&vc).second ) // �ж��Ƿ񽫻�����ظ��ĵ�
				{
					break;
				}

				// ����Ƿ������������++�������䷴���ĳ��ʱ�����䷴����һΪƽ����Ϊ���ˣ���ʵ������
				/*
				if ( flag == 2 && (vc.type == 1 || vc.type == -1) ) // ������-(���ϸ��ؼ���+)��������+������ʷ+����
				{
					if ( vc.type == 1 )
					{
						// ����ϴΣ������Ϊ�����
						new_poly.clear();
						cut_logs.clear();
						cut_logs.insert(&vc);
						it_a = it;
						vc.type = -1;
					}
					else
					{
						// �õ�����
						new_poly.clear();
						break;
					}
				}
				*/

				// ����Ҫ��ת�۵�
				if ( vc.type == flag ) 
				{
					bool last_is_avc = (vcs == &avc); // �˴���a��

					// ��ת��ת
					it = vc.port; 
					vcs = (last_is_avc ? &bvc : &avc);
					flag = (~flag) & 0x3;

					// ����A�ķ�������
					step *= 1;

					// ֻ���a��������,�����ظ�
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

					if ( it == it_a && vcs == &avc ) // ��ת��ص�������
						break;
				}

				if ( (new_poly.get_count() > 0) && _is_equal_point(new_poly.pts.back(), vc.pt) )
					; // ��ӵ���ϴ�һ�������ظ�
				else
					new_poly.add_point( vc.pt ); // ���Լ��������

				it += step;
				if ( it < 0 )
					it = (int)vcs->size() - 1;
				else if ( it >= (int)vcs->size() )
					it = 0;
			} // ����һ�����Ӷ���α���

			// ����ȡ�µĺϷ���
			if ( !poly_ret.empty() )
			{
				double poly_area = poly_ret.back().get_area();
				if ( _is_less_equal( poly_area, 0.0, _eps_area() ) ) // ���<=0���Ϸ�
					poly_ret.pop_back();
			}

		} // ��������������

		return true;
	}

    static bool _poly2d_contain_poly2d_pts( const poly2d& poly_a, const poly2d& poly_b, bool include_edge )
    {
        // ���Զ����a�Ƿ���������b�ĵ㼯��
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
        // �����a,b����Ϊ���滯��͹
        // �ж�a - b = 0
        return _poly2d_contain_poly2d_pts( poly_b, poly_a, true ); // a����b�ڣ�ȫ�����ɵ�
    }

    /*static bool _sub_poly_zero_cut( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // �����a,b����Ϊ���滯��͹
        // �ж�a - b = a

        int cnt = poly_b.get_count();
        for ( int i = 0; i < cnt; ++i )
        {
            if ( poly_a.has_point( poly_b.get_point(i), false ) ) // b��һ���Ƿ���a�ڣ������ߣ�
                return false; // �У���ô��Ҫ�ü�
        }

        // û��Ҳ���������ų������b�Ƿ��б��и�a
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
                    return false; // ���и�
            }
        }

        poly_ret.push_back(poly_a);
        return true; // a��������
    }*/

    static bool _is_cut_hole( const poly2d& poly_b, const vec2_type& a_p, const vec2_type& b_p, int b_i )
    {
        // �ж�ab�Ƿ��и�polyb��ÿ����
        int b_cnt = poly_b.get_count();

        for ( int k = 0; k < b_cnt; ++k )
        {
            if ( k == b_i ) // �������õ������ 
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
        // ��a����b���Ķ��������һ��ab�ߣ�ʹ֮��Ϊ�򵥶����

        int a_cnt = poly_a.get_count();
        int b_cnt = poly_b.get_count();

        for ( a_i = 0; a_i < a_cnt; ++a_i )
        {
            const vec2_type& a_p = poly_a.get_point(a_i); // ѡ��a��ĳ��

            for ( b_i = 0; b_i < b_cnt; ++b_i ) // ѡ��b��ĳ��
            {
                const vec2_type& b_p = poly_b.get_point(b_i);

                // �ж�ab�Ƿ��и�polyb��ÿ���ߣ����뱣֤a,b��͹�ģ���ô�����ж������Ƿ��и�a
                if ( _is_cut_hole(poly_b, a_p, b_p, b_i) /*&& _poly2d_cut_hole(poly_a, a_p, b_p, a_i)*/ )
                    return true;
            }
        }

        return false;
    }

    static bool _sub_poly_by_hole( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // �����a,b����Ϊ���滯��͹
        // �ж�a�Ƿ����bȫ����������ֽ�

        // b��a�ڵĶ����ֽ�
        if ( _poly2d_contain_poly2d_pts( poly_a, poly_b, true ) )
        {
            // ��һ���ֽ�ı�
            int a_i = 0, b_i = 0;
            if ( _hole_normal( poly_a, poly_b, a_i, b_i ) )
            {
                // �ֽ�
                int a_cnt = poly_a.get_count();
                int b_cnt = poly_b.get_count();

                class_type poly_new;

                for ( int i = 0; i <= a_cnt; ++i ) // �������������
                    poly_new.add_point( poly_a.get_wrap_point(a_i + i) );

                for ( int i = 0; i <= b_cnt; ++i ) // �������������
                    poly_new.add_point( poly_b.get_wrap_point(b_i - i) );

                poly_ret.push_back( poly_new );
            }
            else // �ֽ�ʧ�ܣ����۲�����
            {
                // ������Σ����ﱣ��ԭʼ�����
                assert(0);
                poly_ret.push_back( poly_a );
            }

            return true;
        }

        // b����a�ڶ�
        return false;
    }

    static int _sub_poly_once( const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret )
    {
        // ִ��a - b
        // ����α��������滯��͹

        // �ж�a-b=0�����
        if ( _sub_poly_full_cut( poly_a, poly_b ) )
            return 0;

        // �ж�a-b�Ǹ��������
        if ( _sub_poly_by_hole( poly_a, poly_b, poly_ret ) )
            return 1;

        //// �ж�a-b=a�����
        //if ( _sub_poly_zero_cut( poly_a, poly_b, poly_ret ) )
        //    return 2;

        // �������
        if ( _sub_poly_comm( poly_a, poly_b, poly_ret ) )
            return 1;

        // ������a-b=a���������δ���ж���������۲�����
        //assert(0);
        poly_ret.push_back( poly_a );
        return 2;
    }

	static int intersect_poly_once(const poly2d& poly_a, const poly2d& poly_b, std::vector<poly2d>& poly_ret)
	{
		// ִ��a - b
		// ����α��������滯��͹

		// �ж�a-b=0�����
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

		// �������
		if ( _intersect_poly_comm( poly_a, poly_b, poly_ret ) )
			return 1;

		// ������a-b=a���������δ���ж���������۲�����
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

        // ���ȼ���Ƿ��и��
        const vec2_type& q1 = poly_src.get_point(i1);
        const vec2_type& q2 = poly_src.get_point(i2);

        int cnt = poly_src.get_count();
        
        for ( int i = 0; i < cnt; ++i )
        {
            int i_n = poly_src.wrap_index( i + 1 );
            if ( i1 == i || i1 == i_n || i2 == i || i2 == i_n ) // �������Խ�������������
                continue;

            const vec2_type& p1 = poly_src.get_point(i);
            const vec2_type& p2 = poly_src.get_point(i_n);

            vec2_type v;
            T tp, tq;
            if ( _line_vs_line( p1, p2, q1, q2, v, tp, tq ) )
            {
                // �н��������罻����ڶԽ����������ϣ�˵����������ˣ���Ϊ����
                T e_cmm = _eps_comm();
                if ( _is_zero(tq, e_cmm) || _is_equal(tq, (T)1, e_cmm) )
                    continue;

                // ����ཻ��
                return false;
            }
        }

        // ���иȻ���жϣ��Խ����Ƿ��ڶ������
        // ����1�� i1 - i2
        // ����2�� i2 - i1
        _make_sub( poly_src, i1, i2, poly_src1 );
        _make_sub( poly_src, i2, i1, poly_src2 );

        double e = _eps_area();

        if ( _is_less_strict( poly_src1.get_area(), 0.0, e ) ) // �и��Ӧ������
            return false;

        if ( _is_less_strict( poly_src2.get_area(), 0.0, e ) ) // �и��Ӧ������
            return false;

        return true;
    }

    static void _subdiv_to_convex( const poly2d& poly_a, std::vector<poly2d>& poly_ret )
    {
        // ִ�ж����a�����滯��ͬʱ�зֳɶ��͹��
        
        // ���������
        if ( !poly_a.has_area() )
            return;

        // ���滯
        poly2d poly_src = poly_a;
        poly_src.normalize();

        // ����һ������
        int i = 0;
        int cnt = poly_src.get_count();

_REPEAT_DO:
        for ( ; i < cnt; ++i )
        {
            if ( poly_src.is_concave_point(i) )
                break;
        }

        if ( i == cnt ) // ����͹��
        {
            assert( poly_src.is_ccw() );
            poly_ret.push_back( poly_src );
            return;
        }

        // �ҵڶ����㣬���߿����зֶ����
        class_type poly_src1, poly_src2;

        for ( int k = 2; k < cnt - 1; ++k ) // ������ǰ��������
        {
            int i2 = poly_src.wrap_index( i + k );
            
            if ( _can_split( poly_src, i, i2, poly_src1, poly_src2 ) ) // ���з�
            {
                _subdiv_to_convex( poly_src1, poly_ret ); // �����ݹ��з�
                _subdiv_to_convex( poly_src2, poly_ret );
                return;
            }
        }

        // �����ʷ֣����۲����ܣ������ܸպ��е����ϣ���ʱ���Ի�������
        assert(0);
        ++i;
        goto _REPEAT_DO;
    }

    static void _subdiv_to_tris( const poly2d& poly, std::vector<int>& idxs )
    {
        // ��һ��͹����η�Ϊ���������

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
        // ִ��a - b
        // ������ض��͹�����
        // a,b��Ҫ��
        // ����
        // 0 - ȫ�����ü���
        // 1 - ���ֲü�
        // 2 - ��������

        // ���滯ת͹�����
        std::vector<class_type> poly_src_list;
        _subdiv_to_convex( poly_a, poly_src_list );
        if ( poly_src_list.empty() ) // û�����
            return 0; // ������Ϊ���ù�

        // ��ÿһ���ü�ִ��
        // �ü�������Ƿ�������
        std::vector<class_type> poly_dest_list;
        for ( int i = 0; i < (int)poly_b_list.size(); ++i )
        {
            const class_type& poly_b = poly_b_list[i];
            _subdiv_to_convex( poly_b, poly_dest_list ); // ת͹
        }

        if ( poly_dest_list.empty() ) // �ü������û��������ò�
        {
            poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );
            return 2; // ������Ϊ���������������Ƿ������滯���ԭʼ�����
        }

        // ִ�вü�
        std::vector<class_type> tmp_result;
        bool full_vis = true; // �����ɼ�
        for ( int j = 0; j < (int)poly_dest_list.size(); ++j ) // ����ÿ����
        {
            const class_type& poly_dest = poly_dest_list[j];
            tmp_result.clear();

            // ��ÿ��Դ���ü�������浽tmp_result
            for ( int i = 0; i < (int)poly_src_list.size(); ++i ) 
            {
                if ( _sub_poly_once( poly_src_list[i], poly_dest, tmp_result ) != 2 )
                    full_vis = false;
            }

            // ok�ˣ���ô�´�tmp_result����Դ�ˣ����ｫ�м�������͹��
            poly_src_list.clear();
            for ( int k = 0; k < (int)tmp_result.size(); ++k )
                _subdiv_to_convex( tmp_result[k], poly_src_list );
        }

        // ����
        poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );

        if ( full_vis ) // ִ�й��̶�����������
            return 2;
        else 
            return poly_src_list.empty() ? 0 : 1; // �������ղü�����б��ж��Ƿ�ȫ���򲿷ֱ���
    }

	static int intersect_poly( const poly2d& poly_a, const std::vector<poly2d>& poly_b_list, std::vector<poly2d>& poly_ret )
	{
		// ִ��a intersect b
		// ������ض��͹�����
		// a,b��Ҫ��
		// ����
		// 0 - ȫ�����ü���
		// 1 - ���ֲü�
		// 2 - ��������

		// ���滯ת͹�����
		std::vector<class_type> poly_src_list;
		_subdiv_to_convex( poly_a, poly_src_list );
		if ( poly_src_list.empty() ) // û�����
			return 0; // ������Ϊ���ù�

		// ��ÿһ���ü�ִ��
		// �ü�������Ƿ�������
		std::vector<class_type> poly_dest_list;
		for ( int i = 0; i < (int)poly_b_list.size(); ++i )
		{
			const class_type& poly_b = poly_b_list[i];
			_subdiv_to_convex( poly_b, poly_dest_list ); // ת͹
		}

		if ( poly_dest_list.empty() ) // �ü������û��������ò�
		{
			poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );
			return 2; // ������Ϊ���������������Ƿ������滯���ԭʼ�����
		}

		// ִ�вü�
		std::vector<class_type> tmp_result;
		bool full_vis = true; // �����ɼ�
		for ( int j = 0; j < (int)poly_dest_list.size(); ++j ) // ����ÿ����
		{
			const class_type& poly_dest = poly_dest_list[j];
			tmp_result.clear();

			// ��ÿ��Դ���ü�������浽tmp_result
			for ( int i = 0; i < (int)poly_src_list.size(); ++i ) 
			{
				if ( intersect_poly_once( poly_src_list[i], poly_dest, tmp_result ) != 2 )
					full_vis = false;
			}

			// ok�ˣ���ô�´�tmp_result����Դ�ˣ����ｫ�м�������͹��
			poly_src_list.clear();
			for ( int k = 0; k < (int)tmp_result.size(); ++k )
				_subdiv_to_convex( tmp_result[k], poly_src_list );
		}

		// ����
		poly_ret.insert( poly_ret.end(), poly_src_list.begin(), poly_src_list.end() );

		if ( full_vis ) // ִ�й��̶�����������
			return 2;
		else 
			return poly_src_list.empty() ? 0 : 1; // �������ղü�����б��ж��Ƿ�ȫ���򲿷ֱ���
	}

    static void polies_to_tris( std::vector<poly2d>& poly_list, std::vector<int>& tri_idxs )
    {
        // poly_listҪ��Ϊ͹�����
        // �������ϸ�ֺ������ε�����������Ϊ���list��ȫ�ֱ�ʾ

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

            begOff += (int)poly_list[i].get_count(); // ��һ��vbƫ��λ��
        }
    }
};

typedef poly2d<float>  poly2df;
typedef poly2d<double> poly2dd;

//////////////////////////////////////////////////////////////////////////
}

