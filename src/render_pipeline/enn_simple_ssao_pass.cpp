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
// Time: 2013/11/03
// File: enn_simple_ssao_pass.cpp
//

#include "enn_simple_ssao_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

SPSSAOPass::SPSSAOPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, is_prepared_(false)
, temp_rtt_(0)
, ssao_eff_(0)
, curr_cam_(0)
, gbuffer_pass_(0)
, normal_rtt_(0)
, depth_rtt_(0)
, random_normal_tex_(0)
{

}

SPSSAOPass::~SPSSAOPass()
{

}

void SPSSAOPass::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	gbuffer_pass_ = render_pipe_->getGBufferPass();
	normal_rtt_ = gbuffer_pass_->getColor0();
	depth_rtt_ = gbuffer_pass_->getColor1();

	random_normal_tex_ = ENN_NEW TextureUnit();
	random_normal_tex_->setTexName("ssao_rand.jpg");

	ssao_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "sp_ssao.h");

	prepareMesh();

	is_prepared_ = true;
}

void SPSSAOPass::render(Camera* cam)
{
	curr_cam_ = cam;

	checkRTT(cam->getViewport());

	doSSAO();
}

TextureObj* SPSSAOPass::getMapAOBuffer()
{
	return temp_rtt_->get_rtt(0);
}

Program* SPSSAOPass::getRenderEffect(const String& vs_file, const String& ps_file)
{
	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	shader_getFileData(base_path + "eff_comm_def.h", def_code);
	shader_getFileData(base_path + vs_file, vs_code);
	shader_getFileData(base_path + ps_file, ps_code);

	String str_vs = makeMacro(false) + def_code + vs_code;
	String str_ps = makeMacro(true) + def_code + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();

		return 0;
	}

	return pro;
}

String SPSSAOPass::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

	// OpenGL 2.1 ("#version 120")
	if (is_ps) strMacro += String("#version 120\n");

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

void SPSSAOPass::prepareMesh()
{
	if (screen_mesh_)
	{
		return;
	}

	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();

	MeshPtr full_screen_mesh(mesh_mgr->getFullScreenMesh());

	screen_mesh_ = full_screen_mesh;
}

void SPSSAOPass::checkRTT(Viewport* vp)
{
	// create 8 render target.
	int need_w, need_h;
	need_w = vp->getWidth();
	need_h = vp->getHeight();

	int w, h;
	w = rtt_view_port_.getWidth();
	h = rtt_view_port_.getHeight();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	if (!temp_rtt_)
	{
		temp_rtt_ = RenderTarget::createInstance();
	}

	if (!(w == need_w && h == need_h))
	{
		TextureObj* temp_color = temp_rtt_->get_rtt(0);

		if (temp_color)
		{
			rs->getRttPool()->destroyRTTBuffer(temp_color);
		}

		temp_color = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, need_w, need_h);

		temp_rtt_->link_rtt(0, temp_color);
		temp_rtt_->link_depth(depth_temp->depth_);

		rtt_view_port_ = *vp;
		rtt_view_port_.setClearColor(Color::BLACK);

		normal_rtt_ = gbuffer_pass_->getColor0();
		depth_rtt_ = gbuffer_pass_->getColor1();
	}
}

void SPSSAOPass::doSSAO()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(ssao_eff_);

	RenderTarget* rtt = temp_rtt_;
	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::WHITE);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	// begin set shader params
	ssao_eff_->setFloat("SampleRadius", 0.2f);
	ssao_eff_->setFloat("Intensity", 4.0f);
	ssao_eff_->setFloat("Scale", 10.0f);
	ssao_eff_->setFloat("Bias", 0.2f);

	// Focal length
	const int fWidth  = rtt_view_port_.getWidth();
	const int fHeight = rtt_view_port_.getHeight();

	const float fFovY         = curr_cam_->getFov();
	const float fFocalLengthY = 1.0f / Math::tan(fFovY * 0.5f);
	const float fFocalLengthX = fFocalLengthY;
	Vector2 InvFocalLen(1.0f / fFocalLengthX, 1.0f / fFocalLengthY);
	ssao_eff_->setFloatVector("InvFocalLen", InvFocalLen.ptr(), 2);

	const int nRandomNormalsTextureSize = 64;
	ssao_eff_->setInt("RandomSize", nRandomNormalsTextureSize);

	nv::vec2i resolution(fWidth, fHeight);
	ssao_eff_->setIntVector("Resolution", resolution._array, 2);

	ssao_eff_->setFloat("zFar", curr_cam_->getZFar());

	// set texture
	TextureObj* tex_obj0 = normal_rtt_;
	rs->setTexture(0, tex_obj0);

	TextureObj* tex_obj1 = depth_rtt_;
	rs->setTexture(1, tex_obj1);

	TextureObj* tex_obj2 = random_normal_tex_->getTex()->getTextureObj();
	rs->setTexture(2, tex_obj2);

	ssao_eff_->setInt( "NormalMap", 0);
	ssao_eff_->setInt( "DepthMap", 1);
	ssao_eff_->setInt( "RandomNormalsMap", 2);

	ssao_eff_->setFloat("isSwapV", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("d:/5.bmp");
	}
}

}