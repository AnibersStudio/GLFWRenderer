#pragma once

#include <vector>
#include "GLCommon.h"
#include "Mesh.h"
#include "Light.h"
struct PerFrameData
{
	void Clear() {
		Material.clear(); MaterialIndex.clear(); Vertex.clear();
		OpaceCount.clear(); FullTransCount.clear(); SemiTransCount.clear();

		dlist.clear(); plist.clear(); slist.clear(); lightinstancemat.clear();
		pinercount = 0;sinercount = 0;
	}

	std::vector<TexturedMaterial> Material;
	std::vector<unsigned int> MaterialIndex;
	std::vector<Vertex> Vertex;
	std::vector<unsigned int> OpaceCount;
	std::vector<unsigned int> FullTransCount;
	std::vector<unsigned int> SemiTransCount;

	std::vector<DirectionalLight> dlist;
	std::vector<PointLight> plist;
	std::vector<SpotLight> slist;
	std::vector<mat4> lightinstancemat;
	unsigned int pinercount;
	unsigned int sinercount;
};

