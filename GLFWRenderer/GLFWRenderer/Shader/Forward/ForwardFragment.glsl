#version 450
#extension GL_ARB_bindless_texture : enable

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


struct DirectionalLight
{
	vec3 color;

	float intensity;
	float diffuse;
	float specular;

	sampler2D shadowsampler;

	// This will become a ID to transform list
	uint hasshadow;

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
	//uint transformID;
	float transformID;

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
vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction);

void main() 
{
	//Calculate tile
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	//Get material
	vec3 diffuse = texture(diffusetex, texcoord).rgb;
	//Calculate dl
	vec3 color = vec3(0.0f);
	//color += LightByDL(diffuse, vec3(0.4f), 50.0f, fragnormal);
	//color += LightByPL(diffuse, vec3(0.4f), 50.0f, fragnormal, tileindex);
	color += LightBySL(diffuse, vec3(0.4f), 50.0f, fragnormal, tileindex);

	Color = vec4(color, 1.0f);
}

vec3 LightByDL(vec3 diffuse, vec3 specular, float shininess, vec3 normal)
{
	vec3 color = vec3(0.0f);
	for (uint i = 0; i != dlcount; i++)
	{
		color += BlinnPhongFresnel(diffuse * dl[i].diffuse, specular * dl[i].specular, shininess, normal, dl[i].color, dl[i].direction) * dl[i].intensity;
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
			color += BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos)) * thislight.intensity / distdecay;
			i = lightlinked[i].y;
		}
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
			color += BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos)) * thislight.intensity / distdecay * angledecay;
			i = lightlinked[i].y;
		}
		SpotLight thislight = sl[lightlinked[endindex].x];
		float dist = length(thislight.position - fragpos);
		float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;
		float angledecay = clamp((dot(thislight.direction, normalize(fragpos - thislight.position)) - thislight.zerodot) / (thislight.fulldot - thislight.zerodot), 0.0f, 1.0f);
		color += BlinnPhongFresnel(diffuse * thislight.diffuse, specular * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos)) * thislight.intensity / distdecay * angledecay;
	}
	return color;
}

vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction)
{
	return diffuse;
	vec3 halfangle = normalize(normal - direction);
	vec3 schlickfresnel = mix(vec3(1 - pow(dot(-direction, halfangle), 5.0f)), vec3(1.0f), specular);
	vec3 materialfactor = diffuse + (shininess + 2.0f) / 8.0f * pow(dot(normal, halfangle), shininess) * schlickfresnel;
	return materialfactor * color * max(dot(normal, -direction), 0.0f);
}