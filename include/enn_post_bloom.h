//
// Copyright (c) 2013 Pateo Software Ltd
//
// This source file is part of Pateo gis engine.
// For latest info, see http://www.pateo.com.cn/
//
// Time: 2013/10/15
// File: enn_post_bloom.h
//
 
#pragma once

#include "enn_post_block.h"
#include "enn_mesh.h"
#include "enn_gl_program.h"
#include "enn_gl_render_target.h"
#include "enn_viewport.h"

namespace enn
{

class PostBloom : public PostBlock
{
public:
	PostBloom();
	~PostBloom();

	void prepare();
	void render(RenderTarget* dest, RenderTarget* src, Camera* cam);

	void checkRTT(Viewport* vp);

protected:
	void downSample();
	void brightSample();
	void blurSample();
	void combineSample();

	void hblurSample();
	void vblurSample();

protected:
	Program* getRenderEffect(const String& vs_file, const String& ps_file);
	String makeMacro(bool is_ps);

	void prepareMesh();
	void prepareDown4Mesh();

protected:
	MeshPtr				screen_mesh_;
	MeshPtr				down4_screen_mesh_;
	MeshPtr				down4_not_full_mesh_;
	bool				is_prepared_;
	Viewport			rtt_view_port_;

	RenderTarget*		src_rtt_;
	RenderTarget*		dest_rtt_;
	RenderTarget*		temp_rtt_;

	Program*			bright_eff_;
	Program*			render_eff_v_blur_;
	Program*			render_eff_h_blur_;
	Program*			downsample_eff_;
	Program*			combine_sample_eff_;

	float				blur_weight_[13];
	vec2f				h_texel_kernel_[13];
	vec2f				v_texel_kernel_[13];

	Camera*				curr_cam_;
};

}