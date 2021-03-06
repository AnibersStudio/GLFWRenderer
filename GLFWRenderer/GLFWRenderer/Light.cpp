#include "Light.h"

float PointLight::GetRange(float threshold) const
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

float SpotLight::IntenAt(glm::vec3 pos) const
{
	float cosine = glm::dot(glm::normalize(pos - this->position), direction);
	float distance = length(pos - position);
	float decay = intensity / (atten.constant + atten.linear * distance + atten.exp * distance * distance);
	if (decay < 0.00001 || cosine < zerodot)
	{
		return 0.0f;
	}

	float cosinefactor;
	if (cosine > fulldot)
	{
		cosinefactor = 1.0f;
	}
	else
	{
		cosinefactor = (cosine - zerodot) / (fulldot - zerodot);
	}
	return cosinefactor * decay;
}

float SpotLight::GetRange(float threshold) const
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
