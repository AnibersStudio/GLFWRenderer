#pragma once

#include <vector>
#include "GLCommon.h"
#include "Mesh.h"
#include "Light.h"
#include <memory>
#include "ShaderMaterial.h"

struct PerFrameData
{
	void Clear() {
		dlist.clear(); plist.clear(); slist.clear(); lightinstancemat.clear();
		pinercount = 0;sinercount = 0;
	}

	const std::vector<ShaderMaterial>* MaterialList;
	const std::vector<MaterialedVertex>* Opaquelist;
	const std::vector<MaterialedVertex>* Fulltranslist;
	const std::vector<MaterialedVertex>* Translist;
	const std::vector<unsigned int>* Transtasklist;

	std::vector<DirectionalLight> dlist;
	std::vector<PointLight> plist;
	std::vector<SpotLight> slist;
	std::vector<mat4> lightinstancemat;
	unsigned int pinercount;
	unsigned int sinercount;
};

