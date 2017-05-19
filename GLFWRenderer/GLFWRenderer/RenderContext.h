#pragma once
#include "GLCommon.h"
#include "Camera.h"
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
	float FieldOfView = 60;
	/// <summary> The value of monitor gamma </summary>
	float gamma = 2.2;

	/// <summary> How many point light can have shadow </summary>
	unsigned int ShadowPoint = 4;
	/// <summary> How many spot light can have shadow </summary>
	unsigned int ShadowSpot = 8;

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

	bool isdebug = false;
};