/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/20
* File: enn_mesh_manual_creator.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_mesh_manager.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// MeshCubeCreatorImpl
class MeshCubeCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	MeshCubeCreatorImpl(float size) : m_size(size)
	{

	}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context )
	{
		return true;
	}

	virtual bool buildResource( Resource* res )
	{
		float s = m_size * 0.5f;

#define v0_     -s, -s, -s
#define v1_     -s, -s,  s
#define v2_     -s,  s, -s
#define v3_     -s,  s,  s
#define v4_      s, -s, -s
#define v5_      s, -s,  s
#define v6_      s,  s, -s
#define v7_      s,  s,  s

#define nx_     -1,  0,  0
#define px_      1,  0,  0
#define ny_      0, -1,  0
#define py_      0,  1,  0
#define nz_      0,  0, -1
#define pz_      0,  0,  1

#define u0_      0,  1
#define u1_      1,  1
#define u2_      1,  0
#define u3_      0,  0

		VertexP3N3U2 vts[24] =
		{
			// left
			v0_, nx_, u0_,
			v1_, nx_, u1_,
			v3_, nx_, u2_,
			v2_, nx_, u3_,

			// right
			v5_, px_, u0_,
			v4_, px_, u1_,
			v6_, px_, u2_,
			v7_, px_, u3_,

			// bottom
			v0_, ny_, u0_,
			v4_, ny_, u1_,
			v5_, ny_, u2_,
			v1_, ny_, u3_,

			// top
			v3_, py_, u0_,
			v7_, py_, u1_,
			v6_, py_, u2_,
			v2_, py_, u3_,

			// back
			v4_, nz_, u0_,
			v0_, nz_, u1_,
			v2_, nz_, u2_,
			v6_, nz_, u3_,

			// front
			v1_, pz_, u0_,
			v5_, pz_, u1_,
			v7_, pz_, u2_,
			v3_, pz_, u3_
		};

		ushort ids[36] =
		{
			0,1,2,      2,3,0,
			4,5,6,      6,7,4,
			8,9,10,     10,11,8,
			12,13,14,   14,15,12,
			16,17,18,   18,19,16,
			20,21,22,   22,23,20
		};

		Mesh* mesh = static_cast<Mesh*>(res);
		SubMesh* sub_mesh = mesh->createNewSubMesh(0);

		sub_mesh->setPrimType(PRIM_TRIANGLELIST);
		sub_mesh->setVertexType(MASK_P3N3U2);
		sub_mesh->setVertexCount(ENN_ARRAY_SIZE(vts));

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(ENN_ARRAY_SIZE(ids));

		sub_mesh->setVertexData(vts);
		sub_mesh->setIndexData(ids);

		sub_mesh->makeGPUBuffer();

		mesh->updateAABB(true);

		return true;
	}
protected:
	float	m_size;
};

