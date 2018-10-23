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
// Time: 2013/10/12
// File: enn_post_gaussian_blur.cpp
//

#include "enn_post_gaussian_blur.h"
#include "enn_root.h"

namespace enn
{

PostGaussianblur::PostGaussianblur()
: render_eff_v_blur_(0)
, render_eff_h_blur_(0)
, is_prepared_(false)
, curr_src_(0)
, curr_dest_(0)
, curr_cam_(0)
{

}

PostGaussianblur::~PostGaussianblur()
{

}

bool PostGaussianblur::is_post_enable() const
{
	return true;
}

void PostGaussianblur::prepare()
{
	if (!is_prepared_)
	{
		render_eff_h_blur_ = render_eff_v_blur_ = getRenderEffect("postprocess/pass_through_vs.h",
			"postprocess/blur.h");

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
	}

	is_prepared_ = true;
}

void PostGaussianblur::render(RenderTarget* dest, 
	RenderTarget* src, Camera* cam)
{
	curr_cam_ = cam;
	curr_src_ = src;
	curr_dest_ = dest;

	processHBlur();
	processVBlur();
}

String PostGaussianblur::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

void PostGaussianblur::prepareMesh()
{
	if (screen_mesh_)
	{
		return;
	}

	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();

	MeshPtr full_screen_mesh(mesh_mgr->getFullScreenMesh());

	screen_mesh_ = full_screen_mesh;

	is_prepared_ = true;
}

void PostGaussianblur::processHBlur()
{	
	if (!is_prepared_)
	{
		return;
	}
	
	curr_dest_->begin_render();
	Viewport* vp = curr_cam_->getViewport();
	vp->apply();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(render_eff_h_blur_);

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = curr_src_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	render_eff_h_blur_->setInt( "SourceColor", 0 );
	render_eff_h_blur_->setFloat("isSwapV", 1.0f);
	render_eff_h_blur_->setFloatVectorArray("TexelKernel", h_texel_kernel_[0].ptr(), 2, 13);
	render_eff_h_blur_->setFloatArray("BlurWeight", blur_weight_, 13);

	float blur_size = 1.0f / curr_src_->getWidth();
	render_eff_h_blur_->setFloat("BlurSize", blur_size);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	curr_dest_->end_render();
}

void PostGaussianblur::processVBlur()
{
	if (!is_prepared_)
	{
		return;
	}

	Viewport* vp = curr_cam_->getViewport();
	vp->apply();

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	rs->setProgram(render_eff_v_blur_);

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = curr_src_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	render_eff_h_blur_->setInt( "SourceColor", 0 );
	render_eff_h_blur_->setFloat("isSwapV", 1.0f);
	render_eff_h_blur_->setFloatVectorArray("TexelKernel", v_texel_kernel_[0].ptr(), 2, 13);
	render_eff_h_blur_->setFloatArray("BlurWeight", blur_weight_, 13);

	float blur_size = 1.0f / curr_src_->getHeight();
	render_eff_h_blur_->setFloat("BlurSize", blur_size);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);
}

Program* PostGaussianblur::getRenderEffect(const String& vs_file, const String& ps_file)
{
	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	getFileData(base_path + "eff_comm_def.h", def_code);
	getFileData(base_path + vs_file, vs_code);
	getFileData(base_path + ps_file, ps_code);

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

bool PostGaussianblur::getFileData(const String& file_name, String& data)
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