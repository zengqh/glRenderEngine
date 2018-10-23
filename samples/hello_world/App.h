/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: App.h
* Blog: http://www.cnblogs.com/zengqh/
**/

#pragma once


#include "enn.h"

class App
{
public:
	App();
	~App();

	LRESULT window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void create_window(int x, int y, int w, int h);
	void create_root();
	void idle();
	void destroy();
	void msg_loop();
	void run(int x, int y, int w, int h);

protected:
	void create_scene();
	void create_sky_box();
	void create_sun_light();
	bool update_operation();
	void getDeviceSize(int& w, int& h);

protected:
	void create_skeleton();
	void create_sponza();
	void spawn_box();
	void spawn_sphere();
	void create_water();
	void test_raw_image();

protected:
	bool is_pause_;
	HWND hwnd_;
	enn::WindowSystemPlatform* window_system_platform_;
	int x_, y_, w_, h_;
	enn::CameraNode* cam_node_;
	POINT       m_ptMouse;
	float      m_deltaX;
	float      m_deltaY;
	bool		view_port_change_;
	int			box_num_;
	bool		is_spawn_box_;
};