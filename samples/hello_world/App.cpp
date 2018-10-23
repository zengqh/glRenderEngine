/**
* Copyright (c) By zengqh.
*
* This program is just for fun or demo, in the hope that it  
* will be useful, you can redistribute it and/or modify freely.
*
* Time: 2013/06/19
* File: App.cpp
* Blog: http://www.cnblogs.com/zengqh/
**/

#include "App.h"

using namespace enn;

App* g_App = 0;

//////////////////////////////////////////////////////////////////////////
LRESULT WINAPI window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return g_App->window_proc(hWnd, msg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
bool is_key_press(unsigned int key) { return GetAsyncKeyState(key) ? true : false;}
bool is_key_pressed(unsigned int key) { return GetKeyState(key) ? true : false;}

//////////////////////////////////////////////////////////////////////////
App::App()
: is_pause_(false)
, hwnd_(0)
, window_system_platform_(0)
, x_(0), y_(0), w_(800), h_(600)
, cam_node_(0)
, m_deltaX(0.0f), m_deltaY(0.0f)
, view_port_change_(false)
, box_num_(0)
, is_spawn_box_(false)
{
	m_ptMouse.x = m_ptMouse.y = 0;
}

App::~App()
{

}

LRESULT App::window_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_ERASEBKGND:
		{
			return 0;
		}

	case WM_SIZE:
		{
			if(SIZE_MINIMIZED == wParam)
			{
				is_pause_ = true;
			}
			else
			{
				is_pause_ = false;
			}

			if (SIZE_MAXIMIZED == wParam || SIZE_RESTORED)
			{
				view_port_change_ = true;
			}

			break;
		}

	case WM_LBUTTONDOWN:
		{
			is_spawn_box_ = true;
			break;
		}

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		if(wParam == VK_ESCAPE)
		{
			DestroyWindow(hWnd);
			return 0;
		}
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

void App::create_window(int x, int y, int w, int h)
{
	WNDCLASS wndClass;
	ZeroMemory(&wndClass, sizeof(WNDCLASS));
	wndClass.style = CS_DBLCLKS;
	wndClass.lpfnWndProc = ::window_proc;
	wndClass.hInstance = GetModuleHandle(0);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.lpszClassName = "Demo";
	RegisterClass(&wndClass);
	RECT rc; SetRect(&rc, 0, 0, w, h);

	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, 0);

	hwnd_ = CreateWindow( "Demo", "Demo", 
		WS_OVERLAPPEDWINDOW, x, y, 
		(rc.right - rc.left), (rc.bottom - rc.top), 
		0, 0, wndClass.hInstance, 0);

	ShowWindow(hwnd_, SW_SHOWNORMAL);

	x_ = x;
	y_ = y;
	w_ = w;
	h_ = h;
}

void App::create_root()
{
	/** first create gl context */
	WindowSystemPlatform::CreateParam para;

	window_system_platform_ = ENN_NEW enn::WindowSystemPlatform();
	window_system_platform_->create_device((void*)hwnd_, &para);


	/** init root */
	RootConfig root_config;
	root_config.window_handle = (void*)hwnd_;
	root_config.window_width = w_;
	root_config.window_height = h_;
	root_config.file_system_path = "I:\\trainning\\enn_gl\\media";
	root_config.clear_clr = Color::BLUE;

	Root* root = ENN_NEW Root;
	root->init(root_config);
	root->start();
}

void App::idle()
{
	update_operation();
	Root::getSingletonPtr()->run();
	window_system_platform_->present();
}

void App::destroy()
{
	Root* root = Root::getSingletonPtr();
	root->stop();
	root->uninit();
	ENN_SAFE_DELETE(root);

	ENN_SAFE_DELETE(window_system_platform_);
}

void App::msg_loop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while(WM_QUIT != msg.message)
	{
		if(is_pause_)
		{
			if(GetMessage(&msg, NULL, 0U, 0U))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if(PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				idle();
			}
		}
	}
}

