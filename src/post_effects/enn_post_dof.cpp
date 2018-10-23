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
// Time: 2013/11/07
// File: enn_post_dof.cpp
//

#include "enn_post_dof.h"
#include "enn_root.h"

namespace enn
{

PostDOF::PostDOF()
: curr_cam_(0)
, rtt_src_(0)
, rtt_dst_(0)
, depth_blur_(0)
, is_prepared_(false)
, near_depth_plane_(0.5f)
, focal_depth_plane_(10.0f)
, far_depth_plane_(20.0f)
, blurriness_cutoff_(0.8f)
, blur_down_scale_(4.0f)
, downsample_rtt0_(0)
, downsample_rtt1_(0)
, downscale_eff_(0)
, blur_eff_(0)
{
	
}

PostDOF::~PostDOF()
{
	
}

void PostDOF::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	prepareMesh();
	prepareEffect();

	is_prepared_ = true;
}

void PostDOF::render(RenderTarget* dest, RenderTarget* src, Camera* cam)
{
	rtt_src_ = src;
	rtt_dst_ = dest;
	curr_cam_ = cam;

	checkRTT(cam->getViewport());

	calculateDepthBlur();
	calcuateBlur();
	blurHSample();
	blurVSample();
	blurHSample();
	blurVSample();
	doResult();
}

void PostDOF::checkRTT(Viewport* vp)
{
	int need_w, need_h;
	need_w = vp->getWidth() / 2;
	need_h = vp->getHeight() / 2;

	int w, h;
	w = downscale_vp_.getWidth();
	h = downscale_vp_.getHeight();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	if (!downsample_rtt0_)
	{
		downsample_rtt0_ = RenderTarget::createInstance();
	}

	if (!downsample_rtt1_)
	{
		downsample_rtt1_ = RenderTarget::createInstance();
	}

	if (!(w == need_w && h == need_h))
	{
		TextureObj* temp_color0 = downsample_rtt0_->get_rtt(0);
		TextureObj* temp_color1 = downsample_rtt1_->get_rtt(0);

		if (temp_color0)
		{
			rs->getRttPool()->destroyRTTBuffer(temp_color0);
		}

		if (temp_color1)
		{
			rs->getRttPool()->destroyRTTBuffer(temp_color1);
		}

		temp_color0 = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);
		temp_color1 = rs->getRttPool()->getRTTBuffer(U4_RGBA, need_w, need_h);

		RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, need_w, need_h);

		downsample_rtt0_->link_rtt(0, temp_color0);
		downsample_rtt0_->link_depth(depth_temp->depth_);

		downsample_rtt1_->link_rtt(0, temp_color1);
		downsample_rtt1_->link_depth(depth_temp->depth_);

		downscale_vp_.setRect(0, 0, need_w, need_h);
		downscale_vp_.setClearColor(Color::BLACK);
	}
}

void PostDOF::prepareEffect()
{
	depth_blur_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/depth_blur.h");
	downscale_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/dof_downscale.h");
	blur_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/dof_blur.h");
	result_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/dof_result.h");
}

void PostDOF::prepareMesh()
{
	if (screen_mesh_)
	{
		return;
	}

	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();

	MeshPtr full_screen_mesh(mesh_mgr->getFullScreenMesh());

	screen_mesh_ = full_screen_mesh;
}

void PostDOF::calculateDepthBlur()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(depth_blur_);

	RenderTarget* rtt = rtt_dst_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLACK);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = rtt_src_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	TextureObj* depth_tex_obj = ROOTPTR->getRenderPipe()->getGBufferPass()->getColor1();
	rs->setTexture(1, depth_tex_obj);

	if (0)
	{
		depth_tex_obj->saveFile("e:/-2.bmp");
	}

	float zFar = curr_cam_->getZFar();

	vec4f DOFParams;

	DOFParams.x = near_depth_plane_;
	DOFParams.y = focal_depth_plane_;
	DOFParams.z = far_depth_plane_;
	DOFParams.w = blurriness_cutoff_;

	depth_blur_->setInt("RGBTexture", 0);
	depth_blur_->setInt("DepthTexture", 1);
	depth_blur_->setFloat("isSwapV", 1.0f);
	depth_blur_->setFloat("zFar", zFar);
	depth_blur_->setFloatVector("DOFParams", DOFParams.ptr(), 4);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/-1.bmp");
	}
}

void PostDOF::calcuateBlur()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(downscale_eff_);

	RenderTarget* rtt = downsample_rtt0_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLACK);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	vec2f invRTTSize;
	invRTTSize.x = 1.0f / rtt_src_->getWidth();
	invRTTSize.y = 1.0f / rtt_src_->getHeight();

	TextureObj* tex_obj = rtt_src_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	downscale_eff_->setInt("Texture", 0);
	downscale_eff_->setFloatVector("invRTTSize", invRTTSize.ptr(), 2);
	downscale_eff_->setFloat("isSwapV", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/0.bmp");
	}
}

void PostDOF::blurHSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(blur_eff_);

	RenderTarget* rtt = downsample_rtt1_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLACK);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = downsample_rtt0_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	vec2f blurSize(1.0f / rtt->getWidth(), 1.0f / rtt->getHeight());
	vec2f uvScale(1.0f, 0.0f);

	blur_eff_->setInt("SourceColor", 0);
	blur_eff_->setFloatVector("BlurSize", blurSize.ptr(), 2);
	blur_eff_->setFloatVector("UVScale", uvScale.ptr(), 2);
	blur_eff_->setFloat("isSwapV", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/1.bmp");
	}
}

void PostDOF::blurVSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(blur_eff_);

	RenderTarget* rtt = downsample_rtt0_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLACK);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();
	

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = downsample_rtt1_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	vec2f blurSize(1.0f / rtt->getWidth(), 1.0f / rtt->getHeight());
	vec2f uvScale(0.0f, 1.0f);

	blur_eff_->setInt("SourceColor", 0);
	blur_eff_->setFloatVector("BlurSize", blurSize.ptr(), 2);
	blur_eff_->setFloatVector("UVScale", uvScale.ptr(), 2);
	blur_eff_->setFloat("isSwapV", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/2.bmp");
	}
}

void PostDOF::doResult()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(result_eff_);

	RenderTarget* rtt = rtt_src_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLACK);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = rtt_dst_->get_rtt(0);
	rs->setTexture(0, tex_obj);

	TextureObj* tex_obj1 = downsample_rtt0_->get_rtt(0);
	rs->setTexture(1, tex_obj1);

	vec2f pixelSizeHigh(1.0f / rtt->getWidth(), 1.0f / rtt->getHeight());
	vec2f pixelSizeLow(1.0f / downsample_rtt0_->getWidth(), 1.0f / downsample_rtt0_->getHeight());

	result_eff_->setInt("Texture", 0);
	result_eff_->setInt("BlurTexture", 1);
	result_eff_->setFloatVector("pixelSizeHigh", pixelSizeHigh.ptr(), 2);
	result_eff_->setFloatVector("pixelSizeLow", pixelSizeLow.ptr(), 2);
	result_eff_->setFloat("isSwapV", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/3.bmp");
	}
}

Program* PostDOF::getRenderEffect(const String& vs_file, const String& ps_file)
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

String PostDOF::makeMacro(bool is_ps)
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

}