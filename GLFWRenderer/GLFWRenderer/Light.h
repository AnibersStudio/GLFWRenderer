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
	GLuint64 samplerCubemap = 0xFFFFFFFFFFFFFFFF;

	Attenuation atten;
	glm::vec3 position;
	unsigned int hasshadow = false;

	float IntenAt(glm::vec3 pos) { float distance = length(pos - position); return intensity / (atten.constant + atten.linear * distance + atten.exp * distance * distance); }

	static float GetRange(float intensity, Attenuation atten, float threshold);
};

struct SpotLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	GLuint64 sampler2D = 0xFFFFFFFFFFFFFFFF;

	Attenuation atten;
	glm::vec3 position;
	float not_used1[1]{};

	//Must be normalized!
	glm::vec3 direction;
	GLfloat fullcos;

	GLfloat zerocos;
	unsigned int hasshadow = false;
	float not_used2[2]{};

	float IntenAt(glm::vec3 pos);

	static float GetRange(float intensity, Attenuation atten, float threshold) { return PointLight::GetRange(intensity, atten, threshold); };
};