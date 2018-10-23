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
// File: enn_general_render_pass.cpp
//

#include "enn_general_render_pass.h"
#include "enn_root.h"
#include "enn_render_pipe.h"
#include "enn_light.h"

namespace enn
{

GeneralRenderPass::GeneralRenderPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, curr_render_pass_(GR_MATERIAL)
{

}

GeneralRenderPass::~GeneralRenderPass()
{
	solid_ra_list_.clear();
	alpha_ra_list_.clear();
	general_obj_list_.clear();
}

void GeneralRenderPass::render(Camera* cam)
{
	curr_render_cam_ = cam;

	if (isShadowPass())
	{
		dispatch_shadow_obj();
		render_shadow();
	}
	else if (isGBufferPass())
	{
		dispatch_gbuffer_obj();
		render_gbuffer();
	}
	else if (isMateriallPass())
	{
		dispatch_general_obj();
		render_mtr();
	}
	else if (isForwardPass())
	{
		dispatch_forward_obj();
		render_forward();
	}
	else
	{
		ENN_ASSERT(0);
	}
}

void GeneralRenderPass::render_mtr()
{
	RenderSystem* rs = ROOTPTR->get_render_system();
	rs->clear(GL_DEPTH_BUFFER_BIT);

	renderSolidObjs();

	renderAlphaObjs();
}

void GeneralRenderPass::render_shadow()
{
	renderShadowObj();
}

void GeneralRenderPass::render_gbuffer()
{
	renderGBufferObj();
}

void GeneralRenderPass::render_forward()
{
	renderForwardSolidObjs();

	renderForwardAlphaObjs();
}

void GeneralRenderPass::prepare(Camera* cam)
{
	curr_render_cam_ = cam;

	dispatch_general_obj();
}

void GeneralRenderPass::render_solid()
{
	renderSolidObjs();
}

void GeneralRenderPass::render_alpha()
{
	renderAlphaObjs();
}

void GeneralRenderPass::begin_insert_obj(uint32 pass)
{
	curr_render_pass_ = pass;

	switch (curr_render_pass_)
	{
	case GR_MATERIAL:
		general_obj_list_.clear();
		break;
	case GR_SHADOW:
		shadow_obj_list_.clear();
		break;
	case GR_DEFERRED:
		gbuffer_obj_list_.clear();
		break;
	case GR_FORWARD:
		forward_obj_list_.clear();
		break;
	default:
		ENN_ASSERT(0);
	}
}

void GeneralRenderPass::insert_obj(RenderableNode* node)
{
	switch (curr_render_pass_)
	{
	case GR_MATERIAL:
		general_obj_list_.push_back(node);
		break;
	case GR_SHADOW:
		shadow_obj_list_.push_back(node);
		break;
	case GR_DEFERRED:
		gbuffer_obj_list_.push_back(node);
		break;
	case GR_FORWARD:
		forward_obj_list_.push_back(node);
		break;
	default:
		ENN_ASSERT(0);
	}

}

void GeneralRenderPass::end_insert_obj()
{

}

void GeneralRenderPass::dispatch_general_obj()
{
	solid_ra_list_.clear();
	alpha_ra_list_.clear();

	ENN_FOR_EACH(GeneralObjList, general_obj_list_, it)
	{
		RenderableNode* rn = *it;
		rn->addToRender(this);
	}

	general_obj_list_.clear();
}

void GeneralRenderPass::dispatch_shadow_obj()
{
	shadow_ra_list_.clear();

	ENN_FOR_EACH(ShadowObjList, shadow_obj_list_, it)
	{
		RenderableNode* rn = *it;
		rn->addToRender(this);
	}

	shadow_obj_list_.clear();
}

void GeneralRenderPass::dispatch_gbuffer_obj()
{
	gbuffer_ra_list_.clear();

	ENN_FOR_EACH(GBufferObjList, gbuffer_obj_list_, it)
	{
		RenderableNode* rn = *it;
		rn->addToRender(this);
	}

	gbuffer_obj_list_.clear();
}

void GeneralRenderPass::dispatch_forward_obj()
{
	forward_solid_ra_list_.clear();
	forward_alpha_ra_list_.clear();

	ENN_FOR_EACH(GeneralObjList, forward_obj_list_, it)
	{
		RenderableNode* rn = *it;
		rn->addToRender(this);
	}

	forward_obj_list_.clear();
}

bool GeneralRenderPass::isShadowPass() const
{
	return (curr_render_pass_ == GR_SHADOW);
}

bool GeneralRenderPass::isMateriallPass() const
{
	return (curr_render_pass_ == GR_MATERIAL);
}

bool GeneralRenderPass::isGBufferPass() const
{
	return (curr_render_pass_ == GR_DEFERRED);
}

bool GeneralRenderPass::isForwardPass() const
{
	return (curr_render_pass_ == GR_FORWARD);
}

void GeneralRenderPass::addRenderable(Renderable* ra)
{
	if (curr_render_pass_ == GR_MATERIAL)
	{
		addRenderable_mtr_pass(ra);
	}
	else if (curr_render_pass_ == GR_SHADOW)
	{
		addRenderable_shadow_pass(ra);
	}
	else if (curr_render_pass_ == GR_DEFERRED)
	{
		addRenderable_gbuffer_pass(ra);
	}
	else if (curr_render_pass_ == GR_FORWARD)
	{
		addRenderable_forward_pass(ra);
	}
	else
	{
		ENN_ASSERT(0);
	}
}

void GeneralRenderPass::render_single_general_mesh(RenderableNode* node, Camera* cam)
{
	begin_insert_obj(GeneralRenderPass::GR_MATERIAL);

	if (node->isMeshNode())
	{
		MeshNode* mn = static_cast<MeshNode*>(node);
		insert_obj(mn);
	}

	end_insert_obj();

	render(cam);
}

void GeneralRenderPass::render_single_raw_mesh(Mesh* mesh)
{

}

void GeneralRenderPass::addRenderable_mtr_pass(Renderable* ra)
{
	SubMaterial* mat = ra->getMaterial();
	SkeletonInstance* skel_inst = ra->getSkelInst();

	if (mat->hasAlpha())
	{
		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_MATERIAL);
		EffectContext* ec = eff_template->getEffectContext(ra);

		RenderableContext rc;
		rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
		rc.eff_ = ec;
		rc.ra_obj_ = ra;

		alpha_ra_list_.addRenderable(rc);
	}
	else
	{
		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_MATERIAL);
		EffectContext* ec = eff_template->getEffectContext(ra);

