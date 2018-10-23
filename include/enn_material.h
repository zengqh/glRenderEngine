/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/05
* File: enn_material.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_resource.h"
#include "enn_texture.h"

namespace enn
{
//////////////////////////////////////////////////////////////////////////
// TextureUnit
class TextureUnit : public AllocatedObject
{
public:
	TextureUnit();
	~TextureUnit();

public:
	const String& getTexName() const;
	void setTexName(const String& name);

	void setTexNameNoLoading(const String& name);

	void setNeedAsync(bool async);

	uint32 getTexAddrU() const;
	uint32 getTexAddrV() const;

	void setTexAddr(uint32 addr_u, uint32 addr_v);

	uint32 getTexFilterMin() const;
	uint32 getTexFilterMag() const;
	bool isMipmap() const;

	void setTexFilter(uint32 min_filter, uint32 mag_filter);

	Texture* getTex() const
	{
		return tex_.get();
	}

protected:
	String		tex_name_;
	TexturePtr	tex_;
	uint32		addr_u_;
	uint32		addr_v_;
	uint32		min_filter_;
	uint32		mag_filter_;
	bool		need_async_;
};

//////////////////////////////////////////////////////////////////////////
// TextureAnimate
class TextureAniState;
class SubMaterial;
class TextureAnimate : public AllocatedObject
{
public:
	TextureAnimate(SubMaterial* mtr);
	virtual ~TextureAnimate();

public:
	float getUSpeed() const;
	float getVSpeed() const;
	float getRotSpeed() const;
	const vec2f& getRotOff() const;
	float getUScale() const;
	float getVScale() const;

	void setUVScale(float u, float v);

public:
	void setUSpeed(float v);
	void setVSpeed(float v);

	void setUVSpeed(float u = 1.0f, float v = 1.0f);
	void setRotSpeed(float v);
	void setRotOff(const vec2f& v);

public:
	int getSliceFrames() const;
	int getSliceFramesPerRow() const;
	float getSliceWidth() const;
	float getSliceHeight() const;
	float getSliceInterval() const;

public:
	void setSliceFrames(int frames);       
	void setSliceFramesPerRow(int frames); 
	void setSliceWidth(float width );
	void setSliceHeight(float height);
	void setSliceInterval(float itv);

public:
	void update(float elapsedTime, TextureAniState& ani_state);
	void setFrameTime(float time, TextureAniState& ani_state);

protected:
	float wrapTrans(float x);
	float wrapRot(float r);

protected:
	SubMaterial*		mtr_;
	float				u_speed_;
	float				v_speed_;
	float				u_scale_;
	float				v_scale_;
	float				r_speed_;
	vec2f				rot_off_;

	int					slice_frames_;
	int					slice_frames_per_row_;
	float				slice_width_;
	float				slice_height_;
	float				slice_interval_;
};

//////////////////////////////////////////////////////////////////////////
// TextureAniState
class TextureAniState : public AllocatedObject
{
	friend class TextureAnimate;
public:
	TextureAniState(TextureAnimate* ani);
	virtual ~TextureAniState();

public:
	const Matrix4& getMatrix() const;
	Matrix3 getMatrix3() const;

public:
	void update(float elapsedTime);

	void enable(bool en);

	bool isEnable() const;

protected:
	void updateMatrix();

public:
	void setTexAni(TextureAnimate* ani);
	TextureAnimate* getTexAni() const;

protected:
	TextureAnimate*		ani_;
	Matrix4				mat_;

	float				u_;
	float				v_;
	float				rot_;

	float				time_;
	int					frame_;

	bool				is_enable_;
};

//////////////////////////////////////////////////////////////////////////
// SubMaterial
class Material;
class SubMaterial : public AllocatedObject
{
public:
	SubMaterial(Material* parent);
	~SubMaterial();

public:
	const String& getName() const;
	void setName(const String& name);

	const Color& getDiffuse() const;
	Color& getDiffuse();

	const Color& getSpecularShiness() const;
	Color& getSpecularShiness();

	const Color& getEmissive() const;
	Color& getEmissive();

	void setDiffuse(const Color& clr);
	void setSpecularShiness(const Color& clr);
	void setEmissive(const Color& clr);

public:
	bool isEnableLighting() const;
	void enableLighting(bool en);

