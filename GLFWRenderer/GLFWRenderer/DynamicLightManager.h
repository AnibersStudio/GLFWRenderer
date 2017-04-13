#pragma once
#include <list>
#include <vector>
#include <tuple>

#include "LightProxyMesh.h"
#include "Light.h"
#include "LightManager.h"

class DynamicLightManager : public LightManager
{
public:
	typedef std::tuple<unsigned int &, std::list<PointLight>::iterator, std::list<SpotLight>::iterator> stub;
	virtual const std::vector<std::vector<float>> & GetProxyMesh() override;
	virtual unsigned int AppendPointLight(glm::vec3 eye, std::vector<PointLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize) override;
	virtual unsigned int AppendSpotLight(glm::vec3 eye, std::vector<SpotLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize) override;
	virtual const std::vector<DirectionalLight>& GetDirectionalLight() override { return dl; }

	stub Add(DirectionalLight light);
	stub Add(PointLight light);
	stub Add(SpotLight light);
	void Delete(stub light);
private:
	std::vector<DirectionalLight> dl;
	std::list<PointLight> plist;
	std::list<SpotLight> slist;
	std::vector<unsigned int> dindex;
};