void App::run(int x, int y, int w, int h)
{
	create_window(x, y, w, h);
	create_root();
	create_scene();
	msg_loop();
	destroy();
}

void App::create_scene()
{
	Root* root = Root::getSingletonPtr();

	SceneManager* scene_mgr = root->getSceneManager();
	SceneNode* root_node = scene_mgr->getRootNode();

	scene_mgr->setGlobalAmb(Color(0.15f, 0.15f, 0.15f));

	/** create simple cube */
	Material* mat = ENN_NEW Material;
	SubMaterial* sub_mat = ENN_NEW SubMaterial(mat);
	sub_mat->enableLighting(true);
	sub_mat->setDiffuse(Color(0.6f, 0.6f, 0.6f));
	TextureUnit* tu = sub_mat->useTexDiff();
	tu->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
	tu->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
	tu->setTexName("1.bmp");

	mat->addSubMaterial(0, sub_mat);

	/** create mat 2 */
	Material* mat2 = ENN_NEW Material;
	SubMaterial* sub_mat2 = ENN_NEW SubMaterial(mat2);
	sub_mat2->enableLighting(true);
	sub_mat2->setDiffuse(Color(0.9f, 0.9f, 0.9f));
	sub_mat2->setSpecularShiness(Color(0.8f, 0.8f, 0.8f, 100.0f));
	TextureUnit* tu2 = sub_mat2->useTexDiff();
	tu2->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
	tu->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
	tu2->setTexName("BumpyMetal.jpg");
	mat2->addSubMaterial(0, sub_mat2);

	/** create mat 3 */
	Material* mat3 = ENN_NEW Material;
	SubMaterial* sub_mat3 = ENN_NEW SubMaterial(mat3);
	sub_mat3->enableLighting(true);
	sub_mat3->setDiffuse(Color(0.9f, 0.9f, 0.9f));

	TextureUnit* tu3 = sub_mat3->useTexDiff();
	tu3->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
	tu3->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
	tu3->setTexName("grass.png");

	//sub_mat3->useTexAnimate()->setUVScale(10, 10);

	mat3->addSubMaterial(0, sub_mat3);

	MeshPtr cube_mesh(root->get_mesh_mgr()->createCube("mesh_cube", 3.0f));
	cube_mesh->load(false);

	SubMesh* cube_sub_mesh = cube_mesh->getSubMesh(0);

	cube_mesh->setMaterial(mat);
	cube_sub_mesh->setMatIdx(0);

	MeshNode* obj1 = scene_mgr->createSceneNode<MeshNode>("obj1");
	obj1->setPosition(0.0f, 1.5f, 0.0f);
	obj1->setMesh("mesh_cube");
	obj1->setCastShadow(true);

	root_node->addChild(obj1);

	MeshPtr sphere_mesh(root->get_mesh_mgr()->createSphere("sphere", 4.0, 32));
	sphere_mesh->load(false);
	SubMesh* sphere_sub_mesh = sphere_mesh->getSubMesh(0);
	sphere_mesh->setMaterial(mat2);
	sphere_sub_mesh->setMatIdx(0);

	MeshPtr plane_mesh(root->get_mesh_mgr()->createPlane("plane", 1000, 800, 3, 2));
	plane_mesh->load(false);
	SubMesh* plane_sub_mesh = plane_mesh->getSubMesh(0);
	plane_mesh->setMaterial(mat3);
	plane_sub_mesh->setMatIdx(0);


	MeshNode* obj2 = scene_mgr->createSceneNode<MeshNode>("obj2");
	obj2->setPosition(5.0f, 2.0f, 0.0f);
	obj2->setMesh("sphere");
	obj2->setCastShadow(true);
	root_node->addChild(obj2);

	MeshNode* obj3 = scene_mgr->createSceneNode<MeshNode>("obj3");
	obj3->setPosition(0.0f, -3.0f, 0.0f);
	obj3->setOrientation(Quaternion(Math::toRadians(45.0f), Vector3::UNIT_Y));
	obj3->setMesh("plane");
	obj3->setReceiveShadow(true);
	//root_node->addChild(obj3);

	RigidBody* body = obj3->createRigidBody();
	CollisionShape* shape = obj3->createCollisionShape();
	shape->setStaticPlane();

	create_skeleton();

	
	//create_water();
	create_sponza();
	

	
	LightNode* point_light = scene_mgr->createSceneNode<LightNode>("main_point_light");
	point_light->look_at(vec3f(0, 10, 0), vec3f(0, 0, 0), Vector3::UNIT_Y);
	point_light->setLightType(LT_POINT);

	PointLight* pl = point_light->getLightAs<PointLight>();
	pl->setDiffuse(Color(0.8f, 0.8f, 0.2f));
	pl->setRange(50.0f);
	pl->setFadeDist(1);

	root_node->addChild(point_light);
	

	create_sun_light();
	create_sky_box();
	
	/** create camera node */
	CameraNode* cn = scene_mgr->createSceneNode<CameraNode>("main_cam");
	cn->look_at(vec3f(0, 10, 9), vec3f(0, 0, -9.0f), vec3f::UNIT_Y);
	cn->getCamera()->setPerspective(Math::toRadians(60.0f), w_ / (float)h_, 1.0f, 1000.0f);
	root_node->addChild(cn);

	Camera* raw_cam = cn->getCamera();
	Viewport* vp = ENN_NEW Viewport;
	vp->makeRect(0, 0, w_, h_);
	raw_cam->setViewport(vp);

	root->addCamera(cn);

	cam_node_ = cn;
}

