#include "RenderStageImp.h"
#include "BufferObjectSubmiter.h"

DebugOutput::DebugOutput(unsigned int w, unsigned int h)
	: quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
	forwardvao(Vao{ {3, GL_FLOAT},{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true } }),
	quaddisplaycon(ShaderController({ { "Shader/Debug/QuadDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/QuadDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "display", GL_TEXTURE_2D }})),
	forwarddisplaycon(ShaderController({ { "Shader/Debug/ForwardDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/ForwardDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "WVP", GL_FLOAT_MAT4 } })),
	tiledisplaycon(ShaderController({ { "Shader/Debug/TileDisplayVertex.glsl", GL_VERTEX_SHADER },{ "Shader/Debug/TileDisplayFragment.glsl", GL_FRAGMENT_SHADER } }, { { "tilecount", GL_UNSIGNED_INT_VEC2 }, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0} })),
	width(w), height(h)
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad), sizeof(quad));
	glstate.w = width, glstate.h = height;
}

void DebugOutput::Draw(GLuint display, GLState& oldglstate)
{
	glstate.HotSet(oldglstate);
	quaddisplaycon.Clear();
	quaddisplaycon.Set("display", boost::any(display));
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

	forwardvao.SetData(&vertices[0], sizeof(float) * vertices.size(), sizeof(float) * vertices.size());
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

void DebugOutput::Draw(Vao & vao, mat4(WVP), unsigned int vertoffset, unsigned int vertcount, unsigned int instanceoffset, unsigned int instancecount, GLState & oldglstate, bool clear, bool face)
{	
	GLState particustate = glstate;
	if (!clear)
	{
		if (face)
		{
			particustate.cullface = GL_FRONT;
		}
		else
		{
			particustate.cullface = GL_BACK;
		}
		particustate.blend = true;
	}
	particustate.HotSet(oldglstate);
	forwarddisplaycon.Clear();
	forwarddisplaycon.Set("WVP", boost::any(WVP));
	forwarddisplaycon.Draw();

	vao.Bind();
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	
	if (clear)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
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

ForwardStage::ForwardStage(unsigned int w, unsigned int h) : vao(Vao{ {3, GL_FLOAT}, {2, GL_FLOAT}, {3, GL_FLOAT}, {3, GL_FLOAT}, {1, GL_UNSIGNED_INT} }),
fbo(Fbo{ {w, h}, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT, {{GL_TEXTURE_MIN_FILTER, GL_NEAREST}, {GL_TEXTURE_MAG_FILTER, GL_NEAREST}, {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER}, {GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER}}, glm::vec4(1.0, 1.0, 1.0, 1.0) }, {GL_COLOR_ATTACHMENT0_EXT, GL_RGBA, GL_RGBA16F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE }} } } }),
forwardcon{ {{"Shader/Forward/ForwardVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Forward/ForwardFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"tilecount", GL_UNSIGNED_INT_VEC2}, {"eye", GL_FLOAT_VEC3},
          {"dlcount", GL_UNSIGNED_INT}, {"directionallightlist", GL_UNIFORM_BUFFER, 0}, {"pointlightlist", GL_SHADER_STORAGE_BUFFER, 0}, {"spotlightlist", GL_SHADER_STORAGE_BUFFER, 1},
		  {"lighttransformlist", GL_UNIFORM_BUFFER, 1}, {"pointlightindexlist", GL_SHADER_STORAGE_BUFFER, 2}, {"spotlightindexlist", GL_SHADER_STORAGE_BUFFER, 3}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 4},{ "materiallist", GL_SHADER_STORAGE_BUFFER, 5 } } }
{
	opaquestate.w = w, opaquestate.h = h;
	opaquestate.depthfunc = GL_LEQUAL;
	fullstate.w = w, fullstate.h = h;
	fullstate.depthfunc = GL_LEQUAL, fullstate.facetest = false;
	transstate.w = w, transstate.h = h;
	transstate.blend = true, transstate.depthmask = GL_FALSE, transstate.facetest = false;
	lighttransformlistbuffer = BufferObjectSubmiter::GetInstance().Generate((4 + 32 + 48) * sizeof(LightTransform));
	directionallightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(DirectionalLight) * 4);
	pointlightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(PointLight) * 32);
	spotlightbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(SpotLight) * 48);
	materialbuffer = BufferObjectSubmiter::GetInstance().Generate();
}

void ForwardStage::Init(glm::uvec2 tilecount)
{
	forwardcon.Set("tilecount", boost::any(tilecount));
}

