#include "LightProxyMesh.h"

const std::vector<float>& ProxyPyramid::GetVertices()
{
	static std::vector<float> vertices
	{ 0.0f, 0.0f, 0.0f,   0.5f, 0.5f, -1.0f,   -0.5f, 0.5f, -1.0f,
		0.0f, 0.0f, 0.0f,   0.5f, -0.5f, -1.0f,   0.5f, 0.5f, -1.0f,
		0.0f, 0.0f, 0.0f,    -0.5f, -0.5f, -1.0f,   0.5f, -0.5f, -1.0f,
		0.0f, 0.0f, 0.0f,    -0.5f, 0.5f, -1.0f,   -0.5f, -0.5f, -1.0f,
		0.5f, 0.5f, 1.0f,   -0.5f, -0.5f, -1.0f,   -0.5f, 0.5f, -1.0f,
		0.5f, 0.5f, 1.0f,   0.5f, -0.5f, -1.0f,   -0.5f, -0.5f, -1.0f };
	return vertices;
}

bool ProxyPyramid::InCircumscribeLight(glm::vec3 & eye, glm::vec3 & center, float range, glm::vec3 dir, float coszero, float pixelsize)
{
	float distance = glm::max(glm::length(eye - center), 1.0f);
	float additionalsize = distance * pixelsize * 0.5;
	float tanzeroangle = glm::sqrt(1 - coszero * coszero);
	float xoyscale = tanzeroangle * range * 2;
	float additionalscale = glm::max((xoyscale + additionalsize) / xoyscale, (range + additionalsize) / range);
	range *= additionalscale;
	xoyscale *= additionalscale;
	float circumrange = glm::sqrt(range * range + xoyscale * xoyscale / 4);
	float circumcos = range / circumrange;

	if (distance > range)
	{
		return false;
	}
	else
	{
		float cosine = glm::dot(glm::normalize(eye - center), dir);
		return cosine <= circumcos;
	}
}

glm::mat4 ProxyPyramid::GetMatrix(glm::vec3 eye, glm::vec3 & center, float range, glm::vec3 & dir, float coszero, float pixelsize)
{
	float distance = glm::max(glm::length(eye - center), 1.0f);
	float additionalsize = distance * pixelsize * 0.5;
	float tanzeroangle = glm::sqrt((1 - coszero * coszero) / (coszero * coszero));
	float xoyscale = tanzeroangle * range * 2;
	float additionalscale = glm::max((xoyscale + additionalsize) / xoyscale, (range + additionalsize) / range);
	glm::vec3 scalevector = glm::vec3(xoyscale * additionalscale, xoyscale * additionalscale, range * additionalscale);
	glm::mat4 transform = glm::scale(glm::mat4(1.0), scalevector);

	glm::vec3 axis;
	float degrees;
	if (glm::abs(dir.x) < 0.00001 && glm::abs(dir.y) < 0.00001)
	{
		axis = glm::vec3(0.0, 1.0, 0.0);
		degrees = dir.z * 90.0f + 90.0f;
	}
	else
	{
		axis = glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), dir);
		double dotproduct = glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), dir);
		degrees = glm::degrees(glm::acos(dotproduct));
	}
	transform = glm::rotate(transform, degrees, axis);
	return glm::translate(transform, center);
}

const std::vector<float>& ProxyIcosahedron::GetVertices()
{
	static std::vector<float> vertices
	{
		0.52573f, 0.00000f, 0.85065f,   0.00000f, 0.85065f, 0.52573f,   -0.52573f, 0.00000f, 0.85065f,
		0.00000f, 0.85065f, 0.52573f,   -0.85065f, 0.52573f, 0.00000f,   -0.52573f, 0.00000f, 0.85065f,
		0.00000f, 0.85065f, 0.52573f,   0.00000f, 0.85065f, -0.52573f,   -0.85065f, 0.52573f, 0.00000f,
		0.85065f, 0.52573f, 0.00000f,   0.00000f, 0.85065f, -0.52573f,   0.00000f, 0.85065f, 0.52573f,
		0.52573f, 0.00000f, 0.85065f,   0.85065f, 0.52573f, 0.00000f,   0.00000f, 0.85065f, 0.52573f,
		0.52573f, 0.00000f, 0.85065f,   0.85065f, -0.52573f, 0.00000f,   0.85065f, 0.52573f, 0.00000f,
		0.85065f, -0.52573f, 0.00000f,   0.52573f, 0.00000f, -0.85065f,   0.85065f, 0.52573f, 0.00000f,
		0.85065f, 0.52573f, 0.00000f,   0.52573f, 0.00000f, -0.85065f,   0.00000f, 0.85065f, -0.52573f,
		0.52573f, 0.00000f, -0.85065f,   -0.52573f, 0.00000f, -0.85065f,   0.00000f, 0.85065f, -0.52573f,
		0.52573f, 0.00000f, -0.85065f,   0.00000f, -0.85065f, -0.52573f,   -0.52573f, 0.00000f, -0.85065f,
		0.52573f, 0.00000f, -0.85065f,   0.85065f, -0.52573f, 0.00000f,   0.00000f, -0.85065f, -0.52573f,
		0.85065f, -0.52573f, 0.00000f,   0.00000f, -0.85065f, 0.52573f,   0.00000f, -0.85065f, -0.52573f,
		0.00000f, -0.85065f, 0.52573f,   -0.85065f, -0.52573f, 0.00000f,   0.00000f, -0.85065f, -0.52573f,
		0.00000f, -0.85065f, 0.52573f,   -0.52573f, 0.00000f, 0.85065f,   -0.85065f, -0.52573f, 0.00000f,
		0.00000f, -0.85065f, 0.52573f,   0.52573f, 0.00000f, 0.85065f,   -0.52573f, 0.00000f, 0.85065f,
		0.85065f, -0.52573f, 0.00000f,   0.52573f, 0.00000f, 0.85065f,   0.00000f, -0.85065f, 0.52573f,
		-0.85065f, -0.52573f, 0.00000f,   -0.52573f, 0.00000f, 0.85065f,   -0.85065f, 0.52573f, 0.00000f,
		-0.52573f, 0.00000f, -0.85065f,   -0.85065f, -0.52573f, 0.00000f,   -0.85065f, 0.52573f, 0.00000f,
		0.00000f, 0.85065f, -0.52573f,   -0.52573f, 0.00000f, -0.85065f,   -0.85065f, 0.52573f, 0.00000f,
		-0.85065f, -0.52573f, 0.00000f,   -0.52573f, 0.00000f, -0.85065f,   0.00000f, -0.85065f, -0.52573f
	};
	return vertices;
}

bool ProxyIcosahedron::InCircumscribeLight(glm::vec3 & eye, glm::vec3 & center, float range, float pixelsize)
{
	float distance = glm::max(glm::length(eye - center), 1.0f);
	float additionalsize = distance * 0.5 * pixelsize;
	float circumrange = range + 1.2 + additionalsize;
	return distance <= circumrange;
}

glm::mat4 ProxyIcosahedron::GetMatrix(glm::vec3 eye, glm::vec3 & center, float range, float pixelsize)
{
	float distance = glm::max(glm::length(eye - center), 1.0f);
	float additionalsize = distance * pixelsize * 0.5;
	glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(range + additionalsize + 1.2/*1.2 is from innerscribe to circumscribe*/));
	return glm::translate(scale, glm::vec3(center));
}
