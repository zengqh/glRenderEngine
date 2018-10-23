/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/07/21
* File: enn_shadow.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_math.h"

namespace enn
{
class Light;
class DirectionLight;
class Camera;
class TextureObj;
class RenderBuffer;
class RenderTarget;

enum ShadowType
{
	ST_NONE,
	ST_SM,
	ST_PSSM
};

const int MAX_POISSON_DISK = 8;
const int MAX_SHADOW_TAPGROUPS = MAX_POISSON_DISK / 2;

const int MAX_PSSM_CASCADES = 4;

//////////////////////////////////////////////////////////////////////////
struct ShadowInfo : public AllocatedObject
{
public:
	ShadowInfo();
	~ShadowInfo();

	Camera* getCullCaster();
	Camera* getRenderCaster();
	Camera* getCullReceiver();

	Vector2 getFade() const;
	Matrix4* getMatViewProjTexBias();
	int getCascadesCount() const;
	void setCascades(int count);

	void updateViewport(const IntRect* rect);

	void link_rtt(TextureObj* tex, RenderBuffer* depth);

public:
	ShadowType		type_;
	int				resolution_;
	int				tap_groups_;
	float			visible_range_;
	float			fade_range_;
	float			split_scheme_weight_;
	float			blur_size_;
	float			strength_;
	float			bias_;

	Camera*			cam_cull_caster_;			/** shadowmap camera for culling caster */
	Camera*			cam_render_caster_;			/** shadowmap camera for rendering caster */
	Camera*			cam_receiver_caster_;		/** shadowmap camera for culling receiver */

	Matrix4*		view_proj_tex_bias_;
	TextureObj*		texture_;

	float			split_dist_[MAX_PSSM_CASCADES + 1];
	int				need_rtt_size_;
	bool			is_curr_active_;

	RenderTarget*	render_target_;
};

//////////////////////////////////////////////////////////////////////////
class ShadowTechBase : public AllocatedObject
{
public:
	ShadowTechBase()
		: light_(0)
		, shadow_info_(0)
	{

	}

	virtual ~ShadowTechBase()
	{
		
	}

	virtual void init(Light* lit, ShadowInfo* si)
	{
		light_ = lit;
		shadow_info_ = si;
	}

	virtual void updateResolution()
	{
		shadow_info_->need_rtt_size_ = shadow_info_->resolution_;
		shadow_info_->updateViewport(0);
	}

	virtual void updateShadowInfo(Camera* main_cam) = 0;

protected:
	Light*			light_;
	ShadowInfo*		shadow_info_;
};

//////////////////////////////////////////////////////////////////////////
class ShadowInfoWrap
{
public:
	ShadowInfoWrap() : shadow_info_(0), shadow_tech_(0)
	{

	}

	virtual ~ShadowInfoWrap()
	{
		freeAll();
	}
	
public:
	void setShadowType(ShadowType type);
	void setShadowResolution(int resolution);
	void setShadowTapGroups(int tag_groups);
	void setShadowVisibleRange(float range);
	void setShadowFadeRange(float range);
	void setShadowSplitSchemeWeight(float weight);
	void setShadowBlurSize(float bs);
	void setShadowStrength(float strength);
	void setShadowBias(float bias);

	ShadowType getShadowType();
	int getShadowResolution();
	int getShadowTapGroups();
	float getShadowVisibleRange();
	float getShadowFadeRange();
	float getShadowSplitSchemeWeight();
	float getShadowBlurSize();
	float getShadowStrength();
	float getShadowBias();

public:
	void updateShadowInfo(Camera* main_cam);
	bool isShadowCurrActive() const;
	ShadowInfo* getShadowInfo();

protected:
	void freeAll();
	virtual Light* getHost() = 0;
	virtual ShadowTechBase* createShadowTech(ShadowType type);

protected:
	ShadowInfo*			shadow_info_;
	ShadowTechBase*		shadow_tech_;
};

//////////////////////////////////////////////////////////////////////////
class ShadowTechDirect : public ShadowTechBase
{
public:
	ShadowTechDirect()
		: main_camera_(0)
	{

	}

	virtual void init(Light* lit, ShadowInfo* si);
	virtual void updateResolution();
	virtual void updateShadowInfo(Camera* main_cam);

private:
	void calcCamCullReceiver();
	void calcSplitSchemeWeight();
	void calcCamCullCaster(int splits, Vector2* range_x, Vector2* range_y, Vector2* range_z);
	void calcRenderCaster(int splits, const Vector2* range_x, Vector2* range_y, const Vector2* range_z);
	void calcMatrixViewProjBias(int splits);
	void getTexBias(Matrix4& bias_mat, float z_bias);

private:
	Camera*		main_camera_;
	Vector3		max_full_size_[MAX_PSSM_CASCADES];
};

}