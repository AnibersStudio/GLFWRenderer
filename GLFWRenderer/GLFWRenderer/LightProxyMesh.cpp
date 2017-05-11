#include "LightProxyMesh.h"
#include <iostream>
const std::vector<float>& ProxyPyramid::GetVertices()
{
	static std::vector<float> vertices
	{ 
		0.00000f, 0.00000f, 0.00000f,   0.00000f, 0.89443f, 0.44721f,
		0.50000f, 0.50000f, -1.0000f,   0.00000f, 0.89443f, 0.44721f,
		-0.50000f, 0.50000f, -1.0000f,   0.00000f, 0.89443f, 0.44721f,
		0.00000f, 0.00000f, 0.00000f,   -0.89443f, 0.00000f, 0.44721f,
		-0.50000f, 0.50000f, -1.0000f,   -0.89443f, 0.00000f, 0.44721f,
		-0.50000f, -0.50000f, -1.0000f,   -0.89443f, 0.00000f, 0.44721f,
		0.00000f, 0.00000f, 0.00000f,   0.00000f, -0.89443f, 0.44721f,
		-0.50000f, -0.50000f, -1.0000f,   0.00000f, -0.89443f, 0.44721f,
		0.50000f, -0.50000f, -1.0000f,   0.00000f, -0.89443f, 0.44721f,
		0.00000f, 0.00000f, 0.00000f,   0.89443f, -0.00000f, 0.44721f,
		0.50000f, -0.50000f, -1.0000f,   0.89443f, -0.00000f, 0.44721f,
		0.50000f, 0.50000f, -1.0000f,   0.89443f, -0.00000f, 0.44721f,
		0.50000f, 0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f,
		0.50000f, -0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f,
		-0.50000f, -0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f,
		-0.50000f, -0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f,
		-0.50000f, 0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f,
		0.50000f, 0.50000f, -1.0000f,   0.00000f, 0.00000f, -1.0000f 
	};
	return vertices;
}

bool ProxyPyramid::InCircumscribeLight(glm::vec3 eye, glm::vec3 center, float range, glm::vec3 dir, float coszero, float pixelsize)
{
	float distance = glm::length(eye - center);
	float paradistance = distance + 0.5f + range;
	float additionalsize = paradistance * pixelsize * (0.5f);
	additionalsize *= additionalsize;
	float tanzeroangle = glm::sqrt((1 - coszero * coszero) / (coszero * coszero));
	float xoyscale = tanzeroangle * range * 2;
	float additionalscale = glm::max((xoyscale + additionalsize) / xoyscale, (range + additionalsize) / range);
	glm::vec3 scalevector = glm::vec3(xoyscale * additionalscale, xoyscale * additionalscale, range * additionalscale);

	glm::vec3 centertranslate = -(dir) * (scalevector.z - range) * 0.5f;
	center += centertranslate;
	distance = glm::length(eye - center);

	float circumrange = glm::sqrt(range * range + xoyscale * xoyscale / 2);
	float circumcos = range / circumrange;

	if (distance > circumrange)
	{
		std::cout << "out" << std::endl;
		return false;
	}
	else
	{
		float cosine = glm::dot(glm::normalize(eye - center), dir);

		if (cosine >= circumcos)
		{
			std::cout << "in" << std::endl;
		}
		else
		{
			std::cout << "out" << std::endl;
		}

		return cosine >= circumcos;
	}
}

glm::mat4 ProxyPyramid::GetMatrix(glm::vec3 eye, glm::vec3 center, float range, glm::vec3 dir, float coszero, float pixelsize)
{
	float distance = glm::length(eye - center);
	float paradistance = distance + 0.5f + range;
	float additionalsize = paradistance * pixelsize * (0.5f); 
	additionalsize *= additionalsize;
	float tanzeroangle = glm::sqrt((1 - coszero * coszero) / (coszero * coszero));
	float xoyscale = tanzeroangle * range * 2;
	float additionalscale = glm::max((xoyscale + additionalsize) / xoyscale, (range + additionalsize) / range);
	glm::vec3 scalevector = glm::vec3(xoyscale * additionalscale, xoyscale * additionalscale, range * additionalscale);

	glm::vec3 axis;
	float degrees;
	if (glm::abs(dir.x) < 0.00001 && glm::abs(dir.y) < 0.00001)
	{
		axis = glm::vec3(0.0, 1.0, 0.0);
		degrees = dir.z * 90.0f + 90.0f;
	}
	else
	{
		axis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), dir));
		double dotproduct = glm::dot(glm::vec3(0.0f, 0.0f, -1.0f), dir);
		degrees = glm::degrees(glm::acos(dotproduct));
	}
	return glm::scale(glm::translate(glm::rotate(glm::translate(glm::mat4(1.0f), center), degrees, axis), glm::vec3(0.0f, 0.0f, 0.5f * (scalevector.z - range))), scalevector);
}

