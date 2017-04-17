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

void DebugOutput::Draw(GLuint display, GLState& oldglstate)
{
	glstate.HotSet(oldglstate);
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
	{ { "WVP", GL_FLOAT_MAT4 } }))
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
forwardcon{ {{"Shader/Forward/ForwardVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Forward/ForwardFragment.glsl", GL_FRAGMENT_SHADER}}, {{"diffuse", GL_UNSIGNED_INT64_ARB}, {"WVP", GL_FLOAT_MAT4}} }
{
}

void ForwardStage::Prepare(PerFrameData & framedata, glm::mat4 WVP)
{
	data = &framedata;
	if (framedata.Vertex.size())
	{
		vao.SetData(&framedata.Vertex[0].position, framedata.Vertex.size() * sizeof(Vertex));
	}
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

LightCullingStage::LightCullingStage(unsigned int w, unsigned int h) : width(w), height(h),
tilecount(static_cast<unsigned int>(glm::ceil(w / static_cast<float>(tilesize.x))), static_cast<unsigned int>(glm::ceil(h / static_cast<float>(tilesize.y)))),
tiledismatchscale(glm::vec2(w, h) / glm::vec2(tilecount * tilesize)),
depthinitializer{ { { "Shader/LightCulling/SSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } },{ { "ssbo", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthdownscaler{ { { "Shader/LightCulling/DepthDownscaleCompute.glsl", GL_COMPUTE_SHADER }}, {{"resolution", GL_UNSIGNED_INT_VEC2}, {"depthsampler", GL_UNSIGNED_INT64_NV}, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthatomicrenderer{ { {"Shader/LightCulling/DepthAtomicVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/DepthAtomicFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"tilesize", GL_UNSIGNED_INT_VEC2},{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"depthrange", GL_SHADER_STORAGE_BUFFER, 0}} },
depthrangerenderer{ { {"Shader/LightCulling/DepthRangeVertex.glsl", GL_VERTEX_SHADER}, { "Shader/LightCulling/DepthRangeFragment.glsl", GL_FRAGMENT_SHADER }}, {{ "tilecount", GL_UNSIGNED_INT_VEC2 }, {"minormax", GL_UNSIGNED_INT},{ "depthrange", GL_SHADER_STORAGE_BUFFER, 0 } } },
depthrangevao{ { { 2, GL_FLOAT } }, GL_STATIC_DRAW },
mindepth{ Fbo{ {tilecount.x, tilecount.y},{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
maxdepth{ Fbo{ {tilecount.x, tilecount.y },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0, 1.0, 1.0, 1.0) } } } },
lightindexinitializer{ { { "Shader/LightCulling/DoubleSSBOInitializeCompute.glsl", GL_COMPUTE_SHADER } }, { {"ssbo1", GL_SHADER_STORAGE_BUFFER, 0}, {"ssbo2", GL_SHADER_STORAGE_BUFFER, 1} } },
proxyrenderer{ {{"Shader/LightCulling/ProxyVertex.glsl", GL_VERTEX_SHADER}, {"Shader/LightCulling/ProxyFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"instanceoffset", GL_UNSIGNED_INT}, {"tiledismatchscale", GL_FLOAT_VEC2}, {"inerclamp", GL_INT}, {"tilecount", GL_UNSIGNED_INT_VEC2}, {"lightindexlist", GL_SHADER_STORAGE_BUFFER, 0}, {"lightlinkedlist", GL_SHADER_STORAGE_BUFFER, 1}, {"listcounter", GL_ATOMIC_COUNTER_BUFFER, 2}} },
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

	depthdownscaler.Set("resolution", boost::any(glm::uvec2(w, h)));

	depthatomicrenderer.Set("tilesize", boost::any(tilesize));
	depthatomicrenderer.Set("tilecount", boost::any(tilecount));

	depthrangerenderer.Set("tilecount", boost::any(tilecount));

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
	// Draw from SSBO to depth
	{
		depthrangestate.HotSet(oldglstate);
		depthrangerenderer.Set("depthrange", boost::any(depthminmaxbuffer));
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
		glViewport(0, 0, tilecount.x, tilecount.y);

		GLuint lightindexobject[2] { pointlightindex, spotlightindex };
		unsigned int instancecount[4] {inerpointlightcount, pointlightcount - inerpointlightcount, inerspotlightcount, spotlightcount - inerspotlightcount};
		GLState glstates[4] { inerproxystate, outerproxystate, inerproxystate, outerproxystate };
		Fbo * depthfbo[4] { &mindepth, &maxdepth, &mindepth, &maxdepth };
		int inerclamp[4]{ true, false, true, false };

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
					proxyrenderer.Set("inerclamp", boost::any(inerclamp[drawid]));
					depthfbo[drawid]->BindDepth();

					glDrawArraysInstancedBaseInstance(GL_TRIANGLES, vertexorigin[drawid], vertexcount[drawid], instancecount[drawid], instancestride);
					glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
				}
				instancestride += instancecount[drawid];
			}
		}
		glViewport(0, 0, width, height);
	}

	GLuint datap[2 * 43 * 24];
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, pointlightindex);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::uvec2) * tilecount.x * tilecount.y, datap);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	GLuint datas[2 * 43 * 24];
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, spotlightindex);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::uvec2) * tilecount.x * tilecount.y, datas);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);	
	GLuint datal[2 * 43 * 24];
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightlinkedlist);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(glm::uvec2) * tilecount.x * tilecount.y, datal);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	CheckGLError();
	CheckGLError();
}

