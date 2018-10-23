/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/02
* File: enn_root.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once

#include "enn_platform_headers.h"
#include "enn_file_system.h"
#include "enn_gl_render_system.h"
#include "enn_msg_queue.h"
#include "enn_texture_manager.h"
#include "enn_mesh_manager.h"
#include "enn_scene_manager.h"
#include "enn_timer.h"
#include "enn_render_pipe.h"
#include "enn_animation.h"
#include "enn_ft_font_mgr.h"
#include "enn_ft_font_record.h"
#include "enn_profiler.h"
#include "enn_image.h"

namespace enn
{

struct FontFamilyDesc
{
	String			font_name;
	String			ttf_path;
};

//////////////////////////////////////////////////////////////////////////
class RootConfig : public AllocatedObject
{
public:
	RootConfig()
	{
		window_width = 0;
		window_height = 0;
		render_pipe_style = 0;
		window_handle = 0;
		num_threads = 0;
		font_family_cnt = 0;
		font_family_desc = 0;
		font_buffer_w = 512;
		font_buffer_h = 512;
		font_family_cnt = 0;
		font_family_desc = 0;
	}

	~RootConfig()
	{

	}

	int window_width;
	int window_height;
	int render_pipe_style;
	void* window_handle;
	String file_system_path;
	Color clear_clr;
	int num_threads;

	int	font_family_cnt;
	FontFamilyDesc* font_family_desc;
	int font_buffer_w, font_buffer_h;
};

class Root : public AllocatedObject, public Singleton<Root>
{
public:
	typedef enn::list<CameraNode*>::type CameraList;

public:
	Root();
	~Root();

	bool init(const RootConfig& cfg);
	bool uninit();

	FileSystem* get_file_system() const
	{
		return file_system_;
	}

	RenderSystem* get_render_system() const
	{
		return render_system_;
	}

	WorkQueue* get_work_queue() const
	{
		return work_queue_;
	}

	MsgQueueManager* get_queue_mgr() const
	{
		return queue_mgr_;
	}

	TextureManager* get_texture_mgr() const
	{
		return tex_mgr_;
	}

	MeshManager* get_mesh_mgr() const
	{
		return mesh_mgr_;
	}

	TimeSystem* get_time_system() const
	{
		return time_system_;
	}

	DefaultResourceScheduler* get_default_res_scheduler() const
	{
		return res_default_scheduler_;
	}

	SceneManager* getSceneManager() const;

	RenderPipe* getRenderPipe() const
	{
		return render_pipe_;
	}

	const RootConfig& getRootCfg() const
	{
		return root_cfg_;
	}

	FTFontManager* getFTFontMgr() const
	{
		return ft_font_mgr_;
	}

	Profiler* getProfiler() const
	{
		return profiler_;
	}

	void addCamera(CameraNode* cam);
	void removeCamera(CameraNode* cam);

	void run();

	void start();
	void stop();

	void start_console();
	void stop_console();

protected:
	void create_time_system();
	void destroy_time_system();

	void create_file_system();
	void destroy_file_system();

	void create_work_queue();
	void destroy_work_queue();

	void create_msg_queue();
	void destroy_msg_queue();

	void create_render_system();
	void destroy_render_system();

	void create_res_scheduler();
	void destroy_res_scheduler();

	void create_tex_mgr();
	void destroy_tex_mgr();

	void create_mesh_mgr();
	void destroy_mesh_mgr();

	void create_scene_mgr();
	void destroy_scene_mgr();

	void create_render_pipe();
	void destroy_render_pipe();

	void create_ft_font_mgr();
	void destroy_ft_font_mgr();

	void create_profiler();
	void destory_profiler();

	void initFtFontMgr();

protected:
	void finish_ui_queue();

protected:
	FileSystem*						file_system_;
	RenderSystem*					render_system_;
	MsgQueueManager*				queue_mgr_;
	TextureManager*					tex_mgr_;
	MeshManager*					mesh_mgr_;
	TimeSystem*						time_system_;
	WorkQueue*						work_queue_;
	RenderPipe*						render_pipe_;
	DefaultResourceScheduler*		res_default_scheduler_;
	SceneManager*					scene_mgr_;
	FTFontManager*					ft_font_mgr_;
	Profiler*						profiler_;

	RootConfig						root_cfg_;
	CameraList						camera_list_;
};

#define ROOTPTR			Root::getSingletonPtr()
#define ROOTREF			Root::getSingleton()
#define MESH_MGR		ROOTPTR->get_mesh_mgr()
#define RENDER_SYS		ROOTPTR->get_render_system()
#define SCENE_MGR		ROOTPTR->getSceneManager()

}