//////////////////////////////////////////////////////////////////////////
class MeshSphereCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	MeshSphereCreatorImpl(float size, int segments)
		: m_size(size), m_segments(Math::minVal(segments, 256))
	{
		
	}

	virtual void release()
	{
		ENN_DELETE this;
	}

	virtual bool prepareResource( Resource* res, void* context )
	{
		return true;
	}

	virtual bool buildResource( Resource* res )
	{
		float radius = m_size * 0.5f;

		int segmentsDec = m_segments - 1;

		float theta_step = (float)Math::PI / segmentsDec;
		float phi_step = (float)Math::TWO_PI / segmentsDec;
		float uv_step = 1.0f / segmentsDec;

		VertexP3N3U2* vts = ENN_MALLOC_ARRAY_T(VertexP3N3U2, m_segments * m_segments);
		VertexP3N3U2* v = vts;

		for (int row = 0; row < m_segments; ++row)
		{
			float theta = theta_step * row;
			float y = radius * cos( theta );
			float xz = radius * sin( theta );
			float uv_v = uv_step * row;

			for ( int col = 0; col < m_segments; ++col )
			{
				float phi = phi_step * col;

				v->x = xz * cos( phi );
				v->y = y;
				v->z = xz * sin( phi );

				vec3f& vt = *(vec3f*)(&(v->x));

				*(vec3f*)(&(v->nx)) = vt.normalisedCopy();

				v->u = uv_step * col;
				v->v = uv_v;

				++v;
			}
		}

		uint16* ids = ENN_MALLOC_ARRAY_T(uint16, segmentsDec * segmentsDec * 6);
		uint16* iv = ids;

		for ( int row = 0; row < segmentsDec; ++row )
		{
			for ( int col = 0; col < segmentsDec; ++col )
			{
				int lt = row * m_segments + col;
				int rt = lt + 1;
				int lb = lt + m_segments;
				int rb = rt + m_segments;

				*(iv++) = lt;
				*(iv++) = rt;
				*(iv++) = lb;

				*(iv++) = rt;
				*(iv++) = rb;
				*(iv++) = lb;
			}
		}

		Mesh* mesh = static_cast<Mesh*>(res);
		SubMesh* sub_mesh = mesh->createNewSubMesh(0);
		sub_mesh->setPrimType(PRIM_TRIANGLELIST);

		sub_mesh->setVertexType(MASK_P3N3U2);
		sub_mesh->setVertexCount(m_segments * m_segments);

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(segmentsDec * segmentsDec * 6);

		sub_mesh->setVertexData(vts);
		sub_mesh->setIndexData(ids);

		sub_mesh->makeGPUBuffer();

		ENN_SAFE_FREE(vts);
		ENN_SAFE_FREE(ids);

		mesh->updateAABB(true);

		return true;
	}
protected:
	float	m_size;
	int		m_segments;
};


//////////////////////////////////////////////////////////////////////////
class MeshPlaneCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	MeshPlaneCreatorImpl( float xLen, float zLen, int xSegments, int zSegments ) :
	  m_xLen(xLen), m_zLen(zLen)
	  {
		  if ( xSegments * zSegments > 65536 )
		  {
			  if ( xSegments > zSegments )
				  xSegments = 65536 / zSegments;
			  else
				  zSegments = 65536 / xSegments;
		  }

		  m_xSegments = xSegments;
		  m_zSegments = zSegments;
	  }

	  virtual void release()
	  {
		  ENN_DELETE this;
	  }

	  virtual bool prepareResource( Resource* res, void* context )
	  {
		  return true;
	  }

	  virtual bool buildResource( Resource* res )
	  {
		  int xSegmentsDec = m_xSegments - 1;
		  int zSegmentsDec = m_zSegments - 1;

		  float x_step = m_xLen / xSegmentsDec;
		  float z_step = m_zLen / zSegmentsDec;

		  float u_step = 1.0f / xSegmentsDec;
		  float v_step = 1.0f / zSegmentsDec;

		  float halfXLen = m_xLen * 0.5f;
		  float halfZLen = m_zLen * 0.5f;

		  VertexP3N3U2* vts = ENN_MALLOC_ARRAY_T(VertexP3N3U2, m_xSegments * m_zSegments);
		  VertexP3N3U2* v = vts;

		  for ( int row = 0; row < m_zSegments; ++row )
		  {
			  float z = z_step * row - halfZLen;
			  float uv_v = v_step * row;

			  for ( int col = 0; col < m_xSegments; ++col )
			  {    
				  v->x = x_step * col - halfXLen;
				  v->y = 0;
				  v->z = z;

				  *(vec3f*)(&(v->nx)) = vec3f::UNIT_Y;

				  v->u = u_step * col;
				  v->v = uv_v;

				  ++v;
			  }
		  }

		  uint16* ids = ENN_MALLOC_ARRAY_T(uint16, xSegmentsDec*zSegmentsDec*6);
		  uint16* iv = ids;

		  for ( int row = 0; row < zSegmentsDec; ++row )
		  {
			  for ( int col = 0; col < xSegmentsDec; ++col )
			  {
				  int lt = row * m_xSegments + col;
				  int rt = lt + 1;
				  int lb = lt + m_xSegments;
				  int rb = rt + m_xSegments;

				  *(iv++) = lt;
				  *(iv++) = lb;
				  *(iv++) = rt;

				  *(iv++) = rt;
				  *(iv++) = lb;
				  *(iv++) = rb; 
			  }
		  }

		  Mesh* mesh = static_cast<Mesh*>(res);

		  SubMesh* sub_mesh = mesh->createNewSubMesh(0);
		  sub_mesh->setPrimType(PRIM_TRIANGLELIST);

		  sub_mesh->setVertexType(MASK_P3N3U2);
		  sub_mesh->setVertexCount(m_xSegments * m_zSegments);

		  sub_mesh->setIdxType(IDX_16);
		  sub_mesh->setIndexCount(xSegmentsDec * zSegmentsDec * 6);

		  sub_mesh->setVertexData(vts);
		  sub_mesh->setIndexData(ids);

		  sub_mesh->makeGPUBuffer();

		  ENN_SAFE_FREE(vts);
		  ENN_SAFE_FREE(ids);

		  mesh->updateAABB(true);

		  return true;
	  }
