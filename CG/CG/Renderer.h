///Name: Renderer
///Description: The renderer class draws something in the window, who is responsible for rasterization(for this CG homework only, otherwise by hardware) and prepare/change gl pipeline and make gl drawcalls
///Usage: Use Draw() to draw a specific RenderObject
///Coder: Hao Pang
///Date: 2017.4.26
#pragma once
#include <vector>
#include "ShaderController.h"
#include "Renderer.h"
#include "GLCommon.h"
#include "RenderVao.h"
#include "Rasterizer.h"

class Renderer
{
public:
	Renderer(unsigned int w, unsigned int h);
	void Draw(RenderObject& object);

private:
	unsigned int width, height;
	ShaderController pointcon;
	Vao vao;
};