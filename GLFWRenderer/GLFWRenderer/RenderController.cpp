#include "RenderController.h"
#include "GLConstManager.h"
#include "RenderStageImp.h"
#include <iostream>
RenderController::RenderController(MeshManager & mm, LightManager & lm, unsigned int w, unsigned int h) : meshmanager(mm), lightmanager(lm), width(w), height(h), depthstage(PreDepthStage{w, h}), forwardstage(ForwardStage{ w, h }), lightcullingstage(LightCullingStage{ w, h })
{
	depthstage.Init();
	lightcullingstage.Init();
	shadowstage.Init();
	forwardstage.Init(lightcullingstage.GetTileCount());
}

void RenderController::Draw(RenderContext context)
{
	///Calculate Pixel world coord size
	float tileworldsize;
	{
		double pixelworldsize = 2 * glm::tan(glm::radians(context.FieldOfView / 2)) * nearplane / lightcullingstage.GetTileCount().y;
		tileworldsize = glm::max(pixelworldsize / lightcullingstage.GetTileDismatchScale().x, pixelworldsize / lightcullingstage.GetTileDismatchScale().y);
	}
	RenderPrepare(context, tileworldsize);
	///Calculate WVPs
	glm::mat4 V = glm::lookAt(context.eye, context.target, context.up);
	glm::mat4 P = glm::perspective(context.FieldOfView / width * height, (float)width / (float)height, nearplane, context.ViewDistance);
	glm::mat4 WVP = P * V;
	///Calculate Opace Vertices Count
	unsigned int OpaceVerticesCount = 0;
	for (auto & v : framedata.OpaceCount)
	{
		OpaceVerticesCount += v;
	}
	///Calculate Trans Vertices Count
	unsigned int TransVerticesCount = 0;
	for (auto & v : framedata.FullTransCount)
	{
		TransVerticesCount += v;
	}
	for (auto & v : framedata.SemiTransCount)
	{
		TransVerticesCount += v;
	}

	depthstage.Prepare(WVP);
	lightcullingstage.Prepare(WVP, framedata);
	shadowstage.Prepare(context.ShadowPoint, context.ShadowSpot, framedata, context.eye);
	forwardstage.Prepare(framedata, WVP, shadowstage.GetLightTransformList(), shadowstage.GetShadowCount());

	depthstage.Draw(glstate, forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount);
	lightcullingstage.Draw(glstate, forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount, TransVerticesCount);
	shadowstage.Draw(glstate, forwardstage.GetVao(), OpaceVerticesCount);
	forwardstage.Draw(glstate, OpaceVerticesCount + TransVerticesCount, lightcullingstage.GetLightIndexAndLinked());

	ProxyPyramid proxy;

	static DebugOutput screendrawer{width, height};

	//screendrawer.Draw(lightcullingstage.proxyvao, WVP, 60u, 18u, 0u, 1u, glstate);
	if (context.isdebug)
	{
		screendrawer.Draw(lightcullingstage.GetTileCount(), std::get<1>(lightcullingstage.GetLightIndexAndLinked()), glstate);
	}
	//screendrawer.Draw(WVP, proxy.GetVertices(), framedata.lightinstancemat, glstate);
	//screendrawer.Draw(forwardstage.GetFbo().GetDepthID());
	//screendrawer.Draw(shadowstage.singlebluredfbo[0].GetDepthID(), glstate);

	oldcontext = context;
}

void RenderController::RenderPrepare(RenderContext context, float pixelsize)
{
	framedata.Clear();
	meshmanager.AppendMesh(MeshManager::Opace, framedata.Material, framedata.OpaceCount, framedata.Vertex, framedata.MaterialIndex);
	meshmanager.AppendOrderedMesh(MeshManager::FullTrans, context.eye, framedata.Material, framedata.FullTransCount, framedata.Vertex, framedata.MaterialIndex);
	meshmanager.AppendOrderedMesh(MeshManager::SemiTrans, context.eye, framedata.Material, framedata.SemiTransCount, framedata.Vertex, framedata.MaterialIndex);

	for (auto & m : framedata.Material)
	{
		m.diffusetex->SetAF(GLConstManager::GetInstance().GetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT)[0]);
	}
	
	framedata.dlist = lightmanager.GetDirectionalLight();
	framedata.pinercount = lightmanager.AppendPointLight(context.eye, framedata.plist, framedata.lightinstancemat, pixelsize);
	framedata.sinercount = lightmanager.AppendSpotLight(context.eye, framedata.slist, framedata.lightinstancemat, pixelsize);
}