		RenderableContext rc;
		rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
		rc.eff_ = ec;
		rc.ra_obj_ = ra;

		solid_ra_list_.addRenderable(rc);
	}
}

void GeneralRenderPass::addRenderable_shadow_pass(Renderable* ra)
{
	SubMaterial* mat = ra->getMaterial();
	SkeletonInstance* skel_inst = ra->getSkelInst();

	if (mat->hasAlpha())
	{
		return;
	}

	EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_SHADOW);
	EffectContext* ec = eff_template->getEffectContext(ra);

	RenderableContext rc;
	rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
	rc.eff_ = ec;
	rc.ra_obj_ = ra;

	shadow_ra_list_.addRenderable(rc);
}

void GeneralRenderPass::addRenderable_gbuffer_pass(Renderable* ra)
{
	SubMaterial* mat = ra->getMaterial();
	SkeletonInstance* skel_inst = ra->getSkelInst();

	if (mat->hasAlpha())
	{
		return;
	}

	EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_DEFPRE);
	EffectContext* ec = eff_template->getEffectContext(ra);

	RenderableContext rc;
	rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
	rc.eff_ = ec;
	rc.ra_obj_ = ra;

	gbuffer_ra_list_.addRenderable(rc);
}

void GeneralRenderPass::addRenderable_forward_pass(Renderable* ra)
{
	SubMaterial* mat = ra->getMaterial();
	SkeletonInstance* skel_inst = ra->getSkelInst();

	if (mat->hasAlpha())
	{
		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_FORWARD);
		EffectContext* ec = eff_template->getEffectContext(ra);

		RenderableContext rc;
		rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
		rc.eff_ = ec;
		rc.ra_obj_ = ra;

		forward_alpha_ra_list_.addRenderable(rc);
	}
	else
	{
		EffectTemplate* eff_template = render_pipe_->getEffTemplateManager()->getEffectTemplate(ET_DEF_FORWARD);
		EffectContext* ec = eff_template->getEffectContext(ra);

		RenderableContext rc;
		rc.dist_ = (curr_render_cam_->getEyePos() - ra->getAABBWorld().getCenter()).squaredLength();
		rc.eff_ = ec;
		rc.ra_obj_ = ra;

		forward_solid_ra_list_.addRenderable(rc);
	}
}

void GeneralRenderPass::renderSolidObjs()
{
	DefaultEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;
	params.lit_buffer_ = render_pipe_->getLitBufferPass()->getLitBuffer();
	params.map_ao_ = render_pipe_->getSPSSAOPass()->getMapAOBuffer();

	ENN_FOR_EACH(KindRenderableList::RCListMap, solid_ra_list_.getRenderables(), it)
	{
		EffectContext* eff_context = it->first;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);
		RenderableContextList* sub_list = it->second;
		for (int i = 0; i < sub_list->getSize(); ++i)
		{
			RenderableContext* rac = &(sub_list->at(i));

			Renderable* ra = rac->ra_obj_;

			SubMaterial* mtr = ra->getMaterial();

			eff_template->doSetParams(eff_context, ra, &params);

			apply_fvf(ra, true);
			doSolidRenderable(ra);
			apply_fvf(ra, false);
		}	
	}
}