void App::create_sky_box()
{
	SkyboxSceneData* skybox = ENN_NEW SkyboxSceneData;

	skybox->setBackTex("skybox/morning_BK.jpg");
	skybox->setBottomTex("skybox/morning_DN.jpg");
	skybox->setFrontTex("skybox/morning_FR.jpg");
	skybox->setLeftTex("skybox/morning_LF.jpg");
	skybox->setRightTex("skybox/morning_RT.jpg");
	skybox->setTopTex("skybox/morning_UP.jpg");

	SCENE_MGR->setSkyboxData(skybox);
}

void App::create_sun_light()
{
	LightNode* sun_light = SCENE_MGR->createSceneNode<LightNode>("main_dir_light");
	sun_light->look_at(vec3f(1, 1, 1), vec3f(0, 0, 0), Vector3::UNIT_Y);
	sun_light->setLightType(LT_DIRECTIONAL);

	DirectionLight* pl = sun_light->getLightAs<DirectionLight>();
	pl->setDiffuse( Color(0.8f, 0.80f, 0.83f) );
	pl->setShadowType( ST_PSSM );
	pl->setShadowResolution( 1024 );
	pl->setShadowTapGroups( 2 );
	pl->setShadowVisibleRange( 200.0f );
	pl->setShadowFadeRange( 85.0f );
	pl->setShadowSplitSchemeWeight( 0.5f );
	pl->setShadowBlurSize( 0.8f );
	pl->setShadowStrength( 0.9f );
	pl->setShadowBias( 0.0001f );
}

