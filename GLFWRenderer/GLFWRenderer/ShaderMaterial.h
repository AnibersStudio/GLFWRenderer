#pragma once
#include "GLCommon.h"
#include "TexturedMaterial.h"

struct ShaderTexture
{
	GLuint64 handle = 0xFFFFFFFFFFFFFFFF;
	GLboolean is = false;
};

struct ShaderMaterial
{
	Material material;//5 vec3s	
	ShaderTexture diffuse;
	ShaderTexture specular;
	ShaderTexture normal;
	ShaderTexture emissive;
	ShaderTexture trans;
	float not_used[2];
};