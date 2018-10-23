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
// File: enn_water_pass.cpp
//

#include "enn_water_pass.h"
#include "enn_render_pipe.h"
#include "enn_root.h"

namespace enn
{

#define	REFLECT_WIDTH 512

WaterPass::WaterPass(RenderPipe* render_pipe)
: RenderPassBase(render_pipe)
, main_cam_(0)
, water_eff_(0)
, refract_tex_(0)
, is_prepared_(false)
{

}

WaterPass::~WaterPass()
{

}

void WaterPass::prepare()
{
	if (is_prepared_)
	{
		return;
	}

	is_prepared_ = true;
}

void WaterPass::render(Camera* cam)
{
	main_cam_ = cam;

	check_reflect_rtt();
}

void WaterPass::begin_insert_obj()
{
	water_plane_list_.clear();
	water_plane_desc_list_.clear();
}

void WaterPass::insert_obj(RenderableNode* node)
{
	water_plane_list_.push_back(node);

	WaterPlaneNode* water_node = static_cast<WaterPlaneNode*>(node);

	ENN_ASSERT(water_node);

	WaterPlane* water_plane = water_node->getWaterPlane();

	ENN_ASSERT(water_plane);

	// should be world space
	float plane_height = node->getPosition().y;

	water_plane->water_plane_desc_ = insert_plane(plane_height);
}

void WaterPass::end_insert_obj()
{
	
}

void WaterPass::render_reflect()
{
	for (size_t i = 0; i < water_plane_desc_list_.size(); ++i)
	{
		WaterPlaneDesc* plane = &water_plane_desc_list_[i];

		plane->reflect_tex = water_reflect_rtt_list_[i];

		render_plane(plane);
	}
}

void WaterPass::render_water()
{
	refract_tex_ = render_pipe_->getPost0()->get_rtt(0);

	if (!water_eff_)
	{
		water_eff_ = getRenderEffect("water_plane_vs.h", "water_plane_ps.h");
	}

	ENN_FOR_EACH(WaterPlaneList, water_plane_list_, it)
	{
		WaterPlaneNode* node = static_cast<WaterPlaneNode*>(*it);

		render_water_obj(node);
	}
}

WaterPlaneDesc* WaterPass::insert_plane(float plane)
{
	for (size_t i = 0; i < water_plane_desc_list_.size(); ++i)
	{
		WaterPlaneDesc* plane_desc = &water_plane_desc_list_[i];

		if (Math::realEqual(plane_desc->plane, plane, 0.01f))
		{
			return plane_desc;
		}
	}

	WaterPlaneDesc desc;
	desc.plane = plane;
	desc.reflect_tex = 0;

	water_plane_desc_list_.push_back(desc);

	return &water_plane_desc_list_.back();
}

void WaterPass::render_water_obj(WaterPlaneNode* water_node)
{
	RenderSystem* rs = RENDER_SYS;

	rs->setProgram(water_eff_);

	WaterPlane* water_plane = water_node->getWaterPlane();

	Mesh* mesh = water_plane->water_mesh_;
	SubMesh* sub_mesh = mesh->getSubMesh(0);

	VertexBuffer* vb = sub_mesh->getVB();
	IndexBuffer* ib = sub_mesh->getIB();

	rs->setVB(vb);
	rs->setIB(ib);

	set_water_params(water_node);

	uint32 element_mask = sub_mesh->getVertexType();
	rs->setFVF(element_mask, true);

	rs->drawIndexedPrimitives(sub_mesh->getPrimType(), 0, 
		sub_mesh->getIndexCount(), 
		sub_mesh->getIdxType() == IDX_16 ? true : false);

	rs->setFVF(element_mask, false);
}

void WaterPass::render_plane(WaterPlaneDesc* plane)
{
	const Vector3& eye_pos = main_cam_->getEyePos();
	Vector3 look_at = main_cam_->getLookDir().normalisedCopy() * main_cam_->getZFar();
	Vector3 up_dir = main_cam_->getUpDir();

	Plane water_plane(Vector3::UNIT_Y, Vector3(0, plane->plane, 0));

	Vector3 reflect_eye_pos = water_plane.reflect(eye_pos);
	Vector3 reflect_look_at = water_plane.reflect(look_at);

	Camera reflect_cam;

	Matrix4 reflect_mat;
	Math::makeTransformLookAt(reflect_mat, reflect_eye_pos, reflect_look_at, up_dir);

	reflect_cam.setTransform(reflect_mat);
	reflect_cam.setPerspective(main_cam_->getFov(), main_cam_->getAspect(), main_cam_->getZNear(), main_cam_->getZFar());

	plane->reflect_view_proj = reflect_cam.getViewProjMatrix();

	// setup rtt
	plane->reflect_tex->begin_render();

	reflect_vp_.apply();
	render_pipe_->render_reflect(&reflect_cam);

	plane->reflect_tex->end_render();

	ROOTPTR->get_render_system()->restore_main_target();

	if (0)
	{
		plane->reflect_tex->get_rtt(0)->saveFile("e:/1.bmp");
	}
}

void WaterPass::check_reflect_rtt()
{
	int need_rtt_nums = (int)water_plane_desc_list_.size() - (int)water_reflect_rtt_list_.size();
	
	RenderSystem* rs = RENDER_SYS;
	if (need_rtt_nums > 0)
	{
		for (size_t i = 0; i < need_rtt_nums; ++i)
		{
			RenderTarget* rtt = RenderTarget::createInstance();

			TextureObj* tex = rs->getRttPool()->getRTTBuffer(U4_RGBA, REFLECT_WIDTH, REFLECT_WIDTH);

			RttPool::ItemTemp depth_temp = rs->getRttPool()->getDepthBufferTemp(D24, REFLECT_WIDTH, REFLECT_WIDTH);

			rtt->link_rtt(0, tex);
			rtt->link_depth(depth_temp->depth_);

			water_reflect_rtt_list_.push_back(rtt);
		}

		reflect_vp_.makeRect(0, 0, REFLECT_WIDTH, REFLECT_WIDTH);
		reflect_vp_.setClearColor(Color::BLACK);
	}
}

void WaterPass::set_water_params(WaterPlaneNode* water_node)
{
	RenderSystem* rs = RENDER_SYS;

	WaterPlane* water_plane = water_node->getWaterPlane();

	const Matrix4& water_world_matrix = water_node->getDerivedMatrix();
	const Matrix4& view_proj = main_cam_->getViewProjMatrix();
	const Vector3& eye_pos = main_cam_->getEyePos();

	Matrix4 matWVP = view_proj * water_world_matrix;

	water_eff_->setMatrix4("matWVP", matWVP.transpose()[0]);
	water_eff_->setFloatVector("eye_pos", eye_pos.ptr(), 3);

	Vector4 water_rot, water_rot2, water_offset;

	float tick = ROOTPTR->get_time_system()->getElapsedTime();

	calc_rot_offset(tick * 1000, water_plane, 0, water_rot, (Vector2*)&water_offset.x);
	calc_rot_offset(tick * 1000, water_plane, 1, water_rot2, (Vector2*)&water_offset.z);

	water_eff_->setFloatVector("water_rot", water_rot.ptr(), 4);
	water_eff_->setFloatVector("water_rot2", water_rot2.ptr(), 4);
	water_eff_->setFloatVector("water_offset", water_offset.ptr(), 4);

	water_eff_->setFloatVector("water_color", water_plane->water_color_.ptr(), 3);
	water_eff_->setFloatVector("water_info", Vector4(1.0f, water_plane->water_reflect_turb_, 
		water_plane->water_refract_turb_, water_plane->water_refract_fade_).ptr(), 4);

	// fresnel_info
	water_eff_->setFloatVector("fresnel_info", Vector2(water_plane->fresnel_power_, 
		water_plane->fresnel_scale_).ptr(), 2);

	Matrix4 reflect_mvp = water_plane->water_plane_desc_->reflect_view_proj * water_world_matrix;
	water_eff_->setMatrix4("reflect_wvp", reflect_mvp.transpose()[0]);

	// sun light
	LightsInfo* lights_info = water_node->getLightsInfo();

	LightsInfo::LightList* dir_light_list = lights_info->getLightList(LT_DIRECTIONAL);

	Light* main_dir_light = (*dir_light_list)[0].lit;

	const vec3f& light_direction = -main_dir_light->getDirection();
	const vec3f& diffuse = main_dir_light->getDiffuse().getAsVector3();

	water_eff_->setFloatVector("light_dir", light_direction.ptr(), 3);
	water_eff_->setFloatVector("light_color", diffuse.ptr(), 3);
	water_eff_->setFloat("specular_power", water_plane->specular_power_);

	// set texture
	rs->setTexture(0, water_plane->normal_tex_->getTex()->getTextureObj());
	rs->setTexture(1, refract_tex_);
	rs->setTexture(2, water_plane->water_plane_desc_->reflect_tex->get_rtt(0));

	water_eff_->setInt("normal_map_tex", 0);
	water_eff_->setInt("refract_tex", 1);
	water_eff_->setInt("reflect_tex", 2);
}

void WaterPass::calc_rot_offset(int tick, WaterPlane* water, int i, Vector4& rot, Vector2* offset)
{
	rot.x = Math::cos(water->water_rot_[i]);
	rot.y = Math::sin(water->water_rot_[i]);
	rot.z = -rot.y;
	rot.w = rot.x;

	rot *= water->water_scale_[i];
	water->water_offset_[i] += water->water_speed_[i] * tick;

	if(water->water_offset_[i] > 100.0f) water->water_offset_[i] -= 100.0f;
	if(water->water_offset_[i] < -100.0f) water->water_offset_[i] += 100.0f;

	*offset = Vector2(Math::cos(water->flow_dir_[i]), Math::sin(water->flow_dir_[i])) * water->water_offset_[i];
}

Program* WaterPass::getRenderEffect(const String& vs_file, const String& ps_file)
{
	String def_code, vs_code, ps_code;

	String base_path = Root::getSingletonPtr()->get_file_system()->getRootPath();
	base_path += "/shader/";
	shader_getFileData(base_path + "eff_comm_def.h", def_code);
	shader_getFileData(base_path + vs_file, vs_code);
	shader_getFileData(base_path + ps_file, ps_code);

	String str_vs = makeMacro(false) + def_code + vs_code;
	String str_ps = makeMacro(true) + def_code + ps_code;

	Program* pro = Program::createInstance();
	if (!pro->load(str_vs.c_str(), str_vs.size(), str_ps.c_str(), str_ps.size()))
	{
		ENN_ASSERT(0);
		pro->release();

		return 0;
	}

	return pro;
}

String WaterPass::makeMacro(bool is_ps)
{
#define DEFMAC(x)  strMacro = strMacro + "#define " + #x + "\n";

	String strMacro;

#if ENN_PLATFORM == ENN_PLATFORM_WIN32
	DEFMAC(ENN_PLATFORM_WIN32);
#endif

	return strMacro;
}


}