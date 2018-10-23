/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/30
* File: enn_mesh_mqo_txt_creator.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_mesh_mqo_txt_creator.h"
#include "enn_root.h"

namespace enn
{
MeshMqoTxtCreatorImpl::MeshMqoTxtCreatorImpl(const String& sub_dir)
: temp_vb_(0)
, temp_ib_(0)
{
	setMqoSubDir(sub_dir);

	m_basePath = "mesh";
}

MeshMqoTxtCreatorImpl::~MeshMqoTxtCreatorImpl()
{
	
}

bool MeshMqoTxtCreatorImpl::buildResource( Resource* res )
{
	Mesh* mesh = static_cast<Mesh*>(res);
	mesh->makeGPUBuffer();

	return true;
}

bool MeshMqoTxtCreatorImpl::prepareResourceImpl( Resource* res, MemoryBuffer* buff )
{
	int				n_mat = 0;
	MQO_MATDATA		*M = NULL;
	MQO_OBJDATA		obj[MQO_MAX_OBJECT];
	int				n_obj = 0;

	bool is_async_load = res->getLoadAsync();

	String fileName = getLocateFileName( res );
	FileSystem* file_system = Root::getSingletonPtr()->get_file_system();

	if ( !file_system->getFileData( fileName, buff ) )
	{
		ENN_ASSERT(0);
		return false;
	}


	memset(obj,0,sizeof(obj));

	while (!buff->IsEof())
	{
		String curr_line = buff->ReadLine();

		const char* buf = curr_line.c_str();

		if (strstr(curr_line.c_str(), "Material"))
		{
			sscanf(buf,"Material %d", &n_mat);

			M = (MQO_MATDATA *)ENN_MALLOC_ARRAY_T(MQO_MATDATA, n_mat);

			read_mtr(buff, M);
		}

		if (strstr(curr_line.c_str(), "Object"))
		{
			sscanf(curr_line.c_str(),"Object %s", obj[n_obj].objname);
			read_obj(buff, &obj[n_obj]);

			n_obj++;

		}
	}

	ENN_ASSERT(n_mat > 0);
	Material* mtr = ENN_NEW Material;

	/** setup material */
	for (int i = 0; i < n_mat; ++i)
	{
		SubMaterial* sub_mtr = ENN_NEW SubMaterial(mtr);

		if (M[i].useTex)
		{
			TextureUnit* tu = sub_mtr->useTexDiff();
			
			tu->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
			tu->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
			tu->setNeedAsync(is_async_load);
			tu->setTexName(mqo_sub_dir_ + "/" + M[i].texFile);
		}

		sub_mtr->setCullMode(CULL_NONE);

		sub_mtr->setDiffuse(Color(M[i].dif[0], M[i].dif[1], M[i].dif[2], M[i].dif[3]));

		sub_mtr->setSpecularShiness(Color(M[i].spc[0], M[i].spc[1], M[i].spc[2], M[i].power));
		sub_mtr->setEmissive(Color(M[i].emi[0], M[i].emi[1], M[i].emi[2], M[i].emi[3]));
		sub_mtr->enableLighting(true);
		mtr->addSubMaterial(i, sub_mtr);
	}

	/** setup mesh */
	ENN_ASSERT(n_obj > 0);

	Mesh* mesh = static_cast<Mesh*>(res);

	temp_vb_.clear();
	temp_ib_.clear();

	enn::vector<VertexP3N3U2>::type& vb = temp_vb_;
	enn::vector<ushort>::type& ib = temp_ib_;

	VertexP3N3U2 temp_v;
	
	for (int i = 0; i < n_obj; ++i)
	{
		MQO_FACE* F = obj[i].F;

		vec3f* V = obj[i].V;

		int n_face = obj[i].n_face;

		/** prepare vb */
		for (int im = 0; im < mtr->getSubMtrCount(); ++im)
		{
			ushort curr_idx = 0;
			for (int nf = 0; nf < n_face; ++nf)
			{
				if (F[nf].m != im)
				{
					continue;
				}

				if (F[nf].n == 3)
				{
					for (int iv = 0; iv < 3; ++iv)
					{
						temp_v.x = V[F[nf].v[iv]].x;
						temp_v.y = V[F[nf].v[iv]].y;
						temp_v.z = V[F[nf].v[iv]].z;

						/** calc normal later */
						temp_v.nx = 0.0f;
						temp_v.ny = 1.0f;
						temp_v.nz = 0.0f;

						/** uv */
						temp_v.u = F[nf].uv[iv].x;
						temp_v.v = F[nf].uv[iv].y;

						vb.push_back(temp_v);
						ib.push_back(curr_idx++);
					}
				}
				else if (F[nf].n == 4)
				{
					/** 0 1 2 */
					for (int iv = 0; iv < 4; ++iv)
					{
						if (iv == 3)
						{
							continue;
						}

						temp_v.x = V[F[nf].v[iv]].x;
						temp_v.y = V[F[nf].v[iv]].y;
						temp_v.z = V[F[nf].v[iv]].z;

						/** calc normal later */
						temp_v.nx = 0.0f;
						temp_v.ny = 1.0f;
						temp_v.nz = 0.0f;

						/** uv */
						temp_v.u = F[nf].uv[iv].x;
						temp_v.v = F[nf].uv[iv].y;

						vb.push_back(temp_v);
						ib.push_back(curr_idx++);
					}

					/** 0 2 3 */
					for (int iv = 0; iv < 4; ++iv)
					{
						if (iv == 1)
						{
							continue;
						}

						temp_v.x = V[F[nf].v[iv]].x;
						temp_v.y = V[F[nf].v[iv]].y;
						temp_v.z = V[F[nf].v[iv]].z;

						/** calc normal later */
						temp_v.nx = 0.0f;
						temp_v.ny = 1.0f;
						temp_v.nz = 0.0f;

						/** uv */
						temp_v.u = F[nf].uv[iv].x;
						temp_v.v = F[nf].uv[iv].y;

						vb.push_back(temp_v);
						ib.push_back(curr_idx++);
					}
				}
			}

			SubMesh* sub_mesh = mesh->createNewSubMesh(im);

			sub_mesh->setPrimType(PRIM_TRIANGLELIST);

			sub_mesh->setVertexType(MASK_P3N3U2);

			sub_mesh->setVertexCount(vb.size());

			sub_mesh->setIdxType(IDX_16);
			sub_mesh->setIndexCount(ib.size());

			sub_mesh->setVertexData(&vb[0]);
			sub_mesh->setIndexData(&ib[0]);

			sub_mesh->setMatIdx(im);

			vb.clear();
			ib.clear();
		}
	}
	
	mesh->setMaterial(mtr);
	mesh->calcNormal();
	mesh->updateAABB(true);

	for (int i = 0; i < n_obj; ++i)
	{
		ENN_SAFE_FREE(obj[i].V);
		ENN_SAFE_FREE(obj[i].F);
	}

	ENN_SAFE_FREE(M);

	file_system->freeFileData(buff);
	return true;
}

void MeshMqoTxtCreatorImpl::read_mtr(MemoryBuffer* buff, MQO_MATDATA M[])
{
	float		dif, amb, emi, spc;
	Color		c;
	char		*pStrEnd, *pStr;
	int			len;
	int			i = 0;

	while (true)
	{
		String line = buff->ReadLine();

		char* buf = const_cast<char*>(line.c_str());

		if (strstr(buf,"}"))
		{
			break;
		}

		pStr = strstr(buf,"col(");

		sscanf( pStr,
			"col(%f %f %f %f) dif (%f) amb(%f) emi(%f) spc(%f) power(%f)",
			&c.r, &c.g, &c.b, &c.a, &dif, &amb, &emi, &spc, &M[i].power );

		M[i].col = c;
		M[i].dif[0] = dif * c.r;
		M[i].dif[1] = dif * c.g;
		M[i].dif[2] = dif * c.b;
		M[i].dif[3] = c.a;

		M[i].amb[0] = amb * c.r;
		M[i].amb[1] = amb * c.g;
		M[i].amb[2] = amb * c.b;
		M[i].amb[3] = c.a;

		M[i].emi[0] = emi * c.r;
		M[i].emi[1] = emi * c.g;
		M[i].emi[2] = emi * c.b;
		M[i].emi[3] = c.a;

		M[i].spc[0] = spc * c.r;
		M[i].spc[1] = spc * c.g;
		M[i].spc[2] = spc * c.b;
		M[i].spc[3] = c.a;

		if ( (pStr = strstr(buf,"tex(")) != NULL ) 
		{
			M[i].useTex = true;

			pStrEnd = strstr(pStr,")") - 1;

			len = pStrEnd - (pStr+5);

			strncpy(M[i].texFile,pStr+5,len);

			M[i].texFile[len] = (char)0;

			if ( (pStr = strstr(buf,"aplane(")) != NULL ) 
			{
				pStrEnd = strstr(pStr,")")-1;
				len = pStrEnd - (pStr+8);
				strncpy(M[i].alpFile,pStr+8,len);
				M[i].alpFile[len] = (char)0;
			}
			else
			{
				M[i].alpFile[0] = (char)0;
			}
		}
		else
		{
			M[i].useTex = false;
			M[i].texFile[0] = (char)0;
			M[i].alpFile[0] = (char)0;
		}

		i++;
	}
}

void MeshMqoTxtCreatorImpl::read_obj(MemoryBuffer* buff, MQO_OBJDATA* obj)
{
	while (true)
	{
		String line = buff->ReadLine();
		char* buf = const_cast<char*>(line.c_str());

		if (strstr(buf,"}"))
		{
			break;
		}

		// visible
		if (strstr(buf,"visible ")) 
		{
			sscanf(buf," visible %d", &obj->visible);
		}

		// shading
		if (strstr(buf,"shading ")) 
		{
			sscanf(buf," shading %d", &obj->shading);
		}

		// facet
		if (strstr(buf,"facet ")) 
		{
			sscanf(buf," facet %f", &obj->facet);
		}

		// vertex
		if (strstr(buf,"vertex ")) 
		{
			sscanf(buf," vertex %d", &obj->n_vertex);
			obj->V = (vec3f*) ENN_MALLOC_ARRAY_T(vec3f, obj->n_vertex);

			read_vertex(buff, obj->V);
		}

		// face
		if (strstr(buf,"face ")) 
		{
			sscanf(buf," face %d", &obj->n_face);
			obj->F = (MQO_FACE*) ENN_MALLOC_ARRAY_T(MQO_FACE, obj->n_face);
			read_faces(buff, obj->F);
		}

	}
}

void MeshMqoTxtCreatorImpl::read_vertex(MemoryBuffer* buff, vec3f V[])
{
	int  i=0;

	while (true) 
	{
		String line = buff->ReadLine();

		const char* buf = line.c_str();

		if (strstr(buf,"}"))
		{
			break;
		}

		sscanf(buf,"%f %f %f",&V[i].x,&V[i].y,&V[i].z);
		i++;
	}
}

void MeshMqoTxtCreatorImpl::read_faces(MemoryBuffer* buff, MQO_FACE F[])
{
	char *pStr = 0;
	int  i=0;

	while (true)
	{
		String line = buff->ReadLine();

		char* buf = const_cast<char*>(line.c_str());

		if (strstr(buf,"}")) 
		{
			break;
		}

		sscanf(buf,"%d",&F[i].n);

		if ( (pStr = strstr(buf,"V(")) != NULL ) 
		{
			switch (F[i].n) 
			{
				case 3:
					sscanf(pStr,"V(%d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2]);
					break;
				case 4:
					sscanf(pStr,"V(%d %d %d %d)",&F[i].v[0],&F[i].v[1],&F[i].v[2],&F[i].v[3]);
					break;
				default:
					break;
			}
		}

		F[i].m = 0;

		if ( (pStr = strstr(buf,"M(")) != NULL ) 
		{
			sscanf(pStr,"M(%d)",&F[i].m);
		}
		else
		{
			F[i].m = -1;
		}

		if ( (pStr = strstr(buf,"UV(")) != NULL ) 
		{
			switch (F[i].n) 
			{
			case 3:
				sscanf(pStr,"UV(%f %f %f %f %f %f)",
					&F[i].uv[0].x, &F[i].uv[0].y,
					&F[i].uv[1].x, &F[i].uv[1].y,
					&F[i].uv[2].x, &F[i].uv[2].y
					);

				break;
			case 4:
				sscanf(pStr,"UV(%f %f %f %f %f %f %f %f)",
					&F[i].uv[0].x, &F[i].uv[0].y,
					&F[i].uv[1].x, &F[i].uv[1].y,
					&F[i].uv[2].x, &F[i].uv[2].y,
					&F[i].uv[3].x, &F[i].uv[3].y
					);

				break;
			default:
				break;
			}
		}

		i++;
	}
}

}