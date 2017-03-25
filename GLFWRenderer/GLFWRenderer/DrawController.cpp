#include "DrawController.h"
#include <limits>
#include <queue>
#include <iomanip>

DrawController::DrawController(unsigned int w, unsigned int h) : width(w), height(h)
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	{//Depth Render Passes
		//Render Source:
		std::vector<VaoSetting> depthvaosetting = { { 3, GL_FLOAT, sizeof(float) } };
		depthvaoptr = new VaoStruct(depthvaosetting, GL_STREAM_DRAW);
		//Prepare depth controller
		dsc = new DepthShaderController("Shader/DepthVertex.glsl", "Shader/DepthFragment.glsl");
		dsc->LoadShaderPipeline();
		//Render Target:
		ddepthfboptr = new FboStruct(FboSetting(depthtexwidth, depthtexheight, 0U, DepthTextureT, GL_DEPTH_COMPONENT32, GL_NONE, GL_NEAREST, GL_CLAMP_TO_BORDER));
		sdepthfboptr = new FboStruct(FboSetting(depthtexwidth, depthtexheight, 0U, DepthTextureT, GL_DEPTH_COMPONENT32, GL_NONE, GL_NEAREST, GL_CLAMP_TO_BORDER));
		pdepthfboptr = new FboStruct(FboSetting(depthtexwidth, depthtexheight, 0U, DepthCubeMapT, GL_DEPTH_COMPONENT32, GL_NONE, GL_NEAREST, GL_CLAMP_TO_BORDER));
	}
	{//Forward Render Passes
		//Render source:
		std::vector<VaoSetting> forwardvaosetting
		{ { 3, GL_FLOAT, sizeof(float) }, {2, GL_FLOAT, sizeof(float) }, { 3, GL_FLOAT, sizeof(float)  },{ 3, GL_FLOAT, sizeof(float) } };
		forwardvaoptr = new VaoStruct(forwardvaosetting, GL_STREAM_DRAW);
		//Prepare Forward render shader
		flsc = new ForwardLightShaderController("Shader/ForwardVertex.glsl", "Shader/ForwardFragment.glsl");
		flsc->LoadShaderPipeline();
		//Render Target:
		hdrfboptr = new FboStruct(FboSetting(w, h, 2U, DepthBufferT, GL_DEPTH_COMPONENT24, GL_RGBA32F));
	}
	PrepareQuad();//Quad
	{//Bloom Render Passes
		//Render source: Quad
		//Prepare Bloom reprocess shader
		bsc = new BloomShaderController("Shader/GussianBlurVertex.glsl", "Shader/GussianBlurFragment.glsl");
		bsc->LoadShaderPipeline();
		//Render Target:
		FboSetting pingpongsetting(w, h, 1U, None, GL_NONE, GL_RGBA32F);
		for (int i = 0; i != 2; i++)
		{
			pingpongfboptr = new FboStruct[2]{ pingpongsetting, pingpongsetting };
		}
	}
	{//Mix Render Passes
		//Render source: Quad
		//Prepare mix reprocess shader
		msc = new MixShaderController("Shader/MixVertex.glsl", "Shader/MixFragment.glsl");
		msc->LoadShaderPipeline();
		//Render Target:
		mixfboptr = new FboStruct(FboSetting(w, h, 1U, None, GL_NONE, GL_RGBA32F));
	}
	{//HDR Render Passes
		//Render source: Quad
		//Prepare HDR reprocess shader
		hdrsc = new HDRShaderController("Shader/HDRVertex.glsl", "Shader/HDRFragment.glsl");
		hdrsc->LoadShaderPipeline();
		//Render Target: Screen
	}
	{//Prepare HDR render framebuffer/exposure cache/pixelbuffer/quad vertices
		{//Pixelbuffer
			glGenBuffers(1, &eadata.pbo);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, eadata.pbo);
			glBufferData(GL_PIXEL_PACK_BUFFER, w * h * 4 * sizeof(float), NULL, GL_STATIC_READ);
			glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
		}
		{//Exposure cache
			eadata.exposurehistory = new float[eadata.constant.exposurehistorycount];
			init(eadata.exposurehistory, 1.0f, eadata.constant.exposurehistorycount);
			eadata.oldesthistory = 0;
		}
	}
}

