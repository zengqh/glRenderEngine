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
// File: enn_end_pass.cpp
//

#include "enn_end_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

EndPass::EndPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, back_rtt_(0)
, ui_effect_(0)
, full_screen_mesh_(0)
{

}

EndPass::~EndPass()
{

}

void EndPass::prepare()
{
	if (!back_rtt_)
	{
		back_rtt_ = render_pipe_->getPost0();
	}

	// render back rtt
	if (!full_screen_mesh_)
	{
		MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();
		full_screen_mesh_.attach(mesh_mgr->getFullScreenMesh());
	}

	if (!ui_effect_)
	{
		getEffect();
	}
}

void EndPass::render(Camera* cam)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();

	back_rtt_->end_render();

	const Color& clear_clr = Root::getSingletonPtr()->getRootCfg().clear_clr;

	rs->beginFrame();

	rs->clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, clear_clr);
	rs->resetRenderState();

	rs->setProgram(ui_effect_);

	Viewport* vp = cam->getViewport();
	rs->setMainViewport(vp);

	// render full screen
	SubMesh* screen_sub_mesh = full_screen_mesh_->getSubMesh(0);
	VertexBuffer* vb = screen_sub_mesh->getVB();
	IndexBuffer* ib = screen_sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	TextureObj* tex_obj = render_pipe_->getPost1()->get_rtt(0);
	ENN_ASSERT(tex_obj);
	rs->setTexture(0, tex_obj);

	ui_effect_->setFloat("isSwapV", 1.0f);
	ui_effect_->setInt( "texDiff", 0 );

	uint32 element_mask = screen_sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(screen_sub_mesh->getPrimType(), 0, 
		screen_sub_mesh->getIndexCount(), 
		screen_sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);
}

void EndPass::getEffect()
{
	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	shader_getFileData(base_path + "eff_comm_def.h", def_code);
	shader_getFileData(base_path + "uirect_eff_vs.h", vs_code);
	shader_getFileData(base_path + "uirect_eff_ps.h", ps_code);

	String str_vs = makeMacro(false) + def_code + vs_code;
	String str_ps = makeMacro(true) + def_code + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();
	}

	ui_effect_ = pro;
}

String EndPass::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}

}