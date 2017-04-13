#pragma once

#include <vector>

#include "GLCommon.h"
#include "MeshManager.h"
#include "LightManager.h"
#include "RenderContext.h"
#include "RenderStageImp.h"
#include "PerFrameData.h"

class RenderController
{
public:
	RenderController(MeshManager & mm, LightManager & lm, unsigned int w, unsigned int h);
	void Draw(RenderContext context);
private:
	void RenderPrepare(RenderContext context);
	MeshManager & meshmanager;
	LightManager & lightmanager;
	RenderContext oldcontext;
	PerFrameData framedata;

	unsigned int width, height;

	PreDepthStage depthstage;
	ForwardStage forwardstage;
	LightCullingStage lightcullingstage;

	GLState glstate;
};