void ForwardStage::Prepare(PerFrameData & framedata, glm::mat4 WVP, std::vector<LightTransform> & lighttransformlist, vec3 eye)
{
	data = &framedata;

	auto& matlist = *data->MaterialList;
	auto& opaquelist = *data->Opaquelist;
	auto& fulltranslist = *data->Fulltranslist;
	auto& translist = *data->Translist;
	auto& tasklist = *data->Transtasklist;
	
	unsigned int vertexcount = opaquelist.size() + fulltranslist.size() + translist.size();
	vao.SetData(nullptr, vertexcount * sizeof(MaterialedVertex), vertexcount * sizeof(MaterialedVertex));
	unsigned int vertexoffset = 0;
	if (opaquelist.size())
	{
		vao.SetSubData(&opaquelist[0], vertexoffset * sizeof(MaterialedVertex), opaquelist.size() * sizeof(MaterialedVertex));
		vertexoffset += opaquelist.size();
	}
	if (fulltranslist.size())
	{
		vao.SetSubData(&fulltranslist[0], vertexoffset * sizeof(MaterialedVertex), fulltranslist.size() * sizeof(MaterialedVertex));
		vertexoffset += fulltranslist.size();
	}
	if (translist.size())
	{
		vao.SetSubData(&translist[0], vertexoffset * sizeof(MaterialedVertex), translist.size() * sizeof(MaterialedVertex));
		vertexoffset += translist.size();
	}
	if (matlist.size())
	{
		BufferObjectSubmiter::GetInstance().SetData(materialbuffer, &matlist[0].material.ambientcolor[0], matlist.size() * sizeof(ShaderMaterial));
	}

	forwardcon.Clear();
	forwardcon.Set("WVP", boost::any(WVP));
	forwardcon.Set("eye", boost::any(eye));
	forwardcon.Set("dlcount", framedata.dlist.size());
	if (lighttransformlist.size())
	{
		BufferObjectSubmiter::GetInstance().SetData(lighttransformlistbuffer, &lighttransformlist[0].View[0][0], lighttransformlist.size() * sizeof(LightTransform));
	}

	if (framedata.dlist.size())
		BufferObjectSubmiter::GetInstance().SetData(directionallightbuffer, &framedata.dlist[0], sizeof(DirectionalLight) * framedata.dlist.size());
	if (framedata.plist.size())
		BufferObjectSubmiter::GetInstance().SetData(pointlightbuffer, &framedata.plist[0], sizeof(PointLight) * framedata.plist.size());
	if (framedata.slist.size())
		BufferObjectSubmiter::GetInstance().SetData(spotlightbuffer, &framedata.slist[0], sizeof(SpotLight) * framedata.slist.size());

	const std::vector<unsigned int> & transtasklist = *framedata.Transtasklist;
	cmdlist.reserve(transtasklist.size());
	cmdlist.resize(0);
	IndirectDraw indraw;
	indraw.baseInstance = 0;
	indraw.instancecount = 1;
	indraw.first = framedata.Opaquelist->size() + framedata.Fulltranslist->size();
	for (auto c : transtasklist)
	{
		indraw.count = c;
		cmdlist.push_back(indraw);
		indraw.first += c;
	}
}

void ForwardStage::Draw(GLState & oldglstate, std::tuple<GLuint, GLuint, GLuint> lightlinked)
{
	forwardcon.Set("lighttransformlist", lighttransformlistbuffer);
	forwardcon.Set("pointlightindexlist", boost::any(std::get<0>(lightlinked)));
	forwardcon.Set("spotlightindexlist", boost::any(std::get<1>(lightlinked)));
	forwardcon.Set("lightlinkedlist", boost::any(std::get<2>(lightlinked)));
	forwardcon.Set("directionallightlist", boost::any(directionallightbuffer));
	forwardcon.Set("pointlightlist", boost::any(pointlightbuffer));
	forwardcon.Set("spotlightlist", boost::any(spotlightbuffer));
	forwardcon.Set("materiallist", boost::any(materialbuffer));
	forwardcon.Draw();

	vao.Bind();
	fbo.Bind();
	glClear(GL_COLOR_BUFFER_BIT);

	if (data->Opaquelist->size())
	{
		opaquestate.HotSet(oldglstate);
		glDrawArrays(GL_TRIANGLES, 0, data->Opaquelist->size());
	}
	
	if (data->Fulltranslist->size())
	{
		fullstate.HotSet(oldglstate);
		glDrawArrays(GL_TRIANGLES, 0, data->Fulltranslist->size());

	}

	if (cmdlist.size())
	{
		transstate.HotSet(oldglstate);
		glMultiDrawArraysIndirect(GL_TRIANGLES, &cmdlist[0], cmdlist.size(), sizeof(cmdlist));
	}
}

