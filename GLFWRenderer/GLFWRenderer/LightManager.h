#pragma once

#include "Light.h"
#include "LightProxyMesh.h"

class LightManager
{
public:
	virtual const std::vector<std::vector<float>> & GetProxyMesh() = 0;
	/// <summary> Append point lights to existing light list. Iner lights ordered by Intensity at eye, outer lights ordered by disance from eye.
	/// <para> Return the count also the bound index of iner lights.</para>
	/// </summary>
	virtual unsigned int AppendPointLight(glm::vec3 eye, std::vector<PointLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize) = 0;
	/// <summary> Append point lights to existing light list. Iner lights ordered by Intensity at eye, outer lights ordered by disance from eye.
	/// <para> Return the count also the bound index of iner lights.</para>
	/// </summary>
	virtual unsigned int AppendSpotLight(glm::vec3 eye, std::vector<SpotLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize) = 0;
	/// <summary> Get the directional lights light </summary>
	virtual const std::vector<DirectionalLight>& GetDirectionalLight() = 0;
};