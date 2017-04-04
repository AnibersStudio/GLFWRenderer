#pragma once

#include <vector>

#include "GLCommon.h"
#include "MeshManager.h"
#include "RenderContext.h"
#include "RenderStageImp.h"

struct PerFrameData
{
	void Clear() { Material.clear(); MaterialIndex.clear(); Vertex.clear(); 
	OpaceCount.clear(); FullTransCount.clear(); SemiTransCount.clear(); }

	std::vector<TexturedMaterial> Material;
	std::vector<unsigned int> MaterialIndex;
	std::vector<Vertex> Vertex;
	std::vector<unsigned int> OpaceCount;
	std::vector<unsigned int> FullTransCount;
	std::vector<unsigned int> SemiTransCount;
};


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
};