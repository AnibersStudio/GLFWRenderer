#pragma once
#include <vector>

#include "GLCommon.h"

struct ProxyPyramid
{
	
	const std::vector<float>& GetVertices();;
	bool InCircumscribeLight(glm::vec3& eye, glm::vec3& center, float range,  float pixelsize);
	glm::mat4 GetMatrix(glm::vec3 eye, glm::vec3& center, float range, float pixelsize);
};

struct ProxyIcosahedron
{
	const std::vector<float>& GetVertices();
	bool InCircumscribeLight(glm::vec3 & eye, glm::vec3& center, float range, glm::vec3 dir, float coszero, float pixelsize);
	glm::mat4 GetMatrix(glm::vec3 eye, glm::vec3& center, float range, glm::vec3& dir, float coszero, float pixelsize);
};