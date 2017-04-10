#pragma once
#include "GLCommon.h"
#include "RenderVao.h"
#include "RenderFbo.h"
#include "ShaderController.h"
#include "RenderContext.h"
#include "PerFrameData.h"
#include "GLState.h"

class DebugOutput
{
public:
	DebugOutput(unsigned int w, unsigned int h);
	void Draw(GLuint display);

	Vao vao;
	ShaderController displaycon;
	GLState glstate;
	unsigned int width;
	unsigned int height;
};

class PreDepthStage
{
public:
	PreDepthStage();
	void Init() {}
	void Prepare(glm::mat4 WVP);
	void Draw(GLState & oldglstate, Vao & vao, Fbo & fbo, unsigned int vertcount);
	GLState glstate;
private:
	ShaderController depthcontroller;
};

class ForwardStage
{
public:
	ForwardStage(unsigned int w, unsigned int h);

	void Init() {}
	void Prepare(PerFrameData & framedata, glm::mat4 WVP);
	void Draw(GLState & oldglstate, unsigned int vertcount);

	Vao & GetVao() { return vao; }
	Fbo & GetFbo() { return fbo; }

	Vao vao;
	Fbo fbo;
	PerFrameData * data;
	ShaderController forwardcon;
	GLState glstate;
};