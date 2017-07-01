#include "App.h"
App::App()
{
	using namespace glm;
	init(moment.keys, false, 128);
	context.RegisterReceiver(this);

	meshmanager.RegisterMesh("Res/MC/Stein.obj", "Stein");
	meshmanager.RegisterMesh("Res/MC/Sand.obj", "Sand");

	glm::mat4 bigone = scale(mat4(1.0), vec3(20.0, 1.0, 20.0));
	bigone = translate(glm::mat4(1.0), glm::vec3(0.0, -3.5, 0.0)) * bigone;

	meshmanager.Add("Sand", bigone);
	meshmanager.Add("Sand", glm::mat4(1.0f));
	// Delete/Transform/Clean not tested.

	SpotLight spot2{ glm::vec3(1.0f, 1.0f, 1.0f), 8.0f, 1.0f, 1.0f, 0, true,{ 1.0f, 0.2f, 0.0f }, vec3(2.0f, 40.0f, 2.0f), 0.4f, vec3(0.0f, -1.0f, 0.0f), 0.6f };
	//SpotLight spot{ glm::vec3(1.0f, 1.0f, 1.0f), 10.0f, 1.0f, 1.0f, 0, true,{ 4.0f, 0.4f, 0.0f }, vec3(4.0f, 3.0f, 10.0f), 0.4f, vec3(-0.707f, -0.707f, 0.0f), 0.6f };
	PointLight point{ glm::vec3(1.0f, 1.0f, 1.0f), 4.0f, 1.0f, 1.0f, 0, true, {1.0f, 0.4f, 0.0f}, vec3(3.0f, 6.0f, 3.0f) };
	//PointLight point2{ glm::vec3(1.0f, 1.0f, 1.0f), 5.0f, 1.0f, 1.0f, 0, true,{ 2.0f, 0.4f, 0.0f }, vec3(-3.0f, 3.0f, 3.0f) };
	//lightmanager.Add(spot2);
	//lightmanager.Add(spot2);
	stuba = lightmanager.Add(point);
	//stuba = lightmanager.Add(point2);
	std::cout << "Press Z to hide cursor. X to display cursor.";
}

bool App::KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		//decltype(lightmanager.Add(SpotLight())) stub = boost::any_cast<decltype(lightmanager.Add(SpotLight()))>(stuba);
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(winptr, GL_TRUE);
			break;
		case GLFW_KEY_Z:
			context.HideCursor();
			break;
		case GLFW_KEY_X:
			context.ShowCursor();
			moment.firstmousemove = true;
			break;
		case GLFW_KEY_C:
			rendercontext.isdebug = !rendercontext.isdebug;
			break;
		case GLFW_KEY_V:
			//++(*std::get<2>(stub)).position.y;
			//std::cout << ++(*std::get<2>(stub)).position.y;
			break;
		case GLFW_KEY_B:
			//--(*std::get<2>(stub)).position.y;
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
	if (moment.firstmousemove)
	{
		moment.lastx = x;
		moment.lasty = y;
		moment.firstmousemove = false;
	}

	maincamera.Rotate(-(x - moment.lastx) * settings.horsense, -(y - moment.lasty) * settings.vertsense);

	moment.lastx = x;
	moment.lasty = y;
	return false;
}

void App::Run()
{
	double starttime = glfwGetTime();
	double framestarttime = glfwGetTime();
	int frames = 0;
	do 
	{
		UserMove(glfwGetTime() - framestarttime);
		framestarttime = glfwGetTime();
		rendercontext.eye = maincamera.GetEye();
		rendercontext.target = maincamera.GetTarget();
		rendercontext.up = maincamera.GetUp();
		renderer.Draw(rendercontext);
		if (glfwGetTime() - starttime > 1.0)
		{
			std::cout << "fps:" << frames << std::endl;
			starttime += 1.0f;
			frames = 0;
		}
		else
		{
			frames++;
		}

	} while ((context.PollEvents()));
}

App::~App()
{
}


void App::UserMove(double deltatime)
{
	if (moment.keys[GLFW_KEY_W] || moment.keys[GLFW_KEY_S] || moment.keys[GLFW_KEY_A] || moment.keys[GLFW_KEY_D])
	{
		float toward = (moment.keys[GLFW_KEY_W] - moment.keys[GLFW_KEY_S]) * deltatime * player.movespeed;
		float right = (moment.keys[GLFW_KEY_D] - moment.keys[GLFW_KEY_A]) * deltatime * player.movespeed;
		maincamera.Move(toward, right);
	}
}