const std::vector<float>& ProxyIcosahedron::GetVertices()
{
	static std::vector<float> vertices
	{
		0.52573f, 0.00000f, 0.85065f,   0.00000f, 0.35682f, 0.93417f,
		0.00000f, 0.85065f, 0.52573f,   0.00000f, 0.35682f, 0.93417f,
		-0.52573f, 0.00000f, 0.85065f,   0.00000f, 0.35682f, 0.93417f,
		0.00000f, 0.85065f, 0.52573f,   -0.57735f, 0.57735f, 0.57735f,
		-0.85065f, 0.52573f, 0.00000f,   -0.57735f, 0.57735f, 0.57735f,
		-0.52573f, 0.00000f, 0.85065f,   -0.57735f, 0.57735f, 0.57735f,
		0.00000f, 0.85065f, 0.52573f,   -0.35682f, 0.93417f, 0.00000f,
		0.00000f, 0.85065f, -0.52573f,   -0.35682f, 0.93417f, 0.00000f,
		-0.85065f, 0.52573f, 0.00000f,   -0.35682f, 0.93417f, 0.00000f,
		0.85065f, 0.52573f, 0.00000f,   0.35682f, 0.93417f, -0.00000f,
		0.00000f, 0.85065f, -0.52573f,   0.35682f, 0.93417f, -0.00000f,
		0.00000f, 0.85065f, 0.52573f,   0.35682f, 0.93417f, -0.00000f,
		0.52573f, 0.00000f, 0.85065f,   0.57735f, 0.57735f, 0.57735f,
		0.85065f, 0.52573f, 0.00000f,   0.57735f, 0.57735f, 0.57735f,
		0.00000f, 0.85065f, 0.52573f,   0.57735f, 0.57735f, 0.57735f,
		0.52573f, 0.00000f, 0.85065f,   0.93417f, -0.00000f, 0.35682f,
		0.85065f, -0.52573f, 0.00000f,   0.93417f, -0.00000f, 0.35682f,
		0.85065f, 0.52573f, 0.00000f,   0.93417f, -0.00000f, 0.35682f,
		0.85065f, -0.52573f, 0.00000f,   0.93417f, 0.00000f, -0.35682f,
		0.52573f, 0.00000f, -0.85065f,   0.93417f, 0.00000f, -0.35682f,
		0.85065f, 0.52573f, 0.00000f,   0.93417f, 0.00000f, -0.35682f,
		0.85065f, 0.52573f, 0.00000f,   0.57735f, 0.57735f, -0.57735f,
		0.52573f, 0.00000f, -0.85065f,   0.57735f, 0.57735f, -0.57735f,
		0.00000f, 0.85065f, -0.52573f,   0.57735f, 0.57735f, -0.57735f,
		0.52573f, 0.00000f, -0.85065f,   0.00000f, 0.35682f, -0.93417f,
		-0.52573f, 0.00000f, -0.85065f,   0.00000f, 0.35682f, -0.93417f,
		0.00000f, 0.85065f, -0.52573f,   0.00000f, 0.35682f, -0.93417f,
		0.52573f, 0.00000f, -0.85065f,   0.00000f, -0.35682f, -0.93417f,
		0.00000f, -0.85065f, -0.52573f,   0.00000f, -0.35682f, -0.93417f,
		-0.52573f, 0.00000f, -0.85065f,   0.00000f, -0.35682f, -0.93417f,
		0.52573f, 0.00000f, -0.85065f,   0.57735f, -0.57735f, -0.57735f,
		0.85065f, -0.52573f, 0.00000f,   0.57735f, -0.57735f, -0.57735f,
		0.00000f, -0.85065f, -0.52573f,   0.57735f, -0.57735f, -0.57735f,
		0.85065f, -0.52573f, 0.00000f,   0.35682f, -0.93417f, 0.00000f,
		0.00000f, -0.85065f, 0.52573f,   0.35682f, -0.93417f, 0.00000f,
		0.00000f, -0.85065f, -0.52573f,   0.35682f, -0.93417f, 0.00000f,
		0.00000f, -0.85065f, 0.52573f,   -0.35682f, -0.93417f, 0.00000f,
		-0.85065f, -0.52573f, 0.00000f,   -0.35682f, -0.93417f, 0.00000f,
		0.00000f, -0.85065f, -0.52573f,   -0.35682f, -0.93417f, 0.00000f,
		0.00000f, -0.85065f, 0.52573f,   -0.57735f, -0.57735f, 0.57735f,
		-0.52573f, 0.00000f, 0.85065f,   -0.57735f, -0.57735f, 0.57735f,
		-0.85065f, -0.52573f, 0.00000f,   -0.57735f, -0.57735f, 0.57735f,
		0.00000f, -0.85065f, 0.52573f,   0.00000f, -0.35682f, 0.93417f,
		0.52573f, 0.00000f, 0.85065f,   0.00000f, -0.35682f, 0.93417f,
		-0.52573f, 0.00000f, 0.85065f,   0.00000f, -0.35682f, 0.93417f,
		0.85065f, -0.52573f, 0.00000f,   0.57735f, -0.57735f, 0.57735f,
		0.52573f, 0.00000f, 0.85065f,   0.57735f, -0.57735f, 0.57735f,
		0.00000f, -0.85065f, 0.52573f,   0.57735f, -0.57735f, 0.57735f,
		-0.85065f, -0.52573f, 0.00000f,   -0.93417f, 0.00000f, 0.35682f,
		-0.52573f, 0.00000f, 0.85065f,   -0.93417f, 0.00000f, 0.35682f,
		-0.85065f, 0.52573f, 0.00000f,   -0.93417f, 0.00000f, 0.35682f,
		-0.52573f, 0.00000f, -0.85065f,   -0.93417f, 0.00000f, -0.35682f,
		-0.85065f, -0.52573f, 0.00000f,   -0.93417f, 0.00000f, -0.35682f,
		-0.85065f, 0.52573f, 0.00000f,   -0.93417f, 0.00000f, -0.35682f,
		0.00000f, 0.85065f, -0.52573f,   -0.57735f, 0.57735f, -0.57735f,
		-0.52573f, 0.00000f, -0.85065f,   -0.57735f, 0.57735f, -0.57735f,
		-0.85065f, 0.52573f, 0.00000f,   -0.57735f, 0.57735f, -0.57735f,
		-0.85065f, -0.52573f, 0.00000f,   -0.57735f, -0.57735f, -0.57735f,
		-0.52573f, 0.00000f, -0.85065f,   -0.57735f, -0.57735f, -0.57735f,
		0.00000f, -0.85065f, -0.52573f,   -0.57735f, -0.57735f, -0.57735f
	};
	return vertices;
}

bool ProxyIcosahedron::InCircumscribeLight(glm::vec3 eye, glm::vec3 center, float range, float pixelsize)
{
	float distance = glm::length(eye - center);
	float paradistance = distance + 0.5f;
	float additionalsize = paradistance * pixelsize * (0.5f);
	float circumrange = range + 1.2 + additionalsize;
	return distance <= circumrange;
}

glm::mat4 ProxyIcosahedron::GetMatrix(glm::vec3 eye, glm::vec3 center, float range, float pixelsize)
{
	float distance = glm::length(eye - center);
	float paradistance = distance + 0.5f;
	float additionalsize = paradistance * pixelsize * (0.5f);
	
	return glm::scale(glm::translate(glm::mat4(1.0f), center), glm::vec3(range + additionalsize + 1.2/*1.2 is from innerscribe to circumscribe*/));
}
