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
	// This will become a ID to transform list
	unsigned int hasshadow;

	glm::vec3 direction;

	float not_used[4];
};

struct Attenuation
{
	GLfloat constant = 1.0;
	GLfloat linear = 0.0;
	GLfloat exp = 1.0;
};

struct PointLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;

	GLuint64 samplerCubemap = 0xFFFFFFFFFFFFFFFF;
	// This will become a ID to transform list
	unsigned int hasshadow = false;

	Attenuation atten;
	glm::vec3 position;
	float not_used[1]{};

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
	// This will become a ID to transform list
	unsigned int hasshadow = false;

	Attenuation atten;
	glm::vec3 position;
	GLfloat zerodot;

	//Must be normalized!
	glm::vec3 direction;
	GLfloat fulldot;

	float IntenAt(glm::vec3 pos) const;

	float GetRange(float threshold) const;
};

struct LightTransform
{
	//Only directional/spot light need this. Point light will be set to translate(position)
	glm::mat4 View = glm::mat4(1.0f);
	//Only directional/spot light need this. Point light will be set to perspective(90)
	glm::mat4 Proj = glm::mat4(1.0f);
	// .x near plane .y far plane
	glm::vec2 plane = glm::vec2(0.1f, 64.0f);
	float texelworldsize;
	float not_used[1]{};
};