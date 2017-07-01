#version 430 core
#extension GL_ARB_bindless_texture : enable
#define C_VALUE 800.0f
layout (location = 0) out vec4 Color;

in vec3 fragpos;
in vec2 texcoord;
in vec3 fragnormal;
in vec3 fragtangent;
flat in uint fragmatindex;

uniform uvec2 tilesize = uvec2(32u, 32u);
uniform uvec2 tilecount;
uniform uint dlcount;
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
	//Only directional/spot light need this. Point light will be set to translate(position)
	mat4 View;
	//Only directional/spot light need this. Point light will be set to perspective(90)
	mat4 Proj;
	// .x near plane .y far plane
	vec2 plane;
	//
	float texelworldsize;
};

struct Material
{
	vec3 ambientcolor;
	vec3 diffusecolor;
	vec3 specularcolor;
	vec3 emissivecolor;
	float shininess;
	float transparency;
};

struct ShaderTexture
{
	sampler2D handle;
	uint is;
};

struct ShaderMaterial
{
	Material material;//5 vec3s	
	ShaderTexture diffuse;
	ShaderTexture specular;
	ShaderTexture normal;
	ShaderTexture emissive;
	ShaderTexture trans;
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
	LightTransform lighttransform[84];
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

layout (std430, binding = 5) buffer materiallist
{
	ShaderMaterial material[];
};

vec3 LightByDL(vec3 diffuse, vec3 specular, float shininess, vec3 normal);
vec3 LightByPL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex);
vec3 LightBySL(vec3 diffuse, vec3 specular, float shininess, vec3 normal, uint tileindex);
vec3 BlinnPhongFresnel(vec3 diffuse, vec3 specular,	float shininess,vec3 normal, vec3 color, vec3 direction, vec3 eyedir);
float ShadowFactor(sampler2D ztex, vec3 frag);
float ShadowFactor(samplerCube ztex, vec3 frag, float depth);
float Esm(float depth, float zintex);
vec4 NormalOffsetLightSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize);
vec3 NormalOffsetWorldSpace(vec3 worldnormal, vec3 worldpos, mat4 view, mat4 proj, float texelsize);
vec3 NormalByTexture(vec3 linearnormal, vec3 fragtangent, vec3 texnormal);

