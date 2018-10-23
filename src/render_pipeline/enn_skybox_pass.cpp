//
// Copyright (c) 2013-2014 the enn project.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// 
// Time: 2013/08/21
// File: enn_skybox_pass.cpp
//

#include "enn_skybox_pass.h"
#include "enn_camera_node.h"
#include "enn_gl_program.h"
#include "enn_material.h"
#include "enn_mesh.h"
#include "enn_root.h"
#include "enn_gl_render_system.h"
#include "enn_render_pipe.h"

namespace enn
{

SkyboxPass::SkyboxPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, render_eff_(0)
, sky_scene_data_(0)
, sky_box_mesh_(0)
, sky_box_valid_(false)
, is_prepared_(false)
{

}

SkyboxPass::~SkyboxPass()
{
	ENN_SAFE_RELEASE(render_eff_);

	for (int i = 0; i < 6; ++i)
	{
		ENN_SAFE_DELETE(sky_tex_[i]);
	}
}

void SkyboxPass::prepare()
{
	if (!sky_box_valid_ || is_prepared_)
	{
		return;
	}

	prepareSkybox();

	is_prepared_ = true;
}

void SkyboxPass::render(Camera* cam)
{
	if (!sky_box_valid_ || !is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	/** disable z writing */
	rs->enableZWrite(false);

	/** set global matrix */
	const Matrix4& cam_world_matrix = cam->getWorldMatrix();

	Vector3 trans = cam_world_matrix.getTrans();

	Matrix4 world_matrix;
	world_matrix.makeTrans(trans);

	const Matrix4& view_matrix = cam->getViewMatrix();
	const Matrix4& proj_matrix = cam->getProjMatrix();

	Matrix4 wvp = proj_matrix * view_matrix * world_matrix;
	wvp = wvp.transpose();

	rs->enableCullFace(false);
	rs->setProgram(render_eff_);
	render_eff_->setMatrix4("matWVP", wvp[0]);

	const Mesh::SubMeshList& sub_mesh_list = sky_box_mesh_->getSubMeshList();

	ENN_FOR_EACH_CONST(Mesh::SubMeshList, sub_mesh_list, it)
	{
		SubMesh* sub_mesh = const_cast<SubMesh*>(it->second);

		int iface = std::distance(sub_mesh_list.begin(), it);

		renderFace(iface, sub_mesh);
	}

	rs->enableZWrite(true);
}

void SkyboxPass::renderFace(int i, SubMesh* sub_mesh)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	VertexBuffer* vb = sub_mesh->getVB();
	IndexBuffer* ib = sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	/** set mtr */
	applyFaceMtr(i);
	render_eff_->setInt( "texDiff", 0 );

	uint32 element_mask = sub_mesh->getVertexType();

	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(sub_mesh->getPrimType(), 0, sub_mesh->getIndexCount(), 
		sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);
}

void SkyboxPass::applyFaceMtr(int i)
{
	TextureUnit* tu = sky_tex_[i];

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	TextureManager* tm = Root::getSingletonPtr()->get_texture_mgr();

	TextureObj* obj = 0;

	Texture* tex_res = tu->getTex();
	if (tex_res->checkLoad())
	{
		obj = tex_res->getTextureObj();
	}
	else
	{
		obj = tm->getDefaultTexture()->getTextureObj();
	}

	rs->setTexture(0, obj);
}

Program* SkyboxPass::getRenderEffect()
{
	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	getFileData(base_path + "eff_comm_def.h", def_code);
	getFileData(base_path + "forward_eff_skybox_vs.h", vs_code);
	getFileData(base_path + "forward_eff_skybox_ps.h", ps_code);

	String str_vs = makeMacro(false) + def_code + vs_code;
	String str_ps = makeMacro(true) + def_code + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();

		return 0;
	}

	render_eff_ = pro;

	return pro;
}

void SkyboxPass::prepareSkybox()
{
	getRenderEffect();
	prepareMesh();
	prepareMtr();
}

