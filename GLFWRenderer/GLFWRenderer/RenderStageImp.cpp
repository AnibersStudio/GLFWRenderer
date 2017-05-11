#include "RenderStageImp.h"
#include "BufferObjectSubmiter.h"
DebugOutput::DebugOutput(unsigned int w, unsigned int h)
	: quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	forwardvao(Vao{ {3, GL_FLOAT},{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true } }),
	quaddisplaycon(ShaderController({ { "Shader/Debug/QuadDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/QuadDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "display", GL_TEXTURE_2D },{ "winSize",GL_UNSIGNED_INT_VEC2 } })),
	forwarddisplaycon(ShaderController({ { "Shader/Debug/ForwardDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/ForwardDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "WVP", GL_FLOAT_MAT4 } })),
	tiledisplaycon(ShaderController({ { "Shader/Debug/TileDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/TileDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "tilecount", GL_UNSIGNED_INT_VEC2 }, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0} })),
	width(w), height(h)
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad));
	glstate.w = width, glstate.h = height;
}

void DebugOutput::Draw(GLuint display, GLState& oldglstate)
{
	glstate.HotSet(oldglstate);
	quaddisplaycon.Clear();
	quaddisplaycon.Set("display", boost::any(display));
	quaddisplaycon.Set("winSize", boost::any(glm::uvec2(width, height)));
	quaddisplaycon.Draw();

	quadvao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DebugOutput::Draw(glm::mat4 WVP, std::vector<float> vertices, std::vector<glm::mat4> instances, GLState & oldglstate)
{
	glstate.HotSet(oldglstate);
	forwarddisplaycon.Clear();
	forwarddisplaycon.Set("WVP", boost::any(WVP));
	forwarddisplaycon.Draw();

	forwardvao.SetData(&vertices[0], sizeof(float) * vertices.size());
	forwardvao.SetInstanceData(&instances[0][0][0], sizeof(glm::mat4) * instances.size());
	forwardvao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, vertices.size() / 3, instances.size(), 0);
}

void DebugOutput::Draw(glm::uvec2 tilecount, GLuint lightindexbuffer, GLState & oldglstate)
{
	glstate.HotSet(oldglstate);
	tiledisplaycon.Clear();
	tiledisplaycon.Set("tilecount", boost::any(tilecount));
	tiledisplaycon.Set("lightindexlist", boost::any(lightindexbuffer));
	tiledisplaycon.Draw();

	quadvao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void DebugOutput::Draw(Vao & vao, mat4(WVP), unsigned int vertoffset, unsigned int vertcount, unsigned int instanceoffset, unsigned int instancecount, GLState & oldglstate)
{
	glstate.HotSet(oldglstate);
	forwarddisplaycon.Clear();
	forwarddisplaycon.Set("WVP", boost::any(WVP));
	forwarddisplaycon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArraysInstancedBaseInstance(GL_TRIANGLES, vertoffset, vertcount, instancecount, instanceoffset);
}

PreDepthStage::PreDepthStage(unsigned int w, unsigned int h)
	: depthcontroller (ShaderController({ { "Shader/Depth/NativeDepthVertex.glsl", GL_VERTEX_SHADER },
	{ "Shader/Depth/NativeDepthFragment.glsl", GL_FRAGMENT_SHADER } }, 
	{ { "WVP", GL_FLOAT_MAT4 } }))
{
	glstate.w = w, glstate.h = h;
}

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
fbo(Fbo{ {w, h}, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT, {{GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST}, {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER}, {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER}}, glm::vec4(1.0, 1.0, 1.0, 1.0) } } }),
forwardcon{ {{"Shader/Forward/ForwardVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Forward/ForwardFragment.glsl", GL_FRAGMENT_SHADER}}, {{"diffusetex", GL_UNSIGNED_INT64_ARB}, {"WVP", GL_FLOAT_MAT4}, {"lightspacecount", GL_UNSIGNED_INT}, {"tilecount", GL_UNSIGNED_INT_VEC2},
         /* {"dlcount", GL_UNSIGNED_INT}, */{"directionallightlist", GL_UNIFORM_BUFFER, 0}, {"pointlightlist", GL_SHADER_STORAGE_BUFFER, 0}, {"spotlightlist", GL_SHADER_STORAGE_BUFFER, 1},
		  {"lighttransformlist", GL_UNIFORM_BUFFER, 1}, {"pointlightindexlist", GL_SHADER_STORAGE_BUFFER, 2}, {"spotlightindexlist", GL_SHADER_STORAGE_BUFFER, 3}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 4}} }
{
	glstate.w = w, glstate.h = h;
	lighttransformlistbuffer = BufferObjectSubmiter::GetInstance().Generate((4 + 16 + 16) * sizeof(LightTransform));
	directionallightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(DirectionalLight) * 4);
	pointlightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(PointLight) * 16);
	spotlightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(SpotLight) * 16);
}

void ForwardStage::Init(glm::uvec2 tilecount)
{
	forwardcon.Set("tilecount", boost::any(tilecount));
}

void ForwardStage::Prepare(PerFrameData & framedata, glm::mat4 WVP, std::vector<LightTransform> & lighttransformlist, std::tuple<unsigned int, unsigned int, unsigned int> shadowcount)
{
	data = &framedata;
	unsigned int dshadowcount = std::get<0>(shadowcount);
	unsigned int sshadowcount = std::get<2>(shadowcount);

	if (framedata.Vertex.size())
	{
		vao.SetData(&framedata.Vertex[0].position, framedata.Vertex.size() * sizeof(Vertex));
	}
	forwardcon.Clear();
	forwardcon.Set("WVP", boost::any(WVP));
	forwardcon.Set("lightspacecount", boost::any(dshadowcount + sshadowcount));
	//forwardcon.Set("dlcount", framedata.dlist.size());
	if (lighttransformlist.size())
	{
		BufferObjectSubmiter::GetInstance().SetData(lighttransformlistbuffer, &lighttransformlist[0].VP[0][0], lighttransformlist.size() * sizeof(LightTransform));
	}

	if (framedata.dlist.size())
		BufferObjectSubmiter::GetInstance().SetData(directionallightbuffer, &framedata.dlist[0], sizeof(DirectionalLight) * framedata.dlist.size());
	if (framedata.plist.size())
		BufferObjectSubmiter::GetInstance().SetData(pointlightbuffer, &framedata.plist[0], sizeof(PointLight) * framedata.plist.size());
	if (framedata.slist.size())
		BufferObjectSubmiter::GetInstance().SetData(spotlightbuffer, &framedata.slist[0], sizeof(SpotLight) * framedata.slist.size());
}

void ForwardStage::Draw(GLState & oldglstate, unsigned int vertcount, std::tuple<GLuint, GLuint, GLuint> lightlinked)
{
	glstate.HotSet(oldglstate);

	auto diffuse = data->Material[0].diffusetex->GetObjectHandle();
	forwardcon.Set("diffusetex", boost::any(diffuse));
	forwardcon.Set("lighttransformlist", lighttransformlistbuffer);
	forwardcon.Set("pointlightindexlist", boost::any(std::get<0>(lightlinked)));
	forwardcon.Set("spotlightindexlist", boost::any(std::get<1>(lightlinked)));
	forwardcon.Set("lightlinkedlist", boost::any(std::get<2>(lightlinked)));
	forwardcon.Set("directionallightlist", boost::any(directionallightbuffer));
	forwardcon.Set("pointlightlist", boost::any(pointlightbuffer));
	forwardcon.Set("spotlightlist", boost::any(spotlightbuffer));
	forwardcon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, vertcount);
}

LightCullingStage::LightCullingStage(unsigned int w, unsigned int h) : width(w), height(h),
tilecount(static_cast<unsigned int>(glm::ceil(w / static_cast<float>(tilesize.x))), static_cast<unsigned int>(glm::ceil(h / static_cast<float>(tilesize.y)))),
tiledismatchscale(glm::vec2(w, h) / glm::vec2(tilecount * tilesize)),
depthinitializer{ { { "Shader/LightCulling/SSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } },{ { "ssbo", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthdownscaler{ { { "Shader/LightCulling/DepthDownscaleCompute.glsl", GL_COMPUTE_SHADER }}, {{"resolution", GL_UNSIGNED_INT_VEC2}, {"depthsampler", GL_UNSIGNED_INT64_NV}, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthatomicrenderer{ { {"Shader/LightCulling/DepthAtomicVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/DepthAtomicFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"tilesize", GL_UNSIGNED_INT_VEC2},{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthrangevao{ { { 2, GL_FLOAT } }, GL_STATIC_DRAW },
proxydepth{ Fbo{ {tilecount.x, tilecount.y},{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
lightindexinitializer{ { { "Shader/LightCulling/DoubleSSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } }, { {"ssbo1", GL_SHADER_STORAGE_BUFFER, 0}, {"ssbo2", GL_SHADER_STORAGE_BUFFER, 1} } },
proxyrenderer{ {{"Shader/LightCulling/ProxyVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/ProxyFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"lightoffset", GL_UNSIGNED_INT}, {"tiledismatchscale", GL_FLOAT_VEC2}, {"tilecount", GL_UNSIGNED_INT_VEC2}, {"ineroroutertest", GL_UNSIGNED_INT}, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 1},{ "testbuffer", GL_SHADER_STORAGE_BUFFER, 2 },{"listcounter", GL_ATOMIC_COUNTER_BUFFER, 2}, {"depthrangebuffer", GL_SHADER_STORAGE_BUFFER, 2}, {"vertexbuffer", GL_SHADER_STORAGE_BUFFER, 3} } },
proxyvao{ {3, GL_FLOAT}, {4, GL_FLOAT, true},{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true } }
{
	depthatomicstate.depthmask = GL_FALSE;
	depthatomicstate.w = w, depthatomicstate.h = h;
	inerproxystate.cullface = GL_FRONT;
	inerproxystate.depthtest = false;
	inerproxystate.w = tilecount.x, inerproxystate.h = tilecount.y;
	outerproxystate.cullface = GL_BACK;
	outerproxystate.depthtest = false;
	outerproxystate.w = tilecount.x, outerproxystate.h = tilecount.y;
	
	depthminmaxbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);

	depthdownscaler.Set("resolution", boost::any(glm::uvec2(w, h)));

	depthatomicrenderer.Set("tilesize", boost::any(tilesize));
	depthatomicrenderer.Set("tilecount", boost::any(tilecount));

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
	lightlinkedlist = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y * 128);
	lightlinkedlistcounter = BufferObjectSubmiter::GetInstance().Generate(sizeof(unsigned int));

	proxyrenderer.Set("tilecount", boost::any(tilecount));
	proxyrenderer.Set("tiledismatchscale", boost::any(tiledismatchscale));

	



	vertexbuffer = BufferObjectSubmiter::GetInstance().Generate(tilecount.x * tilecount.y * sizeof(glm::uvec2));
}

void LightCullingStage::Prepare(glm::mat4 WVP, PerFrameData framedata)
{
	depthatomicrenderer.Clear();
	depthatomicrenderer.Set("WVP", boost::any(WVP));

	depthdownscaler.Clear();

	depthinitializer.Clear();

	pointlightcount = framedata.plist.size();
	spotlightcount = framedata.slist.size();
	inerpointlightcount = framedata.pinercount;
	inerspotlightcount = framedata.sinercount;

	if (framedata.lightinstancemat.size())
	{
		proxyvao.SetInstanceData(&framedata.lightinstancemat[0][0][0], framedata.lightinstancemat.size() * sizeof(glm::mat4));
	}
	unsigned int countorigin = 1;
	BufferObjectSubmiter::GetInstance().SetData(lightlinkedlistcounter, &countorigin, sizeof(unsigned int));

	lightindexinitializer.Clear();

	proxyrenderer.Clear();
	proxyrenderer.Set("WVP", boost::any(WVP));
}

void LightCullingStage::Draw(GLState & oldglstate, Vao & vao, Fbo & fbo, unsigned int opacevertcount, unsigned int transvertcount)
{
	// Initialize SSBO
	{
		depthinitializer.Set("ssbo", boost::any(depthminmaxbuffer));
		depthinitializer.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Initialize 2 Light index/linked
	{
		lightindexinitializer.Set("ssbo1", boost::any(pointlightindex));
		lightindexinitializer.Set("ssbo2", boost::any(spotlightindex));
		lightindexinitializer.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Trans depth draw to image. And force the depth render to complete
	if (transvertcount)
	{
		depthatomicstate.HotSet(oldglstate);
		depthatomicrenderer.Set("depthrange", boost::any(depthminmaxbuffer));
		depthatomicrenderer.Draw();
		vao.Bind();
		fbo.BindDepth();
		glDrawArrays(GL_TRIANGLES, opacevertcount, transvertcount);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	// Downscale opace depth
	if (opacevertcount)
	{
		depthdownscaler.Set("depthsampler", boost::any(fbo.GetDepthHandle()));
		depthdownscaler.Set("depthrange", boost::any(depthminmaxbuffer));
		depthdownscaler.Draw();
		glDispatchCompute(tilecount.x, tilecount.y, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	}
	

	unsigned int t[2000]{};
	init(t, 1u, 2000);
	proxyrenderer.Set("vertexbuffer", vertexbuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexbuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, tilecount.x * tilecount.y * sizeof(unsigned int), t);

	// Draw Proxies
	{
		proxyrenderer.Set("lightlinkedlist", boost::any(lightlinkedlist));
		proxyrenderer.Set("listcounter", boost::any(lightlinkedlistcounter));
		proxyrenderer.Set("depthrangebuffer", boost::any(depthminmaxbuffer));
		proxyrenderer.Draw();
		proxyvao.Bind();
		proxydepth.BindDepth();

		GLuint lightindexobject[2] { pointlightindex, spotlightindex };
		unsigned int instancecount[4] {inerpointlightcount, pointlightcount - inerpointlightcount, inerspotlightcount, spotlightcount - inerspotlightcount};
		unsigned int lightoffset[4] { 0u, inerpointlightcount, 0u, inerspotlightcount };
		GLState glstates[4] { inerproxystate, outerproxystate, inerproxystate, outerproxystate };
		unsigned int ineroroutertest[4]{ true, false, true, false };

		unsigned int vertexcount[4]{ ProxyIcosahedron::GetVertexCount(), ProxyIcosahedron::GetVertexCount(), ProxyPyramid::GetVertexCount(), ProxyPyramid::GetVertexCount() };
		unsigned int vertexorigin[4]{0, 0, ProxyIcosahedron::GetVertexCount(), ProxyIcosahedron::GetVertexCount()};
		unsigned int instancestride = 0;

		for (unsigned int i = 0; i != 2; i++) //point and spot light
		{
			proxyrenderer.Set("lightindexlist", lightindexobject[i]);
			for (unsigned int j = 0; j != 2; j++) //iner and outer
			{
				unsigned int drawid = i * 2 + j;
				if (instancecount[drawid])
				{
					glstates[drawid].HotSet(oldglstate);
					proxyrenderer.Set("lightoffset", boost::any(lightoffset[drawid]));
					proxyrenderer.Set("ineroroutertest", boost::any(ineroroutertest[drawid]));
					
					glDrawArraysInstancedBaseInstance(GL_TRIANGLES, vertexorigin[drawid], vertexcount[drawid], instancecount[drawid], instancestride);
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				}
				instancestride += instancecount[drawid];
			}
		} 
	}


	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertexbuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, tilecount.x * tilecount.y * sizeof(unsigned int), t);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

ShadowStage::ShadowStage() : maxdrange(96.0f), maxprange(96.0f), maxsrange(96.0f),
quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
lineardepthcon{ {{"Shader/Shadow/ShadowLinearVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Shadow/ShadowLinearFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"plane", GL_FLOAT_VEC2}} },
logspaceblurcon{ {{"Shader/Shadow/ShadowBlurVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Shadow/ShadowBlurFragment.glsl", GL_FRAGMENT_SHADER}}, {{"shadowsampler", GL_TEXTURE_2D}, {"bluraxis", GL_UNSIGNED_INT}} }
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad));
	linearstate.w = linearstate.h = 1024;
	linearhighpstate.w = linearhighpstate.h = 4096;

	blurstate.depthfunc = GL_ALWAYS;
	blurstate.w = blurstate.h = 1024;
	blurhighpstate.depthfunc = GL_ALWAYS;
	blurhighpstate.w = blurhighpstate.h = 4096;
}

void ShadowStage::Init()
{
}

void ShadowStage::Prepare(unsigned int pointshadow, unsigned int spotshadow, PerFrameData & framedata, glm::vec3 eye)
{
	dmaxshadow = 4;
	smaxshadow = glm::min(pointshadow, 16u);
	pmaxshadow = glm::min(spotshadow, 16u);
	SetShadowedLight(framedata);
	CalculateVP(framedata, eye);
	lineardepthcon.Clear();
	logspaceblurcon.Clear();

	InitMiddleFbo();
}

void ShadowStage::Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertscount)
{
	unsigned int dcount = std::get<0>(shadowcount);
	unsigned int pcount = std::get<1>(shadowcount);
	unsigned int scount = std::get<2>(shadowcount);
	linearhighpstate.HotSet(oldglstate);
	vao.Bind();
	lineardepthcon.Draw();

	for (unsigned int i = 0; i != dcount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i].VP));
		lineardepthcon.Set("plane", boost::any(transformlist[i].plane));
		GetMiddleFbo(1).first.BindDepth();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);

		if (highindex == batchcount + 1 || i == dcount - 1)
		{	
			blurhighpstate.HotSet(oldglstate);
			quadvao.Bind();
			logspaceblurcon.Draw();

			logspaceblurcon.Set("bluraxis", boost::any(0u));
			for (unsigned int j = 0; j != highindex; j++)
			{
				auto source = highpmiddlefbo[j];
				auto middle = highpsinglebluredfbo[j];

				logspaceblurcon.Set("shadowsampler", boost::any(source.GetDepthID()));

				middle.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			logspaceblurcon.Set("bluraxis", boost::any(1u));
			for (unsigned int j = 0; j != highindex; j++)
			{
				auto middle = highpsinglebluredfbo[j];
				Fbo & dest = directionalfbo[i - highindex + 1 + j];

				logspaceblurcon.Set("shadowsampler", boost::any(middle.GetDepthID()));

				dest.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

			linearhighpstate.HotSet(oldglstate);
			vao.Bind();
			lineardepthcon.Draw();
			InitMiddleFbo();
		}
	}

	linearstate.HotSet(oldglstate);
	for (unsigned int i = 0; i != scount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i + dcount].VP));
		lineardepthcon.Set("plane", boost::any(transformlist[i + dcount].plane));
		GetMiddleFbo(0).first.BindDepth();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);
		if (index == batchcount + 1 || i == scount - 1)
		{
			blurstate.HotSet(oldglstate);
			quadvao.Bind();
			logspaceblurcon.Draw();
			logspaceblurcon.Set("bluraxis", boost::any(0u));
			for (unsigned int j = 0; j != index; j++)
			{
				auto & source = middlefbo[j];
				auto & middle = singlebluredfbo[j];

				logspaceblurcon.Set("shadowsampler", boost::any(source.GetDepthID()));

				middle.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			logspaceblurcon.Set("bluraxis", boost::any(1u));
			for (unsigned int j = 0; j != index; j++)
			{
				auto middle = singlebluredfbo[j];
				Fbo & dest = spotfbo[i - index + 1 + j];
				logspaceblurcon.Set("shadowsampler", boost::any(middle.GetDepthID()));
				dest.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

			linearstate.HotSet(oldglstate);
			vao.Bind();
			lineardepthcon.Draw();
			InitMiddleFbo();
		}
	}

	for (unsigned int point = 0; point != pcount; point++)
	{
		for (unsigned int subface = 0; subface != 6; subface++)
		{
			unsigned int i = point * 6 + subface;
			lineardepthcon.Set("WVP", boost::any(pointvplist[i + dcount].VP));
			lineardepthcon.Set("plane", boost::any(pointvplist[i + dcount].plane));
			GetMiddleFbo(0).first.BindDepth();
			glClear(GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, opacevertscount);

			if (index == batchcount + 1 || i == pcount * 6 - 1)
			{
				blurstate.HotSet(oldglstate);
				quadvao.Bind();
				logspaceblurcon.Draw();

				logspaceblurcon.Set("bluraxis", boost::any(0u));
				for (unsigned int j = 0; j != index; j++)
				{
					auto & source = middlefbo[j];
					auto & middle = singlebluredfbo[j];

					logspaceblurcon.Set("shadowsampler", boost::any(source.GetDepthID()));

					middle.BindDepth();
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}
				logspaceblurcon.Set("bluraxis", boost::any(1u));
				for (unsigned int j = 0; j != index; j++)
				{
					auto & middle = singlebluredfbo[j];
					Fbo & dest = pointfbo[(i - index + 1 + j) / 6u][(i - index + 1 + j) % 6u];

					logspaceblurcon.Set("shadowsampler", boost::any(middle.GetDepthID()));

					dest.BindDepth();
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}

				linearstate.HotSet(oldglstate);
				vao.Bind();
				lineardepthcon.Draw();
				InitMiddleFbo();
			}
		}
	}
	CheckGLError();
	CheckGLError();
}

std::pair<Fbo&, Fbo&> ShadowStage::GetMiddleFbo(int option)
{
	switch (option)
	{
	case 0:
		index++;
		if (index > middlefbo.size())
		{
			middlefbo.push_back(Fbo{ { 1024, 1024 }, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE } }} } });
			singlebluredfbo.push_back(Fbo{ { 1024, 1024 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE } }} } });
		}
		return std::pair<Fbo&, Fbo&>(middlefbo[index - 1], singlebluredfbo[index - 1]);
		break;
	case 1:
		highindex++;
		if (highindex > highpmiddlefbo.size())
		{
			highpmiddlefbo.push_back(Fbo{ { 4096, 4096 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE } }, glm::vec4(1.0) } } });
			highpsinglebluredfbo.push_back(Fbo{ { 4096, 4096 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE } }, glm::vec4(1.0) } } });
		}
		return std::pair<Fbo&, Fbo&>(highpmiddlefbo[highindex - 1], highpsinglebluredfbo[highindex - 1]);
		break;
	default:
		throw DrawErrorException("ShadowStage::GetMiddleFbo", "Option" + tostr(option) + "cannot be recognized.");
	}
}

