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
	// This will become a ID to transform list
	unsigned int hasshadow;
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
	// This will become a ID to transform list
	unsigned int hasshadow = false;

	float IntenAt(glm::vec3 pos) const { float distance = length(pos - position); return intensity / (atten.constant + atten.linear * distance + atten.exp * distance * distance); }

	float GetRange(float threshold) const;
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
	// This will become a ID to transform list
	unsigned int hasshadow = false;

	//Must be normalized!
	glm::vec3 direction;
	GLfloat fullcos;

	GLfloat zerocos;
	float not_used[3]{};

	float IntenAt(glm::vec3 pos) const;

	float GetRange(float threshold) const;
};

struct LightTransform
{
	//Only directional/spot light need this. Point light will be set to glm::mat4(1.0)
	glm::mat4 VP = glm::mat4(1.0f);
	// .x near plane .y far plane
	glm::vec2 plane = glm::vec2(0.1f, 64.0f);
	float not_used[2]{};
};