#pragma once

#include <vector>

#include "GLCommon.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "RenderContext.h"
#include "RenderStageImp.h"
#include "PerFrameData.h"
#include "PostProcess.h"

class RenderController
{
public:
	RenderController(MeshManager & mm, LightManager & lm, unsigned int w, unsigned int h);
	void Draw(RenderContext context);
private:
	void RenderPrepare(RenderContext context, float pixelsize);
	MeshManager & meshmanager;
	LightManager & lightmanager;
	RenderContext oldcontext;
	PerFrameData framedata;
	
	const float  nearplane= 0.2f;
	unsigned int width, height;

	PreDepthStage depthstage;
	ForwardStage forwardstage;
	LightCullingStage lightcullingstage;
	ShadowStage shadowstage;

	Bloomer bloomer;
	EyeAdapter eyeadapter;
	ToneMapper tonemapper;

	GLState glstate;
};