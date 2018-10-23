/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/05/26
* File: enn_gl_render_system.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_gl_interface.h"
#include "enn_gl_rtt_pool.h"
#include "enn_viewport.h"

namespace enn
{

/// CPU-side scratch buffer for vertex data updates.
struct ScratchBuffer
{
	ScratchBuffer() :
	size_(0),
	reserved_(false)
	{

	}

	/// Buffer data.
	uchar* data_;
	/// Data size.
	unsigned size_;
	/// Reserved flag.
	bool reserved_;
};

class VertexBuffer;
class IndexBuffer;
class FrameBuffer;
class TextureObj;
class RenderBuffer;
class Program;
class RttPool;
class RenderSystem : public Noncopyable, public AllocatedObject
{
	typedef enn::list<RenderTarget*>::type RTStack;
public:
	RenderSystem();
	~RenderSystem();

public:
	bool init();
	void shutdown();

	bool beginFrame();
	void endFrame();

	uint32 getCurrFrameNo() const
	{
		return curr_frame_no_;
	}

	void setVB(VertexBuffer* vb);
	void setIB(IndexBuffer* ib);

	void setTexture(int sampler, TextureObj* tex);
	void setTextureCube(int sampler, TextureObj* tex);

	void activeTexture(int sampler);

	void setProgram(Program* pro);

	void resetRenderState();

	void setWireFrame(bool b);
	void setFrontFace(bool is_ccw);
	void setCullFace(bool is_back);
	void enableCullFace(bool en);
	void enableZTest(bool en);
	void enableZWrite(bool en);
	void enableBlend(bool en);
	void setBlendFunc(GLenum sfactor, GLenum dfactor);

	void drawPrimitives(uint32 type, uint32 offset, uint32 num_vertices);
	void drawIndexedPrimitives(uint32 type, uint32 start_idx, uint32 num_idx_count, bool is_idx_16bit = true);

	void clear(uint32 flags, const Color& clr = Color::BLACK, float z = 1.0f);
	void setFVF(uint32 element_mask, bool en);

	void setRTT(int channel, TextureObj* color, RenderBuffer* rb);
	void setCubeRTT(int channel, int face, TextureObj* color, RenderBuffer* rb);

public:
	void* reserveScratchBuffer(uint32 size);
	void freeScratchBuffer(void* buffer);

	void push_active_render_target();
	void pop_render_target();
	void set_render_target(RenderTarget* rt);
	void restore_main_target();

public:
	RttPool* getRttPool()
	{
		return &rtt_pool_;
	}

public:
	void setViewport(Viewport* vp);
	void setMainViewport(Viewport* vp);
	void restoreMainViewport();

public:
	/// Return vertex size corresponding to a vertex element mask.
	static unsigned GetVertexSize(unsigned elementMask);
	/// Return element offset from an element mask.
	static unsigned GetElementOffset(unsigned elementMask, VertexElement element);

public:
	uint32 getRenderPrims() const;
	uint32 getRenderDPs() const;

public:
	/// Vertex element sizes in bytes.
	static const unsigned elementSize[];
	/// Vertex element OpenGL types.
	static const unsigned elementType[];
	/// Vertex element OpenGL component counts.
	static const unsigned elementComponents[];
	/// Vertex element OpenGL normalization.
	static const unsigned elementNormalize[];
	/// Vertex element names.
	static const String elementName[];

protected:
	void enableCap( GLenum cap, bool en );

protected:
	typedef enn::vector<ScratchBuffer>::type ScratchBufferList;
protected:
	uint32								num_batches_rendered_;
	uint32								num_prim_rendered_;
	float								fps_;
	uint32								curr_frame_no_;
	uint32								curr_render_state_[RS_NUM];
	uint32								blend_src_factor_, blend_dst_factor_;
	uint32								curr_element_mask_;
	VertexBuffer*						curr_vb_;
	IndexBuffer*						curr_ib_;
	Program*							curr_eff_;
	TextureObj*							tex_list_[MAX_TEXTURE_UNITS];
	int									curr_active_tex_;
	FrameBuffer*						curr_fb_;
	void*								vb_data_;
	void*								ib_data_;
	ScratchBufferList					scratch_buffers_;
	uint32								max_scratch_buffers_request_;

	RttPool								rtt_pool_;
	Viewport							main_view_port_;
	RenderTarget*						main_view_target_;
	RenderTarget*						curr_target_;
	RTStack								rt_stack_;
};

}