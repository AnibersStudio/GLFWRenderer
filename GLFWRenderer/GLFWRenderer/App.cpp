#include "App.h"
App::App()
{
	context = new GLFWcontext(settings.width, settings.height, "GLFWapp", settings.isfullscreen);
	context->RegisterReceiver(this);

	drawer = new DrawController(settings.width, settings.height);

	IndexedModel t("Res/MC/Stein.obj", true);
	ArrayModel am(t);
	ArrayModel am5(t);
	IndexedModel t2("Res/MC/Sand.obj");
	ArrayModel am2(t2);
	ArrayModel am3(t2);
	//ArrayModel am4(t2);

	am.Transform(scale(mat4(1.0), vec3(40.0, 1.0, 40.0)));
	am.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -3.5, 0.0)));
	//am5.Transform(scale(mat4(1.0), vec3(40.0, 1.0, 40.0)));
	//am5.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -3.5, 0.0)));
	am2.Transform(translate(glm::mat4(1.0), glm::vec3(0.0, -2.5, 0.0)));
	am3.Transform(translate(glm::mat4(1.0), glm::vec3(-5.0, 0.0, 0.0)));
	//am4.Transform(translate(glm::mat4(1.0), glm::vec3(-6.0, 0.0, 0.0)));
	

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
	drawer->GetDLight()[0] = dl[0];
	//drawer->GetDLight()[1] = dl[1];
	//drawer->GetPLight()[0] = pl[0];
	//drawer->GetSLight()[0] = sl[0];
	drawstate.isHDR = false;
	drawstate.isEyeAdapt = EyeAdaptOff;//Be awared: eye adapt touching bandwith bottleneck.
	drawstate.Bloom = 0;//Be awared: Bloom touching bandwith bottleneck. 
	drawstate.isShadow = false;

	init(moment.keys, false, 128);

	*drawer << PositionedArrayModel(am2) << PositionedArrayModel(am3) << PositionedArrayModel(am) ;

}

bool App::KeyCallback(GLFWwindow * winptr, int key, int scancode, int action, int mode)
{
	if (action == GLFW_PRESS)
	{
		vec3& direction = drawer->GetDLight()[0].direction;
		vec4 curdir;
		switch (key)
		{

		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(winptr, GL_TRUE);
			break;
		case GLFW_KEY_Z:
			context->HideCursor();
			break;
		case GLFW_KEY_X:
			context->ShowCursor();
			moment.firstmousemove = true;
			break;
		case GLFW_KEY_V:
			curdir = (vec4(direction, 1.0) * glm::rotate(glm::mat4(1.0f), 10.0f, vec3(1.0, 0.0, 0.0)));
			direction.x = curdir.x;
			direction.y = curdir.y;
			direction.z = curdir.z;
			break;
		case GLFW_KEY_B:
			curdir = (vec4(direction, 1.0) * glm::rotate(glm::mat4(1.0f), -10.0f, vec3(1.0, 0.0, 0.0)));
			direction.x = curdir.x;
			direction.y = curdir.y;
			direction.z = curdir.z;
			break;
		case GLFW_KEY_N:
			curdir = (vec4(direction, 1.0) * glm::rotate(glm::mat4(1.0f), 10.0f, vec3(0.0, 0.0, 1.0)));
			direction.x = curdir.x;
			direction.y = curdir.y;
			direction.z = curdir.z;
			break;
		case GLFW_KEY_M:
			curdir = (vec4(direction, 1.0) * glm::rotate(glm::mat4(1.0f), -10.0f, vec3(0.0, 0.0, 1.0)));
			direction.x = curdir.x;
			direction.y = curdir.y;
			direction.z = curdir.z;
			break;
		case GLFW_KEY_O:
			std::cout << "SLight Y:" << ++drawer->GetSLight()[0].position.y << std::endl;
			break;
		case GLFW_KEY_P:
			std::cout << "SLight Y:" << --drawer->GetSLight()[0].position.y << std::endl;
			break;
		case GLFW_KEY_U:
			std::cout << "PLight Y:" << ++drawer->GetPLight()[0].position.y << std::endl;
			break;
		case GLFW_KEY_I:
			std::cout << "PLight Y:" << --drawer->GetPLight()[0].position.y << std::endl;
			break;
		default:
			moment.keys[key] = true;
		}
	}
	if (action == GLFW_RELEASE)
		moment.keys[key] = false;
	if (action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_T:
			drawer->GetDLight()[1].intensity++;
			break;
		case GLFW_KEY_Y:
			drawer->GetDLight()[1].intensity--;
			break;
		}
	}
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
	drawstate.W = glm::mat4(1.0);
	drawstate.V = maincamera.GetLookat();
	drawstate.P = glm::perspective(50.0f, (float)settings.width / settings.height, 0.1f, 100.0f);
	double starttime = glfwGetTime();
	double framestarttime = glfwGetTime();
	int frames = 0;
	do 
	{

		UserMove(glfwGetTime() - framestarttime);
		framestarttime = glfwGetTime();
		drawstate.W = glm::mat4(1.0);
		drawstate.V = maincamera.GetLookat();
		drawstate.P = glm::perspective(50.0f, (float)settings.width / settings.height, 0.1f, 100.0f);
		drawer->Draw(drawstate);
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

	} while ((context->PollEvents()));
}

App::~App()
{
	delete context;
	delete drawer;
}


void App::UserMove(double deltatime)
{
	if (moment.keys[GLFW_KEY_W] || moment.keys[GLFW_KEY_S] || moment.keys[GLFW_KEY_A] || moment.keys[GLFW_KEY_D])
	{
		float toward = (moment.keys[GLFW_KEY_W] - moment.keys[GLFW_KEY_S]) * deltatime * player.movespeed;
		float right = (moment.keys[GLFW_KEY_D] - moment.keys[GLFW_KEY_A]) * deltatime * player.movespeed;
		maincamera.Move(toward, right);
		drawstate.V = maincamera.GetLookat();
	}
}
