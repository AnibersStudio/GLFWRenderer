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
	//Only directional/spot light need this. Point light will be set to translate(position)
	mat4 View;
	//Only directional/spot light need this. Point light will be set to perspective(90)
	mat4 Proj;
	// .x near plane .y far plane
	vec2 plane;
	//worldsize for this texel
	float texelworldsize;
};

layout (std140) uniform lighttransformlist
{
	// index is transformID - 1
	LightTransform lighttransform[36];
};

vec4 NormalOffsetLightSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize);
vec3 NormalOffsetWorldSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize);

void main()
{
	gl_Position = WVP * vec4(position, 1.0f);
	fragpos = position;
	texcoord = texturecoord;
	fragnormal = normal;
	for (unsigned int i = 0u; i != lightspacecount; i++)
	{
		vec4 lightspacepos = NormalOffsetLightSpace(normal, position, lighttransform[i].View, lighttransform[i].Proj, lighttransform[i].texelworldsize);
		//vec4 lightspacepos = lighttransform[i].Proj * lighttransform[i].View * vec4(position, 1.0f);
		lightspace[i].xy = (lightspacepos.xy / lightspacepos.w * 0.5f + 0.5f) * lightspacepos.w;
		lightspace[i].z = lightspacepos.w;
	}
}

vec4 NormalOffsetLightSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize)
{
	{//ScaleShadowOffsetByShadowDepth
		vec4 lightviewpos = view * vec4(worldpos, 1.0f);
		float shadowfovfactor = max(proj[0].x, proj[1].y);
		texelsize *= abs(lightviewpos.z) * shadowfovfactor;
	}
	//Angle Based Bias
	vec3 lightpos = - view[3].xyz;
	vec3 vertextolight = normalize(lightpos - worldpos);
	float coslightangle = dot(vertextolight, worldnormal);
	float normaloffsetscale = clamp(1 - coslightangle, 0.0f, 1.0f);
	vec3 shadowoffset = normaloffsetscale * worldnormal;
	{//Only offset UV texture coord
		vec4 lightspacepos = proj * view * vec4(worldpos, 1.0f);

		vec3 offsetworldpos = worldpos + shadowoffset;
		vec4 offsetlightspacepos = proj * view * vec4(offsetworldpos, 1.0f);
		lightspacepos.xy = offsetlightspacepos.xy;
		return lightspacepos;
	}
	//{//3D normal offset
	//	worldpos += shadowoffset;
	//	return proj * view * vec4(worldpos, 1.0f);
	//}
}

vec3 NormalOffsetWorldSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize)
{
	{//ScaleShadowOffsetByShadowDepth
		vec4 lightviewpos = view * vec4(worldpos, 1.0f);
		float shadowfovfactor = max(proj[0].x, proj[1].y);
		texelsize *= abs(lightviewpos.z) * shadowfovfactor;
	}
	//Angle Based Bias
	vec3 lightpos = - view[3].xyz;
	vec3 vertextolight = normalize(lightpos - worldpos);
	float coslightangle = dot(vertextolight, worldnormal);
	float normaloffsetscale = clamp(1 - coslightangle, 0.0f, 1.0f);
	vec3 shadowoffset = normaloffsetscale * worldnormal;
	return worldpos + shadowoffset;
}