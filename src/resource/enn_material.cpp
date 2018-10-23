/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/05
* File: enn_material.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_material.h"
#include "enn_root.h"
#include "enn_texture_manager.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// TextureUnit
TextureUnit::TextureUnit()
: addr_u_(TEXADDR_WRAP)
, addr_v_(TEXADDR_WRAP)
, min_filter_(TEXF_MIP_LINEAR)			/** default mip map is enabled */
, mag_filter_(TEXF_LINEAR)
, need_async_(false)
{

}

TextureUnit::~TextureUnit()
{
	
}

const String& TextureUnit::getTexName() const
{
	return tex_name_;
}

void TextureUnit::setTexName(const String& name)
{
	tex_name_ = name;

	if (!tex_name_.empty())
	{
		Texture* tex = static_cast<Texture*>(Root::getSingletonPtr()->get_texture_mgr()->getOrCreateResource(tex_name_));

		tex->setLoadAsync(need_async_);

		Texture::SamplerState& sampler_state = tex->getSamplerState();
		
		sampler_state.address_u_ = addr_u_;
		sampler_state.address_v_ = addr_v_;
		sampler_state.is_mipmap = (min_filter_ == TEXF_MIP_LINEAR) ? true : false;
		sampler_state.mag_filter_ = mag_filter_;
		sampler_state.min_filter_ = min_filter_;
		
		tex->checkLoad();

		tex_.attach(tex);
	}
}

void TextureUnit::setTexNameNoLoading(const String& name)
{
	tex_name_ = name;
}

void TextureUnit::setNeedAsync(bool async)
{
	need_async_ = async;
}

uint32 TextureUnit::getTexAddrU() const
{
	return addr_u_;
}

uint32 TextureUnit::getTexAddrV() const
{
	return addr_v_;
}

void TextureUnit::setTexAddr(uint32 addr_u, uint32 addr_v)
{
	addr_u_ = addr_u;
	addr_v_ = addr_v;
}

uint32 TextureUnit::getTexFilterMin() const
{
	return min_filter_;
}

uint32 TextureUnit::getTexFilterMag() const
{
	return mag_filter_;
}

bool TextureUnit::isMipmap() const
{
	return (min_filter_ == TEXF_MIP_LINEAR) ? true : false;
}

void TextureUnit::setTexFilter(uint32 min_filter, uint32 mag_filter)
{
	min_filter_ = min_filter;
	mag_filter_ = mag_filter;
}

//////////////////////////////////////////////////////////////////////////
// TextureAnimate
TextureAnimate::TextureAnimate(SubMaterial* mtr)
: mtr_(mtr)
, u_speed_(0.0f)
, v_speed_(0.0f)
, u_scale_(1.0f)
, v_scale_(1.0f)
, r_speed_(0.0f)
, slice_frames_(0)
, slice_frames_per_row_(0)
, slice_width_(0.0f)
, slice_height_(0.0f)
, slice_interval_(0.0f)
, rot_off_(0.5f, 0.5f)
{
	
}

TextureAnimate::~TextureAnimate()
{

}

float TextureAnimate::getUSpeed() const
{
	return u_speed_;
}

float TextureAnimate::getVSpeed() const
{
	return v_speed_;
}

float TextureAnimate::getRotSpeed() const
{
	return r_speed_;
}

const vec2f& TextureAnimate::getRotOff() const
{
	return rot_off_;
}

float TextureAnimate::getUScale() const
{
	return u_scale_;
}

float TextureAnimate::getVScale() const
{
	return v_scale_;
}

void TextureAnimate::setUVScale(float u, float v)
{
	u_scale_ = u;
	v_scale_ = v;
}

void TextureAnimate::setUSpeed(float v)
{
	u_speed_ = v;
}

void TextureAnimate::setVSpeed(float v)
{
	v_speed_ = v;
}

void TextureAnimate::setUVSpeed(float u, float v)
{
	u_speed_ = u;
	v_speed_ = v;
}

void TextureAnimate::setRotSpeed(float v)
{
	r_speed_ = v;
}

void TextureAnimate::setRotOff(const vec2f& v)
{
	rot_off_ = v;
}	

int TextureAnimate::getSliceFrames() const
{
	return slice_frames_;
}

int TextureAnimate::getSliceFramesPerRow() const
{
	return slice_frames_per_row_;
}

float TextureAnimate::getSliceWidth() const
{
	return slice_width_;
}

float TextureAnimate::getSliceHeight() const
{
	return slice_height_;
}

float TextureAnimate::getSliceInterval() const
{
	return slice_interval_;
}

void TextureAnimate::setSliceFrames(int frames)
{
	slice_frames_ = frames;
}