ShadowRenderer::ShadowRenderer() : maxdrange(96.0f), maxprange(96.0f), maxsrange(96.0f),
quadvao(Vao({ { 2, GL_FLOAT } }, GL_STATIC_DRAW)),
lineardepthcon{ {{"Shader/Shadow/ShadowLinearVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Shadow/ShadowLinearFragment.glsl", GL_FRAGMENT_SHADER}}, {{"WVP", GL_FLOAT_MAT4}, {"planes", GL_FLOAT_VEC2}} },
logspaceblurcon{ {{"Shader/Shadow/ShadowBlurVertex.glsl", GL_VERTEX_SHADER}, {"Shader/Shadow/ShadowBlurFragment.glsl", GL_FRAGMENT_SHADER}}, {{"shadowsampler", GL_UNSIGNED_INT64_NV}, {"bluraxis", GL_UNSIGNED_INT}} }
{
	float quad[] = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	quadvao.SetData(quad, sizeof(quad));
}

void ShadowRenderer::Init()
{
}

void ShadowRenderer::Prepare(unsigned int pointshadow, unsigned int spotshadow, PerFrameData & framedata, glm::vec3 eye)
{
	dmaxshadow = 4;
	smaxshadow = glm::min(pointshadow, 32u);
	pmaxshadow = glm::min(spotshadow, 32u);
	SetShadowedLight(framedata);
	CalculateVP(framedata, eye);
}

void ShadowRenderer::Draw(GLState & oldglstate, Vao & vao, unsigned int opacevertscount)
{
	unsigned int dcount = std::get<0>(shadowcount);
	unsigned int pcount = std::get<1>(shadowcount);
	unsigned int scount = std::get<2>(shadowcount);

	glstate.HotSet(oldglstate);
	vao.Bind();
	lineardepthcon.Draw();
	for (unsigned int i = 0; i != dcount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i].VP));
		lineardepthcon.Set("plane", boost::any(transformlist[i].plane));
		GetMiddleFbo(1).first.BindDepth();
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);

		if (highindex % batchcount == 0 || i == dcount - 1)
		{	
			quadvao.Bind();
			logspaceblurcon.Draw();

			GetMiddleFbo(2);
			logspaceblurcon.Set("bluraxis", boost::any(0u));
			for (unsigned int j = 0; j != highindex; j++)
			{
				auto sourcemiddle = GetMiddleFbo(1);
				Fbo & dest = directionalfbo[i - highindex + j];

				logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.first.GetDepthHandle()));

				sourcemiddle.second.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			GetMiddleFbo(2);
			logspaceblurcon.Set("bluraxis", boost::any(1u));
			for (unsigned int j = 0; j != highindex; j++)
			{
				auto sourcemiddle = GetMiddleFbo(1);
				Fbo & dest = directionalfbo[i - highindex + j];

				logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.second.GetDepthHandle()));

				dest.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

			vao.Bind();
			lineardepthcon.Draw();
			GetMiddleFbo(2);
		}
	}

	for (unsigned int i = 0; i != scount; i++)
	{
		lineardepthcon.Set("WVP", boost::any(transformlist[i + dcount].VP));
		lineardepthcon.Set("plane", boost::any(transformlist[i + dcount].plane));
		GetMiddleFbo(0).first.BindDepth();
		glDrawArrays(GL_TRIANGLES, 0, opacevertscount);

		if (index % batchcount == 0 || i == scount - 1)
		{
			quadvao.Bind();
			logspaceblurcon.Draw();

			GetMiddleFbo(2);
			logspaceblurcon.Set("bluraxis", boost::any(0u));
			for (unsigned int j = 0; j != index; j++)
			{
				auto sourcemiddle = GetMiddleFbo(0);
				Fbo & dest = spotfbo[i - index + j];

				logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.first.GetDepthHandle()));

				sourcemiddle.second.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}
			GetMiddleFbo(2);
			logspaceblurcon.Set("bluraxis", boost::any(1u));
			for (unsigned int j = 0; j != index; j++)
			{
				auto sourcemiddle = GetMiddleFbo(0);
				Fbo & dest = spotfbo[i - index + j];

				logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.second.GetDepthHandle()));

				dest.BindDepth();
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			}

			vao.Bind();
			lineardepthcon.Draw();
			GetMiddleFbo(2);
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
			glDrawArrays(GL_TRIANGLES, 0, opacevertscount);

			if (index % batchcount == 0 || i == pcount * 6)
			{
				quadvao.Bind();
				logspaceblurcon.Draw();

				GetMiddleFbo(2);
				logspaceblurcon.Set("bluraxis", boost::any(0u));
				for (unsigned int j = 0; j != index; j++)
				{
					auto sourcemiddle = GetMiddleFbo(0);
					Fbo & dest = pointfbo[(i - index + j) / 6u][(i - index + j) % 6u];

					logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.first.GetDepthHandle()));

					sourcemiddle.second.BindDepth();
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}
				GetMiddleFbo(2);
				logspaceblurcon.Set("bluraxis", boost::any(1u));
				for (unsigned int j = 0; j != index; j++)
				{
					auto sourcemiddle = GetMiddleFbo(0);
					Fbo & dest = pointfbo[(i - index + j) / 6u][(i - index + j) % 6u];

					logspaceblurcon.Set("shadowsampler", boost::any(sourcemiddle.second.GetDepthHandle()));

					dest.BindDepth();
					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				}

				vao.Bind();
				lineardepthcon.Draw();
				GetMiddleFbo(2);
			}
		}
	}


	
}

