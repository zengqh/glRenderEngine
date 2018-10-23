//
// Copyright (c) 2013 Pateo Software Ltd
//
// This source file is part of Pateo gis engine.
// For latest info, see http://www.pateo.com.cn/
//
// Time: 2013/10/15
// File: enn_post_bloom.cpp
//

#include "enn_post_bloom.h"
#include "enn_root.h"

namespace enn
{

PostBloom::PostBloom()
: is_prepared_(false)
, render_eff_v_blur_(0)
, render_eff_h_blur_(0)
, downsample_eff_(0)
, combine_sample_eff_(0)
, temp_rtt_(0)
, bright_eff_(0)
, curr_cam_(0)
{
	
}


PostBloom::~PostBloom()
{

}

void PostBloom::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	if (!temp_rtt_)
	{
		temp_rtt_ = RenderTarget::createInstance();
	}

	// render effect
	render_eff_h_blur_ = render_eff_v_blur_ = getRenderEffect("postprocess/pass_through_vs.h",
		"postprocess/blur.h");
	bright_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/bright_pass.h");
	downsample_eff_ = getRenderEffect("uirect_eff_vs.h", "uirect_eff_ps.h");
	combine_sample_eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/combine_add.h");

	prepareMesh();

	// weight
	blur_weight_[0] = 0.002216f;
	blur_weight_[1] = 0.008764f;
	blur_weight_[2] = 0.026995f;
	blur_weight_[3] = 0.064759f;
	blur_weight_[4] = 0.120985f;
	blur_weight_[5] = 0.176033f;
	blur_weight_[6] = 0.199471f;
	blur_weight_[7] = 0.176033f;
	blur_weight_[8] = 0.120985f;
	blur_weight_[9] = 0.064759f;
	blur_weight_[10] = 0.026995f;
	blur_weight_[11] = 0.008764f;
	blur_weight_[12] = 0.002216f;

	float bias = 4.0f;

	// v kernel
	v_texel_kernel_[0] = vec2f(0.0f, -6.0f * bias);
	v_texel_kernel_[1] = vec2f(0.0f, -5.0f * bias);
	v_texel_kernel_[2] = vec2f(0.0f, -4.0f * bias);
	v_texel_kernel_[3] = vec2f(0.0f, -3.0f * bias);
	v_texel_kernel_[4] = vec2f(0.0f, -2.0f * bias);
	v_texel_kernel_[5] = vec2f(0.0f, -1.0f * bias);
	v_texel_kernel_[6] = vec2f(0.0f, 0.0f * bias);
	v_texel_kernel_[7] = vec2f(0.0f, 1.0f * bias);
	v_texel_kernel_[8] = vec2f(0.0f, 2.0f * bias);
	v_texel_kernel_[9] = vec2f(0.0f, 3.0f * bias);
	v_texel_kernel_[10] = vec2f(0.0f, 4.0f * bias);
	v_texel_kernel_[11] = vec2f(0.0f, 5.0f * bias);
	v_texel_kernel_[12] = vec2f(0.0f, 6.0f * bias);

	// h texel kernel
	h_texel_kernel_[0] = vec2f(-6.0f * bias, 0.0f);
	h_texel_kernel_[1] = vec2f(-5.0f * bias, 0.0f);
	h_texel_kernel_[2] = vec2f(-4.0f * bias, 0.0f);
	h_texel_kernel_[3] = vec2f(-3.0f * bias, 0.0f);
	h_texel_kernel_[4] = vec2f(-2.0f * bias, 0.0f);
	h_texel_kernel_[5] = vec2f(-1.0f * bias, 0.0f);
	h_texel_kernel_[6] = vec2f(0.0f * bias, 0.0f);
	h_texel_kernel_[7] = vec2f(1.0f * bias, 0.0f);
	h_texel_kernel_[8] = vec2f(2.0f * bias, 0.0f);
	h_texel_kernel_[9] = vec2f(3.0f * bias, 0.0f);
	h_texel_kernel_[10] = vec2f(4.0f * bias, 0.0f);
	h_texel_kernel_[11] = vec2f(5.0f * bias, 0.0f);
	h_texel_kernel_[12] = vec2f(6.0f * bias, 0.0f);

	is_prepared_ = true;
}

void PostBloom::render(RenderTarget* dest, RenderTarget* src, Camera* cam)
{
	src_rtt_ = src;
	dest_rtt_ = dest;
	curr_cam_ = cam;

	// prepare down 4 mesh
	prepareDown4Mesh();

	checkRTT(cam->getViewport());

	// 1. down sample
	// src->dest
	downSample();

	// 2. bright pass
	// dest->temp
	brightSample();
	
	// 3. blur
	// temp->dest
	// dest->temp
	blurSample();

	// 4. combine
	// src + temp
	combineSample();
}

