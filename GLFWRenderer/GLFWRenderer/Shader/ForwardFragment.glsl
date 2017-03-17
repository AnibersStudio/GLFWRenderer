#version 430 core
#define MAXPOINTLIGHT 512
#define MAXSPOTLIGHT 256
#define MAXDIRECTIONALLIGHT 4

struct BaseLight
{
	vec3 color;
	float intensity;
	float diffuse;
	float specular;
};

struct DirectionalLight
{
	BaseLight bl;
	vec3 direction;

};

struct Attenuation
{
	float constant;
	float linear;
	float exp;
};

struct PointLight
{
	BaseLight bl;
	vec3 position;
	Attenuation atten;
};

struct SpotLight
{
	BaseLight bl;
	vec3 position;
	float fullcos;
	vec3 direction;
	float zerocos;
	Attenuation atten;
};

struct Material
{
	vec3 ambientcolor;
	vec3 diffusecolor;
	vec3 specularcolor;
	vec3 emissivecolor;
	float shininess;
	float trans;
};

in vec3 fragposition;
in vec2 texcoord;
in vec3 fragnormal;
in vec3 fragtangent;
in vec3 dlspacepos;
in vec3 slspacepos;
uniform float plfarplane;

layout(location = 0) out vec4 pixelcolor;
layout(location = 1) out vec4 brightcolor;

uniform bool isdiffuse;
uniform sampler2D diffusesampler;
uniform bool isspecular;
uniform sampler2D specularsampler;
uniform bool isemissive;
uniform sampler2D emissivesampler;
uniform bool isnormal;
uniform sampler2D normalsampler;
uniform bool istrans;
uniform sampler2D transsampler;

uniform bool isdldepth;
uniform sampler2D dldepthsampler;

uniform bool ispldepth;
uniform samplerCube pldepthsampler;

uniform bool issldepth;
uniform sampler2D sldepthsampler;

uniform bool isbloom;
uniform vec3 Eye;

uniform DirectionalLight dl[MAXDIRECTIONALLIGHT];
uniform uint dlcount;

layout (std140) uniform pointlight
{
	PointLight pl[MAXPOINTLIGHT];
};
layout (std140) uniform spotlight
{
	SpotLight sl[MAXSPOTLIGHT];
};

uniform Material material;

vec3 CalLight(BaseLight light, vec3 lightdir, vec3 normal, vec3 diffusecolor, vec3 specularcolor, float multiplier);
vec3 CalDL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor);
vec3 CalPL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor);
vec3 CalSL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor);
float calculateshadow(vec3 lightdir, sampler2D depthsampler, vec3 lightspacepos, float distance);
float calculateshadow(vec3 normal, vec3 lightdir, sampler2D depthsampler, vec3 lightspacepos);
float calculateshadow(vec3 lightpos, samplerCube sampler, vec3 fragpos, float farplane);
vec4 visualizecubemap(vec3 lightpos, samplerCube sampler, vec3 fragpos);

vec3 sampleoffsetdir[12] = vec3[]
(
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);

