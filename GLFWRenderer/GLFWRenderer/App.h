#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include <iostream>
#include "GLCommon.h"
#include "Camera.h"
#include "GLFWcontext.h"
#include "RenderController.h"
#include "DynamicMeshManager.h"
#include "DynamicLightManager.h"

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
	double lastx;
	double lasty;
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
	Camera maincamera = Camera(glm::vec3(0.0f, 5.0f, 20.0f), 0.0f, -20.0f);
	RenderContext rendercontext;
	DynamicMeshManager meshmanager;
	DynamicLightManager lightmanager;
	RenderController renderer{ meshmanager, lightmanager, settings.width, settings.height };
};