void GeneralRenderPass::renderAlphaObjs()
{
	DefaultEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;

	int cnt = alpha_ra_list_.getSize();
	for (int i = 0; i < cnt; ++i)
	{
		RenderableContext* rac = &(alpha_ra_list_.at(i));
		EffectContext* eff_context = rac->eff_;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);

		Renderable* ra = rac->ra_obj_;
		SubMaterial* mtr = ra->getMaterial();

		eff_template->doSetParams(eff_context, ra, &params);

		apply_fvf(ra, true);
		doAlphaRenderable(ra);
		apply_fvf(ra, false);
	}	
}

void GeneralRenderPass::renderShadowObj()
{
	ShadowPreEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;

	ENN_FOR_EACH(KindRenderableList::RCListMap, shadow_ra_list_.getRenderables(), it)
	{
		EffectContext* eff_context = it->first;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);
		RenderableContextList* sub_list = it->second;
		for (int i = 0; i < sub_list->getSize(); ++i)
		{
			RenderableContext* rac = &(sub_list->at(i));

			Renderable* ra = rac->ra_obj_;

			SubMaterial* mtr = ra->getMaterial();

			if (mtr->isRenderHomogeneous())
			{
				continue;
			}

			eff_template->doSetParams(eff_context, ra, &params);

			apply_fvf(ra, true);
			doSolidRenderable(ra);
			apply_fvf(ra, false);
		}	
	}
}

void GeneralRenderPass::renderGBufferObj()
{
	GBufferEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;

	ENN_FOR_EACH(KindRenderableList::RCListMap, gbuffer_ra_list_.getRenderables(), it)
	{
		EffectContext* eff_context = it->first;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);
		RenderableContextList* sub_list = it->second;
		for (int i = 0; i < sub_list->getSize(); ++i)
		{
			RenderableContext* rac = &(sub_list->at(i));

			Renderable* ra = rac->ra_obj_;

			SubMaterial* mtr = ra->getMaterial();

			if (mtr->isRenderHomogeneous())
			{
				continue;
			}

			eff_template->doSetParams(eff_context, ra, &params);

			apply_fvf(ra, true);
			doSolidRenderable(ra);
			apply_fvf(ra, false);
		}	
	}
}

void GeneralRenderPass::renderForwardSolidObjs()
{
	ForwardEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;

	ENN_FOR_EACH(KindRenderableList::RCListMap, forward_solid_ra_list_.getRenderables(), it)
	{
		EffectContext* eff_context = it->first;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);
		RenderableContextList* sub_list = it->second;
		for (int i = 0; i < sub_list->getSize(); ++i)
		{
			RenderableContext* rac = &(sub_list->at(i));

			Renderable* ra = rac->ra_obj_;

			SubMaterial* mtr = ra->getMaterial();

			eff_template->doSetParams(eff_context, ra, &params);

			apply_fvf(ra, true);
			doSolidRenderable(ra);
			apply_fvf(ra, false);
		}	
	}
}

void GeneralRenderPass::renderForwardAlphaObjs()
{
	ForwardEffectParams params;
	params.curr_render_cam_ = curr_render_cam_;

	int cnt = alpha_ra_list_.getSize();
	for (int i = 0; i < cnt; ++i)
	{
		RenderableContext* rac = &(forward_alpha_ra_list_.at(i));
		EffectContext* eff_context = rac->eff_;
		EffectTemplate* eff_template = eff_context->owner_;

		Root::getSingletonPtr()->get_render_system()->setProgram(eff_context->eff_);

		Renderable* ra = rac->ra_obj_;
		SubMaterial* mtr = ra->getMaterial();

		eff_template->doSetParams(eff_context, ra, &params);

		apply_fvf(ra, true);
		doAlphaRenderable(ra);
		apply_fvf(ra, false);
	}	
}

void GeneralRenderPass::doSolidRenderable(Renderable* ra)
{
	ra->render();
}

void GeneralRenderPass::doAlphaRenderable(Renderable* ra)
{
	ra->render();
}


void GeneralRenderPass::apply_fvf(Renderable* ra, bool en)
{
	RenderSystem* rs = Root::getSingletonPtr()->get_render_system();
	uint32 element_mask = ra->getVB()->getElementMask();

	rs->setFVF(element_mask, en);
}

}