void main()
{
	vec3 diffusecolor, ambientcolor, specularcolor, emissivecolor;

	if (isdiffuse)
	{
		diffusecolor = (texture(diffusesampler, texcoord)).xyz;
	}
	else
	{
		diffusecolor = material.diffusecolor;
	}
	ambientcolor = diffusecolor * material.ambientcolor;
	if (isspecular)
	{
		specularcolor = (texture(specularsampler, texcoord)).xyz;
	}
	else
	{
		specularcolor = material.specularcolor;
	}
	if (isemissive)
	{
		emissivecolor = (texture(emissivesampler, texcoord)).xyz;
	}
	else
	{
		emissivecolor = material.emissivecolor;
	}

	vec3 normal = normalize(fragnormal);;
	if (isnormal)
	{
		vec3 fragtangentn = normalize(fragtangent);
		fragtangentn = normalize(fragtangentn - dot(fragtangentn, normal) * normal);
		vec3 fragbitangentn = cross(normal, fragtangentn);
		mat3 TBN = mat3(fragtangentn, fragbitangentn, normal);
		normal = (texture(normalsampler, texcoord)).xyz;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(TBN * normal);
	}												
	float trans = material.trans;
	if (istrans)
	{
		trans = (texture(transsampler, texcoord)).a;
	}

	vec4 dlfactor = vec4(0.0, 0.0, 0.0, 0.0);
	if (dlcount > 0U)
	{
		float shadowfactor = 1.0;
		if (isdldepth)
		{
			shadowfactor = calculateshadow(normal, dl[0].direction, dldepthsampler, dlspacepos);
			dlfactor += shadowfactor * vec4(CalDL(0U, normal, diffusecolor, specularcolor), 0.0);

		}
	}
	for (uint i = 1U; i < dlcount; i++)
	{
		dlfactor += vec4(CalDL(i, normal, diffusecolor, specularcolor), 0.0);
	}
	
	vec4 plfactor = vec4 (0.0, 0.0, 0.0, 0.0);
	{
		float shadowfactor = 1.0;
		if (ispldepth)
		{
			shadowfactor = calculateshadow(pl[0].position, pldepthsampler, fragposition, plfarplane);
			plfactor += shadowfactor * vec4(CalPL(0U, normal, diffusecolor, specularcolor), 0.0);

		}
	}
	for (uint i = 1U; i != 1U; i++ )
	{
		plfactor += vec4(CalPL(i, normal, diffusecolor, specularcolor), 0.0);
	}

	vec4 slfactor = vec4(0.0, 0.0, 0.0, 0.0);
	{
		float shadowfactor = 1.0;
		if (issldepth)
		{
			shadowfactor = calculateshadow(sl[0].direction, sldepthsampler, slspacepos, length(fragposition - sl[0].position));
			slfactor += shadowfactor * vec4(CalSL(0U, normal, diffusecolor, specularcolor), 0.0);

		}
	}
	for (uint i = 1U; i != 1U; i++)
	{
		slfactor += vec4(CalSL(i, normal, diffusecolor, specularcolor), 0.0);
	}

	vec4 outcolor = dlfactor + plfactor + slfactor + vec4(emissivecolor, 1.0) + vec4(ambientcolor, 1.0);
	outcolor.a = trans;

	pixelcolor = outcolor;

	if (isbloom)
	{
		float brightness = dot(outcolor.xyz, vec3(0.299, 0.587, 0.144));//Perceived luminance
		if (brightness > 1.0)
		{
			brightcolor = outcolor;
		}
		else
		{
			brightcolor = vec4(0.0);
		}
	}
}

vec3 CalLight(BaseLight light, vec3 lightdir, vec3 normal, vec3 diffusecolor, vec3 specularcolor, float multiplier)
{
	float diffusefactor = dot(normal, -lightdir);

	vec3 diffusecomponent = vec3(0.0, 0.0, 0.0);
	vec3 specularcomponent = vec3(0.0, 0.0, 0.0);
	if (diffusefactor > 0)
	{
		if (diffusefactor * multiplier < 0.01)
		{
			return vec3(0.0);
		}

		diffusecomponent = light.diffuse * light.color * diffusefactor * diffusecolor;

		vec3 eyedir = normalize(Eye - fragposition);
		vec3 halfdir = normalize(eyedir - lightdir);
		float specularfactor = pow(dot(normal, halfdir), material.shininess);
		specularfactor = clamp(specularfactor, 0.0, 1.0);
		specularcomponent = light.specular * light.color * specularfactor * specularcolor;
	}
	return (diffusecomponent + specularcomponent) * multiplier;
}

vec3 CalDL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor)
{
	return CalLight(dl[index].bl, dl[index].direction, normal, diffusecolor, specularcolor, dl[index].bl.intensity) ;
}

vec3 CalPL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor)
{
	vec3 lightdir = fragposition - pl[index].position;
	float distance = length(lightdir);
	float atten = 1.0 /(pl[index].atten.constant + distance * pl[index].atten.linear + distance * distance * pl[index].atten.exp);
	lightdir = normalize(lightdir);
	
	return CalLight(pl[index].bl, lightdir, normal, diffusecolor, specularcolor, atten * pl[index].bl.intensity);
}

