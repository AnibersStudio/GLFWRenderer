///Name: GLFWcontext
///Description: This GLFWcontext class initializes OpenGL context/GLFW context/GLEW context as well as manage keyborad/mouse callbacks
///Usage: Construct to create a series of context. Use Show*()/Hide*()/Change*() to modify the context. Use PollEvent() to let GLFW handle events. Use RegisterReceiver() to register Callbacks
///Coder: Hao Pang
///Date: 2017.4.26
#pragma once
#include <string>
#include "GLCommon.h"
#include "CommonTools.h"

class Receiver
{
public:
	virtual bool CursorPosCallback(GLFWwindow * winptr, double x, double y) = 0;
	virtual bool KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode) = 0;
	virtual bool MouseKeyCallback(GLFWwindow * winptr, int key, int action, int mods) = 0;
};

class GLFWcontext
{
public:
	GLFWcontext(int w, int h, std::string t, bool isfullscr);
	~GLFWcontext();
	GLFWwindow * GetWindow() { return winptr; }
	void ShowCursor() const;
	void HideCursor() const;
	bool ChangeResolution(int w, int h);
	bool PollEvents() const;
	void RegisterReceiver(Receiver * r) const; 
private:
	int width;
	int height;
	std::string title;
	bool isfullscreen;
	GLFWwindow * winptr;
};