void TextureAnimate::setSliceFramesPerRow(int frames)
{
	slice_frames_per_row_ = frames;
}

void TextureAnimate::setSliceWidth(float width )
{
	slice_width_ = width;
}

void TextureAnimate::setSliceHeight(float height)
{
	slice_height_ = height;
}

void TextureAnimate::setSliceInterval(float itv)
{
	slice_interval_ = itv;
}

void TextureAnimate::update(float elapsedTime, TextureAniState& ani_state)
{
	if (!ani_state.isEnable())
	{
		return;
	}

	if (!Math::realEqual(r_speed_, 0.0f))
	{
		ani_state.rot_ = wrapRot(ani_state.rot_ + r_speed_ * elapsedTime);
	}

	if (!Math::realEqual(u_speed_, 0.0f))
	{
		ani_state.u_ = wrapTrans(ani_state.u_ + u_speed_ * elapsedTime);
	}

	if (!Math::realEqual(v_speed_, 0.0f))
	{
		ani_state.v_ = wrapTrans(ani_state.v_ + v_speed_ * elapsedTime);
	}

	if (slice_frames_ > 0)
	{
		ani_state.time_ += elapsedTime;
		ani_state.frame_ = (int)(ani_state.time_ / slice_interval_);
		ani_state.frame_ = ani_state.frame_ % slice_frames_;
	}

	ani_state.updateMatrix();
}

void TextureAnimate::setFrameTime(float time, TextureAniState& ani_state)
{
	if (slice_frames_ > 0)
	{
		ani_state.time_ = time;
		ani_state.frame_ = (int)(ani_state.time_ / slice_interval_);
		ani_state.frame_ = ani_state.frame_ % slice_frames_;

		ani_state.updateMatrix();
	}
}

float TextureAnimate::wrapTrans(float x)
{
	while ( x < 0.0f )
	{
		x += 1.0f;
	}

	while ( x > 1.0f )
	{
		x -= 1.0f;
	}

	return x;
}

float TextureAnimate::wrapRot(float r)
{
	while ( r < 0.0f )
	{
		r += static_cast<float>(Math::TWO_PI);
	}

	while ( r > static_cast<float>(Math::TWO_PI) )
	{
		r -= static_cast<float>(Math::TWO_PI);
	}

	return r;
}


//////////////////////////////////////////////////////////////////////////
// TextureAniState
TextureAniState::TextureAniState(TextureAnimate* ani)
: ani_(ani)
, u_(0.0f)
, v_(0.0f)
, rot_(0.0f)
, time_(0.0f)
, frame_(0)
, is_enable_(false)
{
	
}

TextureAniState::~TextureAniState()
{

}

const Matrix4& TextureAniState::getMatrix() const
{
	return mat_;
}

Matrix3 TextureAniState::getMatrix3() const
{
	return mat_.toMatrix3();
}

void TextureAniState::update(float elapsedTime)
{
	if (ani_)
	{
		ani_->update(elapsedTime, *this);
	}
}

void TextureAniState::enable(bool en)
{
	is_enable_ = en;
}

bool TextureAniState::isEnable() const
{
	return is_enable_;
}

void TextureAniState::updateMatrix()
{
	mat_ = Matrix4::IDENTITY;

	if (!ani_)
	{
		return;
	}

	float u_scale = ani_->getUScale();
	float v_scale = ani_->getVScale();

	// first scale, then rotate, the last translate.
	// scale
	if (!Math::realEqual(u_scale, 1.0f) || !Math::realEqual(v_scale, 1.0f))
	{
		Matrix4 um;
		um.setScale(vec3f(u_scale, v_scale, 1.0f));
		mat_ = um;
	}

	// rotate
	if (!Math::realEqual(ani_->getRotSpeed(), 0.0f))
	{
		/** -0.5f */
		Matrix4 off;
		off.setTrans(vec3f(-ani_->getRotOff(), 0.0f));

		// rot
		Quaternion q;
		q.fromAngleAxis(rot_, vec3f(0.0f, 0.0f, 1.0f));
		Matrix3 mq;
		q.toRotationMatrix(mq);

		/** 0.5f */
		Matrix4 off1;
		off1.setTrans(vec3f(ani_->getRotOff(), 0.0f));

		mat_ = off1 * Matrix4(mq) * off * mat_;
	}

	/** uv offset */
	if (!Math::realEqual(ani_->getUSpeed(), 0.0f) || !Math::realEqual(ani_->getVSpeed(), 0.0f))
	{
		Matrix4 mt;
		mt.setTrans(vec3f(u_, v_, 0.0f));

		mat_ = mt * mat_;
	}

	/** frame animation. */
	if (ani_->getSliceFrames() > 0)
	{
		int fpr = ani_->getSliceFramesPerRow();
		int fpc = ani_->getSliceFrames() / fpr;

		float subw = ani_->getSliceWidth();
		float subh = ani_->getSliceHeight();

		if (fpr > 0 && subw > 0 && subh > 0)
		{
			float u1 = (frame_ % fpr) / (float)fpr;
			float v1 = (frame_ / fpr) / (float)fpc;

			Matrix4 mt;
			mt.setTrans(vec3f(u1, v1, 0.0f));

			mat_ = mt * mat_;
		}
	}
}

