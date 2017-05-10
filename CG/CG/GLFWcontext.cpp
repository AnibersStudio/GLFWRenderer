#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glfw3dll.lib")

#include "GLFWcontext.h"
#include "CommonTools.h"
#include <deque>
#include <iostream>

std::deque<Receiver *> receivers;

void CursorPosCallback(GLFWwindow * winptr, double x, double y);
void KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode);
void MouseKeyCallback(GLFWwindow * winptr, int key, int action, int mods);

void APIENTRY MyProc(GLenum source, GLenum type, GLuint id, GLenum serverity, GLsizei length, const GLchar *message, const GLvoid *userParam)
{
	std::cout << "source: " << tostr(source) << "; "
		<< "type: " << tostr(type) << "; "
		<< "id: " << id << "; "
		<< "severity: " << tostr(serverity) << "; "
		<< "message: " << message << std::endl;
}
GLFWcontext::GLFWcontext(int w, int h, std::string t, bool isfullscr) : width(w), height(h), title(t), isfullscreen(isfullscr)
{
	glfwInit();						//GLFW initialize
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);//GL version 4.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);	
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	//GLFW only uses GL's core functionalities

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	if (!isfullscr)
		winptr = glfwCreateWindow(w, h, t.c_str(), nullptr, nullptr);	//Open a window with width, height, title, monitor.
	else
		winptr = glfwCreateWindow(w, h, t.c_str(), glfwGetPrimaryMonitor(), nullptr);	//Open a window with width, height, title, monitor.
	if (winptr == nullptr)
	{
		glfwTerminate();
		throw std::exception("Cannot initialize OpenGL context. Maybe uncapable gpu or driver?");
	}
	glfwMakeContextCurrent(winptr);					//Bind winptr context to current thread

	glfwSetCursorPosCallback(winptr, CursorPosCallback);
	glfwSetKeyCallback(winptr, KeyCallback);
	glfwSetMouseButtonCallback(winptr, MouseKeyCallback);

	glewExperimental = GL_TRUE;						//Enable modern features
	if (glewInit() != GLEW_OK)						//Init GLEW
	{
		glfwTerminate();
		throw;
	}

	glfwGetFramebufferSize(winptr, &w, &h);//Get window size

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//glEnable(GL_DEBUG_OUTPUT);
	//glDebugMessageCallback(&MyProc, nullptr);
}

GLFWcontext::~GLFWcontext()
{
	glfwTerminate();
}

void GLFWcontext::ShowCursor() const
{
	glfwSetInputMode(winptr, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void GLFWcontext::HideCursor() const
{
	glfwSetInputMode(winptr, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

bool GLFWcontext::ChangeResolution(int w, int h)
{
	if (width == w && height == h)
	{
		return false;
	}
	glfwSetWindowSize(winptr, w, h);
	width = w;
	height = h;
	return true;
}

bool GLFWcontext::PollEvents() const
{
	glfwPollEvents();
	return !glfwWindowShouldClose(winptr);
}

void GLFWcontext::RegisterReceiver(Receiver * r) const
{
	receivers.push_front(r);
}

void CursorPosCallback(GLFWwindow * winptr, double x, double y)
{
	for (auto i : receivers)
	{
		if (!i->CursorPosCallback(winptr, x, y))
			break;
	}
}

void KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode)
{
	for (auto i : receivers)
	{
		if (!i->KeyCallback(winptr, key, scancode, action, mode))
			break;
	}
}

void MouseKeyCallback(GLFWwindow * winptr, int key, int action, int mods)
{
	for (auto i : receivers)
	{
		if (!i->MouseKeyCallback(winptr, key, action, mods))
			break;
	}

}