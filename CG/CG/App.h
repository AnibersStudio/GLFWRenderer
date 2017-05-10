///Name: App
///Description: An application class, including all the callbacks and program states. The implementation should just create an App object and call App::Run()
///Usage: Construct to create an application instance. Call Run() to start and stay in the application
///Coder: Hao Pang
///Date: 2017.4.26
#pragma once
#include <thread>
#include <iostream>
#include "GLCommon.h"
#include "GLFWcontext.h"
#include "Renderer.h"

struct SettingInfo
{
	int width = 1366;
	int height = 768;
	bool isfullscreen = false;
};

struct MomentInfo
{
	bool firstmousemove = true;
	bool keys[128];
	double lastx;
	double lasty;
};

class App : Receiver
{
public:
	App(RenderObject perm = RenderObject());
	bool KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode) override;
	bool CursorPosCallback(GLFWwindow * winptr, double x, double y) override;
	bool MouseKeyCallback(GLFWwindow * winptr, int key, int action, int mods) override;
	void Run();
	~App();

private:
	void ColorChange(double timeperiod);
	void IndicatorChange();
	SettingInfo settings;
	MomentInfo moment;
	std::thread::id threadid;
	GLFWcontext context{ settings.width, settings.height, "GLFWapp", settings.isfullscreen };
	GLFWwindow * winptr;
	Renderer renderer;
	glm::vec3 color = glm::vec3(1.0f);

	Rasterizer rasterizer;
	RenderObject permanent;
	RenderObject current;
	RenderObject colorindicator;
};