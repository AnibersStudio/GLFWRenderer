#include "RenderController.h"
#include "GLConstManager.h"
#include "RenderStageImp.h"
#include "TextureLoader.h"
#include <iostream>
#include <iomanip>
RenderController::RenderController(MeshManager & mm, LightManager & lm, unsigned int w, unsigned int h) : meshmanager(mm), lightmanager(lm), width(w), height(h),
depthstage(PreDepthStage{ w, h }), forwardstage(ForwardStage{ w, h }), lightcullingstage(LightCullingStage{ w, h }),
bloomer(Bloomer(w, h)), eyeadapter(EyeAdapter(w, h)), tonemapper(ToneMapper(w, h))
{
	depthstage.Init();
	lightcullingstage.Init();
	shadowstage.Init();
	forwardstage.Init(lightcullingstage.GetTileCount());
	bloomer.Init(forwardstage.GetFbo());
	eyeadapter.Init(forwardstage.GetFbo());
	tonemapper.Init(forwardstage.GetFbo(), bloomer.Result(), eyeadapter.Result());
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
	unsigned int OpaceVerticesCount = framedata.Opaquelist->size();
	unsigned int FulltransVerticesCount = framedata.Fulltranslist->size();
	unsigned int TransVerticesCount = framedata.Translist->size();

	depthstage.Prepare(WVP);
	lightcullingstage.Prepare(WVP, framedata);
	shadowstage.Prepare(context.ShadowPoint, context.ShadowSpot, framedata, context.eye);
	forwardstage.Prepare(framedata, WVP, shadowstage.GetLightTransformList(), context.eye);
	bloomer.Prepare();
	eyeadapter.Prepare();
	tonemapper.Prepare(context.ToneMapping, context.isBloom, context.EyeAdapt, context.BloomRatio, context.BloomThreshold, context.EyeAdaptFreshRate, context.gamma);

	depthstage.Draw(glstate, forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount);
	lightcullingstage.Draw(glstate, forwardstage.GetVao(), forwardstage.GetFbo(), OpaceVerticesCount, TransVerticesCount);
	shadowstage.Draw(glstate, forwardstage.GetVao(), OpaceVerticesCount);
	forwardstage.Draw(glstate, lightcullingstage.GetLightIndexAndLinked());
	bloomer.Draw(glstate);
	eyeadapter.Draw(glstate);
	tonemapper.Draw(glstate);

	static DebugOutput screendrawer{width, height};

	//screendrawer.Draw(lightcullingstage.proxyvao, WVP, 60u, 18u, 0u, 1u, glstate, false, lightcullingstage.inerspotlightcount);
	//screendrawer.Draw(WVP, proxy.GetVertices(), framedata.lightinstancemat, glstate);
	if (context.isdebug)
	{
		//screendrawer.Draw(lightcullingstage.GetTileCount(), std::get<0>(lightcullingstage.GetLightIndexAndLinked()), glstate);
		//screendrawer.Draw(shadowstage.spotfbo[0].GetDepthID(), glstate);
	}
	//screendrawer.Draw(forwardstage.GetFbo().GetColorID()[0], glstate);
	//screendrawer.Draw(bloomer.result.GetColorID()[0], glstate);
	//screendrawer.Draw(eyeadapter.middlelist[10].GetColorID()[0], glstate);

	oldcontext = context;
}

void RenderController::RenderPrepare(RenderContext context, float pixelsize)
{
	framedata.Clear();

	if (context.anisotropic)
	{
		TextureLoader::GetInstance().SetAF(true);
	}
	else
	{
		TextureLoader::GetInstance().SetAF(false);
	}
	
	framedata.Opaquelist = &meshmanager.GetOpaqueVertex();
	framedata.Fulltranslist = &meshmanager.GetFulltransVertex();
	framedata.Transtasklist = &meshmanager.GenerateTransTask(context.eye);
	framedata.Translist = &meshmanager.GetTransVertex();
	framedata.MaterialList = &meshmanager.GetShaderMatList();

	framedata.dlist = lightmanager.GetDirectionalLight();
	framedata.pinercount = lightmanager.AppendPointLight(context.eye, framedata.plist, framedata.lightinstancemat, pixelsize);
	framedata.sinercount = lightmanager.AppendSpotLight(context.eye, framedata.slist, framedata.lightinstancemat, pixelsize);
}
