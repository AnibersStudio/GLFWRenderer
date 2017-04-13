#include "DynamicLightManager.h"

const std::vector<std::vector<float>>& DynamicLightManager::GetProxyMesh()
{
	static std::vector<std::vector<float>> proxymeshes{ ProxyPyramid::GetVertices(), ProxyIcosahedron::GetVertices() };
	return proxymeshes;
}

unsigned int DynamicLightManager::AppendPointLight(glm::vec3 eye, std::vector<PointLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize)
{
	ProxyIcosahedron proxy;
	lightlist.reserve(lightlist.size() + plist.size());
	lightproxytransform.reserve(lightproxytransform.size() + plist.size());

	std::list<PointLight> inerlist;
	std::list<PointLight> outerlist;
	for (auto & l : plist)
	{
		if (proxy.InCircumscribeLight(eye, l.position, l.GetRange(0.005), pixelsize))
		{
			inerlist.push_back(l);
		}
		else
		{
			outerlist.push_back(l);
		}
	}
	
	static auto intenatcompare = [eye](const PointLight & lhs, const PointLight & rhs) { return lhs.IntenAt(eye) > rhs.IntenAt(eye); };
	static auto distancecompare = [eye](const PointLight & lhs, const PointLight & rhs) { return glm::length(eye - lhs.position) < glm::length(eye - rhs.position); };
	inerlist.sort(intenatcompare);
	outerlist.sort(distancecompare);

	for (auto & l : inerlist)
	{
		lightlist.push_back(l);
		lightproxytransform.push_back(proxy.GetMatrix(eye, l.position, l.GetRange(0.005), pixelsize));
	}
	for (auto & l : outerlist)
	{
		lightlist.push_back(l);
		lightproxytransform.push_back(proxy.GetMatrix(eye, l.position, l.GetRange(0.005), pixelsize));
	}
	return inerlist.size();
}

unsigned int DynamicLightManager::AppendSpotLight(glm::vec3 eye, std::vector<SpotLight>& lightlist, std::vector<glm::mat4>& lightproxytransform, float pixelsize)
{
	ProxyPyramid proxy;
	lightlist.reserve(lightlist.size() + slist.size());
	lightproxytransform.reserve(lightproxytransform.size() + slist.size());

	std::list<SpotLight> inerlist;
	std::list<SpotLight> outerlist;
	for (auto & l : slist)
	{
		if (proxy.InCircumscribeLight(eye, l.position, l.GetRange(0.005),l.direction, l.zerocos, pixelsize))
		{
			inerlist.push_back(l);
		}
		else
		{
			outerlist.push_back(l);
		}
	}

	static auto intenatcompare = [eye](const SpotLight & lhs, const SpotLight & rhs) { return lhs.IntenAt(eye) > rhs.IntenAt(eye); };
	static auto distancecompare = [eye](const SpotLight & lhs, const SpotLight & rhs) { return glm::length(eye - lhs.position) < glm::length(eye - rhs.position); };
	inerlist.sort(intenatcompare);
	outerlist.sort(distancecompare);

	for (auto & l : inerlist)
	{
		lightlist.push_back(l);
		lightproxytransform.push_back(proxy.GetMatrix(eye, l.position, l.GetRange(0.005), l.direction, l.zerocos, pixelsize));
	}
	for (auto & l : outerlist)
	{
		lightlist.push_back(l);
		lightproxytransform.push_back(proxy.GetMatrix(eye, l.position, l.GetRange(0.005), l.direction, l.zerocos, pixelsize));
	}

	return inerlist.size();
}

DynamicLightManager::stub DynamicLightManager::Add(DirectionalLight light)
{
	dl.push_back(light);
	dindex.push_back(dl.size() - 1);
	return stub(*(dindex.end() - 1), plist.end(), slist.end());
}

DynamicLightManager::stub DynamicLightManager::Add(PointLight light)
{
	static unsigned int invaliddindex = 0xFFFFFFFF;
	plist.push_front(light);
	return stub(invaliddindex, plist.begin(), slist.end());
}

DynamicLightManager::stub DynamicLightManager::Add(SpotLight light)
{
	static unsigned int invaliddindex = 0xFFFFFFFF;
	slist.push_front(light);
	return stub(invaliddindex, plist.end(), slist.begin());
}

void DynamicLightManager::Delete(stub light)
{
	auto itd = std::get<0>(light);
	auto itp = std::get<1>(light);
	auto its = std::get<2>(light);
	if (itd != 0xFFFFFFFF)
	{
		dl.erase(dl.begin() + itd);
		dindex.erase(dindex.begin() + itd);
		for (unsigned int i = itd; i != dindex.size(); i++)
		{
			dindex[i]--;
		}
	}
	if (itp != plist.end())
	{
		plist.erase(itp);
	}
	if (its != slist.end())
	{
		slist.erase(its);
	}
}

