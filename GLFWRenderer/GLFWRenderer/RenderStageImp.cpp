#include "RenderStageImp.h"
#include "BufferObjectSubmiter.h"
DebugOutput::DebugOutput(unsigned int w, unsigned int h)
	: vao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	displaycon(ShaderController({ { "Shader/Debug/QuadDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/QuadDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "display", GL_TEXTURE_2D },{ "winSize",GL_UNSIGNED_INT_VEC2 } })),
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
	displaycon.Set("winSize", boost::any(glm::uvec2(width, height)));
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

	depthcontroller.Draw();

	vao.Bind();
	fbo.BindDepth();
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

ForwardStage::ForwardStage(unsigned int w, unsigned int h) : vao(Vao{ {3, GL_FLOAT}, {2, GL_FLOAT}, {3, GL_FLOAT}, {3, GL_FLOAT} }),
fbo(Fbo{ {w, h}, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT, {{GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST}, {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER}, {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER}}, glm::vec4(1.0, 1.0, 1.0, 1.0) } } }),
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

	auto diffuse = data->Material[0].diffusetex->GetObjectHandle();
	forwardcon.Set("diffuse", boost::any(diffuse));
	forwardcon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

LightCullingStage::LightCullingStage(unsigned int w, unsigned int h) : 
tilecount(static_cast<unsigned int>(glm::ceil(w / static_cast<float>(tilesize.x))), static_cast<unsigned int>(glm::ceil(h / static_cast<float>(tilesize.y)))),
depthinitializer{ { { "Shader/LightCulling/SSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } },{ { "ssbo", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthdownscaler{ { { "Shader/LightCulling/DepthDownscaleCompute.glsl", GL_COMPUTE_SHADER }}, {{"resolution", GL_UNSIGNED_INT_VEC2}, {"depthsampler", GL_UNSIGNED_INT64_NV}, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthatomicrenderer{ { {"Shader/LightCulling/DepthAtomicVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/DepthAtomicFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_MATRIX4_ARB}, {"tilesize", GL_UNSIGNED_INT_VEC2},{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthrangerenderer{ { {"Shader/LightCulling/DepthRangeVertex.glsl", GL_VERTEX_SHADER}, { "Shader/LightCulling/DepthRangeFragment.glsl", GL_FRAGMENT_SHADER }}, {{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"minormax", GL_UNSIGNED_INT},{ "depthrange", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthrangevao{ { { 2, GL_FLOAT } }, GL_STATIC_DRAW },
mindepth{ Fbo{ {tilecount.x, tilecount.y},{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
maxdepth{ Fbo{ {tilecount.x, tilecount.y },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } }
{
	depthatomicstate.depthmask = GL_FALSE;
	depthrangestate.depthfunc = GL_ALWAYS;

	depthminmaxbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(uvec2) * tilecount.x * tilecount.y);

	depthinitializer.Set("ssbo", boost::any(depthminmaxbuffer));

	depthdownscaler.Set("depthrange", boost::any(depthminmaxbuffer));
	depthdownscaler.Set("resolution", boost::any(uvec2(w, h)));

	depthatomicrenderer.Set("depthrange", boost::any(depthminmaxbuffer));
	depthatomicrenderer.Set("tilesize", boost::any(tilesize));
	depthatomicrenderer.Set("tilecount", boost::any(tilecount));

	depthrangerenderer.Set("tilecount", boost::any(tilecount));
	depthrangerenderer.Set("depthrange", boost::any(depthminmaxbuffer));

	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	depthrangevao.SetData(quad, sizeof(quad));
}

void LightCullingStage::Init(Fbo & forwardstagefbo)
{
	forwardfbo = &forwardstagefbo;

	depthdownscaler.Set("depthsampler", boost::any(forwardstagefbo.GetDepthHandle()));
}

void LightCullingStage::Prepare(glm::mat4 WVP)
{
	depthatomicrenderer.Clear();
	depthatomicrenderer.Set("WVP", boost::any(WVP));

	depthdownscaler.Clear();

	depthinitializer.Clear();

	depthrangerenderer.Clear();
	depthrangerenderer.Set("minormax", boost::any(1u));
}

void LightCullingStage::Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertcount, unsigned int transvertcount)
{
	// Initialize SSBO
	{
		depthinitializer.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Trans depth draw to image. And force the depth render to complete
	if (transvertcount)
	{
		depthatomicstate.HotSet(oldglstate);
		depthatomicrenderer.Draw();
		vao.Bind();
		forwardfbo->BindDepth();
		glDrawArrays(GL_TRIANGLES, opacevertcount, transvertcount);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Downscale opace depth
	if (opacevertcount)
	{
		depthdownscaler.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Draw from SSBO to depth
	{
		depthrangestate.HotSet(oldglstate);
		depthrangerenderer.Draw();
		depthrangevao.Bind();

		mindepth.BindDepth();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		
		depthrangerenderer.Set("minormax", boost::any(0u));
		maxdepth.BindDepth();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	CheckGLError();
	CheckGLError();
}