void SkyboxPass::prepareMesh()
{
	static const float facesize = 50.0f;

	Mesh* sky_box_mesh = static_cast<Mesh*>(Root::getSingletonPtr()->get_mesh_mgr()->createResource("__skybox"));
	sky_box_mesh_.attach(sky_box_mesh);

	enn::vector<VertexP3U2>::type vts;

	vec3f vmin(-facesize, -facesize, -facesize);
	vec3f vmax(facesize,  facesize,  facesize);

	float vertex_list[][3] = 
	{
		vmin.x, vmin.y, vmin.z,
		//-0.5f, -0.5f, -0.5f, 
		vmax.x, vmin.y, vmin.z,
		//0.5f, -0.5f, -0.5f, 
		vmin.x, vmax.y, vmin.z,
		//-0.5f, 0.5f, -0.5f, 
		vmax.x, vmax.y, vmin.z,
		//0.5f, 0.5f, -0.5f, 
		vmin.x, vmin.y, vmax.z,
		//-0.5f, -0.5f, 0.5f,
		vmax.x, vmin.y, vmax.z,
		//0.5f, -0.5f, 0.5f, 
		vmin.x, vmax.y, vmax.z,
		//-0.5f, 0.5f, 0.5f,
		vmax.x, vmax.y, vmax.z,
		//0.5f, 0.5f, 0.5f, 
	};

	ushort index_list[6][4] =
	{ 
		0, 1, 3, 2, // 后面
		4, 0, 2, 6, // 左面
		1, 5, 7, 3, // 右面
		5, 4, 6, 7, // 前面
		2, 3, 7, 6, // 上面
		4, 5, 1, 0  // 下面
	};

	float tex_list[6][4][2] =
	{
		0,1, 1,1, 1,0, 0,0, // 后面
		0,1, 1,1, 1,0, 0,0, // 左面
		0,1, 1,1, 1,0, 0,0, // 右面
		0,1, 1,1, 1,0, 0,0, // 前面
		0,1, 1,1, 1,0, 0,0, // 上面
		0,1, 1,1, 1,0, 0,0  // 下面
	};

	ushort ids[36] =
	{
		0, 1, 2, 2, 3, 0
	};

	for (int i = 0; i < 6; ++i)
	{
		SubMesh* sub_mesh = sky_box_mesh_->createNewSubMesh(i);

		VertexP3U2 v;
		for (int j = 0; j < 4; ++j)
		{
			v.x = vertex_list[index_list[i][j]][0];
			v.y = vertex_list[index_list[i][j]][1];
			v.z = vertex_list[index_list[i][j]][2];

			v.u = tex_list[i][j][0];
			v.v = tex_list[i][j][1];

			vts.push_back(v);
		}


		sub_mesh->setPrimType(PRIM_TRIANGLELIST);
		sub_mesh->setVertexType(MASK_P3U2);
		sub_mesh->setVertexCount(vts.size());

		sub_mesh->setIdxType(IDX_16);
		sub_mesh->setIndexCount(ENN_ARRAY_SIZE(ids));

		sub_mesh->setVertexData(&vts[0]);
		sub_mesh->setIndexData(&ids[0]);

		sub_mesh->makeGPUBuffer(true);

		vts.clear();
	}
}

void SkyboxPass::prepareMtr()
{
	String skybox_tex_list[6] =
	{
		sky_scene_data_->cubemap_tex_[FACE_NEGATIVE_Z],
		sky_scene_data_->cubemap_tex_[FACE_NEGATIVE_X],
		sky_scene_data_->cubemap_tex_[FACE_POSITIVE_X],
		sky_scene_data_->cubemap_tex_[FACE_POSITIVE_Z],
		sky_scene_data_->cubemap_tex_[FACE_POSITIVE_Y],
		sky_scene_data_->cubemap_tex_[FACE_NEGATIVE_Y]
	};

	for (int i = 0; i < 6; ++i)
	{
		TextureUnit* tu = new TextureUnit();

		tu->setTexAddr(TEXADDR_CLAMP, TEXADDR_CLAMP);
		tu->setTexFilter(TEXF_LINEAR, TEXF_LINEAR);

		tu->setTexName(skybox_tex_list[i]);

		sky_tex_[i] = tu;
	}
}

String SkyboxPass::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

bool SkyboxPass::getFileData(const String& file_name, String& data)
{
	data.clear();

	FILE* fp = NULL;

	fp = fopen( file_name.c_str(), "rb" );

	if ( !fp )
	{
		return false;
	}

	fseek( fp, 0, SEEK_END );
	size_t len = ftell( fp );
	fseek( fp, 0, SEEK_SET );

	data.resize( len );
	fread( &data[0], 1, len, fp );
	fclose( fp );
	return true;
}

}