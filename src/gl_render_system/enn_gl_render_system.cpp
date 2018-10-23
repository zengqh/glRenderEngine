/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_render_system.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_gl_render_system.h"
#include "enn_gl_vb.h"
#include "enn_gl_ib.h"
#include "enn_gl_frame_buffer.h"
#include "enn_gl_texture_obj.h"
#include "enn_gl_render_buffer.h"
#include "enn_gl_program.h"
#include "enn_root.h"

namespace enn
{

//////////////////////////////////////////////////////////////////////////
// static element attribute
const unsigned RenderSystem::elementSize[] =
{
	3 * sizeof(float),				// Position
	3 * sizeof(float),				// Normal
	4 * sizeof(unsigned char),		// Color
	2 * sizeof(float),				// Texcoord1
	2 * sizeof(float),				// Texcoord2
	3 * sizeof(float),				// Cubetexcoord1
	3 * sizeof(float),				// Cubetexcoord2
	3 * sizeof(float),				// Tangent
	4 * sizeof(float),				// Blendweights
	4 * sizeof(unsigned char)		// Blendindices
};

const unsigned RenderSystem::elementType[] =
{
	GL_FLOAT,				// Position
	GL_FLOAT,				// Normal
	GL_UNSIGNED_BYTE,		// Color
	GL_FLOAT,				// Texcoord1
	GL_FLOAT,				// Texcoord2
	GL_FLOAT,				// Cubetexcoord1
	GL_FLOAT,				// Cubetexcoord2
	GL_FLOAT,				// Tangent
	GL_FLOAT,				// Blendweights
	GL_UNSIGNED_BYTE,		// Blendindices
};

const unsigned RenderSystem::elementComponents[] =
{
	3, // Position
	3, // Normal
	4, // Color
	2, // Texcoord1
	2, // Texcoord2
	3, // Cubetexcoord1
	3, // Cubetexcoord2
	4, // Tangent
	4, // Blendweights
	4  // Blendindices
};

const unsigned RenderSystem::elementNormalize[] =
{
	GL_FALSE, // Position
	GL_FALSE, // Normal
	GL_TRUE,  // Color
	GL_FALSE, // Texcoord1
	GL_FALSE, // Texcoord2
	GL_FALSE, // Cubetexcoord1
	GL_FALSE, // Cubetexcoord2
	GL_FALSE, // Tangent
	GL_FALSE, // Blendweights
	GL_FALSE  // Blendindices
};

const String RenderSystem::elementName[] =
{
	"vPos",
	"vNormal",
	"vColor",
	"vTex",
	"vTex2",
	"vCubetexcoord1",
	"vCubetexcoord2",
	"vTangent",
	"vBoneWeights",
	"vBoneIndices"
};


/// Return vertex size corresponding to a vertex element mask.
unsigned RenderSystem::GetVertexSize(unsigned elementMask)
{
	unsigned vertexSize = 0;

	for (unsigned i = 0; i < ELEMENT_NUM; ++i)
	{
		if (elementMask & (1 << i))
		{
			vertexSize += elementSize[i];
		}
	}

	return vertexSize;
}

/// Return element offset from an element mask.
unsigned RenderSystem::GetElementOffset(unsigned elementMask, VertexElement element)
{
	unsigned offset = 0;

	for (int i = 0; i < ELEMENT_NUM; ++i)
	{
		if (i == element)
			break;

		if (elementMask & (1 << i))
			offset += elementSize[i];
	}

	return offset;
}

//////////////////////////////////////////////////////////////////////////
// RenderSystem
RenderSystem::RenderSystem()
: num_batches_rendered_(0)
, num_prim_rendered_(0)
, fps_(0.0f)
, curr_element_mask_(0)
, curr_vb_(0)
, curr_ib_(0)
, curr_eff_(0)
, curr_fb_(0)
, vb_data_(0)
, ib_data_(0)
, curr_active_tex_(-1)
, max_scratch_buffers_request_(0)
, curr_frame_no_(0)
, main_view_target_(0)
, curr_target_(0)
{
	for (int i = 0; i < MAX_TEXTURE_UNITS; ++i)
	{
		tex_list_[i] = 0;
	}

	for (int i = 0; i < RS_NUM; ++i)
	{
		curr_render_state_[i] = (uint32)(-1);
	}

	curr_fb_ = FrameBuffer::createInstance();
}

RenderSystem::~RenderSystem()
{
	shutdown();
}

bool RenderSystem::init()
{
#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	GLeeInit();
#endif

	if (!curr_fb_->getHandle())
	{
		curr_fb_->createFB();
	}

	main_view_target_ = RenderTarget::createInstance();
	main_view_target_->setScreenRTT(0, 0);
	curr_target_ = main_view_target_;

	resetRenderState();

	return true;
}

void RenderSystem::shutdown()
{
	ENN_SAFE_RELEASE(curr_fb_);
}

bool RenderSystem::beginFrame()
{
	curr_frame_no_++;

	num_batches_rendered_ = 0;
	num_prim_rendered_ = 0;

	//getRttPool()->resetAllBuffers();

	ROOTPTR->getProfiler()->begin_frame();

	return true;
}

void RenderSystem::endFrame()
{
	ROOTPTR->getProfiler()->end_frame();
}

void RenderSystem::setVB(VertexBuffer* vb)
{
	if (vb)
	{
		GLuint handle = vb->getHandle();
		gl.glBindBuffer( GL_ARRAY_BUFFER, handle );
		
		if (handle)
		{
			vb_data_ = 0;
		}
		else
		{
			vb_data_ = vb->getData();
		}
	}
	else
	{
		gl.glBindBuffer( GL_ARRAY_BUFFER, 0 );
		vb_data_ = 0;
	}

	curr_vb_ = vb;

	checkGLError();
}

void RenderSystem::setIB(IndexBuffer* ib)
{
	if (ib)
	{
		GLuint handle = ib->getHandle();
		gl.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, handle );

		if (handle)
		{
			ib_data_ = 0;
		}
		else
		{
			ib_data_ = ib->getData();
		}
	}
	else
	{
		gl.glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

		ib_data_ = 0;
	}