vec3 CalSL(uint index, vec3 normal, vec3 diffusecolor, vec3 specularcolor)
{
	vec3 lightdir = fragposition - sl[index].position;
	float distance = length(lightdir);
	float atten = 1.0 / (sl[index].atten.constant + distance * sl[index].atten.linear + distance * distance * sl[index].atten.exp);
	
	lightdir = normalize(lightdir);
	float lightcos = dot(lightdir, sl[index].direction);
	if (lightcos > sl[index].zerocos)
	{
		float spotfactor = 1.0;
		if (lightcos < sl[index].fullcos)
		{
			spotfactor = (lightcos - sl[index].zerocos) / (sl[index].fullcos - sl[index].zerocos);
		}
		return CalLight(sl[index].bl, lightdir, normal, diffusecolor, specularcolor, spotfactor * atten * sl[index].bl.intensity);
	}
	return vec3(0.0, 0.0, 0.0);
}

float calculateshadow(vec3 lightdir, sampler2D sampler, vec3 lightspacepos, float distance)
{
	float bias = 1.0f/(distance * distance - 2 * distance);
	vec3 projcoord = lightspacepos;
	projcoord =	projcoord * 0.5f + 0.5f;
	if (projcoord.z > 1.0f || projcoord.z < 0.0f)
		return 1.0f;
	float currentdepth = projcoord.z;
	float shadow;
	vec2 texelSize = 1.0 / textureSize(sampler, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfdepth = texture(sampler, projcoord.xy + vec2(x, y) * texelSize).r; 
			shadow += currentdepth - bias > pcfdepth ? 0.0 : 1.0;        
		}    
	}
	shadow /= 9;
	return shadow;
}

float calculateshadow(vec3 normal, vec3 lightdir, sampler2D sampler, vec3 lightspacepos)
{
	if (dot(normal, -lightdir) < 0.04)
		return 1.0f;
	float bias = max(0.02 * (1.0 - dot(normal, -lightdir)), 0.0001);
	vec3 projcoord = lightspacepos;
	projcoord =	projcoord * 0.5f + 0.5f;
	if (projcoord.z > 1.0f || projcoord.z < 0.0f)
		return 1.0f;
	float currentdepth = projcoord.z;
	float shadow;
	vec2 texelSize = 1.0 / textureSize(sampler, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfdepth = texture(sampler, projcoord.xy + vec2(x, y) * texelSize).r; 
			shadow += currentdepth - bias > pcfdepth ? 0.0 : 1.0;        
		}    
	}
	shadow /= 9;
	return shadow;
}

float calculateshadow(vec3 lightpos, samplerCube sampler, vec3 fragpos, float farplane)
{
	vec3 fragtolight = fragpos - lightpos;

	float currentdepth = length(fragtolight);
	float bias = 0.2;
	
	float shadow;
	int samplecount = 12;
	float diskradius = 0.01; 
	if (currentdepth > farplane)
	{
		shadow = 1.0;
	}
	else
	{
		for (int i = 0; i != samplecount; i++)
		{
			float pcfdepth = texture(sampler, fragtolight + sampleoffsetdir[i] * diskradius).r;
			pcfdepth *= farplane;
			shadow += currentdepth - bias > pcfdepth ? 0.0 : 1.0;
		}
	}
	shadow /= samplecount;

	return shadow;
}

vec4 visualizecubemap(vec3 lightpos, samplerCube sampler, vec3 fragpos)
{
	vec3 fragtolight = fragpos - lightpos;
	float closestdepth = texture(sampler, fragtolight).r;
	closestdepth *= plfarplane;
	float currentdepth = length(fragtolight);
	float bias = 0.01;

	float shadow = currentdepth > closestdepth ? 0.0 : 1.0;

	return vec4(vec3(closestdepth/plfarplane), 1.0);
	//return vec4(vec3(currentdepth/plfarplane), 1.0);
}