void main() 
{
	//Calculate tile
	uvec2 tilecoord = uvec2(gl_FragCoord.xy) / tilesize;
	uint tileindex = tilecoord.x * tilecount.y + tilecoord.y;
	//Get material
	ShaderMaterial fragmat = material[fragmatindex];
	vec3 ambient = fragmat.material.ambientcolor;
	vec3 diffuse = fragmat.material.diffusecolor;
	vec3 specular = fragmat.material.specularcolor;
	vec3 emissive = fragmat.material.emissivecolor;
	float trans = fragmat.material.transparency;
	float shininess = fragmat.material.shininess;
	vec3 normal = fragnormal;
	if (fragmat.trans.is > 0)
	{
		trans = texture(fragmat.trans.handle, texcoord).a;
	}
	if (trans < 0.00001)
	{
		discard;
	}
	if (fragmat.diffuse.is > 0)
	{
		diffuse = texture(fragmat.diffuse.handle, texcoord).rgb;
	}
	if (fragmat.specular.is > 0)
	{
		specular = texture(fragmat.specular.handle, texcoord).rgb;
	}
	if (fragmat.emissive.is > 0)
	{
		emissive = texture(fragmat.emissive.handle, texcoord).rgb;
	}

	if (fragmat.normal.is > 0)
	{
		normal = NormalByTexture(fragnormal, fragtangent, texture(fragmat.normal.handle, texcoord).xyz);
	}
	ambient *= diffuse;

	//Calculate dl
	vec3 color = vec3(0.0f);
	color += LightByDL(diffuse, specular, shininess, normal);
	color += LightByPL(diffuse, specular, shininess, normal, tileindex);
	color += LightBySL(diffuse, specular, shininess, normal, tileindex);

	Color = vec4(color.xyz + ambient + emissive, trans);
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
			vec4 lightspacepos = NormalOffsetLightSpace(normal, fragpos, lighttransform[thislight.transformID - 1].View, lighttransform[thislight.transformID - 1].Proj, lighttransform[thislight.transformID - 1].texelworldsize);
			lightspacepos.xy = (lightspacepos.xy / lightspacepos.w * 0.5f + 0.5f);
			lightspacepos.z = lightspacepos.w;
			lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
			shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos.xyz);
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.color * thislight.diffuse, specular * thislight.color * thislight.specular, shininess, normal, thislight.color, -thislight.direction, normalize(eye - fragpos)) * thislight.intensity;
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
				mat4 view = lighttransform[thislight.transformID - 1].View;
				mat4 proj = lighttransform[thislight.transformID - 1].Proj;
				float texelworldsize = lighttransform[thislight.transformID - 1].texelworldsize;
				vec3 offsetfragpos = NormalOffsetWorldSpace(normal, fragpos, view, proj, texelworldsize); 
				shadowfactor = ShadowFactor(thislight.shadowsampler, normalize(offsetfragpos - thislight.position), (length(offsetfragpos - thislight.position) -  plane.x) / (plane.y - plane.x));
			}
			color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.color * thislight.diffuse, specular * thislight.color * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay;
			i = lightlinked[i].y;
		}
		PointLight thislight = pl[lightlinked[endindex].x];
		float dist = length(thislight.position - fragpos);
		float distdecay = thislight.atten.constant + thislight.atten.linear * dist + thislight.atten.exponential * dist * dist;

		float shadowfactor = 1.0f;
		if (thislight.transformID > 0)
		{
			vec2 plane = lighttransform[thislight.transformID - 1].plane;
			mat4 view = lighttransform[thislight.transformID - 1].View;
			mat4 proj = lighttransform[thislight.transformID - 1].Proj;
			float texelworldsize = lighttransform[thislight.transformID - 1].texelworldsize;
			vec3 offsetfragpos = NormalOffsetWorldSpace(normal, fragpos, view, proj, texelworldsize); 
			shadowfactor = ShadowFactor(thislight.shadowsampler, normalize(offsetfragpos - thislight.position), (length(offsetfragpos - thislight.position) -  plane.x) / (plane.y - plane.x));
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.color * thislight.diffuse, specular * thislight.color * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay;
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
				vec4 lightspacepos = NormalOffsetLightSpace(normal, fragpos, lighttransform[thislight.transformID - 1].View, lighttransform[thislight.transformID - 1].Proj, lighttransform[thislight.transformID - 1].texelworldsize);
				lightspacepos.xy = (lightspacepos.xy / lightspacepos.w * 0.5f + 0.5f);
				lightspacepos.z = lightspacepos.w;
				lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
				shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos.xyz);
			}
			color +=  shadowfactor * BlinnPhongFresnel(diffuse * thislight.color * thislight.diffuse, specular * thislight.color * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay * angledecay;
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
			vec4 lightspacepos = NormalOffsetLightSpace(normal, fragpos, lighttransform[thislight.transformID - 1].View, lighttransform[thislight.transformID - 1].Proj, lighttransform[thislight.transformID - 1].texelworldsize);
			lightspacepos.xy = (lightspacepos.xy / lightspacepos.w * 0.5f + 0.5f);
			lightspacepos.z = lightspacepos.w;
			lightspacepos.z = (lightspacepos.z - plane.x) / (plane.y - plane.x);
			shadowfactor = ShadowFactor(thislight.shadowsampler, lightspacepos.xyz);
		}
		color += shadowfactor * BlinnPhongFresnel(diffuse * thislight.color * thislight.diffuse, specular * thislight.color * thislight.specular, shininess, normal, thislight.color, normalize(thislight.position - fragpos), normalize(eye - fragpos)) * thislight.intensity / distdecay * angledecay;
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

vec3 NormalByTexture(vec3 linearnormal, vec3 fragtangent, vec3 texnormal)
{
	linearnormal = normalize(linearnormal);
	fragtangent = normalize(fragtangent);
	fragtangent = normalize(fragtangent - dot(fragtangent, linearnormal) * linearnormal);
	vec3 fragbitangent = cross(linearnormal, fragtangent);
	mat3 TBN = mat3(fragtangent, fragbitangent, linearnormal);
	texnormal = normalize(texnormal * 2.0f - 1.0f);
	return normalize(TBN * texnormal);
}