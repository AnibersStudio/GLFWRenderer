#pragma once

#include <vector>
#include "GLCommon.h"
#include "Mesh.h"

struct PerFrameData
{
	void Clear() {
		Material.clear(); MaterialIndex.clear(); Vertex.clear();
		OpaceCount.clear(); FullTransCount.clear(); SemiTransCount.clear();
	}

	std::vector<TexturedMaterial> Material;
	std::vector<unsigned int> MaterialIndex;
	std::vector<Vertex> Vertex;
	std::vector<unsigned int> OpaceCount;
	std::vector<unsigned int> FullTransCount;
	std::vector<unsigned int> SemiTransCount;
};

