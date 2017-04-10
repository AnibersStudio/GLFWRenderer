#pragma once
#include "GLCommon.h"
#include "RenderVao.h"
#include "RenderFbo.h"
#include "ShaderController.h"
#include "RenderContext.h"
#include "PerFrameData.h"

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
	DebugOutput(unsigned int w, unsigned int h);
	void Draw(GLuint display);

	Vao vao;
	ShaderController displaycon;
	unsigned int width;
	unsigned int height;
};

class ForwardStage
{
public:
	ForwardStage(unsigned int w, unsigned int h);

	void Init() {}
	void Prepare(PerFrameData & framedata) { vao.SetData(&framedata.Vertex[0].position, framedata.Vertex.size() * sizeof(Vertex)); }

	Vao & GetVao() { return vao; }
	Fbo & GetFbo() { return fbo; }

	Vao vao;
	Fbo fbo;
};