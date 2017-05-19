#include "App.h"
App::App()
{
	using namespace glm;
	init(moment.keys, false, 128);
	context.RegisterReceiver(this);

	IndexedModel t("Res/MC/Stein.obj");
	ArrayModel am(t);
	IndexedModel t2("Res/MC/Sand.obj");
	ArrayModel am2(t2);
	ArrayModel am3(t2);
	ArrayModel am4(t2);
	IndexedModel t3("Res/MC/Diamond.obj");
	ArrayModel am5(t3);

	am.Transform(scale(mat4(1.0), vec3(40.0, 1.0, 40.0)));
	am.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -3.5, 0.0)));
	am2.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -2.5, 0.0)));
	am3.Transform(translate(glm::mat4(1.0), glm::vec3(-5.0, 0.0, 0.0)));
	am5.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, 5.0, -5.0)));
	am5.Transform(rotate(glm::mat4(1.0), 90.0f, glm::vec3(0.0, 1.0, 0.0)));
	am4.Transform(scale(glm::mat4(1.0), glm::vec3(0.1, 0.1, 0.1)));
	am4.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, 5.0, 0.0)));

	meshmanager.Add(am);
	meshmanager.Add(am2);
	//meshmanager.Add(am3);
	//meshmanager.Add(am5);
	meshmanager.Add(am4);

	SpotLight spot2{ glm::vec3(1.0f, 1.0f, 1.0f), 6.0f, 1.0f, 1.0f, 0, true,{ 4.0f, 0.4f, 0.0f }, vec3(0.0f, 10.0f, 0.0f), 0.4f, vec3(0.0f, -1.0f, 0.0f), 0.6f };
	SpotLight spot{ glm::vec3(1.0f, 1.0f, 1.0f), 10.0f, 1.0f, 1.0f, 0, true,{ 4.0f, 0.4f, 0.0f }, vec3(4.0f, 8.0f, 0.0f), 0.4f, vec3(-0.707f, -0.707f, 0.0f), 0.6f };
	PointLight point{ glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 1.0f, 1.0f, 0, true, {2.0f, 0.4f, 0.0f}, vec3(3.0f, 3.0f, 3.0f) };
	PointLight point2{ glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 1.0f, 1.0f, 0, true,{ 2.0f, 0.4f, 0.0f }, vec3(-3.0f, 3.0f, 3.0f) };
	//stuba = lightmanager.Add(spot);
	//lightmanager.Add(spot2);
	lightmanager.Add(point);
	stuba = lightmanager.Add(point2);
}

bool App::KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		decltype(lightmanager.Add(SpotLight())) stub = boost::any_cast<decltype(lightmanager.Add(SpotLight()))>(stuba);
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
			(* std::get<2>(stub)).position.y ++;
			break;
		case GLFW_KEY_B:
			(*std::get<2>(stub)).position.y--;
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
