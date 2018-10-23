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
// Time: 2013/11/25
// File: enn_post_motion_blur.cpp
//

#include "enn_post_motion_blur.h"
#include "enn_root.h"

namespace enn
{

PostMotionBlur::PostMotionBlur()
: num_of_samples_(20)
, motion_blur_factor_(1.0f)
, target_fps_(60.0f)
, eff_(0)
, is_prepared_(false)
, depth_obj_(0)
, rtt_dst_(0)
, rtt_src_(0)
, curr_cam_(0)
, is_mat_first_(true)
{

}

PostMotionBlur::~PostMotionBlur()
{
	
}

void PostMotionBlur::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	// effect
	eff_ = getRenderEffect("postprocess/pass_through_vs.h", "postprocess/motion_blur.h");

	// screen mesh
	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();

	MeshPtr full_screen_mesh(mesh_mgr->getFullScreenMesh());

	screen_mesh_ = full_screen_mesh;

	is_prepared_ = true;
}

void PostMotionBlur::render(RenderTarget* dest, RenderTarget* src, Camera* cam)
{
	rtt_src_ = src;
	rtt_dst_ = dest;
	curr_cam_ = cam;

	RenderSystem* rs = RENDER_SYS;
	RenderTarget* rtt = rtt_dst_;

	rtt->begin_render();

	Viewport vp;
	vp.setClearColor(Color::BLUE);
	vp.makeRect(0, 0, rtt->getWidth(), rtt->getHeight());

	vp.apply();

	rs->setProgram(eff_);

	SubMesh* screen_sub_mesh = screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	// matrix
	const Matrix4& worldToClipMat = curr_cam_->getViewProjMatrix();
	Matrix4 invWorldToClipMat = worldToClipMat.inverse();

	if (is_mat_first_)
	{
		prevWorldToClipSpaceMat = worldToClipMat;
		is_mat_first_ = false;
	}

	// z near far
	float zn = curr_cam_->getZNear();
	float zf = curr_cam_->getZFar();

	TextureObj* tex_obj = rtt_src_->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	TextureObj* depth_buf = ROOTPTR->getRenderPipe()->getGBufferPass()->getColor1();
	rs->setTexture(1, depth_buf);

	// set params
	eff_->setFloat("isSwapV", 1.0f);
	eff_->setInt("NumberOfSamples", num_of_samples_);
	eff_->setFloat("VelocityFactor", motion_blur_factor_);
	eff_->setMatrix4("ClipSpaceToWorldSpace", invWorldToClipMat.transpose()[0]);
	eff_->setMatrix4("PreviousWorldSpaceToClipSpace", prevWorldToClipSpaceMat.transpose()[0]);
	eff_->setFloat("zNear", zn);
	eff_->setFloat("zFar", zf);
	eff_->setInt("ColorTexture", 0);
	eff_->setInt("DepthTexture", 1);

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);

	rtt->end_render();

	prevWorldToClipSpaceMat = worldToClipMat;
}

Program* PostMotionBlur::getRenderEffect(const String& vs_file, const String& ps_file)
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

String PostMotionBlur::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

}