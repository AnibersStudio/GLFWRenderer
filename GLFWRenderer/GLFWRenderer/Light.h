#pragma once
#include "GLCommon.h"
//Lights Defs
using namespace glm;

struct DirectionalLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	GLuint64 sampler2D;

	glm::vec3 direction;
	float not_used[1]{};
};

struct Attenuation
{
	GLfloat constant = 1.0;
	GLfloat linear = 0.0;
	GLfloat exp = 1.0;
	float not_used[1]{};
};

struct PointLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	GLuint64 samplerCubemap;

	Attenuation atten;
	glm::vec3 position;
	unsigned int hasshadow = false;

	static float GetRange(float intensity, Attenuation atten, float threshold);
};

struct SpotLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	GLuint64 sampler2D;

	Attenuation atten;
	glm::vec3 position;
	float not_used[1]{};

	glm::vec3 direction;
	GLfloat fullcos;

	GLfloat zerocos;
	unsigned int hasshadow = false;
	float not_used[2]{};

	static float GetRange(float intensity, Attenuation atten, float threshold) { return PointLight::GetRange(intensity, atten, threshold); };
};