bool App::update_operation()
{
	if (is_key_press(VK_ESCAPE))
	{
		return false;
	}

	if (view_port_change_)
	{
		getDeviceSize(w_, h_);

		Camera* cam = cam_node_->getCamera();

		cam->getViewport()->makeRect(0, 0, w_, h_);
		cam->setAspect(w_ / (float)h_);

		view_port_change_ = false;
	}

	float elapsed_time = Root::getSingletonPtr()->get_time_system()->getElapsedTime();

	/** keyboard event */
	{
		float uints = elapsed_time * 70.0f;
		if (is_key_press('W'))
		{
			cam_node_->translate(vec3f::NEGATIVE_UNIT_Z * uints, Node::TS_LOCAL);
		}
		else if (is_key_press('S'))
		{
			cam_node_->translate(vec3f::UNIT_Z * uints, Node::TS_LOCAL);
		}
		else if (is_key_press('D'))
		{
			cam_node_->translate(vec3f::UNIT_X * uints, Node::TS_LOCAL);
		}
		else if (is_key_press('A'))
		{
			cam_node_->translate(vec3f::NEGATIVE_UNIT_X * uints, Node::TS_LOCAL);
		}
		else if (is_key_press('E'))
		{
			cam_node_->translate(vec3f::UNIT_Y * uints, Node::TS_LOCAL);
		}
		else if (is_key_press('Q'))
		{
			cam_node_->translate(vec3f::NEGATIVE_UNIT_Y * uints, Node::TS_LOCAL);
		}
	}

	/** mouse event */
	{
		float units = -0.01f;

		POINT curMouse = { 0 };
		GetCursorPos( &curMouse );

		if (is_key_press(VK_RBUTTON))
		{
			LONG  offx  = curMouse.x - m_ptMouse.x;
			LONG  offy  = curMouse.y - m_ptMouse.y;

			float fPercentOfNew = 1.0f / 2;
			float fPercentOfOld = 1.0f - fPercentOfNew;

			m_deltaX = m_deltaX * fPercentOfOld + offx * units * fPercentOfNew;
			m_deltaY = m_deltaY * fPercentOfOld + offy * units * fPercentOfNew;

			cam_node_->yaw(m_deltaX, Node::TS_PARENT);
			cam_node_->pitch(m_deltaY);
		}

		if (is_spawn_box_)
		{
			spawn_box();

			is_spawn_box_ = false;
		}

		m_ptMouse = curMouse;
	}

	return true;
}

void App::getDeviceSize(int& w, int& h)
{
	RECT rc;
	GetClientRect(hwnd_, &rc);
	w = rc.right - rc.left;
	h = rc.bottom - rc.top;
}

void App::create_skeleton()
{
	Root* root = Root::getSingletonPtr();

	SceneManager* scene_mgr = root->getSceneManager();
	SceneNode* root_node = scene_mgr->getRootNode();

	MeshPtr skel_mesh(static_cast<Mesh*>(root->get_mesh_mgr()->createResource("boblamp.mesh")));
	skel_mesh->load(false);

	MeshNode* skel_node = scene_mgr->createSceneNode<MeshNode>("skel_test");
	skel_node->setMesh(skel_mesh.get());
	skel_node->setPosition(10.0f, 2.0f, 0.0f);
	skel_node->setScale(0.1f, 0.1f, 0.1f);
	skel_node->rotate(vec3f::UNIT_X, Math::toRadians(-90));
	skel_node->setCastShadow(true);
	root_node->addChild(skel_node);
}

void App::create_sponza()
{
	Root* root = Root::getSingletonPtr();

	SceneManager* scene_mgr = root->getSceneManager();
	SceneNode* root_node = scene_mgr->getRootNode();

	MeshPtr sponza_mesh(static_cast<Mesh*>(root->get_mesh_mgr()->createResource("sponza.mesh")));
	sponza_mesh->load(false);
	

	MeshNode* sponza_node = scene_mgr->createSceneNode<MeshNode>("sponza");

	sponza_node->setMesh(sponza_mesh.get());
	sponza_node->setPosition(0.0f, -4.0f, 0.0f);
	sponza_node->setScale(0.1f, 0.1f, 0.1f);
	sponza_node->setReceiveShadow(true);
	root_node->addChild(sponza_node);
}