	curr_ib_ = ib;

	checkGLError();
}

void RenderSystem::setTexture(int sampler, TextureObj* tex)
{
	activeTexture( sampler );

	GLuint handle = tex ? tex->getHandle() : 0;

	if ( handle )
	{
		glBindTexture( GL_TEXTURE_2D, handle );
	}
	else
	{
		glBindTexture( GL_TEXTURE_2D, 0 );
	}

	checkGLError();
}

void RenderSystem::setTextureCube(int sampler, TextureObj* tex)
{
	activeTexture( sampler );

	GLuint handle = tex ? tex->getHandle() : 0;

	if ( handle )
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, handle );
	}
	else
	{
		glBindTexture( GL_TEXTURE_CUBE_MAP, 0 );
	}
}

void RenderSystem::activeTexture(int sampler)
{
	if (sampler >= MAX_TEXTURE_UNITS)
	{
		ENN_ASSERT(0);
		return;
	}

	if (curr_active_tex_ != sampler)
	{
		gl.glActiveTexture( GL_TEXTURE0 + sampler );
		checkGLError();

		curr_active_tex_ = sampler;
	}
}

void RenderSystem::setProgram(Program* pro)
{
	if (curr_eff_ != pro)
	{
		GLuint handle = pro ? pro->getHandle() : 0;
		gl.glUseProgram( handle );

		curr_eff_ = pro;
	}

	checkGLError();
}