void ShadowStage::InitMiddleFbo()
{
	highindex = 0;
	index = 0;
}

void ShadowStage::SetShadowedLight(PerFrameData & framedata)
{
	unsigned int pcount = 0, scount = 0, dcount = 0;
	for (auto & l : framedata.dlist)
	{
		if (l.hasshadow)
		{
			if (dcount != dmaxshadow)
			{
				dcount++;
			}
			else
			{
				l.hasshadow = false;
			}
		}
	}
	for (auto & l : framedata.plist)
	{
		if (l.hasshadow)
		{
			if (pcount != pmaxshadow)
			{
				pcount++;
			}
			else
			{
				l.hasshadow = false;
			}
		}
	}
	for (auto & l : framedata.slist)
	{
		if (l.hasshadow)
		{
			if (scount != smaxshadow)
			{
				scount++;
			}
			else
			{
				l.hasshadow = false;
			}
		}
	}
	shadowcount = std::tuple<unsigned int, unsigned int, unsigned int>(dcount, pcount, scount);

	for (unsigned int i = directionalfbo.size(); i != dcount; i++)
	{
		directionalfbo.push_back(Fbo{ { 4096, 4096 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
	}
	for (unsigned int i = pointfbo.size(); i != pcount; i++)
	{
		Fbo fbo = Fbo{ { 1024, 1024 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } }, GL_TEXTURE_CUBE_MAP};
		pointfbo.push_back(fbo.GetCubeMapSubFbo());
		pointfbo[pointfbo.size() - 1].push_back(std::move(fbo));
	}
	for (unsigned int i = spotfbo.size(); i != scount; i++)
	{
		spotfbo.push_back(Fbo{ { 1024, 1024 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
	}

	pcount = dcount = scount = 0;
	for (auto & l : framedata.dlist)
	{
		if (l.hasshadow)
		{
			l.sampler2D = directionalfbo[dcount].GetDepthHandle();
			dcount++;
		}
		else
		{
			l.sampler2D = 0xFFFFFFFFFFFFFFFF;
		}
	}
	for (auto & l : framedata.plist)
	{
		if (l.hasshadow)
		{
			l.samplerCubemap = pointfbo[pcount][6].GetDepthHandle();
		}
		else
		{
			l.samplerCubemap = 0xFFFFFFFFFFFFFFFF;
		}
	}
	for (auto & l : framedata.slist)
	{
		if (l.hasshadow)
		{
			l.sampler2D = spotfbo[scount].GetDepthHandle();
		}
		else
		{
			l.sampler2D = 0xFFFFFFFFFFFFFFFF;
		}
	}
}

void ShadowStage::CalculateVP(PerFrameData & framedata, glm::vec3 eye)
{
	transformlist.resize(0u);
	pointvplist.resize(0u);
	for (auto & l : framedata.dlist)
	{
		if (l.hasshadow)
		{
			glm::vec3 anchorpoint = eye - l.direction * maxdrange;
			glm::vec3 up{ 0.0, 1.0, 0.0 };
			if (glm::abs(glm::dot(l.direction, up)) > 0.9999)
			{
				up = glm::vec3(1.0, 0.0, 0.0);
			}
			glm::mat4 VP = glm::ortho(maxdrange, maxdrange, maxdrange, maxdrange, 0.0f, maxdrange * 2) * glm::lookAt(anchorpoint, anchorpoint + l.direction, up);
			transformlist.push_back(LightTransform{VP, vec2(0.0f, 2 * maxdrange)});
			l.hasshadow = transformlist.size();
		}
	}
	for (auto & l : framedata.slist)
	{
		if (l.hasshadow)
		{
			glm::vec3 up{ 0.0, 1.0, 0.0 };
			if (glm::abs(glm::dot(l.direction, up)) > 0.9999)
			{
				up = glm::vec3(1.0, 0.0, 0.0);
			}
			float range = glm::clamp(l.GetRange(0.01), 0.2f, maxsrange);
			glm::mat4 VP = glm::perspective(glm::degrees(glm::acos(l.zerodot)), 1.0f, 0.1f, range) * glm::lookAt(l.position, l.position + l.direction, up);
			transformlist.push_back(LightTransform{ VP, vec2(0.1f, range) });
			l.hasshadow = transformlist.size();
		}
	}
	for (auto & l : framedata.plist)
	{
		if (l.hasshadow)
		{
			for (unsigned int i = 0; i != 6; i++)
			{
				static vec3 facedir[6] = {
					vec3{ 1.0, 0.0, 0.0 },
					vec3{ -1.0, 0.0, 0.0 },
					vec3{ 0.0, 1.0, 0.0 },
					vec3{ 0.0, -1.0, 0.0 },
					vec3{ 0.0, 0.0, 1.0 },
					vec3{ 0.0, 0.0, -1.0 }
				};
				static vec3 faceupvec[6] = {
					vec3{ 0.0, -1.0, 0.0 },
					vec3{ 0.0, -1.0, 0.0 },
					vec3{ 0.0, 0.0, 1.0 },
					vec3{ 0.0, 0.0, -1.0 },
					vec3{ 0.0, -1.0, 0.0 },
					vec3{ 0.0, -1.0, 0.0 }
				};
				float range = glm::clamp(l.GetRange(0.01), 0.2f, maxprange);
				glm::mat4 VP = glm::perspective(90.0f, 1.0f, 0.1f, range) * glm::lookAt(l.position, l.position + facedir[i], faceupvec[i]);
				pointvplist.push_back(LightTransform{ VP, glm::vec2(0.1f, range) });
				transformlist.push_back(LightTransform{ mat4(1.0), glm::vec2(0.1f, range) });
				l.hasshadow = transformlist.size();
			}
		}
	}
}