std::pair<Fbo&, Fbo&> ShadowRenderer::GetMiddleFbo(int option)
{
	switch (option)
	{
	case 2:
		highindex = 0;
		index = 0;
		break;
	case 0:
		index++;
		if (index > middlefbo.size())
		{
			middlefbo.push_back(Fbo{ { 1024, 1024 }, { { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
			singlebluredfbo.push_back(Fbo{ { 1024, 1024 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT24, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
		}
		return std::pair<Fbo&, Fbo&>(middlefbo[index - 1], singlebluredfbo[index - 1]);
		break;
	case 1:
		highindex++;
		if (highindex > highpmiddlefbo.size())
		{
			highpmiddlefbo.push_back(Fbo{ { 4096, 4096 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
			highpsinglebluredfbo.push_back(Fbo{ { 4096, 4096 },{ { GL_DEPTH_ATTACHMENT_EXT, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT32, GL_FLOAT,{ { GL_TEXTURE_MIN_FILTER, GL_NEAREST },{ GL_TEXTURE_MAG_FILTER, GL_NEAREST },{ GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER },{ GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER } }, glm::vec4(1.0) } } });
		}
		return std::pair<Fbo&, Fbo&>(highpmiddlefbo[highindex - 1], highpsinglebluredfbo[highindex - 1]);
		break;
	default:
		throw DrawErrorException("ShadowStage::GetMiddleFbo", "Option" + tostr(option) + "cannot be recognized.");
	}
	return std::pair<Fbo&, Fbo&>(middlefbo[0], singlebluredfbo[0]);
}

void ShadowRenderer::SetShadowedLight(PerFrameData & framedata)
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

void ShadowRenderer::CalculateVP(PerFrameData & framedata, glm::vec3 eye)
{
	transformlist.resize(1u);
	pointvplist.resize(0u);
	for (auto & l : framedata.dlist)
	{
		if (l.hasshadow)
		{
			l.hasshadow = transformlist.size();
			glm::vec3 anchorpoint = eye - l.direction * maxdrange;
			glm::vec3 up{ 0.0, 1.0, 0.0 };
			if (glm::dot(l.direction, up) > 0.9999)
			{
				up = glm::vec3(1.0, 0.0, 0.0);
			}
			glm::mat4 VP = glm::ortho(maxdrange, maxdrange, maxdrange, maxdrange, 0.0f, maxdrange * 2) * glm::lookAt(anchorpoint, anchorpoint + l.direction, up);
			transformlist.push_back(LightTransform{VP, vec2(0.0f, 2 * maxdrange)});
		}
	}
	for (auto & l : framedata.slist)
	{
		if (l.hasshadow)
		{
			l.hasshadow = transformlist.size();
			glm::vec3 up{ 0.0, 1.0, 0.0 };
			if (glm::dot(l.direction, up) > 0.9999)
			{
				up = glm::vec3(1.0, 0.0, 0.0);
			}
			float range = glm::min(l.GetRange(0.05), maxsrange);
			glm::mat4 VP = glm::perspective(glm::degrees(glm::acos(l.zerocos)), 1.0f, 0.1f, range) * glm::lookAt(l.position, l.position + l.direction, up);
			transformlist.push_back(LightTransform{ VP, vec2(0.1f, range) });
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
				float range = glm::min(l.GetRange(0.05), maxprange);
				glm::mat4 VP = glm::perspective(90.0f, 1.0f, 0.1f, range) * glm::lookAt(l.position, l.position + facedir[i], faceupvec[i]);
				pointvplist.push_back(LightTransform{VP, glm::vec2(0.1f, range)});
			}
		}
	}
}