	bool isEnablePointLighting() const;
	void enablePointLighting(bool en);

	bool isEnableSpotLighting() const;
	void enableSpotLighting(bool en);

	bool isEnableFog() const;
	void enableFog(bool en);

public:
	bool hasSpecular() const;
	bool hasEmissive() const;
	bool hasAlpha() const;

	float getAlpha() const;
	void setAlpha(float a);

	void setCullMode(uint32 cm);
	uint32 getCullMode() const;

	void setRenderHomogeneous(bool b);
	bool isRenderHomogeneous() const;

	void setWireFrame(bool en);
	bool isWireFrame() const;

	void setFrontFace(bool is_ccw = true);
	bool isFrontFace() const;

	void setAlphaTest(float a);
	float getAlphaTest() const;
	bool hasAlphaTest() const;
	void disableAlphaTest();

	void setTexClr(const Color& clr);
	Color* getTexClr() const;
	void clearTexClr();

/** diffuse, light, normal, specular map support. */
#define DEF_METHOD_USE_TEX_(name) \
public:	\
	TextureUnit* useTex##name(); \
	void clearTex##name(); \
	const TextureUnit* getTex##name() const { return m_tex##name; } \
	TextureUnit* getTex##name() { return m_tex##name; } \
protected: \
	TextureUnit* m_tex##name;

	DEF_METHOD_USE_TEX_(Diff)
	DEF_METHOD_USE_TEX_(Diff2)
	DEF_METHOD_USE_TEX_(Normal)
	DEF_METHOD_USE_TEX_(Specular)
	DEF_METHOD_USE_TEX_(Emissive)

#undef DEF_METHOD_USE_TEX_

/** texture animation support. */
#define DEF_METHOD_USE_XX_(xx, t, v) \
public:	\
	t* use##xx(); \
	void clear##xx(); \
	const t* get##xx() const { return v; } \
	t* get##xx() { return v; }

	DEF_METHOD_USE_XX_(TexAnimate, TextureAnimate, tex_ani_)

#undef DEF_METHOD_USE_XX_

protected:
	String			name_;
	Color			diffuse_;
	Color			specular_;
	Color			emissive_;

	Color*			tex_clr_;			

	bool			is_lighting_enable_;
	bool			is_enable_fog_;

	bool			is_point_light_enable_;
	bool			is_spot_light_enable_;

	bool			is_render_homogeneous_;
	bool			is_wire_frame_;
	bool			is_front_face_;

	TextureAnimate*	tex_ani_;
	int				cull_mode_;
	float			custom_alpha_value_;
	float			custom_alpha_ref_;

	Material*		parent_;
};

//////////////////////////////////////////////////////////////////////////
enum MaterialPass
{
	MP_MATERIAL,
	MP_SHADOW,
	MP_REFLECT,
	MP_DEFPRE,
	MP_MAX,
	MP_LITACC
};

enum 
{
	ET_DEF_MATERIAL = 0,
	ET_DEF_SHADOW   = 1,
	ET_DEF_REFLECT  = 2,
	ET_DEF_DEFPRE   = 3,
	ET_DEF_UI       = 10,
	ET_DEF_LITACC   = 11,
	ET_DEF_LIT		= 12,
	ET_DEF_FORWARD	= 13
};

//////////////////////////////////////////////////////////////////////////
// Material
class Material : public AllocatedObject, public Noncopyable
{
public:
	typedef enn::map<int, SubMaterial*>::type SubMaterialList;

	Material();
	~Material();

public:
	SubMaterial* createSubMaterial(int idx);
	void addSubMaterial(int idx, SubMaterial* sub_mat);

public:
	SubMaterial* getSubMaterial(int idx) const;
	void destroySubMaterial(int idx);
	void destroyAllSubMaterial();

	int getSubMtrCount() const;

	int firstMtrIdx();
	int nextMtrIdx();

public:
	void addRef() 
	{
		++ref_count_; 
	}

	void release();

protected:
	SubMaterialList						sub_material_list_;
	uint								ref_count_;
	SubMaterialList::iterator			mtr_curr_iter_;
};

}