void DrawController::Draw(DrawContext & context)
{

	typedef  struct {
		uint  count;
		uint  instanceCount;
		uint  first;
		uint  baseInstance;
	} DrawArraysIndirectCommand;

	DrawArraysIndirectCommand i;

	glDisable(GL_BLEND);
	ForwardLightData lightdata;
	std::vector<vec3> nontransvertices;

	{//Prepare Depth render
		nontransvertices = GetNonTransObjList();
		depthvaoptr->SetData(&nontransvertices[0], nontransvertices.size() * sizeof(vec3));
	}
	lightdata = RenderShadow(context.isShadow, depthtexwidth, depthtexheight, context.PlayerCamera->GetEye(), nontransvertices.size());
	{//Render Depth only
		RenderDepthSingleFace(hdrfboptr, context.W * context.V * context.P, nontransvertices.size());
	}
	hdrfboptr->BindFrameBuffer();
	//screenfbo.BindFrameBuffer();
	{//Prepare Foward render
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		flsc->Use();
		flsc->SetMatrix(context.W, context.V, context.P);
		flsc->SetEye(context.PlayerCamera->GetEye());
		flsc->SetBloom(context.Bloom != 0);

		flsc->SetTextureDepth(lightdata.dlshadow ? ddepthfboptr->DepthComponent : 0, lightdata.plshadow ? pdepthfboptr->DepthComponent : 0, lightdata.slshadow ? sdepthfboptr->DepthComponent : 0);
		flsc->SetLightWVP(lightdata.dwvp, lightdata.swvp, lightdata.plfarplane);

		flsc->SetDLight(lightdata.dlist.size(), lightdata.dlist.size() ? &lightdata.dlist[0] : nullptr);
		flsc->SetPLight(lightdata.plist.size(), lightdata.plist.size() ? &lightdata.plist[0] : nullptr);
		flsc->SetSLight(lightdata.slist.size(), lightdata.slist.size() ? &lightdata.slist[0] : nullptr);

		forwardvaoptr->BindVao();
	}

	for (auto & matvecpair : mesh)//For every material that vertices share
	{//Render non-trans them
		auto & material = matvecpair.first;
		auto & vertvec = matvecpair.second;
		flsc->SetTexMaterial(material);

		forwardvaoptr->SetData(&vertvec[0], vertvec.size() * sizeof(Vertex));

		i.count = vertvec.size();
		i.first = 0;
		i.baseInstance = 0;
		i.instanceCount = 1;

		GLuint indirect;
		glGenBuffers(1, &indirect);
		glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirect);
		glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(i), &i, GL_STREAM_DRAW);
		glMultiDrawArraysIndirectAMD(GL_TRIANGLES, 0, 1, 0);
		//glDrawArraysInstanced(GL_TRIANGLES, 0, i.count, 1);
		//glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, i.count, 1, 0);
		//glDrawArrays(GL_TRIANGLES, 0, vertvec.size());

	}

	//glEnable(GL_BLEND);
	//glm::vec3 eye = context.Eye;
	//auto comparer = [eye](const PositionedArrayModel & lhs, const PositionedArrayModel & rhs) {return glm::length(eye - lhs.Position) > glm::length(eye - rhs.Position); };
	//std::priority_queue<PositionedArrayModel, std::vector<PositionedArrayModel>, std::reference_wrapper<decltype(comparer)>> transqueue(comparer);
	//for (auto & pmodel : trobjlist)
	//{
	//	transqueue.push(pmodel);
	//}
	//while (transqueue.size())
	//{
	//	auto pmodel = transqueue.top();//Transparent object not batched.
	//	transqueue.pop();
	//	for (auto & matvecpair : pmodel.GetMesh())
	//	{
	//		auto & material = matvecpair.first;
	//		auto & vertvec = matvecpair.second;
	//		flsc->SetTexMaterial(material);

	//		forwardvaoptr->SetData(&vertvec[0], vertvec.size() * sizeof(Vertex));
	//		glDrawArrays(GL_TRIANGLES, 0, vertvec.size());
	//		/*unsigned int vertleft = vertvec.size();
	//		unsigned int vertthispass = 0;
	//		while (vertleft)
	//		{
	//			vertthispass = vertleft > SIZE30M ? SIZE30M : vertleft;
	//			forwardvaoptr->SetData(&vertvec[vertvec.size() - vertleft], vertthispass * sizeof(Vertex));

	//			glDrawArrays(GL_TRIANGLES, 0, vertthispass);
	//			vertleft -= vertthispass;
	//		}*/
	//	}
	//}
	//glDisable(GL_BLEND);

	//if (context.Bloom){//Bloom render to brightcolor 
	//	bsc->Use();
	//	bool firstpass = true;
	//	quadvaoptr->BindVao();
	//	for (int i = 0; i != context.Bloom; i++)
	//	{
	//		pingpongfboptr[i % 2].BindFrameBuffer();
	//		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	//		bsc->SetBlurDirection((bool)(i % 2));

	//		if (firstpass)
	//		{
	//			bsc->SetImageTexture(hdrfboptr->ColorBuffer[1]);
	//			firstpass = false;
	//		}
	//		else
	//		{
	//			bsc->SetImageTexture(pingpongfboptr[!(i % 2)].ColorBuffer[0]);
	//		}
	//		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//	}
	//	glBindVertexArray(0);
	//}


	//if (context.Bloom){//Mix brightcolor and color
	//	mixfboptr->BindFrameBuffer();
	//	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//	quadvaoptr->BindVao();
	//	msc->Use();
	//	msc->SetBrightColorTexture(pingpongfboptr[(context.Bloom - 1) % 2].ColorBuffer[0]);
	//	msc->SetColorTexture(hdrfboptr->ColorBuffer[0]);
	//	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	//	glBindVertexArray(0);
	//}

	switch (context.isEyeAdapt)
	{
	case EyeAdaptReadback:
		EyeAdaptMC();
		break;
	case EyeAdaptRenderToTex:

		break;
	}
	screenfbo.BindFrameBuffer();
	{//HDR reprocess
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		hdrsc->Use();
		if (context.Bloom)
		{
			hdrsc->SetScreenTexture(mixfboptr->ColorBuffer[0]);
		}
		else
		{
			//hdrsc->SetScreenTexture(ddepthfboptr->DepthComponent);

			hdrsc->SetScreenTexture(hdrfboptr->ColorBuffer[0]);
		}

		hdrsc->SetIsHDR(context.ToneMapping);
		hdrsc->SetGamma(context.gamma);
		hdrsc->SetExposure(eadata.exposure);
		quadvaoptr->BindVao();
		
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
	}
}

