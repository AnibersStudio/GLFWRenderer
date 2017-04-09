#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include "GLCommon.h"
#include "Camera.h"
#include "GLFWcontext.h"
#include "DrawController.h"
#include "RenderController.h"
#include "DynamicMeshManager.h"
#include <iostream>

struct SettingInfo
{
	int width = 1366;
	int height = 768;
	bool isfullscreen = false;
	float horsense = 0.05f;
	float vertsense = 0.04f;
	float gamma = 2.2f;
};

struct PlayerInfo
{
	float movespeed = 20.0f;
};

struct MomentInfo
{
	bool firstmousemove = true;
	bool keys[128];
	int lastx;
	int lasty;
};

class App : Receiver
{
public:
	App();
	bool KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode) override;
	bool CursorPosCallback(GLFWwindow * winptr, double x, double y) override;
	void Run();
	~App();
protected:
	void UserMove(double deltatime);
private:
	SettingInfo settings;
	PlayerInfo player;
	MomentInfo moment;

	GLFWcontext context{ settings.width, settings.height, "GLFWapp", settings.isfullscreen };
	Camera maincamera = Camera(glm::vec3(0.0f, 0.0f, 10.0f));
	RenderContext rendercontext;
	DynamicMeshManager manager;
	RenderController renderer{ manager, settings.width, settings.height };
};