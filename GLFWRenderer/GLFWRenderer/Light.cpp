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