protected:
	float m_xLen;
	float m_zLen;
	int   m_xSegments;
	int   m_zSegments;
};

//////////////////////////////////////////////////////////////////////////
// MeshConeCreatorImpl
class MeshConeCreatorImpl : public AllocatedObject, public IResourceCreator
{
public:
	MeshConeCreatorImpl( float width, float height, int wsegments, int hsegments, bool useNZDir ) :
	  m_width(width), m_height(height), 
		  m_wsegments(wsegments), m_hsegments(hsegments),
		  m_useNZDir(useNZDir)
	  {
		  if ( 1 + wsegments + wsegments * hsegments > 65536 )
		  {
			  m_wsegments = (65536 - 1) / (1 + hsegments);
		  }
	  }

	  virtual void release()
	  {
		  ENN_DELETE this;
	  }

	  virtual bool prepareResource( Resource* res, void* context )
	  {
		  return true;
	  }

	  virtual bool buildResource( Resource* res )
	  {
		  float radius = m_width * 0.5f;

		  int vtxCnt = 1 + m_wsegments + m_wsegments * m_hsegments;
		  VertexP3N3U2* vts = ENN_MALLOC_ARRAY_T(VertexP3N3U2, vtxCnt);
		  VertexP3N3U2* v = vts;

		  // 底部中心点
		  v->u = v->v = 0.5f;
		  v->x = v->y = v->z = 0;
		  *(Vector3*)(&(v->nx)) = Vector3::NEGATIVE_UNIT_Y;
		  ++v;

		  // 底部圆上的点
		  for ( int i = 0; i < m_wsegments; ++i )
		  {
			  // 链接底部中心的点
			  float phi = Math::TWO_PI / (m_wsegments - 1) * i;
			  float phi_cos = Math::cos( phi );
			  float phi_sin = Math::sin( phi );

			  v->u = phi_cos * 0.5f + 0.5f;
			  v->v = phi_sin * 0.5f + 0.5f;

			  v->x = radius * phi_cos;
			  v->y = 0;
			  v->z = radius * phi_sin;

			  *(Vector3*)(&(v->nx)) = Vector3::NEGATIVE_UNIT_Y;
			  ++v;
		  }

		  // 顶部中点位置
		  Vector3 topCenter(0, m_height, 0);

		  // 侧面的点
		  for ( int y = 0; y < m_hsegments; ++y )
		  {
			  float cur_height = y * m_height / (m_hsegments - 1);
			  float cur_radius = radius / m_height * (m_height - cur_height);

			  for ( int x = 0; x < m_wsegments; ++x )
			  {
				  float phi = Math::TWO_PI / (m_wsegments - 1) * x;
				  float phi_cos = Math::cos( phi );
				  float phi_sin = Math::sin( phi );

				  v->u = x / float(m_wsegments - 1);
				  v->v = y / float(m_hsegments - 1);

				  v->x = cur_radius * phi_cos;
				  v->y = cur_height;
				  v->z = cur_radius * phi_sin;

				  Vector3 bottomPt(phi_cos, 0, phi_sin);
				  Vector3 bottomY = topCenter - bottomPt;
				  Vector3 bottomZ = bottomPt.crossProduct( bottomY );
				  Vector3 bottomX = bottomY.crossProduct(bottomZ).normalisedCopy();

				  *(Vector3*)(&(v->nx)) = bottomX;
				  ++v;
			  }
		  }

		  if ( m_useNZDir )
			  _convert( vts, vtxCnt );

		  // 处理索引
		  int idxCnt = (m_wsegments-1)*3 + (m_hsegments-1)*(m_wsegments-1)*6;
		  uint16* ids = ENN_MALLOC_ARRAY_T(uint16, idxCnt);
		  uint16* iv = ids;

		  const int startBtmIdx = 1;

		  // 底面
		  for ( int i = 0; i < m_wsegments-1; ++i )
		  {
			  // 底部三角形
			  *(iv++) = 0;
			  *(iv++) = i+startBtmIdx;
			  *(iv++) = i+startBtmIdx+1;
		  }

		  // 侧面
		  const int startSideIdx = startBtmIdx + m_wsegments;

		  for ( int y = 0; y < m_hsegments-1; ++y )
		  {
			  for ( int x = 0; x < m_wsegments-1; ++x )
			  {
				  int rb = y * m_wsegments + x + startSideIdx;
				  int lb = rb + 1;
				  int rt = rb + m_wsegments;
				  int lt = lb + m_wsegments;

				  *(iv++) = rt;
				  *(iv++) = lt;
				  *(iv++) = lb;

				  *(iv++) = lb;
				  *(iv++) = rb;
				  *(iv++) = rt; 
			  }
		  }

		  Mesh* mesh = static_cast<Mesh*>(res);
		  SubMesh* subMesh = mesh->createNewSubMesh(0);
		  subMesh->setPrimType(PRIM_TRIANGLELIST);

		  subMesh->setVertexType(MASK_P3N3U2);
		  subMesh->setVertexCount(vtxCnt);

		  subMesh->setIdxType(IDX_16);
		  subMesh->setIndexCount(idxCnt);

		  subMesh->setVertexData(vts);
		  subMesh->setIndexData(ids);
	

		  ENN_FREE(vts);
		  ENN_FREE(ids);

		  mesh->updateAABB( true );

		  return true;
	  }

private:
	void _makeMatrix( Matrix4& matAdjPos, Matrix3& matAdjNormal )
	{
		// 绕x轴转90
		Quaternion q;
		q.fromAngleAxis( Math::HALF_PI, Vector3::UNIT_X );
		q.toRotationMatrix( matAdjNormal );

		// z轴方向平移-height
		Matrix4 matTrans;
		matTrans.makeTrans( 0, 0, -m_height );

		// 组合
		matAdjPos = matTrans * Matrix4(matAdjNormal);
	}

	void _convert( VertexP3N3U2* pts, int cnt )
	{
		Matrix4 matAdjPos;
		Matrix3 matAdjNormal;
		_makeMatrix( matAdjPos, matAdjNormal );

		for ( int i = 0; i < cnt; ++i )
		{
			VertexP3N3U2& pt = pts[i];
			Vector3& pos  = *(Vector3*)&pt.x;
			Vector3& norm = *(Vector3*)&pt.nx;

			pos = matAdjPos.transformAffine( pos );
			norm = matAdjNormal * norm;
		}
	}

private:
	float m_width;
	float m_height;
	int   m_wsegments;
	int   m_hsegments;
	bool  m_useNZDir;
};

}