DrawController & DrawController::operator<<(const ArrayModel & rhs)
{
	bool istrans = false;
	for (auto & pair : rhs.GetMesh())
	{
		if (pair.first.transparency < 0.9999f)
		{
			istrans = true;
		}
	}
	if (istrans)
	{
		trobjlist.push_back(rhs);
	}
	else
	{
		(*this).operator+=(rhs);
	}
	return *this;
}

void DrawController::EyeAdaptMC()
{
	if (eadata.pboframecount == 0)
	{
		glBindBufferARB(GL_PIXEL_PACK_BUFFER, eadata.pbo);
		hdrfboptr->BindFrameBuffer();
		glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
		glReadPixels(0, 0, width, height, GL_RGBA, GL_FLOAT, 0);
		glBindBufferARB(GL_PIXEL_PACK_BUFFER, 0);
	}
	else if (eadata.pboframecount == eadata.constant.pbosampleperiod - 1)
	{
		glBindBufferARB(GL_PIXEL_PACK_BUFFER, eadata.pbo);
		eadata.screencolor = (float *)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (eadata.screencolor)
		{
			srand(glfwGetTime() * 100);
			unsigned int groupsize = (width * height) / eadata.constant.MCsamplecount;//warning: MCsamplecount must be smaller than total pixel count!
			float averageilumi = 0;
			unsigned int firstsampleindex = rand() % groupsize;
			for (int j = 0; j != eadata.constant.MCsamplecount; j++)
			{
				unsigned int sampleindex = groupsize * j + firstsampleindex;
				//averageilumi += ((screencolor[i * 4] * 0.2126 + screencolor[i * 4 + 1] * 0.7152 + screencolor[i * 4 + 2] * 0.0722));//Relative luminance;
				float thisilumi = ((eadata.screencolor[sampleindex * 4] * 0.299 + eadata.screencolor[sampleindex * 4 + 1] * 0.587 + eadata.screencolor[sampleindex * 4 + 2] * 0.144));//Perceived luminance;
				averageilumi += thisilumi;
			}
			averageilumi /= eadata.constant.MCsamplecount;
			float curexposure = 0.5 / averageilumi;
			eadata.exposurehistory[eadata.oldesthistory] = curexposure;
			eadata.oldesthistory = (eadata.oldesthistory + 1) % eadata.constant.exposurehistorycount;
			float averageexposure = 0.0f;
			for (int i = 0; i != eadata.constant.exposurehistorycount; i++)
			{
				averageexposure += eadata.exposurehistory[i];
			}
			averageexposure /= eadata.constant.exposurehistorycount;
			if (abs(eadata.exposure - averageexposure) > eadata.constant.exposurechangethreshold)
			{
				eadata.exposure = eadata.constant.exposurememory * eadata.exposure + (1 - eadata.constant.exposurememory) * averageexposure;
			}
			eadata.exposure = glm::clamp(eadata.exposure, eadata.constant.exposuremin, eadata.constant.exposuremax);
			std::cout << "exposure: " << eadata.exposure << std::endl;
		}
		glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
		glBindBufferARB(GL_PIXEL_PACK_BUFFER, 0);
	}
	eadata.pboframecount = (eadata.pboframecount + 1) % eadata.constant.pbosampleperiod;

}

