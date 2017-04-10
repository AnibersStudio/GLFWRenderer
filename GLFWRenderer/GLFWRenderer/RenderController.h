#pragma once

#include <vector>

#include "GLCommon.h"
#include "MeshManager.h"
#include "RenderContext.h"
#include "RenderStageImp.h"
#include "PerFrameData.h"
class RenderController
{
public:
	RenderController(MeshManager & mm, unsigned int w, unsigned int h);
	void Draw(RenderContext context);
private:
	void RenderPrepare(RenderContext context);
	MeshManager & meshmanager;
	RenderContext oldcontext;
	PerFrameData framedata;

	unsigned int width, height;

	PreDepthStage depthstage;
	ForwardStage forwardstage;

	GLState glstate;
};