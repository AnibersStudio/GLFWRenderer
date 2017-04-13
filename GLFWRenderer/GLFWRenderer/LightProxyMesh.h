#pragma once
#include <vector>

#include "GLCommon.h"

struct ProxyPyramid
{
	static const std::vector<float>& GetVertices();
	bool InCircumscribeLight(glm::vec3 & eye, glm::vec3& center, float range, glm::vec3 dir, float coszero, float pixelsize);
	glm::mat4 GetMatrix(glm::vec3 eye, glm::vec3& center, float range, glm::vec3& dir, float coszero, float pixelsize);
};

struct ProxyIcosahedron
{
	static const std::vector<float>& GetVertices();
	bool InCircumscribeLight(glm::vec3& eye, glm::vec3& center, float range, float pixelsize);
	glm::mat4 GetMatrix(glm::vec3 eye, glm::vec3& center, float range, float pixelsize);
};