#include "App.h"
App::App()
{
	init(moment.keys, false, 128);
	context.RegisterReceiver(this);

	IndexedModel t("Res/MC/Stein.obj");
	ArrayModel am(t);
	ArrayModel am5(t);
	IndexedModel t2("Res/MC/Sand.obj");
	ArrayModel am2(t2);
	ArrayModel am3(t2);
	ArrayModel am4(am3);
	
	am.Transform(scale(mat4(1.0), vec3(40.0, 1.0, 40.0)));
	am.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -3.5, 0.0)));
	am2.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -2.5, 0.0)));
	am3.Transform(translate(glm::mat4(1.0), glm::vec3(-5.0, 0.0, 0.0)));
	

	DirectionalLight dl[] = { 
		DirectionalLight(1.0f,  glm::vec3(1.0f, 1.0f, 1.0f), 1.0, 0.2, glm::vec3(0.0f, -1.0f, 0.0f)),
		DirectionalLight(0.0f,  glm::vec3(1.0f, 1.0f, 1.0f), 1.0, 0.2,  glm::vec3(0.0f, 0.0f, -1.0f))
	};
	PointLight pl[] = {
		PointLight(2.0f, glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 0.5f, true, glm::vec3(0.0f, 3.0f, 2.0f), 1.0f, 0.0f, 0.02f)
	};
	SpotLight sl[] = {
		SpotLight(2.0, glm::vec3(1.0f, 1.0f, 1.0f), 1.0, 0.5 ,true, glm::vec3(0.0f, 5.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), 1.0, 0.0, 0.02, 0.9, 0.7)
	};
	
	manager.Add(am);
	manager.Add(am2);
	manager.Add(am3);
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
		case GLFW_KEY_Z:
			context.HideCursor();
			break;
		case GLFW_KEY_X:
			context.ShowCursor();
			moment.firstmousemove = true;
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

	maincamera.Rotate((x - moment.lastx) * settings.horsense, (y - moment.lasty) * settings.vertsense);

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
