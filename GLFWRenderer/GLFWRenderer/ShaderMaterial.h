#pragma once
#include "GLCommon.h"
#include "TexturedMaterial.h"

struct ShaderTexture
{
	GLuint64 handle = 0xFFFFFFFFFFFFFFFF;
	GLuint is = false;
	float not_used[1];
};

struct ShaderMaterial
{
	Material material;
	ShaderTexture diffuse;
	ShaderTexture specular;
	ShaderTexture normal;
	ShaderTexture emissive;
	ShaderTexture trans;
};