void RenderSystem::resetRenderState()
{
	setWireFrame(false);
	setFrontFace(true);
	setCullFace(true);
	enableCullFace(true);
	enableZTest(true);
	enableZWrite(true);
	enableBlend(false);
	setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void RenderSystem::setWireFrame(bool b)
{
#if !defined(GL_ES_VERSION_2_0)
	if (curr_render_state_[RS_WIREFRAME] != static_cast<uint32>(b))
	{
		glPolygonMode( GL_FRONT_AND_BACK, b ? GL_LINE : GL_FILL );
		checkGLError();

		curr_render_state_[RS_WIREFRAME] = static_cast<uint32>(b);
	}
#endif
}

void RenderSystem::setFrontFace(bool is_ccw)
{
	if (curr_render_state_[RS_FRONTCCW] != static_cast<uint32>(is_ccw))
	{
		glFrontFace( is_ccw ? GL_CCW : GL_CW );
		checkGLError();

		curr_render_state_[RS_FRONTCCW] = static_cast<uint32>(is_ccw);
	}
}

void RenderSystem::setCullFace(bool is_back)
{
	if (curr_render_state_[RS_CULLFACE] != static_cast<uint32>(is_back))
	{
		glCullFace( is_back ? GL_BACK : GL_FRONT );
		checkGLError();

		curr_render_state_[RS_CULLFACE] = static_cast<uint32>(is_back);
	}
}

void RenderSystem::enableCullFace(bool en)
{
	if (curr_render_state_[RS_CULLFACEEN] != static_cast<uint32>(en))
	{
		enableCap(GL_CULL_FACE, en);
		checkGLError();

		curr_render_state_[RS_CULLFACEEN] = static_cast<uint32>(en);
	}
}

void RenderSystem::enableZTest(bool en)
{
	if (curr_render_state_[RS_ZTEST] != static_cast<uint32>(en))
	{
		enableCap(GL_DEPTH_TEST, en);
		checkGLError();

		curr_render_state_[RS_ZTEST] = static_cast<uint32>(en);
	}
}

void RenderSystem::enableZWrite(bool en)
{
	if (curr_render_state_[RS_ZWRITE] != static_cast<uint32>(en))
	{
		glDepthMask( en ? GL_TRUE : GL_FALSE );
		checkGLError();

		curr_render_state_[RS_ZWRITE] = static_cast<uint32>(en);
	}
}

void RenderSystem::enableBlend(bool en)
{
	if (curr_render_state_[RS_BLEND] != static_cast<uint32>(en))
	{
		enableCap(GL_BLEND, en);
		checkGLError();

		curr_render_state_[RS_BLEND] = static_cast<uint32>(en);
	}
}

void RenderSystem::setBlendFunc(GLenum sfactor, GLenum dfactor)
{
	if (blend_src_factor_ != sfactor || blend_dst_factor_ != dfactor)
	{
		glBlendFunc( sfactor, dfactor );
		checkGLError();

		blend_src_factor_ = sfactor;
		blend_dst_factor_ = dfactor;
	}
}

void RenderSystem::drawPrimitives(uint32 type, uint32 offset, uint32 num_vertices)
{
	if (type >= PRIM_NUM)
	{
		return;
	}

	if (!num_vertices)
	{
		return;
	}

	uint32 primitiveCount = 0;
	switch (type)
	{
	case PRIM_LINELIST:
		primitiveCount = num_vertices / 2;
		glDrawArrays(GL_LINES, offset, num_vertices);
		break;
	case PRIM_TRIANGLELIST:
		primitiveCount = num_vertices / 3;
		glDrawArrays(GL_TRIANGLES, offset, num_vertices);
		break;
	}

	num_prim_rendered_ += primitiveCount;
	num_batches_rendered_++;
}

void RenderSystem::drawIndexedPrimitives(uint32 type, uint32 start_idx, uint32 num_idx_count, bool is_idx_16bit)
{
	if (type >= PRIM_NUM)
	{
		return;
	}

	if (!num_idx_count)
	{
		return;
	}

	uint32 primitiveCount = 0;

	GLenum mode;
	GLenum data_type;
	size_t bytes;
	int pn = 3;

	if ( type == PRIM_TRIANGLELIST )
	{
		mode = GL_TRIANGLES;
		primitiveCount = num_idx_count / 3;
		pn = 3;
	}
	else
	{
		mode = GL_LINES;
		primitiveCount = num_idx_count / 2;
		pn = 2;
	}

	if ( is_idx_16bit )
	{
		data_type  = GL_UNSIGNED_SHORT;
		bytes = sizeof(GLushort);
	}
	else
	{
		data_type  = GL_UNSIGNED_INT;
		bytes = sizeof(GLuint);
	}

	glDrawElements( mode, num_idx_count, data_type, (const void*)((char*)ib_data_ + bytes * start_idx) );
	checkGLError();
}

void RenderSystem::clear(uint32 flags, const Color& clr, float z)
{
	if ( flags & GL_COLOR_BUFFER_BIT )
	{
		glClearColor( clr.r, clr.g, clr.b, clr.a );
	}

	if ( flags & GL_DEPTH_BUFFER_BIT )
	{
		gl.glClearDepth( z );
	}

	glClear( flags );
}

void RenderSystem::setFVF(uint32 element_mask, bool en)
{
	if (en)
	{
		uint32 stride = GetVertexSize(element_mask);

		for (int i = 0; i < ELEMENT_NUM; ++i)
		{
			if (BITS(i) & element_mask)
			{
				int attr_idx = curr_eff_->getAttribSlot(RenderSystem::elementName[i].c_str());

				if (attr_idx == -1)
				{
					continue;
				}

				ENN_ASSERT(attr_idx != -1);

				gl.glEnableVertexAttribArray(attr_idx);

				gl.glVertexAttribPointer(attr_idx, RenderSystem::elementComponents[i], 
					RenderSystem::elementType[i], RenderSystem::elementNormalize[i], stride, 
					(uchar*)vb_data_ + GetElementOffset(element_mask, (VertexElement)i));

				checkGLError();
			}
		}
	}
	else
	{
		for (int i = 0; i < ELEMENT_NUM; ++i)
		{
			if (BITS(i) & element_mask)
			{
				int attr_idx = curr_eff_->getAttribSlot(RenderSystem::elementName[i].c_str());

				if (attr_idx == -1)
				{
					continue;
				}

				ENN_ASSERT(attr_idx != -1);

				gl.glDisableVertexAttribArray(attr_idx);
			}
		}
	}
}

void RenderSystem::setRTT(int channel, TextureObj* color, RenderBuffer* rb)
{
	curr_fb_->active( true );
	curr_fb_->attachRTT( channel, color );
	curr_fb_->attachDepthStencil(rb );
}

void RenderSystem::setCubeRTT(int channel, int face, TextureObj* color, RenderBuffer* rb)
{
	curr_fb_->active( true );
	curr_fb_->attachCubeRTT( channel, color, face );
	curr_fb_->attachDepthStencil(rb );
}

void* RenderSystem::reserveScratchBuffer(uint32 size)
{
	if (!size) return 0;

	if (size > max_scratch_buffers_request_)
	{
		max_scratch_buffers_request_ = size;
	}

	for (ScratchBufferList::iterator iter = scratch_buffers_.begin();
		iter != scratch_buffers_.end(); ++iter)
	{
		if (!iter->reserved_ && iter->size_ >= size)
		{
			iter->reserved_ = true;
			return iter->data_;
		}
	}

	for (ScratchBufferList::iterator iter = scratch_buffers_.begin();
		iter != scratch_buffers_.end(); ++iter)
	{
		if (!iter->reserved_)
		{
			ENN_SAFE_FREE(iter->data_);
			iter->data_ = (uchar*)ENN_MALLOC(size);
			iter->reserved_ = true;

			return iter->data_;
		}
	}

	ScratchBuffer newBuffer;
	newBuffer.data_ = (uchar*)ENN_MALLOC(size);
	newBuffer.size_ = size;
	newBuffer.reserved_ = true;
	scratch_buffers_.push_back(newBuffer);

	return newBuffer.data_;
}

void RenderSystem::freeScratchBuffer(void* buffer)
{
	if (!buffer)
	{
		return;
	}

	for (ScratchBufferList::iterator iter = scratch_buffers_.begin();
		iter != scratch_buffers_.end(); ++iter)
	{
		if (!iter->reserved_ && iter->data_ >= buffer)
		{
			iter->reserved_ = false;
			return;
		}
	}

	ENN_ASSERT(0);
}

void RenderSystem::push_active_render_target()
{
	rt_stack_.push_back(curr_target_);
}

void RenderSystem::pop_render_target()
{
	ENN_ASSERT(!rt_stack_.empty());

	set_render_target(rt_stack_.back());

	rt_stack_.pop_back();
}

// not support cube rtt
void RenderSystem::set_render_target(RenderTarget* rt)
{
	ENN_ASSERT(rt);

	if (rt == curr_target_)
	{
		return;
	}
	
	if (rt->isScreenRTT())
	{
		curr_fb_->active(false);
		curr_target_ = rt;
		return;
	}
	else
	{
		curr_fb_->active(true);
	}

	// color buffer
	for (int i = 0; i < MAX_FRAME_BUFFER_CHANNEL; ++i)
	{
		TextureObj* tex = rt->get_rtt(i);

		curr_fb_->attachRTT(i, tex);
	}

	RenderBuffer* depth = rt->get_depth();
	if (depth)
	{
		curr_fb_->attachDepthStencil(depth);
	}

	curr_target_ = rt;
}

void RenderSystem::restore_main_target()
{
	curr_fb_->active(false);
}

void RenderSystem::setViewport(Viewport* vp)
{
	const IntRect& rect = vp->getRect();
	glViewport(rect.left, rect.top, rect.getWidth(), rect.getHeight());
}

void RenderSystem::setMainViewport(Viewport* vp)
{
	setViewport(vp);

	main_view_target_->setScreenRTT(vp->getWidth(), vp->getHeight());

	main_view_port_ = *vp;
}

void RenderSystem::restoreMainViewport()
{
	setViewport(&main_view_port_);
}

void RenderSystem::enableCap( GLenum cap, bool en )
{
	if (en)
	{
		glEnable(cap);
	}
	else
	{
		glDisable(cap);
	}
}

uint32 RenderSystem::getRenderPrims() const
{
	return num_prim_rendered_;
}

uint32 RenderSystem::getRenderDPs() const
{
	return num_batches_rendered_;
}

}