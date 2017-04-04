#pragma once
#include "GLCommon.h"
#include "RenderVao.h"
#include "RenderFbo.h"
#include "ShaderController.h"
#include "RenderContext.h"
class PreDepthStage
{
public:
	PreDepthStage();
	void Init() {}
	void Prepare(glm::mat4 WVP);
	void Draw(Vao & vao, Fbo & fbo, unsigned int vertcount);

private:
	ShaderController depthcontroller;
};

class DebugOutput
{
public:
	DebugOutput();
	void Init() {};
	void Prepare() {};
	void Draw(GLuint display);

	Vao vao;
	ShaderController displaycon;
};