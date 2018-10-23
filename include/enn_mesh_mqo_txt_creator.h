/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/30
* File: enn_mesh_mqo_txt_creator.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_mesh_manager.h"
#include "enn_math.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// from https://code.google.com/p/glmetaseq/, by a japanese guy.
#define MQO_SIZE_STR			(256)
#define MQO_MAX_OBJECT			(50)
class MeshMqoTxtCreatorImpl : public ResAutoCreatorBase
{
	struct MQO_MATDATA
	{
		Color		col;				
		float		dif[4];				
		float		amb[4];				
		float		emi[4];				
		float		spc[4];				
		float		power;				
		int			useTex;				
		char		texFile[MQO_SIZE_STR];	
		char		alpFile[MQO_SIZE_STR];	
		uint		texName;	
	};

	struct MQO_FACE
	{
		int			n;		
		int			m;		
		int			v[4];	
		vec2f		uv[4];	
	};

	struct MQO_OBJDATA
	{
		char		objname[MQO_SIZE_STR];	
		int			visible;			
		int			shading;			
		float		facet;				
		int			n_face;				
		int			n_vertex;			
		MQO_FACE	*F;					
		vec3f		*V;					
	};

public:
	MeshMqoTxtCreatorImpl(const String& sub_dir);
	~MeshMqoTxtCreatorImpl();

public:
	void setMqoSubDir(const String& sub_dir)
	{
		mqo_sub_dir_ = sub_dir;
	}

	const String& getMqoSubDDir() const
	{
		return mqo_sub_dir_;
	}

public:
	virtual bool buildResource( Resource* res );

protected:
	virtual bool prepareResourceImpl( Resource* res, MemoryBuffer* buff );

protected:
	void read_mtr(MemoryBuffer* buff, MQO_MATDATA M[]);
	void read_obj(MemoryBuffer* buff, MQO_OBJDATA* obj);
	void read_vertex(MemoryBuffer* buff, vec3f V[]);
	void read_faces(MemoryBuffer* buff, MQO_FACE F[]);

protected:
	enn::vector<VertexP3N3U2>::type temp_vb_;
	enn::vector<ushort>::type temp_ib_;
	String mqo_sub_dir_;
};

}