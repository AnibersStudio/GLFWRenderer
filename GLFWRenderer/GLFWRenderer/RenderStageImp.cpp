#include "RenderStageImp.h"

DebugOutput::DebugOutput(unsigned int w, unsigned int h)
	: vao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	displaycon(ShaderController({ { "Shader/Debug/QuadDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/QuadDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "display", GL_TEXTURE_2D },{ "winSize",GL_FLOAT_VEC2 } })),
	width(w), height(h)
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	vao.SetData(quad, sizeof(quad));
}

void DebugOutput::Draw(GLuint display)
{
	glstate.ColdSet();
	displaycon.Clear();
	displaycon.Set("display", boost::any(display));
	displaycon.Set("winSize", boost::any(glm::vec2(width, height)));
	displaycon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

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

void PreDepthStage::Draw(GLState & oldglstate, Vao & vao, Fbo & fbo, unsigned int vertcount)
{
	glstate.HotSet(oldglstate);
	oldglstate = glstate;

	depthcontroller.Draw();

	vao.Bind();
	fbo.BindDepth();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

ForwardStage::ForwardStage(unsigned int w, unsigned int h) : vao(Vao{ {3, GL_FLOAT}, {2, GL_FLOAT}, {3, GL_FLOAT}, {3, GL_FLOAT} }),
fbo(Fbo{ {w, h}, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT, {{GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST}, {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER}, {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER}}, glm::vec4(1.0, 0.0, 0.0, 0.0) } } }),
forwardcon{ {{"Shader/Forward/ForwardVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Forward/ForwardFragment.glsl", GL_FRAGMENT_SHADER}}, {{"diffuse", GL_UNSIGNED_INT64_ARB}, {"WVP", GL_MATRIX4_ARB}} }
{
}

void ForwardStage::Prepare(PerFrameData & framedata, glm::mat4 WVP)
{
	data = &framedata;
	vao.SetData(&framedata.Vertex[0].position, framedata.Vertex.size() * sizeof(Vertex));
	forwardcon.Clear();
	forwardcon.Set("WVP", boost::any(WVP));
}

void ForwardStage::Draw(GLState & oldglstate, unsigned int vertcount)
{
	glstate.HotSet(oldglstate);
	oldglstate = glstate;

	auto diffuse = data->Material[0].diffusetex->GetObjectHandle();
	forwardcon.Set("diffuse", boost::any(diffuse));
	forwardcon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

