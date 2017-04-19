#version 430 core
#extension GL_ARB_bindless_texture : enable

layout (location = 0) out vec4 Color;

in vec3 fragpos;
in vec2 texcoord;
in vec3 lightspace[20];

uniform uvec2 tilesize = uvec2(32u, 32u);
uniform uvec2 tilecount;
uniform uint lightspacecount;
uniform sampler2D diffuse;

struct DirectionalLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	sampler2D shadowsampler;

	vec3 direction;
	// This will become a ID to transform list
	uint hasshadow;
};

struct Attenuation
{
	float constant;
	float linear;
	float exponential;
	float not_used[1];
};

struct PointLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	samplerCube shadowsampler;

	Attenuation atten;
	vec3 position;
	// This - 1 will become a ID to transform list
	uint transformID;
};

struct SpotLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
	sampler2D shadowsampler;

	Attenuation atten;
	vec3 position;
	// This - 1 will become a ID to transform list
	uint hasshadow;

	//Must be normalized!
	vec3 direction;
	float fullcos;

	float zerocos;
	float not_used[3];
};

struct LightTransform
{
	//Only directional/spot light need this. Point light will be set to glm::mat4(1.0)
	mat4 VP;
	// .x near plane .y far plane
	vec2 plane;
	float not_used[2];
};


layout (std140) uniform directionallightlist
{
	DirectionalLight dl[4];
};

layout (std430, binding = 0) buffer pointlightlist
{
	PointLight pl[];
};

layout (std430, binding = 1) buffer spotlightlist
{
    SpotLight sl[];
};

layout (std140) uniform lighttransformlist
{
	LightTransform lighttransform[36];
};

layout (std430, binding = 2) buffer pointlightindexlist
{
	uvec2 pointlightindex[];
	//.x means start
	//.y means tail
};

layout (std430, binding = 3) buffer spotlightindexlist
{
	uvec2 spotlightindex[];
	//.x means start
	//.y means tail
};

layout (std430, binding = 4) buffer lightlinkedlist
{
	uvec2 lightlinked[];// lightlinked[0]/[0xFFFFFFFF] is left empty on purpose
	//.x means light id
	//.y means next node
};

void main() 
{
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	if (pointlightindex[tileindex].x == uint(0xFFFFFFFF))
	{
		Color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	else
	{
		//float zerodot = 0.4;
		//float fulldot = 0.5;
		//vec3 position = vec3(0.0, 5.0, 0.0);
		//vec3 direction = vec3(0.0, -1.0, 0.0);
		//float dist = length(fragpos - position);
		//float cosinefactor = max(1.0f, (dot(direction, normalize(fragpos - position)) -  zerodot) /(fulldot - zerodot) );
		//float attenuation = 1.0 + 1.0 * dist + 0.2 * dist * dist;
		Color = texture(diffuse, texcoord);/*/ attenuation * 10.0 * cosinefactor;*/
	}

}