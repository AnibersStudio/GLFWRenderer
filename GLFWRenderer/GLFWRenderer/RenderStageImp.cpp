#include "RenderStageImp.h"

PreDepthStage::PreDepthStage() 
	: depthcontroller (ShaderController({ { "Shader/Depth/NativeDepthVertex.glsl", GL_VERTEX_SHADER },
	{ "Shader/Depth/NativeDepthFragment.glsl", GL_FRAGMENT_SHADER } }, 
	{ { "WVP", GL_MATRIX4_ARB } }))
{}

void PreDepthStage::Prepare(glm::mat4 WVP)
{
	depthcontroller.Clear();
	depthcontroller.Set("WVP", boost::any(WVP));
}

void PreDepthStage::Draw(Vao & vao, Fbo & fbo, unsigned int vertcount)
{
	depthcontroller.Draw();

	vao.Bind();
	fbo.BindDepth();
	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

DebugOutput::DebugOutput(unsigned int w, unsigned int h)
	: vao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)), 
	displaycon(ShaderController({ { "Shader/Debug/QuadDisplayVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Debug/QuadDisplayFragment.glsl", GL_FRAGMENT_SHADER} }, { {"display", GL_TEXTURE_2D}, {"winSize",GL_FLOAT_VEC2} })),
	width(w), height(h)
{
	float quad[] = {-1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f};
	vao.SetData(quad, sizeof(quad));
}

void DebugOutput::Draw(GLuint display)
{
	displaycon.Clear();
	displaycon.Set("display", boost::any(display));
	displaycon.Set("winSize", boost::any(glm::vec2(width, height)));
	displaycon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
