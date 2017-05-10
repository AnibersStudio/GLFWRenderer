#include "App.h"
#include <thread>

App::App(RenderObject perm): permanent(perm), renderer(settings.width, settings.height), rasterizer(settings.width, settings.height)
{
	using namespace glm;
	permanent.Init(&rasterizer);
	current.Init(&rasterizer);
	colorindicator.Init(&rasterizer);

	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(359,48), vec3(1.0) });
	permanent.NextVertex({ vec2(403,108), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(197,147), vec3(1.0) });
	permanent.NextVertex({ vec2(578,171), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(197,147), vec3(1.0) });
	permanent.NextVertex({ vec2(171,394), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(186,353), vec3(1.0) });
	permanent.NextVertex({ vec2(552,340), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(386,156), vec3(1.0) });
	permanent.NextVertex({ vec2(358,387), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_POINT);
	permanent.NextVertex({ vec2(33,314), vec3(1.0) });
	permanent.NextVertex({ vec2(170,534), vec3(1.0) });
	permanent.NextVertex({ vec2(148,403), vec3(1.0) });
	permanent.NextVertex({ vec2(92,533), vec3(1.0) });
	permanent.NextVertex({ vec2(72,533), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_POINT);
	permanent.NextVertex({ vec2(223,310), vec3(1.0) });
	permanent.NextVertex({ vec2(360,530), vec3(1.0) });
	permanent.NextVertex({ vec2(338,399), vec3(1.0) });
	permanent.NextVertex({ vec2(282,529), vec3(1.0) });
	permanent.NextVertex({ vec2(262,529), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(459,430), vec3(1.0) });
	permanent.NextVertex({ vec2(362,556), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(434,233), vec3(1.0) });
	permanent.NextVertex({ vec2(485,283), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(556,527), vec3(1.0) });
	permanent.NextVertex({ vec2(543,435), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(556,527), vec3(1.0) });
	permanent.NextVertex({ vec2(416,526), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(366,348), vec3(1.0) });
	permanent.NextVertex({ vec2(414,522), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(556,527), vec3(1.0) });
	permanent.NextVertex({ vec2(416,526), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(827,92), vec3(1.0) });
	permanent.NextVertex({ vec2(867,217), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(832,92), vec3(1.0) });
	permanent.NextVertex({ vec2(1060,87), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(1060,87), vec3(1.0) });
	permanent.NextVertex({ vec2(1023,215), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(861,159), vec3(1.0) });
	permanent.NextVertex({ vec2(1041,148), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(875,210), vec3(1.0) });
	permanent.NextVertex({ vec2(1019,208), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(812,310), vec3(1.0) });
	permanent.NextVertex({ vec2(1072,312), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(768,407), vec3(1.0) });
	permanent.NextVertex({ vec2(1102,409), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(936,310), vec3(1.0) });
	permanent.NextVertex({ vec2(924,408), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(924,408), vec3(1.0) });
	permanent.NextVertex({ vec2(823,523), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(926,402), vec3(1.0) });
	permanent.NextVertex({ vec2(1017,519), vec3(1.0) });
	permanent.CloseVertex();
	permanent.New(GL_LINE);
	permanent.NextVertex({ vec2(1017,519), vec3(1.0) });
	permanent.NextVertex({ vec2(1079,533), vec3(1.0) });
	permanent.CloseVertex();

	permanent.Rasterize();

	winptr = context.GetWindow();
	init(moment.keys, false, 128);
	context.RegisterReceiver(this);
	threadid = std::this_thread::get_id();
	current.New(GL_LINE);

	
}

bool App::KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(winptr, GL_TRUE);
			break;
		case GLFW_KEY_1:
			current.New(GL_LINE);
			IndicatorChange();
			break;
		case GLFW_KEY_2:
			current.New(GL_POINT);
			IndicatorChange();
			break;
		case GLFW_KEY_3:
			current.New(GL_POLYGON);
			IndicatorChange();
			break;
		case GLFW_KEY_4:
			current.New(GL_POINTS);
			IndicatorChange();
			break;
		case GLFW_KEY_R:
			color = glm::vec3(1.0f, 0.0f, 0.0f);
			IndicatorChange();
			break;
		case GLFW_KEY_G:
			color = glm::vec3(0.0f, 1.0f, 0.0f);
			IndicatorChange();
			break;
		case GLFW_KEY_B:
			color = glm::vec3(0.0f, 0.0f, 1.0f);
			IndicatorChange();
			break;
		case GLFW_KEY_SPACE:
			current.Clear();
			current.New();
			renderer.Draw(permanent + current + colorindicator);
			glfwSwapBuffers(winptr);
			break;
		default:
			moment.keys[key] = true;
		}
	}
	if (action == GLFW_RELEASE)
		moment.keys[key] = false;
	return false;
}

