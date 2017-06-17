#pragma once
#include "GLCommon.h"
#include "RenderContext.h"

class PostProcessRenderStage
{
public:
	virtual void Init(unsigned int width, unsigned int height) = 0;
	virtual void Prepare(RenderContext context, GLuint source) = 0;
	virtual void Draw() = 0;
};