void DrawController::PrepareQuad()
{
	GLfloat quadVertices[] = {
		// Positions      // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	std::vector<VaoSetting> quadvaosetting = { { 3, GL_FLOAT, sizeof(float) }, { 2, GL_FLOAT, sizeof(float) } };
	quadvaoptr = new VaoStruct(quadvaosetting, GL_STATIC_DRAW);
	quadvaoptr->SetData(quadVertices, sizeof(quadVertices));
}

void DrawController::RenderDepthSingleFace(FboStruct * target, mat4 lightwvp, size_t vertsize)
{
	depthvaoptr->BindVao();
	target->BindFrameBufferForDepth();
	glClear(GL_DEPTH_BUFFER_BIT);
	dsc->Use();
	dsc->SetLightWVP(lightwvp);
	glDrawArrays(GL_TRIANGLES, 0, vertsize);
}

void DrawController::RenderDepthCubeFace(FboStruct * target, const mat4 lightview[6], const vec3& lightpos, size_t vertsize, float farplane)
{
	depthvaoptr->BindVao();
	dsc->Use();
	for (int i = 0; i != 6; i++)
	{
		target->BindFrameBufferForDepth(i);
		glClear(GL_DEPTH_BUFFER_BIT);
		dsc->SetLightWVP(lightview[i]);
		dsc->SetLinearDepth(lightpos, farplane);
		glDrawArrays(GL_TRIANGLES, 0, vertsize);
	}
	dsc->SetSafeState();
}


ForwardLightData DrawController::RenderShadow(bool ishadow, unsigned int w, unsigned int h, vec3 eye, size_t vertsize)
{
	ForwardLightData res;
	glViewport(0, 0, w, h);//GLcontext set

	try 
	{
		//Choose a DirectionalLight
		DirectionalLight depthdl = dlist.at(0);
		res.dlshadow = true;
		res.dlist.push_back(depthdl);
		//Calculate WVP
		mat4 othoproj = ortho(-80.0f, 80.0f, -80.0f, 80.0f, 0.1f, 128.0f);
		vec3 lightpos = eye - depthdl.direction * 64.0f;
		vec3 up(0.0, 1.0, 0.0);
		if ((up.x / depthdl.direction.x) - (up.y / depthdl.direction.y) < 0.0001 || (up.z / depthdl.direction.z) - (up.y / depthdl.direction.y))
			up = vec3(1.0, 0.0, 0.0);
		mat4 lightview = lookAt(lightpos, lightpos + depthdl.direction, up);
		res.dwvp = othoproj * lightview;
		//Render depth
		RenderDepthSingleFace(ddepthfboptr, res.dwvp, vertsize);
	}
	catch (std::out_of_range)
	{}
	for (auto & l : dlist)
	{
		if (l.first && res.dlist.size() <= MAXDIRECTIONALLIGHT)
		{
			res.dlist.push_back(l.second);
		}
	}

	{
		//Choose a SpotLight
		auto scomparator = [eye](const SpotLight & lhs, const SpotLight & rhs) ->bool { return lhs.IntenAt(eye) < rhs.IntenAt(eye); };
		std::priority_queue<SpotLight, std::vector<SpotLight>, decltype(scomparator)> squeue(scomparator);
		for (auto & l : slist)
		{
			squeue.push(l.second);
		}
		res.slist.push_back(SpotLight());
		while (squeue.size() && res.slist.size() <= MAXSPOTLIGHT && !res.slshadow)
		{
			const SpotLight & thisspotlight = squeue.top();
			if (thisspotlight.atten.hasshadow)
			{
				res.slist[0] = thisspotlight;
				res.slshadow = true;
				squeue.pop();
				break;
			}
			else
			{
				res.slist.push_back(thisspotlight);
			}
		}
		while (squeue.size() && res.slist.size() <= MAXSPOTLIGHT)
		{
			const SpotLight & thisspotlight = squeue.top();
			res.slist.push_back(thisspotlight);
		}
		if (res.slshadow && ishadow)
		{//Calculate WVP
			float fov = 2 * degrees(acos(res.slist[0].zerocos));
			float range = min(64.0f, CalculateLightRange(res.slist[0].bl.intensity, res.slist[0].atten, 0.02));
			float nearplane = clamp((range - 16.0) / 32.0 * 0.4 + 0.1, 0.1, 0.5);
			mat4 persproj = perspective(fov, w / (float)h, nearplane, range);
			vec3 up(0.0, 1.0, 0.0);
			if ((up.x * res.slist[0].direction.y) - (up.y * res.slist[0].direction.x) < 0.0001 && (up.z * res.slist[0].direction.y) - (up.y * res.slist[0].direction.z) < 0.0001)
				up = vec3(1.0, 0.0, 0.0);
			mat4 lightview = lookAt(res.slist[0].position, res.slist[0].position + res.slist[0].direction, up);
			res.swvp = persproj * lightview;
			
			//Render depth
			RenderDepthSingleFace(sdepthfboptr, res.swvp, vertsize);
		}
		else
		{
			res.slshadow = false;
		}
	}

	{
		//Choose a PointLight
		auto pcomparator = [eye](const PointLight & lhs, const PointLight & rhs) ->bool { return lhs.IntenAt(eye) < rhs.IntenAt(eye); };
		std::priority_queue<PointLight, std::vector<PointLight>, decltype(pcomparator)> pqueue(pcomparator);
		for (auto & l : plist)
		{
			pqueue.push(l.second);
		}
		res.plist.push_back(PointLight());
		while (pqueue.size() && res.plist.size() <= MAXPOINTLIGHT && !res.plshadow)
		{
			const PointLight & thispointlight = pqueue.top();
			if (thispointlight.atten.hasshadow)
			{
				res.plist[0] = thispointlight;
				res.plshadow = true;
				pqueue.pop();
				break;
			}
			else
			{
				res.plist.push_back(thispointlight);
			}
		}
		while (pqueue.size() && res.plist.size() <= MAXPOINTLIGHT)
		{
			const PointLight & thispointlight = pqueue.top();
			res.plist.push_back(thispointlight);
		}
		if (res.plshadow && ishadow)
		{
			//Calculate WVP
			float fov = 90.0f;
			float range = min(48.0f, CalculateLightRange(res.plist[0].bl.intensity, res.plist[0].atten, 0.05));
			float nearplane = clamp((range - 16.0) / 32.0 * 0.4 + 0.1, 0.1, 0.5);
			mat4 persproj = perspective(fov, 1.0f, nearplane, range);
			res.plfarplane = range;
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
			mat4 lightviewarray[6];
			for (int i = 0; i != 6; i++)
			{
				mat4 lightview = lookAt(res.plist[0].position, res.plist[0].position + facedir[i], faceupvec[i]);
				lightviewarray[i] = persproj * lightview;
			}
			RenderDepthCubeFace(pdepthfboptr, lightviewarray, res.plist[0].position, vertsize, res.plfarplane);
		}
		else
		{
			res.plshadow = false;
		}
	}

	glViewport(0, 0, width, height);//GLcontext resume

	return res;
}

float DrawController::CalculateLightRange(float intensity, Attenuation atten, float threshold)
{
	float range;
	if (atten.exp > 0.0001)
	{
		float lambda = atten.linear * atten.linear - 4 * atten.exp * (atten.constant - 1 / threshold * intensity);
		range = (pow(lambda, 0.5f) - atten.linear) / (2 * atten.exp);
	}
	else
	{
		range = (1 / threshold * intensity - atten.constant) / atten.linear;
	}
	return range;
}

std::vector<vec3> DrawController::GetNonTransObjList() const
{
	std::vector<vec3> vertices;//set vertices data
	for (auto & matvecpair : mesh)
	{
		for (auto & vert : matvecpair.second)
		{
			vertices.push_back(vert.position);

		}
	}
	for (auto & trmodel : trobjlist)
	{
		for (auto & matvecpair : trmodel.GetMesh())
		{
			if (matvecpair.first.transparency > 0.9999)//if trtex is set, tr value is for set whether this object cause shadow
			{
				for (auto & vert : matvecpair.second)
				{
					vertices.push_back(vert.position);
				}
			}
		}
	}
	return vertices;
}

FboStruct::FboStruct(FboSetting setting) : Setting(setting)
{
	glGenFramebuffers(1, &FrameBufferObject);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferObject);

	for (unsigned int i = 0; i != setting.ColorBufferCount; i++)
	{
		ColorBuffer.push_back(0);
	}
	for (unsigned int i = 0; i != setting.ColorBufferCount; i++)
	{
		glGenTextures(1, &ColorBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, ColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, setting.ColorFormat, setting.Width, setting.Height, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setting.ColorClamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setting.ColorClamp);
		if (setting.ColorClamp == GL_CLAMP_TO_BORDER)
		{
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &setting.BorderColor[0]);
		}
		glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT + i, ColorBuffer[i], 0);
	}
	GLenum * DrawBuffersPtr = new GLenum[setting.ColorBufferCount];
	for (unsigned int i = 0; i != setting.ColorBufferCount; i++)
	{
		DrawBuffersPtr[i] = GL_COLOR_ATTACHMENT0_EXT + i;
	}
	glDrawBuffers(setting.ColorBufferCount, DrawBuffersPtr);

	switch (setting.DepthType)
	{
	case DepthBufferT:
		glGenRenderbuffersEXT(1, &DepthComponent);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, DepthComponent);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, setting.DepthFormat, setting.Width, setting.Height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, DepthComponent);
		break;
	case DepthTextureT:
		glGenTextures(1, &DepthComponent);
		glBindTexture(GL_TEXTURE_2D, DepthComponent);

		glTexImage2D(GL_TEXTURE_2D, 0, setting.DepthFormat, setting.Width, setting.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, setting.ColorClamp);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, setting.ColorClamp);
		if (setting.ColorClamp == GL_CLAMP_TO_BORDER)
		{
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, &setting.BorderColor[0]);
		}
		glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, DepthComponent, 0);
		break;
	case DepthCubeMapT:
		glGenTextures(1, &DepthComponent);
		glBindTexture(GL_TEXTURE_CUBE_MAP, DepthComponent);
		for (GLuint i = 0; i < 6; i++)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, setting.DepthFormat, setting.Width, setting.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, setting.ColorTexFilter);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, setting.ColorClamp);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, setting.ColorClamp);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, setting.ColorClamp);

		if (setting.ColorClamp == GL_CLAMP_TO_BORDER)
		{
			glTexParameterfv(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BORDER_COLOR, &setting.BorderColor[0]);
		}
		glFramebufferTextureEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, DepthComponent, 0);//Attentions
		break;
	default:
		break;
	}

	if (glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT) != GL_FRAMEBUFFER_COMPLETE_EXT)
		throw "Create Framebuffer failed";
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void FboStruct::BindFrameBuffer() const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferObject);

	GLenum * DrawBuffersPtr = new GLenum[Setting.ColorBufferCount];
	for (unsigned int i = 0; i != Setting.ColorBufferCount; i++)
	{
		DrawBuffersPtr[i] = GL_COLOR_ATTACHMENT0_EXT + i;
	}
	glDrawBuffers(Setting.ColorBufferCount, DrawBuffersPtr);
}