LightCullingStage::LightCullingStage(unsigned int w, unsigned int h) : width(w), height(h),
tilecount(static_cast<unsigned int>(glm::ceil(w / static_cast<float>(tilesize.x))), static_cast<unsigned int>(glm::ceil(h / static_cast<float>(tilesize.y)))),
tiledismatchscale(glm::vec2(w, h) / glm::vec2(tilecount * tilesize)),
depthinitializer{ { { "Shader/LightCulling/SSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } },{ { "ssbo", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthdownscaler{ { { "Shader/LightCulling/DepthDownscaleCompute.glsl", GL_COMPUTE_SHADER }}, {{"resolution", GL_UNSIGNED_INT_VEC2}, {"depthsampler", GL_UNSIGNED_INT64_ARB}, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthatomicrenderer{ { {"Shader/LightCulling/DepthAtomicVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/DepthAtomicFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"tilesize", GL_UNSIGNED_INT_VEC2},{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthrangevao{ { { 2, GL_FLOAT } }, GL_STATIC_DRAW },
proxydepth{ Fbo{ {tilecount.x, tilecount.y},{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT16, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
lightindexinitializer{ { { "Shader/LightCulling/DoubleSSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } }, { {"ssbo1", GL_SHADER_STORAGE_BUFFER, 0}, {"ssbo2", GL_SHADER_STORAGE_BUFFER, 1} } },
proxyrenderer{ {{"Shader/LightCulling/ProxyVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/ProxyFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"lightoffset", GL_UNSIGNED_INT}, {"tiledismatchscale", GL_FLOAT_VEC2}, {"tilecount", GL_UNSIGNED_INT_VEC2}, {"ineroroutertest", GL_UNSIGNED_INT}, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 1},{ "testbuffer", GL_SHADER_STORAGE_BUFFER, 2 },{"listcounter", GL_ATOMIC_COUNTER_BUFFER, 2}, {"depthrangebuffer", GL_SHADER_STORAGE_BUFFER, 2} } },
proxyvao{ {3, GL_FLOAT}, {4, GL_FLOAT, true},{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true },{ 4, GL_FLOAT, true } }
{
	depthatomicstate.depthmask = GL_FALSE;
	depthatomicstate.w = w, depthatomicstate.h = h;
	inerproxystate.cullface = GL_FRONT;
	inerproxystate.depthtest = false;
	inerproxystate.w = tilecount.x, inerproxystate.h = tilecount.y;
	inerproxystate.depthclamp = true;
	outerproxystate.cullface = GL_BACK;
	outerproxystate.depthtest = false;
	outerproxystate.w = tilecount.x, outerproxystate.h = tilecount.y;
	outerproxystate.depthclamp = true;
	
	depthminmaxbuffer = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);

	depthdownscaler.Set("resolution", boost::any(glm::uvec2(w, h)));

	depthatomicrenderer.Set("tilesize", boost::any(tilesize));
	depthatomicrenderer.Set("tilecount", boost::any(tilecount));

	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	depthrangevao.SetData(quad, sizeof(quad), sizeof(quad));

	std::vector<float> proxymesh = ProxyIcosahedron::GetVertices();
	const std::vector<float>& proxymesh2 = ProxyPyramid::GetVertices();
	for (auto f : proxymesh2)
	{
		proxymesh.push_back(f);
	}
	proxyvao.SetData(&proxymesh[0], proxymesh.size() * sizeof(float), proxymesh.size() * sizeof(float));

	pointlightindex = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);
	spotlightindex = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y);
	lightlinkedlist = BufferObjectSubmiter::GetInstance().Generate(sizeof(glm::uvec2) * tilecount.x * tilecount.y * 128);
	lightlinkedlistcounter = BufferObjectSubmiter::GetInstance().Generate(sizeof(unsigned int));

	proxyrenderer.Set("tilecount", boost::any(tilecount));
	proxyrenderer.Set("tiledismatchscale", boost::any(tiledismatchscale));
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
}

ShadowStage::ShadowStage() : maxdrange(96.0f), maxprange(96.0f), maxsrange(96.0f),
lineardepthcon{ {{"Shader/Shadow/ShadowLinearVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Shadow/ShadowLinearFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"plane", GL_FLOAT_VEC2}} },
omnidirectionalcon{ { {"Shader/Shadow/ShadowOmnidirectionalVertex.glsl", GL_VERTEX_SHADER}, { "Shader/Shadow/ShadowOmnidirectionalFragment.glsl", GL_FRAGMENT_SHADER}}, { { "WVP", GL_FLOAT_MAT4 },{ "plane", GL_FLOAT_VEC2 }, {"center", GL_FLOAT_VEC3}} }
{
}

void ShadowStage::Init()
{
}

void ShadowStage::Prepare(unsigned int pointshadow, unsigned int spotshadow, PerFrameData & framedata, glm::vec3 eye)
{
	unsigned int oldpmax = pmaxshadow;
	unsigned int oldsmax = smaxshadow;
	dmaxshadow = 4;
	smaxshadow = glm::min(pointshadow, 32u);
	pmaxshadow = glm::min(spotshadow, 48u);
	PrepareFbo(dmaxshadow, oldpmax, oldsmax, pmaxshadow, smaxshadow);
	SetShadowedLight(framedata);
	CalculateVP(framedata, eye);
	lineardepthcon.Clear();
	omnidirectionalcon.Clear();
}

void ShadowStage::Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertscount)
{
	unsigned int dcount = std::get<0>(shadowcount);
	unsigned int pcount = std::get<1>(shadowcount);
	unsigned int scount = std::get<2>(shadowcount);
	vao.Bind();
	lineardepthcon.Draw();

	for (unsigned int i = 0; i != dcount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i].Proj * transformlist[i].View));
		lineardepthcon.Set("plane", boost::any(transformlist[i].plane));
		directionalstate[i].HotSet(oldglstate);
		directionalfbo[i].BindDepth();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);
	}

	for (unsigned int i = 0; i != scount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i + dcount].Proj * transformlist[i + dcount].View));
		lineardepthcon.Set("plane", boost::any(transformlist[i + dcount].plane));
		spotstate[i].HotSet(oldglstate);
		spotfbo[i].BindDepth();
		glClear(GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);
	}

	omnidirectionalcon.Draw();
	for (unsigned int point = 0; point != pcount; point++)
	{
		for (unsigned int subface = 0; subface != 6; subface++)
		{
			unsigned int i = point * 6 + subface;
			omnidirectionalcon.Set("WVP", boost::any(pointvplist[i].Proj * pointvplist[i].View));
			omnidirectionalcon.Set("plane", boost::any(pointvplist[i].plane));
			omnidirectionalcon.Set("center", boost::any(pointposlist[point]));
			pointstate[point].HotSet(oldglstate);
			pointfbo[point][subface].BindDepth();
			glClear(GL_DEPTH_BUFFER_BIT);
			glDrawArrays(GL_TRIANGLES, 0, opacevertscount);
		}
	}
}

void ShadowStage::PrepareFbo(unsigned int dmax, unsigned int oldpmax, unsigned int oldsmax, unsigned int pmax, unsigned int smax)
{
	for (unsigned int i = directionalfbo.size(); i > dmax; i++)
	{
		directionalfbo.pop_back();
	}
	for (unsigned int i = directionalfbo.size(); i < dmax; i++)
	{
		directionalfbo.push_back(Fbo{ { basedresolution, basedresolution },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
		GLState dstate;
		dstate.w = dstate.h = basedresolution;
		directionalstate.push_back(dstate);
	}

	if (oldpmax != pmax)
	{
		pointfbo.clear();
		pointstate.clear();
		auto resolist = CalculateResolution(pmax, highresolutionratio, basepresolution, lowresolutiondivisor);
		for (unsigned int i = 0; i != pmax; i++)
		{
			unsigned int resolution = resolist[i];
			Fbo fbo = Fbo{ { resolution, resolution },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } }, GL_TEXTURE_CUBE_MAP };
			pointfbo.push_back(fbo.GetCubeMapSubFbo());
			pointfbo[pointfbo.size() - 1].push_back(std::move(fbo));
			GLState pstate;
			pstate.w = pstate.h = resolution;
			pointstate.push_back(pstate);
		}
	}

	if (oldsmax != smax)
	{
		spotfbo.clear();
		spotstate.clear();
		auto resolist = CalculateResolution(smax, highresolutionratio, basesresolution, lowresolutiondivisor);
		for (unsigned int i = 0; i != smax; i++)
		{
			unsigned int resolution = resolist[i];
			spotfbo.push_back(Fbo{ { resolution, resolution },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32F, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
			GLState sstate;
			sstate.w = sstate.h = resolution;
			spotstate.push_back(sstate);
		}
	}

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
				l.sampler2D = directionalfbo[dcount].GetDepthHandle();
				dcount++;
			}
			else
			{
				l.hasshadow = false;
				l.sampler2D = 0xFFFFFFFFFFFFFFFF;
			}
		}
	}
	for (auto & l : framedata.plist)
	{
		if (l.hasshadow)
		{
			if (pcount != pmaxshadow)
			{
				l.samplerCubemap = pointfbo[pcount][6].GetDepthHandle();
				pcount++;
			}
			else
			{
				l.hasshadow = false;
				l.samplerCubemap = 0xFFFFFFFFFFFFFFFF;
			}
		}
	}
	for (auto & l : framedata.slist)
	{
		if (l.hasshadow)
		{
			if (scount != smaxshadow)
			{
				l.sampler2D = spotfbo[scount].GetDepthHandle();
				scount++;
			}
			else
			{
				l.hasshadow = false;
				l.sampler2D = 0xFFFFFFFFFFFFFFFF;
			}
		}
	}
	shadowcount = std::tuple<unsigned int, unsigned int, unsigned int>(dcount, pcount, scount);
}

void ShadowStage::CalculateVP(PerFrameData & framedata, glm::vec3 eye)
{
	transformlist.resize(0u);
	pointvplist.resize(0u);
	pointposlist.resize(0u);
	unsigned int dcount = 0, pcount = 0, scount = 0;
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
			glm::mat4 View = glm::lookAt(anchorpoint, anchorpoint + l.direction, up);
			glm::mat4 Proj = glm::ortho(maxdrange, maxdrange, maxdrange, maxdrange, 0.0f, maxdrange * 2);
			float texelworldsize = 2 * maxdrange / glm::max(directionalstate[dcount].w, directionalstate[dcount].h);
			transformlist.push_back(LightTransform{View, Proj, vec2(0.0f, 2 * maxdrange), texelworldsize});
			l.hasshadow = transformlist.size();
			dcount++;
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
			glm::mat4 View = glm::lookAt(l.position, l.position + l.direction, up);
			glm::mat4 Proj = glm::perspective(glm::degrees(glm::acos(l.zerodot)), 1.0f, 0.1f, range);
			float texelworldsize = 2 * glm::tan(glm::acos(l.zerodot)) * 0.1f / glm::max(spotstate[scount].w, spotstate[scount].h);
			transformlist.push_back(LightTransform{ View, Proj, vec2(0.1f, range), texelworldsize});
			l.hasshadow = transformlist.size();
			scount++;
		}
	}
	for (auto & l : framedata.plist)
	{
		if (l.hasshadow)
		{
			float range = glm::clamp(l.GetRange(0.01), 0.2f, maxprange);
			glm::mat4 View = glm::translate(glm::mat4(1.0f), -l.position);
			glm::mat4 Proj = glm::perspective(90.0f, 1.0f, 0.1f, range);
			float texelworldsize = 2 * 0.1f / glm::max(pointstate[pcount].w, pointstate[pcount].h);
			transformlist.push_back(LightTransform{ View, Proj, glm::vec2(0.1f, range), texelworldsize });
			pointposlist.push_back(l.position);
			l.hasshadow = transformlist.size();
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
				glm::mat4 Proj = glm::perspective(90.0f, 1.0f, 0.1f, range);
				glm::mat4 View = glm::lookAt(l.position, l.position + facedir[i], faceupvec[i]);
				pointvplist.push_back(LightTransform{ View, Proj, glm::vec2(0.1f, range), texelworldsize });
				pcount++;
			}
		}
	}
}

std::vector<unsigned int> ShadowStage::CalculateResolution(unsigned int shadowcount, float highpratio, unsigned int baseresolution, unsigned int resolutiondivisor)
{
	unsigned int highpcount = static_cast<unsigned int>(glm::ceil(static_cast<float> (shadowcount) * highpratio));
	unsigned int lowpcount = shadowcount - highpcount;
	unsigned int highpresolution = baseresolution;
	unsigned int lowpresolution = baseresolution / resolutiondivisor;

	std::vector<unsigned int> resolist;
	for (unsigned int i = 0; i < highpcount; i++)
	{
		resolist.push_back(highpresolution);
	}
	for (unsigned int i = 0; i < lowpcount; i++)
	{
		resolist.push_back(lowpresolution);
	}
	return resolist;
}