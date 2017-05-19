#version 430 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texturecoord;
layout (location = 2) in vec3 normal;

out vec3 fragpos;
out vec2 texcoord;
out vec3 fragnormal;
// transformID - 1 is the index of this array
out vec3 lightspace[20];

uniform mat4 WVP;
uniform uint lightspacecount;

struct LightTransform
{
	//Only directional/spot light need this. Point light will be set to glm::mat4(1.0)
	mat4 VP;
	// .x near plane .y far plane
	vec2 plane;
};

layout (std140) uniform lighttransformlist
{
	// index is transformID - 1
	LightTransform lighttransform[36];
};

void main()
{
	gl_Position = WVP * vec4(position, 1.0f);
	fragpos = position;
	texcoord = texturecoord;
	fragnormal = normal;
	for (unsigned int i = 0u; i != lightspacecount; i++)
	{
		vec4 lightspacepos = lighttransform[i].VP * vec4(position, 1.0f);
		lightspace[i].xy = (lightspacepos.xy / lightspacepos.w * 0.5f + 0.5f) * lightspacepos.w;
		lightspace[i].z = lightspacepos.w;
	}
}