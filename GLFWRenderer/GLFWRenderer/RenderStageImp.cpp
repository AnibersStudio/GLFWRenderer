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
tiledismatchscale(glm::vec2(w, h) / glm::vec2(tilecount * tilesize)),
depthinitializer{ { { "Shader/LightCulling/SSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } },{ { "ssbo", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthdownscaler{ { { "Shader/LightCulling/DepthDownscaleCompute.glsl", GL_COMPUTE_SHADER }}, {{"resolution", GL_UNSIGNED_INT_VEC2}, {"depthsampler", GL_UNSIGNED_INT64_NV}, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthatomicrenderer{ { {"Shader/LightCulling/DepthAtomicVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/DepthAtomicFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_MATRIX4_ARB}, {"tilesize", GL_UNSIGNED_INT_VEC2},{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthrangerenderer{ { {"Shader/LightCulling/DepthRangeVertex.glsl", GL_VERTEX_SHADER}, { "Shader/LightCulling/DepthRangeFragment.glsl", GL_FRAGMENT_SHADER }}, {{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"minormax", GL_UNSIGNED_INT},{ "depthrange", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthrangevao{ { { 2, GL_FLOAT } }, GL_STATIC_DRAW },
mindepth{ Fbo{ {tilecount.x, tilecount.y},{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
maxdepth{ Fbo{ {tilecount.x, tilecount.y },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
lightindexinitializer{ { { "Shader/LightCulling/DoubleSSBOInitialize.glsl", GL_COMPUTE_SHADER } }, { {"ssbo1", GL_SHADER_STORAGE_BUFFER, 0}, {"ssbo2", GL_SHADER_STORAGE_BUFFER, 1} } },
proxyrenderer{ {{"Shader/LightCulling/ProxyVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/ProxyFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_MATRIX4_ARB}, {"instanceoffset", GL_UNSIGNED_INT}, {"tiledismatchscale", GL_FLOAT_VEC2}, {"tilecount", GL_UNSIGNED_INT_VEC2}, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 1}, {"listcounter", GL_ATOMIC_COUNTER_BUFFER, 2}} },
proxyvao{ {3, GL_FLOAT}, {4, GL_FLOAT, true},{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true } }
{
	depthatomicstate.depthmask = GL_FALSE;
	depthrangestate.depthfunc = GL_ALWAYS;
	inerproxystate.cullface = GL_FRONT;
	inerproxystate.depthfunc = GL_GREATER;
	inerproxystate.depthmask = GL_FALSE;
	outerproxystate.cullface = GL_BACK;
	outerproxystate.depthfunc = GL_LESS;
	outerproxystate.depthmask = GL_FALSE;

	depthminmaxbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);

	depthinitializer.Set("ssbo", boost::any(depthminmaxbuffer));

	depthdownscaler.Set("depthrange", boost::any(depthminmaxbuffer));
	depthdownscaler.Set("resolution", boost::any(glm::uvec2(w, h)));

	depthatomicrenderer.Set("depthrange", boost::any(depthminmaxbuffer));
	depthatomicrenderer.Set("tilesize", boost::any(tilesize));
	depthatomicrenderer.Set("tilecount", boost::any(tilecount));

	depthrangerenderer.Set("tilecount", boost::any(tilecount));
	depthrangerenderer.Set("depthrange", boost::any(depthminmaxbuffer));

	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	depthrangevao.SetData(quad, sizeof(quad));

	std::vector<float> proxymesh = ProxyIcosahedron::GetVertices();
	const std::vector<float>& proxymesh2 = ProxyPyramid::GetVertices();
	for (auto f : proxymesh2)
	{
		proxymesh.push_back(f);
	}
	proxyvao.SetData(&proxymesh[0], proxymesh.size() * sizeof(float));

	pointlightindex = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);
	spotlightindex = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);
	lightlinkedlist = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y * 256);
	lightlinkedlistcounter = BufferObjectSubmiter::GetInstance().Generate(sizeof(unsigned int));

	proxyrenderer.Set("tilecount", boost::any(tilecount));
	proxyrenderer.Set("tiledismatchscale", boost::any(tiledismatchscale));
}

void LightCullingStage::Init(Fbo & forwardstagefbo)
{
	forwardfbo = &forwardstagefbo;

	depthdownscaler.Set("depthsampler", boost::any(forwardstagefbo.GetDepthHandle()));
}

void LightCullingStage::Prepare(glm::mat4 WVP, PerFrameData framedata)
{
	depthatomicrenderer.Clear();
	depthatomicrenderer.Set("WVP", boost::any(WVP));

	depthdownscaler.Clear();

	depthinitializer.Clear();

	depthrangerenderer.Clear();
	depthrangerenderer.Set("minormax", boost::any(1u));

	pointlightcount = framedata.plist.size();
	spotlightcount = framedata.slist.size();
	inerpointlightcount = framedata.pinercount;
	inerspotlightcount = framedata.sinercount;

	proxyvao.SetInstanceData(&framedata.lightinstancemat[0][0][0], framedata.lightinstancemat.size() * sizeof(glm::mat4));
	unsigned int countorigin = 1;
	BufferObjectSubmiter::GetInstance().SetData(lightlinkedlistcounter, &countorigin, sizeof(unsigned int));

	lightindexinitializer.Clear();
	proxyrenderer.Clear();
	proxyrenderer.Set("WVP", boost::any(WVP));
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
	// Initialize 2 Light index/linked
	{
		lightindexinitializer.Set("ssbo1", boost::any(pointlightindex));
		lightindexinitializer.Set("ssbo2", boost::any(spotlightindex));
		lightindexinitializer.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Draw Proxies
	{
		proxyrenderer.Set("lightlinkedlist", boost::any(lightlinkedlist));
		proxyrenderer.Set("listcounter", boost::any(lightlinkedlistcounter));
		proxyrenderer.Draw();
		proxyvao.Bind();

		unsigned int instanceoffset[4] {0, inerpointlightcount, 0, inerspotlightcount};
		GLuint lightindexobject[2] { pointlightindex, spotlightindex };
		unsigned int instancecount[4] {inerpointlightcount, pointlightcount - inerpointlightcount, inerspotlightcount, spotlightcount - inerspotlightcount};
		GLState glstates[4] { inerproxystate, outerproxystate, inerproxystate, outerproxystate };
		Fbo * fbo[4] { &mindepth, &maxdepth, &mindepth, &maxdepth };

		unsigned int vertexcount[4]{ ProxyIcosahedron::GetVertexCount(), ProxyIcosahedron::GetVertexCount(), ProxyPyramid::GetVertexCount(), ProxyPyramid::GetVertexCount() };
		unsigned int vertexorigin[4]{0, 0, ProxyIcosahedron::GetVertexCount(), ProxyIcosahedron::GetVertexCount()};
		unsigned int instancestride = 0;
		for (unsigned int i = 0; i != 2; i++) //point and spot light
		{
			proxyrenderer.Set("lightindexlist", lightindexobject[i]);
			for (unsigned int j = 0; j != 2; j++) //iner and outer
			{
				unsigned int drawid = i * 2 + j;
				glstates[drawid].HotSet(oldglstate);
				fbo[drawid]->BindDepth();

				glDrawArraysInstancedBaseInstance(GL_TRIANGLES, vertexorigin[drawid], vertexcount[drawid], instancecount[drawid], instancestride);
				glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				instancestride += instancecount[drawid];
			}
		}
	}
	CheckGLError();
	CheckGLError();
}