void TextureAniState::setTexAni(TextureAnimate* ani)
{
	ani_ = ani;
}

TextureAnimate* TextureAniState::getTexAni() const
{
	return ani_;
}

//////////////////////////////////////////////////////////////////////////
// SubMaterial
SubMaterial::SubMaterial(Material* parent)
: is_lighting_enable_(false)
, is_point_light_enable_(1)
, is_spot_light_enable_(1)
, is_enable_fog_(0)
, is_render_homogeneous_(0)
, is_wire_frame_(0)
, is_front_face_(0)
, tex_ani_(0)
, cull_mode_(CULL_CCW)
, custom_alpha_value_(1.0f)
, custom_alpha_ref_(1.0f)
, parent_(parent)
, m_texDiff(0)
, m_texDiff2(0)
, m_texNormal(0)
, m_texSpecular(0)
, m_texEmissive(0)
, tex_clr_(0)
, emissive_(Color::BLACK)
{

}

SubMaterial::~SubMaterial()
{
	clearTexAnimate();
	clearTexDiff();
	clearTexDiff2();
	clearTexNormal();
	clearTexSpecular();
	clearTexEmissive();
	clearTexClr();
}

const String& SubMaterial::getName() const
{
	return name_;
}

void SubMaterial::setName(const String& name)
{
	name_ = name;
}

const Color& SubMaterial::getDiffuse() const
{
	return diffuse_;
}

Color& SubMaterial::getDiffuse()
{
	return diffuse_;
}

const Color& SubMaterial::getSpecularShiness() const
{
	return specular_;
}

Color& SubMaterial::getSpecularShiness()
{
	return specular_;
}

const Color& SubMaterial::getEmissive() const
{
	return emissive_;
}

Color& SubMaterial::getEmissive()
{
	return emissive_;
}

void SubMaterial::setDiffuse(const Color& clr)
{
	diffuse_ = clr;
}

void SubMaterial::setSpecularShiness(const Color& clr)
{
	specular_ = clr;
}

void SubMaterial::setEmissive(const Color& clr)
{
	emissive_ = clr;
}

bool SubMaterial::isEnableLighting() const
{
	return is_lighting_enable_;
}

void SubMaterial::enableLighting(bool en)
{
	is_lighting_enable_ = en;
}

bool SubMaterial::isEnablePointLighting() const
{
	return is_point_light_enable_;
}

void SubMaterial::enablePointLighting(bool en)
{
	is_point_light_enable_ = en;
}

bool SubMaterial::isEnableSpotLighting() const
{
	return is_spot_light_enable_;
}

void SubMaterial::enableSpotLighting(bool en)
{
	is_spot_light_enable_ = en;
}

bool SubMaterial::isEnableFog() const
{
	return is_enable_fog_;
}

void SubMaterial::enableFog(bool en)
{
	is_enable_fog_ = en;
}

