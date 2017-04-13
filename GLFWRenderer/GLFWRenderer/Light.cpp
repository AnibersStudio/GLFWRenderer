#include "Light.h"

float PointLight::GetRange(float intensity, Attenuation atten, float threshold)
{
	float range;
	if (atten.exp > 0.00001)
	{
		float lambda = atten.linear * atten.linear - 4 * atten.exp * (atten.constant - 1 / threshold * intensity);
		range = (pow(lambda, 0.5f) - atten.linear) / (2 * atten.exp);
	}
	else
	{
		range = (1 / threshold * intensity - atten.constant) / atten.linear;
	}
	return range;
}

float SpotLight::IntenAt(glm::vec3 pos)
{
	float inten;
	float cosine = glm::dot(glm::normalize(pos - this->position), direction);
	float distance = length(pos - position);
	float decay = intensity / (atten.constant + atten.linear * distance + atten.exp * distance * distance);
	if (decay < 0.00001 || cosine > zerocos)
	{
		return 0.0f;
	}

	float cosinefactor;
	if (cosine < fullcos)
	{
		cosinefactor = 1.0f;
	}
	else
	{
		cosinefactor = (cosine - fullcos) / (zerocos - fullcos);
	}
	return cosinefactor * decay;
}
