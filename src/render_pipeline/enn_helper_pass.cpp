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
// File: enn_helper_pass.cpp
//

#include "enn_helper_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

HelperPass::HelperPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, screen_temp_mesh_(0)
, curr_cam_(0)
{

}

HelperPass::~HelperPass()
{

}

void HelperPass::prepare()
{
	debug_rtt_list_.clear();
}

void HelperPass::render(Camera* cam)
{
	curr_cam_ = cam;

	ENN_FOR_EACH(DebugRTTList, debug_rtt_list_, it)
	{
		renderDebugRTT(*it);
	}
}

DebugRTTDesc* HelperPass::prepareDebugRTT(int x, int y, int w, int h, TextureObj* tex)
{
	DebugRTTDesc* rtt_desc = ENN_NEW_T(DebugRTTDesc);

	rtt_desc->x = x;
	rtt_desc->y = y;
	rtt_desc->w = w;
	rtt_desc->h = h;
	rtt_desc->tex = tex;

	ScreenRectDesc rect_desc(x, y, w, h, 800, 600);

	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();
	rtt_desc->screen_mesh = mesh_mgr->createScreenMesh("", rect_desc);
	rtt_desc->screen_mesh->load(false);

	return rtt_desc;
}

void HelperPass::freeDebugRTT(DebugRTTDesc*& rtt)
{
	ENN_SAFE_RELEASE(rtt->screen_mesh);
	ENN_SAFE_DELETE_T(rtt);
}

void HelperPass::addRenderDebugRTT(DebugRTTDesc* rtt)
{
	debug_rtt_list_.push_back(rtt);
}

void HelperPass::renderDebugRTT(DebugRTTDesc* rtt)
{
	UIRectEffectParams params;

	params.tex_ = rtt->tex;
	params.swap_uv = true;

	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	MeshManager* mesh_mgr = Root::getSingletonPtr()->get_mesh_mgr();
	Mesh* screen_mesh = rtt->screen_mesh;

	EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_UI);
	EffectContext* ec = eff_template->getEffectContext(0);

	rs->setProgram(ec->eff_);

	VertexBuffer* vb = screen_mesh->getSubMesh(0)->getVB();
	IndexBuffer* ib = screen_mesh->getSubMesh(0)->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	eff_template->doSetParams(ec, 0, &params);

	uint32 element_mask = vb->getElementMask();

	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(PRIM_TRIANGLELIST, 0, screen_mesh->getSubMesh(0)->getIndexCount(), true);

	rs->setFVF(element_mask, false);
}

}