void PostBloom::checkRTT(Viewport* vp)
{
	// create 8 render target.
	int need_w, need_h;
	need_w = vp->getWidth();
	need_h = vp->getHeight();

	int w, h;
	w = rtt_view_port_.getWidth();
	h = rtt_view_port_.getHeight();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

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
	}
}

void PostBloom::downSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(downsample_eff_);

	RenderTarget* rtt = dest_rtt_;
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

	TextureObj* tex_obj = src_rtt_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	downsample_eff_->setInt( "texDiff", 0 );
	downsample_eff_->setFloat("isSwapV", 1.0f);

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

void PostBloom::brightSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(bright_eff_);

	RenderTarget* rtt = temp_rtt_;

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

	TextureObj* tex_obj = dest_rtt_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	bright_eff_->setInt( "SourceColor", 0 );
	bright_eff_->setFloat("Luminance", 0.08f);
	bright_eff_->setFloat("MiddleGray", 0.18f);
	bright_eff_->setFloat("WhiteCutoff", 0.8f);
	bright_eff_->setFloat("EffectWeight", 1.0f);
	bright_eff_->setFloat("isSwapV", 1.0f);

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

void PostBloom::blurSample()
{
	hblurSample();
	vblurSample();
}

void PostBloom::combineSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(combine_sample_eff_);

	RenderTarget* rtt = dest_rtt_;

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

	TextureObj* tex_obj = temp_rtt_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	TextureObj* src_obj = src_rtt_->get_rtt(0);
	rs->setTexture(1, src_obj);

	combine_sample_eff_->setInt( "SourceColor", 0 );
	combine_sample_eff_->setInt( "SceneColor", 1 );
	combine_sample_eff_->setFloat("isSwapV", 1.0f);

	combine_sample_eff_->setFloat("EffectWeight", 1.0f);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/5.bmp");
	}
}

void PostBloom::hblurSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(render_eff_h_blur_);

	RenderTarget* rtt = dest_rtt_;

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

	TextureObj* tex_obj = temp_rtt_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	render_eff_h_blur_->setInt( "SourceColor", 0 );
	render_eff_h_blur_->setFloat("isSwapV", 1.0f);
	render_eff_h_blur_->setFloatVectorArray("TexelKernel", h_texel_kernel_[0].ptr(), 2, 13);
	render_eff_h_blur_->setFloatArray("BlurWeight", blur_weight_, 13);

	float blur_size = 1.0f / rtt->getWidth();
	render_eff_h_blur_->setFloat("BlurSize", blur_size);

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

void PostBloom::vblurSample()
{
	if (!is_prepared_)
	{
		return;
	}

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(render_eff_v_blur_);

	RenderTarget* rtt = temp_rtt_;

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

	TextureObj* tex_obj = dest_rtt_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	render_eff_h_blur_->setInt( "SourceColor", 0 );
	render_eff_h_blur_->setFloat("isSwapV", 1.0f);
	render_eff_h_blur_->setFloatVectorArray("TexelKernel", v_texel_kernel_[0].ptr(), 2, 13);
	render_eff_h_blur_->setFloatArray("BlurWeight", blur_weight_, 13);

	float blur_size = 1.0f / rtt->getHeight();
	render_eff_h_blur_->setFloat("BlurSize", blur_size);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	if (0)
	{
		rtt->get_rtt(0)->saveFile("e:/4.bmp");
	}
}

Program* PostBloom::getRenderEffect(const String& vs_file, const String& ps_file)
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

String PostBloom::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

void PostBloom::prepareMesh()
{
	if (screen_mesh_)
	{
		return;
	}

	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();

	MeshPtr full_screen_mesh(mesh_mgr->getFullScreenMesh());

	screen_mesh_ = full_screen_mesh;
}

void PostBloom::prepareDown4Mesh()
{
	if (down4_screen_mesh_)
	{
		return;
	}

	int vw = src_rtt_->getWidth();
	int vh = src_rtt_->getHeight();

	int w = vw / 4;
	int h = vh / 4;
	int x = 3 * vw / 8;
	int y = 3 * vh / 8;

	ScreenRectDesc rect_desc(x, y, w, h, vw, vh, true);

	MeshManager* mesh_mgr = ROOTPTR->get_mesh_mgr();
	down4_screen_mesh_.attach(mesh_mgr->createScreenMesh("", rect_desc));
	down4_screen_mesh_->load(false);

	ScreenRectDesc rect_desc2(x, y, w, h, vw, vh, false);
	down4_not_full_mesh_.attach(mesh_mgr->createScreenMesh("", rect_desc2));
	down4_not_full_mesh_->load(false);
}

}