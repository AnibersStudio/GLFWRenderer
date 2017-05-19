#version 450
#extension GL_ARB_bindless_texture : enable
#define C_VALUE 400.0f
layout (location = 0) out vec4 Color;

in vec3 fragpos;
in vec2 texcoord;
in vec3 fragnormal;
in vec3 lightspace[20];

uniform uvec2 tilesize = uvec2(32u, 32u);
uniform uvec2 tilecount;
uniform uint lightspacecount;
uniform uint dlcount;
uniform sampler2D diffusetex;
uniform vec3 eye;

struct DirectionalLight
{
	vec3 color;

	float intensity;
	float diffuse;
	float specular;

	sampler2D shadowsampler;

	// This will become a ID to transform list
	uint transformID;

	vec3 direction;
};

struct Attenuation
{
	float constant;
	float linear;
	float exponential;
};

struct PointLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;

	samplerCube shadowsampler;
	// This - 1 will become a ID to transform list
	uint transformID;

	Attenuation atten;
	vec3 position;
	float not_used[1];
};

struct SpotLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;

	sampler2D shadowsampler;
	//// This - 1 will become a ID to transform list
	uint transformID;

	Attenuation atten;
	vec3 position;
	float zerodot;

	////Must be normalized!
	vec3 direction;
	float fulldot;
};

struct LightTransform
{
	//Only directional/spot light need this. Point light will be set to glm::mat4(1.0)
	mat4 VP;
	// .x near plane .y far plane
	vec2 plane;
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

vec3 LightByDL(vec3 diffuse, vec3 specular, float shininess, vec3 normal);
vec3 LightByPL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex);
vec3 LightBySL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex);
vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction, vec3 eyedir);
float ShadowFactor(sampler2D ztex, vec3 frag);
float ShadowFactor(samplerCube ztex, vec3 frag, float depth);
float Esm(float depth, float zintex);

void main() 
{
	//Calculate tile
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	//Get material
	vec3 diffuse = texture(diffusetex, texcoord).rgb;
	//Calculate dl
	vec3 color = vec3(0.0f);
	color += LightByDL(diffuse, vec3(0.4f), 100.0f, fragnormal);
	color += LightByPL(diffuse, vec3(0.4f), 100.0f, fragnormal, tileindex);
	color += LightBySL(diffuse, vec3(0.4f), 100.0f, fragnormal, tileindex);

	Color = vec4(color.xyz, 1.0f);
}

vec3 LightByDL(vec3 diffuse, vec3 specular, float shininess, vec3 normal)
{
	vec3 color = vec3(0.0f);
	for (uint i = 0; i != dlcount; i++)
	{
		DirectionalLight thislight = dl[i];

		float shadowfactor = 1.0f;
		if (thislight.transformID > 0)
		{
			vec2 plane = lighttransform[thislight.transformID - 1].plane;
			vec3 lightspacepos = lightspace[thislight.transformID - 1];
			lightspacepos.xy /= lightspacepos.z;
			lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
			shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos);
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, -thislight.direction, normalize(eye - fragpos)) * thislight.intensity;
	}
	return color;
}

vec3 LightByPL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex)
{
	vec3 color = vec3(0.0f);
	uint i = pointlightindex[tileindex].x;
	uint endindex = pointlightindex[tileindex].y;
	if (i != uint(0xFFFFFFFF))
	{
		while(i != endindex)
		{	
			PointLight thislight = pl[lightlinked[i].x];
			float dist = length(thislight.position - fragpos);
			float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;

			float shadowfactor = 1.0f;
			if (thislight.transformID > 0)
			{
				vec2 plane = lighttransform[thislight.transformID - 1].plane;
				shadowfactor = ShadowFactor(thislight.shadowsampler, normalize(fragpos - thislight.position), (length(fragpos - thislight.position) -  plane.x) / (plane.y - plane.x));
			}
			color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay;
			i = lightlinked[i].y;
		}
		PointLight thislight = pl[lightlinked[endindex].x];
		float dist = length(thislight.position - fragpos);
		float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;

		float shadowfactor = 1.0f;
		if (thislight.transformID > 0)
		{
			vec2 plane = lighttransform[thislight.transformID - 1].plane;
			shadowfactor = ShadowFactor(thislight.shadowsampler, normalize(fragpos - thislight.position), (length(fragpos - thislight.position) -  plane.x) / (plane.y - plane.x));
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay;
	}
	return color;
}
vec3 LightBySL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex)
{
	vec3 color = vec3(0.0f);
	uint i = spotlightindex[tileindex].x;
	uint endindex = spotlightindex[tileindex].y;
	if (i != uint(0xFFFFFFFF))
	{
		while(i != endindex)
		{
			SpotLight thislight = sl[lightlinked[i].x];
			float dist = length(thislight.position - fragpos);
			float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;
			float angledecay = clamp((dot(thislight.direction, normalize(fragpos - thislight.position)) - thislight.zerodot) / (thislight.fulldot - thislight.zerodot), 0.0f, 1.0f);

			float shadowfactor = 1.0f;
			if (thislight.transformID > 0)
			{
				vec2 plane = lighttransform[thislight.transformID - 1].plane;
				vec3 lightspacepos = lightspace[thislight.transformID - 1];
				lightspacepos.xy /= lightspacepos.z;
				lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
				shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos);
			}
			color +=  shadowfactor * BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay * angledecay;
			i = lightlinked[i].y;
		}
		SpotLight thislight = sl[lightlinked[endindex].x];
		float dist = length(thislight.position - fragpos);
		float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;
		float angledecay = clamp((dot(thislight.direction, normalize(fragpos - thislight.position)) - thislight.zerodot) / (thislight.fulldot - thislight.zerodot), 0.0f, 1.0f);
		float shadowfactor = 1.0f;
		if (thislight.transformID > 0)
		{
			vec2 plane = lighttransform[thislight.transformID - 1].plane;
			vec3 lightspacepos = lightspace[thislight.transformID - 1];
			lightspacepos.xy /= lightspacepos.z;
			lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
			shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos);
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay * angledecay;
	}
	return color;
}

//vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction, vec3 eyedir)
//{
//	vec3 halfangle = normalize(normal + direction);
//	vec3 schlickfresnel = mix(vec3(1 - pow(dot(direction, halfangle), 5.0f)), vec3(1.0f), specular);
//	vec3 materialfactor = diffuse + (shininess + 2.0f) / 8.0f * pow(dot(normal, halfangle), shininess) * schlickfresnel;
//	return materialfactor * color * max(dot(normal, direction), 0.0f);
//}
vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction, vec3 eyedir)
{
	vec3 halfangle = normalize(eyedir + direction);
	float diffusefactor = dot(normal, direction);
	if (diffusefactor <= 0)
		return vec3(0.0f);
	float specularfactor = pow(dot(halfangle, normal), shininess);
	return (diffusefactor * diffuse + specularfactor * specular);
}

float ShadowFactor(sampler2D ztex, vec3 frag)
{
	return Esm(min(frag.z, 1.0f), texture(ztex, frag.xy).x);
}
float ShadowFactor(samplerCube ztex, vec3 frag, float depth)
{
	return Esm(min(depth, 1.0f), texture(ztex, frag).x);
}

float Esm(float depth, float zintex)
{
	return clamp(exp(C_VALUE * (zintex - depth)), 0.0f, 1.0f);
}