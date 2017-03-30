#pragma once

#include <vector>

#include "GLCommon.h"
#include "MeshManager.h"
struct RenderContext
{
	/// <summary> Camera position </summary>
	glm::vec3 eye = glm::vec3(0.0, 0.0, 0.0);
	/// <summary> Camera orientation </summary>
	glm::vec3 target = glm::vec3(0.0, 0.0, -1.0);
	/// <summary> Camera rotation along axes </summary>
	glm::vec3 up = glm::vec3(0.0, 1.0, 0.0);

	/// <summary> Objects farther than this will not be drawn </summary>
	float ViewDistance = 192;
	/// <summary> The Horizonal field of view value in degrees </summary>
	float FieldOfView = 45;
	/// <summary> The value of monitor gamma </summary>
	float gamma = 2.2;

	/// <summary> How many light can have shadow </summary>
	unsigned int ShadowLight = 1;

	/// <summary> Is anisotropic filter on or not </summary>
	bool anisotropic = false;
	/// <summary> Adapt the illumination dynamiclly or not. Fair cost </summary>
	bool EyeAdapt = true;
	/// <summary> Let the light source bloom or not. Costly </summary>
	bool isBloom = false;
	/// <summary> Apply tone mapping or not </summary>
	bool ToneMapping = true;
	/// <summary> Apply Global Illumination. Costly </summary>
	bool isGI = true;
};

struct PerFrameData
{
	void Clear() { OpacePosition.clear(); TransPosition.clear(); }
	std::vector<glm::vec3> OpacePosition;
	std::vector<glm::vec3> TransPosition;
};


class RenderController
{
public:
	RenderController(MeshManager & mm);
	void Draw(RenderContext context);
private:
	void RenderPrepare();
	MeshManager & meshmanager;
	RenderContext oldcontext;
	PerFrameData framedata;
};