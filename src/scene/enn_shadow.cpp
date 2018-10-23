/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/21
* File: enn_shadow.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_shadow.h"
#include "enn_camera.h"
#include "enn_light.h"
#include "enn_gl_texture_obj.h"
#include "enn_gl_render_target.h"

namespace enn
{

ShadowInfo::ShadowInfo()
	: type_(ST_SM)
	, resolution_(1024)
	, tap_groups_(1)
	, visible_range_(50.0f)
	, fade_range_(45.0f)
	, split_scheme_weight_(0.5f)
	, blur_size_(1.0f)
	, strength_(0.5f)
	, bias_(0.001f)
	, cam_cull_caster_(0)
	, cam_render_caster_(0)
	, cam_receiver_caster_(0)
	, view_proj_tex_bias_(0)
	, need_rtt_size_(1024)
	, is_curr_active_(false)
	, texture_(0)
{
	ENN_CLEAR_ZERO(split_dist_);

	render_target_ = RenderTarget::createInstance();
}

ShadowInfo::~ShadowInfo()
{
	ENN_SAFE_FREE(view_proj_tex_bias_);
	ENN_DELETE [] cam_cull_caster_;

	if (cam_cull_caster_ != cam_render_caster_)
	{
		ENN_DELETE [] cam_cull_caster_;
	}

	ENN_DELETE cam_receiver_caster_;
}

Camera* ShadowInfo::getCullCaster()
{
	if (cam_cull_caster_)
	{
		return cam_cull_caster_;
	}

	int curr_cnt = render_target_->getViewportCounts();
	cam_cull_caster_ = ENN_NEW Camera[curr_cnt];
	return cam_cull_caster_;
}

Camera* ShadowInfo::getRenderCaster()
{
	if (cam_render_caster_)
	{
		return cam_render_caster_;
	}

	int curr_cnt = render_target_->getViewportCounts();

	cam_render_caster_ = ENN_NEW Camera[curr_cnt];

	return cam_render_caster_;
}

Camera* ShadowInfo::getCullReceiver()
{
	if (cam_receiver_caster_)
	{
		return cam_receiver_caster_;
	}

	cam_receiver_caster_ = ENN_NEW Camera;

	return cam_receiver_caster_;
}

Vector2 ShadowInfo::getFade() const
{
	float zn = cam_receiver_caster_->getZNear();
	return Vector2(zn + fade_range_, zn + visible_range_);
}

Matrix4* ShadowInfo::getMatViewProjTexBias()
{
	if (view_proj_tex_bias_)
	{
		return view_proj_tex_bias_;
	}

	int curr_cnt = render_target_->getViewportCounts();
	view_proj_tex_bias_ = ENN_NEW_ARRAY_T(Matrix4, curr_cnt);

	return view_proj_tex_bias_;
}

int ShadowInfo::getCascadesCount() const
{
	return render_target_->getViewportCounts();
}

void ShadowInfo::setCascades(int count)
{
	int curr_cnt = render_target_->getViewportCounts();

	if (count == curr_cnt)
	{
		return;
	}

	render_target_->clearAllViewports();

	for (int i = 0; i < count; ++i)
	{
		Viewport* vp = render_target_->createViewport(i);
		vp->setClearColor(Color::WHITE);
	}
}

void ShadowInfo::updateViewport(const IntRect* rect)
{
	Camera* render_caster = getRenderCaster();

	IntRect vp_default(0, 0, resolution_, resolution_);

	int k = 0;

	RenderTarget::Iterator itvp = render_target_->getViewports();

	while (itvp.hasNext())
	{
		Viewport* vp = itvp.get().second;

		if (rect)
		{
			vp->setRect(rect[k]);
		}
		else
		{
			vp->setRect(vp_default);
		}

		vp->link_camera(render_caster + k);

		++k;
	}
}

void ShadowInfo::link_rtt(TextureObj* tex, RenderBuffer* depth)
{
	texture_ = tex;

	render_target_->link_rtt(tex);
	render_target_->link_depth(depth);
}


//////////////////////////////////////////////////////////////////////////
void ShadowInfoWrap::setShadowType(ShadowType type)
{
	freeAll();

	if (type == ST_NONE)
	{
		return;
	}

	shadow_tech_ = createShadowTech(type);

	if (!shadow_tech_)
	{
		return;
	}

	shadow_info_ = ENN_NEW ShadowInfo();
	shadow_info_->type_ = type;
	shadow_tech_->init(getHost(), shadow_info_);
}

void ShadowInfoWrap::setShadowResolution(int resolution)
{
	shadow_info_->resolution_ = Math::maxVal(resolution, 0);
	shadow_tech_->updateResolution();
}

void ShadowInfoWrap::setShadowTapGroups(int tag_groups)
{
	shadow_info_->tap_groups_ = Math::clamp(tag_groups, 1, MAX_SHADOW_TAPGROUPS);
}

void ShadowInfoWrap::setShadowVisibleRange(float range)
{
	shadow_info_->visible_range_ = Math::maxVal(range, 0.0f);
	shadow_info_->fade_range_ = Math::clamp(shadow_info_->fade_range_, 0.0f,
		shadow_info_->visible_range_);
}

void ShadowInfoWrap::setShadowFadeRange(float range)
{
	shadow_info_->fade_range_ = Math::clamp(range, 0.0f, shadow_info_->visible_range_);
}

void ShadowInfoWrap::setShadowSplitSchemeWeight(float weight)
{
	shadow_info_->split_scheme_weight_ = Math::clamp(weight, 0.0f, 1.0f);
}

void ShadowInfoWrap::setShadowBlurSize(float bs)
{
	shadow_info_->blur_size_ = Math::maxVal(0.0f, bs);
}

void ShadowInfoWrap::setShadowStrength(float strength)
{
	shadow_info_->strength_ = Math::clamp(strength, 0.0f, 1.0f);
}

void ShadowInfoWrap::setShadowBias(float bias)
{
	shadow_info_->bias_ = bias;
}

ShadowType ShadowInfoWrap::getShadowType()
{
	return shadow_info_ ? shadow_info_->type_ : ST_NONE;
}

int ShadowInfoWrap::getShadowResolution()
{
	return shadow_info_ ? shadow_info_->resolution_ : 0;
}

int ShadowInfoWrap::getShadowTapGroups()
{
	return shadow_info_ ? shadow_info_->tap_groups_ : 0;
}

float ShadowInfoWrap::getShadowVisibleRange()
{
	return shadow_info_ ? shadow_info_->visible_range_ : 0;
}

float ShadowInfoWrap::getShadowFadeRange()
{
	return shadow_info_ ? shadow_info_->fade_range_ : 0;
}

float ShadowInfoWrap::getShadowSplitSchemeWeight()
{
	return shadow_info_ ? shadow_info_->split_scheme_weight_ : 0;
}

float ShadowInfoWrap::getShadowBlurSize()
{
	return shadow_info_ ? shadow_info_->blur_size_ : 0;
}

float ShadowInfoWrap::getShadowStrength()
{
	return shadow_info_ ? shadow_info_->strength_ : 0;
}

float ShadowInfoWrap::getShadowBias()
{
	return shadow_info_ ? shadow_info_->bias_ : 0;
}

void ShadowInfoWrap::updateShadowInfo(Camera* main_cam)
{
	if (!shadow_info_)
	{
		return;
	}

	if (shadow_info_->type_ == ST_NONE)
	{
		shadow_info_->is_curr_active_ = false;
		return;
	}

	shadow_tech_->updateShadowInfo(main_cam);
}

bool ShadowInfoWrap::isShadowCurrActive() const
{
	return shadow_info_ ? shadow_info_->is_curr_active_ : 0;
}

ShadowInfo* ShadowInfoWrap::getShadowInfo()
{
	return shadow_info_;
}

void ShadowInfoWrap::freeAll()
{
	ENN_SAFE_DELETE(shadow_tech_);
	ENN_SAFE_DELETE(shadow_info_);
}

ShadowTechBase* ShadowInfoWrap::createShadowTech(ShadowType type)
{
	return 0;
}


//////////////////////////////////////////////////////////////////////////
void ShadowTechDirect::init(Light* lit, ShadowInfo* si)
{
	ShadowTechBase::init(lit, si);

	if (si->type_ == ST_SM)
	{
		shadow_info_->setCascades(1);
	}
	else
	{
		shadow_info_->setCascades(MAX_PSSM_CASCADES);
	}
}

void ShadowTechDirect::updateResolution()
{
	if (shadow_info_->type_ == ST_SM)
	{
		shadow_info_->need_rtt_size_ = shadow_info_->resolution_;
		shadow_info_->updateViewport(0);
	}
	else
	{
		int half_size = shadow_info_->resolution_;
		int full_size = half_size * 2;

		shadow_info_->need_rtt_size_ = full_size;

		
		IntRect rc[MAX_PSSM_CASCADES] =
		{
			IntRect(0, 0, half_size, half_size),
			IntRect(half_size, 0, full_size, half_size),
			IntRect(0, half_size, half_size, full_size),
			IntRect(half_size, half_size, full_size, full_size),
		};
		

		shadow_info_->updateViewport(rc);
	}
}

void ShadowTechDirect::updateShadowInfo(Camera* main_cam)
{
	main_camera_ = main_cam;

	shadow_info_->is_curr_active_ = true;

	calcCamCullReceiver();

	calcSplitSchemeWeight();

	int vp_cnt = shadow_info_->render_target_->getViewportCounts();

	Vector2 rangeX[MAX_PSSM_CASCADES], rangeY[MAX_PSSM_CASCADES], rangeZ[MAX_PSSM_CASCADES];

	calcCamCullCaster(vp_cnt, rangeX, rangeY, rangeZ);

	calcRenderCaster(vp_cnt, rangeX, rangeY, rangeZ);

	calcMatrixViewProjBias(vp_cnt);
}

void ShadowTechDirect::calcCamCullReceiver()
{
	Camera* cam_cull_receiver = shadow_info_->getCullReceiver();

	cam_cull_receiver->setTransform(main_camera_->getWorldMatrix());

	cam_cull_receiver->setPerspective(main_camera_->getFov(), main_camera_->getAspect(),
		main_camera_->getZNear(), main_camera_->getZNear() + shadow_info_->visible_range_);
}

void ShadowTechDirect::calcSplitSchemeWeight()
{
	// calculates split plane distances in view space
	// Practical split scheme:
	//
	// CLi = n*(f/n)^(i/numsplits)
	// CUi = n + (f-n)*(i/numsplits)
	// Ci = CLi*(lambda) + CUi*(1-lambda)
	//
	// lambda scales between logarithmic and uniform
	//

	Camera* cull_receiver = shadow_info_->getCullReceiver();

	if (shadow_info_->type_ != ST_PSSM)
	{
		shadow_info_->split_dist_[0] = cull_receiver->getZNear();
		shadow_info_->split_dist_[1] = cull_receiver->getZFar();
		return;
	}

	float weight = shadow_info_->split_scheme_weight_;
	float one_sub_weight = 1 -weight;

	float z_near = cull_receiver->getZNear();
	float z_far = cull_receiver->getZFar();
	float far_div_near = z_far / z_near;
	float far_sub_near = z_far - z_near;

	for (int i = 1; i < MAX_PSSM_CASCADES; ++i)
	{
		float fIDM = i / (float)MAX_PSSM_CASCADES;
		float fLog = z_near * Math::pow(far_div_near, fIDM);
		float fUniform = z_near + far_sub_near * fIDM;

		shadow_info_->split_dist_[i] = fLog * weight + fUniform * one_sub_weight;
	}

	shadow_info_->split_dist_[0] = z_near;
	shadow_info_->split_dist_[MAX_PSSM_CASCADES] = z_far;
}

void ShadowTechDirect::calcCamCullCaster(int splits, Vector2* range_x, Vector2* range_y, Vector2* range_z)
{
	const Vector3& right = light_->getRight();
	const Vector3& up = light_->getUp();
	const Vector3& zdir = -light_->getDirection();

	float max_len = main_camera_->getZFar() / Math::cos(main_camera_->getFov() * 0.5f);
	Vector3 max_pos = main_camera_->getEyePos() + zdir * max_len;

	float z_max = zdir.dotProduct(max_pos);

	Matrix4 mat_cull;

	mat_cull.setColumn(0, right, 0);
	mat_cull.setColumn(1, up, 0);
	mat_cull.setColumn(2, zdir, 0);
	mat_cull.setColumn(3, Vector3::ZERO, 1);

	Camera cam_cull_receive_sub;
	cam_cull_receive_sub.setTransform(main_camera_->getWorldMatrix());

	for (int i = 0; i < splits; ++i)
	{
		cam_cull_receive_sub.setPerspective(main_camera_->getFov(),
			main_camera_->getAspect(), shadow_info_->split_dist_[i], shadow_info_->split_dist_[i + 1]);

		const Vector3* corners = cam_cull_receive_sub.getCorners();

		ObjectAlignedBox::getRange(right, up, zdir, corners, 8, range_x[i], range_y[i], range_z[i]);

		range_z[i].makeRange(z_max);

		Camera& cull_caster = shadow_info_->getCullCaster()[i];

		cull_caster.setTransform(mat_cull);

		cull_caster.setOrtho(range_x[i].x, range_y[i].y, range_x[i].y,
			range_y[i].x, -range_z[i].y, -range_z[i].x);
	}
}

void ShadowTechDirect::calcRenderCaster(int splits, const Vector2* range_x, Vector2* range_y, const Vector2* range_z)
{
	for (int i = 0; i < splits; ++i)
	{
		Vector3 currFullSize( range_x[i].getRange(), range_y[i].getRange(), range_z[i].getRange() );

		max_full_size_[i].makeCeil(currFullSize);

		/** distance per pixel */
		Vector2 unitXY(max_full_size_[i].x / shadow_info_->resolution_,
			max_full_size_[i].y / shadow_info_->resolution_);

		Vector2 newRangeX, newRangeY;

		/** snap min side */
		newRangeX.x = Math::floor(range_x[i].x / unitXY.x) * unitXY.x;
		newRangeY.x = Math::floor(range_y[i].x / unitXY.y) * unitXY.y;
		newRangeX.y = newRangeX.x + max_full_size_[i].x;
		newRangeY.y = newRangeY.x + max_full_size_[i].y;

		/** check if new max side less than old max side */
		
		if (newRangeX.y < range_x[i].y)
		{
			newRangeX.y += unitXY.x;
			max_full_size_[i].x = newRangeX.getRange();
		}

		if (newRangeY.y < range_y[i].y)
		{
			newRangeY.y += unitXY.y;
			max_full_size_[i].y = newRangeY.getRange();
		}
		

		Camera& renderCaster = shadow_info_->getRenderCaster()[i];

		renderCaster.setTransform(shadow_info_->getCullCaster()[i].getWorldMatrix());

		
		renderCaster.setOrtho(newRangeX.x, newRangeY.y, newRangeX.y, newRangeY.x,
			-range_z[i].y, -range_z[i].x);
		
	}
}

void ShadowTechDirect::calcMatrixViewProjBias(int splits)
{
	Matrix4 mat_tex_bias;
	getTexBias(mat_tex_bias, -shadow_info_->bias_);

	if (shadow_info_->type_ == ST_PSSM)
	{
		Matrix4 split_off[4];
		
		split_off[0] = Matrix4::getTransScale( Vector3(0.5f, 0.5f, 1.0f), Vector3(0.0f, 0.0f, 0.0f) );
		split_off[1] = Matrix4::getTransScale( Vector3(0.5f, 0.5f, 1.0f), Vector3(0.5f, 0.0f, 0.0f) );
		split_off[2] = Matrix4::getTransScale( Vector3(0.5f, 0.5f, 1.0f), Vector3(0.0f, 0.5f, 0.0f) );
		split_off[3] = Matrix4::getTransScale( Vector3(0.5f, 0.5f, 1.0f), Vector3(0.5f, 0.5f, 0.0f) );

		for (int i = 0; i < splits; ++i)
		{
			shadow_info_->getMatViewProjTexBias()[i] = split_off[i] *
				mat_tex_bias * shadow_info_->cam_render_caster_[i].getViewProjMatrix();
		}
	}
	else
	{
		ENN_ASSERT(splits == 1);
		*(shadow_info_->getMatViewProjTexBias()) = mat_tex_bias * shadow_info_->cam_cull_caster_->getViewProjMatrix();
	}
}

void ShadowTechDirect::getTexBias(Matrix4& bias_mat, float z_bias)
{
	bias_mat[0][0] = 0.5f;	bias_mat[0][1] = 0;		bias_mat[0][2] = 0;		bias_mat[0][3] = 0.5f;
	bias_mat[1][0] = 0;		bias_mat[1][1] = 0.5f;	bias_mat[1][2] = 0;		bias_mat[1][3] = 0.5f;
	bias_mat[2][0] = 0;		bias_mat[2][1] = 0;		bias_mat[2][2] = 1.0f;	bias_mat[2][3] = z_bias;
	bias_mat[3][0] = 0;		bias_mat[3][1] = 0;		bias_mat[3][2] = 0;		bias_mat[3][3] = 1.0f;
}

}