bool SubMaterial::hasSpecular() const
{
	if (!Math::realEqual(specular_.r, 0.0f)
		|| !Math::realEqual(specular_.g, 0.0f)
		|| !Math::realEqual(specular_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool SubMaterial::hasEmissive() const
{
	if (!Math::realEqual(emissive_.r, 0.0f)
		|| !Math::realEqual(emissive_.g, 0.0f)
		|| !Math::realEqual(emissive_.b, 0.0f))
	{
		return true;
	}

	return false;
}

bool SubMaterial::hasAlpha() const
{
	if (!Math::realEqual(custom_alpha_value_, 1.0f))
	{
		return true;
	}

	return false;
}

float SubMaterial::getAlpha() const
{
	return custom_alpha_value_;
}

void SubMaterial::setAlpha(float a)
{
	custom_alpha_value_ = a;
}

void SubMaterial::setCullMode(uint32 cm)
{
	cull_mode_ = cm;
}

uint32 SubMaterial::getCullMode() const
{
	return cull_mode_;
}

void SubMaterial::setRenderHomogeneous(bool b)
{
	is_render_homogeneous_ = b;
}

bool SubMaterial::isRenderHomogeneous() const
{
	return is_render_homogeneous_;
}

void SubMaterial::setWireFrame(bool en)
{
	is_wire_frame_ = en;
}

bool SubMaterial::isWireFrame() const
{
	return is_wire_frame_;
}

void SubMaterial::setFrontFace(bool is_ccw)
{
	is_front_face_ = is_ccw;
}

bool SubMaterial::isFrontFace() const
{
	return is_front_face_;
}

void SubMaterial::setAlphaTest(float a)
{
	custom_alpha_ref_ = a;
}

float SubMaterial::getAlphaTest() const
{
	return custom_alpha_ref_;
}

bool SubMaterial::hasAlphaTest() const
{
	return (!Math::realEqual(custom_alpha_ref_, 1.0f));
}

void SubMaterial::disableAlphaTest()
{
	custom_alpha_ref_ = 1.0f;
}

void SubMaterial::setTexClr(const Color& clr)
{
	if (!tex_clr_)
	{
		tex_clr_ = ENN_NEW_T(Color);
	}

	*tex_clr_ = clr;
}

Color* SubMaterial::getTexClr() const
{
	return tex_clr_;
}

void SubMaterial::clearTexClr()
{
	ENN_SAFE_DELETE_T(tex_clr_);
}

#define IMPL_METHOD_USE_TEX_(name) \
	TextureUnit* SubMaterial::useTex##name() \
{ \
	if ( m_tex##name ) \
	return m_tex##name; \
	m_tex##name = ENN_NEW TextureUnit; \
	return m_tex##name; \
} \
	void SubMaterial::clearTex##name() \
{ \
	if ( m_tex##name ) \
{ \
	ENN_DELETE m_tex##name; \
	m_tex##name = 0; \
} \
}

IMPL_METHOD_USE_TEX_(Diff)
IMPL_METHOD_USE_TEX_(Diff2)
IMPL_METHOD_USE_TEX_(Normal)
IMPL_METHOD_USE_TEX_(Specular)
IMPL_METHOD_USE_TEX_(Emissive)

#undef IMPL_METHOD_USE_TEX_

TextureAnimate* SubMaterial::useTexAnimate()
{
	if (tex_ani_)
	{
		return tex_ani_;
	}

	tex_ani_ = ENN_NEW TextureAnimate(this);
	return tex_ani_;
}

void SubMaterial::clearTexAnimate()
{
	ENN_SAFE_DELETE(tex_ani_);
}

//////////////////////////////////////////////////////////////////////////
// Material
Material::Material()
: ref_count_(1)
{
	
}

Material::~Material()
{
	destroyAllSubMaterial();
}

SubMaterial* Material::createSubMaterial(int idx)
{
	SubMaterialList::iterator i = sub_material_list_.find(idx);
	ENN_ASSERT(i == sub_material_list_.end());

	SubMaterial* sub_mtr = new SubMaterial(this);

	sub_material_list_.insert(SubMaterialList::value_type(idx, sub_mtr));

	return sub_mtr;
}

void Material::addSubMaterial(int idx, SubMaterial* sub_mat)
{
	SubMaterialList::iterator i = sub_material_list_.find(idx);
	ENN_ASSERT(i == sub_material_list_.end());

	sub_material_list_.insert(SubMaterialList::value_type(idx, sub_mat));
}

SubMaterial* Material::getSubMaterial(int idx) const
{
	SubMaterialList::const_iterator i = sub_material_list_.find(idx);
	ENN_ASSERT(i != sub_material_list_.end());

	return i->second;
}

void Material::destroySubMaterial(int idx)
{
	SubMaterialList::iterator i = sub_material_list_.find(idx);
	ENN_ASSERT(i != sub_material_list_.end());

	if (i != sub_material_list_.end())
	{
		SubMaterial* sm = i->second;
		ENN_SAFE_DELETE(sm);

		sub_material_list_.erase(i);
	}
}

void Material::destroyAllSubMaterial()
{
	ENN_FOR_EACH(SubMaterialList, sub_material_list_, it)
	{
		SubMaterial* sm = it->second;
		ENN_SAFE_DELETE(sm);
	}

	sub_material_list_.clear();
}

int Material::getSubMtrCount() const
{
	return sub_material_list_.size();
}

void Material::release()
{
	if (--ref_count_ == 1)
	{
		ENN_DELETE this;
	}
}

int Material::firstMtrIdx()
{
	mtr_curr_iter_ = sub_material_list_.begin();
	return nextMtrIdx();
}

int Material::nextMtrIdx()
{
	int idx = -1;

	if (mtr_curr_iter_ != sub_material_list_.end())
	{
		idx = mtr_curr_iter_->first;
		++mtr_curr_iter_;
	}

	return idx;
}

}