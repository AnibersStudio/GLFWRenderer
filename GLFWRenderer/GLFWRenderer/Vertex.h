#pragma once
#include "GLCommon.h"
struct Vertex
{
	glm::vec3 position;
	glm::vec2 texcoord;
	glm::vec3 normal;
	glm::vec3 tangent;
	void Transform(const glm::mat4& transformmatrix);
};

struct MaterialedVertex
{
	Vertex vertex;
	unsigned int material;// 0 means cull this vertex
};