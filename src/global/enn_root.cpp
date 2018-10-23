/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: enn_root.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "enn_root.h"
#include "enn_resource_manager.h"
#include "enn_file_system.h"
#include "enn_gl_render_system.h"
#include "enn_msg_queue.h"
#include "enn_texture_manager.h"
#include "enn_mesh_manager.h"
#include "enn_work_queue.h"
#include "enn_render_pipe.h"

namespace enn
{
template <> Root* Singleton<Root>::singleton_ = 0;

Root::Root()
: file_system_(0)
, render_system_(0)
, queue_mgr_(0)
, tex_mgr_(0)
, mesh_mgr_(0)
, time_system_(0)
, work_queue_(0)
, render_pipe_(0)
, res_default_scheduler_(0)
, scene_mgr_(0)
{

}

Root::~Root()
{

}

bool Root::init(const RootConfig& cfg)
{
	root_cfg_ = cfg;

	/** creation */
	create_time_system();
	create_work_queue();
	create_msg_queue();
	create_res_scheduler();
	create_file_system();
	create_render_system();
	create_profiler();
	create_tex_mgr();
	create_mesh_mgr();
	create_ft_font_mgr();
	create_scene_mgr();
	create_render_pipe();

	return true;
}

bool Root::uninit()
{
	destroy_render_pipe();
	destroy_scene_mgr();
	destroy_ft_font_mgr();
	destroy_mesh_mgr();
	destroy_tex_mgr();
	destory_profiler();
	destroy_render_system();
	destroy_file_system();
	destroy_res_scheduler();
	destroy_msg_queue();
	destroy_work_queue();
	destroy_time_system();

	return true;
}

SceneManager* Root::getSceneManager() const
{
	ENN_ASSERT(scene_mgr_);

	return scene_mgr_;
}

void Root::addCamera(CameraNode* cam)
{
	camera_list_.push_back(cam);
}

void Root::removeCamera(CameraNode* cam)
{
	CameraList::iterator it = std::find(camera_list_.begin(),
		camera_list_.end(), cam);

	ENN_ASSERT(it != camera_list_.end());

	if (it != camera_list_.end())
	{
		camera_list_.erase(it);
	}
}

void Root::run()
{
	time_system_->update();

	float ui_queue_time_piece = Math::maxVal(1.0f / 60 - time_system_->getElapsedTime(), 0.005f);

	queue_mgr_->updateQueue(MsgQueueManager::UI_QUEUE, ui_queue_time_piece);

	scene_mgr_->update();

	ENN_FOR_EACH(CameraList, camera_list_, it)
	{
		CameraNode* node = (*it);

		render_pipe_->render(node);
	}
}

void Root::start()
{
	work_queue_->create_threads(root_cfg_.num_threads);
	work_queue_->init();

	render_system_->init();
	tex_mgr_->init();
	mesh_mgr_->init();

	initFtFontMgr();

	scene_mgr_->init();
}

void Root::stop()
{
	work_queue_->uninit();
	scene_mgr_->uinit();
	
}

void Root::start_console()
{
	work_queue_->create_threads(root_cfg_.num_threads);
	work_queue_->init();

	mesh_mgr_->init();
	scene_mgr_->init();
}

void Root::stop_console()
{
	scene_mgr_->uinit();
	work_queue_->uninit();
}

void Root::create_time_system()
{
	time_system_ = ENN_NEW TimeSystem();
}

void Root::destroy_time_system()
{
	ENN_SAFE_DELETE(time_system_);
}

void Root::create_file_system()
{
	file_system_ = ENN_NEW FileSystem();
	file_system_->setRootPath(root_cfg_.file_system_path);
	file_system_->setQueueManager(queue_mgr_);
	file_system_->setFileListener(res_default_scheduler_);
}

void Root::destroy_file_system()
{
	ENN_SAFE_DELETE(file_system_);
}

void Root::create_work_queue()
{
	work_queue_ = ENN_NEW WorkQueue();
}

void Root::destroy_work_queue()
{
	ENN_SAFE_DELETE(work_queue_);
}

void Root::create_msg_queue()
{
	queue_mgr_ = ENN_NEW MsgQueueManager(work_queue_);
}

void Root::destroy_msg_queue()
{
	ENN_SAFE_DELETE(queue_mgr_);
}

void Root::create_render_system()
{
	render_system_ = ENN_NEW RenderSystem();
}

void Root::destroy_render_system()
{
	ENN_SAFE_DELETE(render_system_);
}

void Root::create_res_scheduler()
{
	res_default_scheduler_ = ENN_NEW DefaultResourceScheduler();
	res_default_scheduler_->setQueueMgr(queue_mgr_);
	res_default_scheduler_->registerMsg();
}

void Root::destroy_res_scheduler()
{
	ENN_SAFE_DELETE(res_default_scheduler_);
}

void Root::create_tex_mgr()
{
	tex_mgr_ = ENN_NEW TextureManager();
}

void Root::destroy_tex_mgr()
{
	ENN_SAFE_DELETE(tex_mgr_);
}

void Root::create_mesh_mgr()
{
	mesh_mgr_ = ENN_NEW MeshManager();
}

void Root::destroy_mesh_mgr()
{
	ENN_SAFE_DELETE(mesh_mgr_);
}

void Root::create_scene_mgr()
{
	scene_mgr_ = ENN_NEW SceneManager();
}

void Root::destroy_scene_mgr()
{
	ENN_SAFE_DELETE(scene_mgr_);
}

void Root::create_render_pipe()
{
	render_pipe_ = ENN_NEW RenderPipe();
}

void Root::destroy_render_pipe()
{
	ENN_SAFE_DELETE(render_pipe_);
}

void Root::create_ft_font_mgr()
{
	ft_font_mgr_ = ENN_NEW FTFontManager(this);
}

void Root::destroy_ft_font_mgr()
{
	ENN_SAFE_DELETE(ft_font_mgr_);
}

void Root::create_profiler()
{
	profiler_ = ENN_NEW Profiler;
}

void Root::destory_profiler()
{
	ENN_SAFE_DELETE(profiler_);
}

void Root::initFtFontMgr()
{
	ft_font_mgr_->initalize(root_cfg_.font_buffer_w, root_cfg_.font_buffer_h);

	for (int i = 0; i < root_cfg_.font_family_cnt; ++i)
	{
		FontFamilyDesc* desc = &root_cfg_.font_family_desc[i];

		if (desc)
		{
			String ff_name = desc->font_name;
			String ff_path = desc->ttf_path;

			ft_font_mgr_->registerFamilyFont(ff_name, ff_path);
		}
	}
}

void Root::finish_ui_queue()
{
	queue_mgr_->updateQueue(MsgQueueManager::UI_QUEUE, 9999.0f);
}

}