void FboStruct::BindFrameBufferForDepth() const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferObject);
	glDrawBuffer(GL_NONE);
}

void FboStruct::BindFrameBufferForDepth(int face) const
{
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, FrameBufferObject);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + face, DepthComponent, 0);
	glDrawBuffer(GL_NONE);
}

VaoStruct::VaoStruct(const std::vector<VaoSetting>& setting, GLenum changehint) : StorageHint(changehint)
{
	glGenVertexArrays(1, &Vao);
	glGenBuffers(1, &Vbo);
	size_t totalsize = 0;
	for (auto c : setting)
	{
		totalsize += c.NumofValues * c.ValueSize;
	}
	size_t stride = 0;
	for (unsigned int i = 0; i != setting.size(); i++)
	{
		glVertexArrayVertexAttribOffsetEXT(Vao, Vbo, i, setting[i].NumofValues, setting[i].ValueType, GL_FALSE, totalsize, stride);
		glEnableVertexArrayAttribEXT(Vao, i);
		stride += setting[i].NumofValues * setting[i].ValueSize;
	}
}


void VaoStruct::BindVao() const
{
	glBindVertexArray(Vao);
}

void VaoStruct::SetData(const void * dataptr, size_t datasize)
{
	/*if (datasize > VboSize)
	{
		VboSize = datasize * 1.2;
		MallocVbo(VboSize);
	}
	SetSubData(dataptr, datasize);*/
	glNamedBufferDataEXT(Vbo, (GLsizeiptr)datasize, dataptr, StorageHint);
}

void VaoStruct::MallocVbo(size_t vbosize) const
{
	glNamedBufferDataEXT(Vbo, (GLsizeiptr)vbosize, nullptr, StorageHint);
}

void VaoStruct::SetSubData(const void * dataptr, size_t datasize) const
{

	//glBindBuffer(GL_ARRAY_BUFFER, Vbo);
	//glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)0, (GLsizeiptr)datasize, dataptr);

	glNamedBufferSubDataEXT(Vbo, (GLintptr)0, (GLsizeiptr)datasize, dataptr);
}