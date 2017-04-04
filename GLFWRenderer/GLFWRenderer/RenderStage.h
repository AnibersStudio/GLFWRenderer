#pragma once
#include "GLCommon.h"
#include "RenderContext.h"

class PostProcessRenderStage
{
public:
	virtual void Prepare(RenderContext context, GLuint source) = 0;
	virtual void Draw() = 0;
	virtual GLuint Result() = 0;
};