bool App::CursorPosCallback(GLFWwindow * winptr, double x, double y)
{
	return false;
}

bool App::MouseKeyCallback(GLFWwindow * winptr, int key, int action, int mods)
{
	double x, y;
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_MOUSE_BUTTON_LEFT)
		{
			glfwGetCursorPos(winptr, &x, &y);
			current.NextVertex(Vertex{ glm::vec2(x, y), color });
			renderer.Draw(permanent + current + colorindicator);
			glfwSwapBuffers(winptr);
		}
		else if (key == GLFW_MOUSE_BUTTON_RIGHT)
		{
			glfwGetCursorPos(winptr, &x, &y);
			current.NextVertex(Vertex{ glm::vec2(x, y), color });
			current.CloseVertex();
			current.New();
			renderer.Draw(permanent + current + colorindicator);
			glfwSwapBuffers(winptr);
		}
	}

	return false;
}

void App::Run()
{
	double time = glfwGetTime();
	IndicatorChange();
	renderer.Draw(permanent + current + colorindicator);
	glfwSwapBuffers(winptr);
	while (context.PollEvents())
	{
		ColorChange(glfwGetTime() - time);
		time = glfwGetTime();
	};
}

App::~App()
{
}

void App::ColorChange(double timeperiod)
{
	color += glm::vec3(moment.keys[GLFW_KEY_Z], moment.keys[GLFW_KEY_X], moment.keys[GLFW_KEY_C]) * float(timeperiod) * 20.0f;
	if (color.r > 1.0f) color.r = 0.0f;
	if (color.r < 0.0f) color.r = 1.0f;
	if (color.g > 1.0f) color.g = 0.0f;
	if (color.g < 0.0f) color.g = 1.0f;
	if (color.b > 1.0f) color.b = 0.0f;
	if (color.b < 0.0f) color.b = 1.0f;

	if (moment.keys[GLFW_KEY_Z] || moment.keys[GLFW_KEY_X] || moment.keys[GLFW_KEY_C])
	{
		IndicatorChange();
	}
}

void App::IndicatorChange()
{
	colorindicator.Clear();
	glm::ivec2 indicatorloc = glm::ivec2(settings.width, settings.height) - glm::ivec2(30, 20);
	colorindicator.New(current.last);
	switch (current.last)
	{
	case GL_LINE:
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc - glm::ivec2(15, 0)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, 0)), color });
		break;
	case GL_POLYGON:
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, 8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(-10, 8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(-10, -8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, -8)), color });
		break;
	case GL_POINT:
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, 8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(-10, -8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, 0)), color });
		break;
	case GL_POINTS:
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, 8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(-10, 8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(-10, -8)), color });
		colorindicator.NextVertex(Vertex{ glm::vec2(indicatorloc + glm::ivec2(10, -8)), color });
		break;
	}
	colorindicator.CloseVertex();
	renderer.Draw(permanent + current + colorindicator);
	glfwSwapBuffers(winptr);
}