void App::spawn_box()
{
	SceneManager* scene_mgr = ROOTPTR->getSceneManager();

	String node_name = String("box") + to_string(box_num_++);

	MeshNode* mesh_node = scene_mgr->createSceneNode<MeshNode>(node_name);

	mesh_node->setPosition(cam_node_->getPosition());
	mesh_node->setOrientation(cam_node_->getQuaternion());
	mesh_node->setCastShadow(true);


	Material* mat = ENN_NEW Material;
	SubMaterial* sub_mat = mat->createSubMaterial(0);
	sub_mat->enableLighting(true);
	sub_mat->setDiffuse(Color(0.6f, 0.6f, 0.6f));
	TextureUnit* tu = sub_mat->useTexDiff();
	tu->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
	tu->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
	tu->setTexName("1.bmp");

	MeshPtr cube_mesh(static_cast<Mesh*>(ROOTPTR->get_mesh_mgr()->getOrCreateResource("mesh_cube")));
	cube_mesh->setMaterial(mat);
	cube_mesh->load(false);

	mesh_node->setMesh(cube_mesh.get());

	// create physics
	CollisionShape* collision_shape = mesh_node->createCollisionShape();
	RigidBody* rigid_body = mesh_node->createRigidBody();

	rigid_body->setMass(1.25f);
	rigid_body->setFriction(1.75f);

	collision_shape->setBox(Vector3(3, 3, 3));

	const float OBJECT_VELOCITY = 30.0f;

	rigid_body->setLinearVelocity(cam_node_->getQuaternion() * Vector3(0.0f, 0.25f, -1.0f) * OBJECT_VELOCITY);

	ROOTPTR->getSceneManager()->getRootNode()->addChild(mesh_node);
}

void App::spawn_sphere()
{
	SceneManager* scene_mgr = ROOTPTR->getSceneManager();

	String node_name = String("sphere") + to_string(box_num_++);

	MeshNode* mesh_node = scene_mgr->createSceneNode<MeshNode>(node_name);

	mesh_node->setPosition(cam_node_->getPosition());
	mesh_node->setOrientation(cam_node_->getQuaternion());
	mesh_node->setCastShadow(true);


	Material* mat = ENN_NEW Material;
	SubMaterial* sub_mat = mat->createSubMaterial(0);
	sub_mat->enableLighting(true);
	sub_mat->setDiffuse(Color(0.6f, 0.6f, 0.6f));
	sub_mat->setSpecularShiness(Color(0.8f, 0.8f, 0.8f, 20.0f));
	TextureUnit* tu = sub_mat->useTexDiff();
	tu->setTexAddr(TEXADDR_WRAP, TEXADDR_WRAP);
	tu->setTexFilter(TEXF_MIP_LINEAR, TEXF_LINEAR);
	tu->setTexName("1.bmp");

	MeshPtr sphere_mesh(static_cast<Mesh*>(ROOTPTR->get_mesh_mgr()->getOrCreateResource("sphere")));
	sphere_mesh->setMaterial(mat);
	sphere_mesh->load(false);

	mesh_node->setMesh(sphere_mesh.get());

	// create physics
	CollisionShape* collision_shape = mesh_node->createCollisionShape();
	RigidBody* rigid_body = mesh_node->createRigidBody();

	rigid_body->setMass(1.25f);
	rigid_body->setFriction(3.75f);

	collision_shape->setSphere(4);

	const float OBJECT_VELOCITY = 30.0f;

	rigid_body->setLinearVelocity(cam_node_->getQuaternion() * Vector3(0.0f, 0.25f, -1.0f) * OBJECT_VELOCITY);

	ROOTPTR->getSceneManager()->getRootNode()->addChild(mesh_node);
}

void App::create_water()
{
	WaterPlaneNode* water_node = SCENE_MGR->createSceneNode<WaterPlaneNode>("water_demo");

	water_node->createWaterPlane("water_demo", 400, 400);

	water_node->getWaterPlane()->water_normal_tex_name_ = "water0.tga";
	water_node->getWaterPlane()->create_mtr();

	water_node->setPosition(0, 5, 0);

	ROOTPTR->getSceneManager()->getRootNode()->addChild(water_node);
}

void App::test_raw_image()
{
	enn::Image image;
	image.load_raw("e:/water0.raw");

	image.saveTGA("e:/water0.tga");
}

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR str, int)
{
	App _instance;
	g_App = &_instance;
	_instance.run(0, 0, 800, 600);
	return 0;
}