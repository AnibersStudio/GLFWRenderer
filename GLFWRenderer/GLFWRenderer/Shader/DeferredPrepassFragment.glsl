#version 430 core

struct Material
{
	vec3 albedocolor;
	float metalness;
	vec3 emissivecolor;
	float shininess;
	float trans;
};

in vec2 fragtexcoord;
in vec3 fragnormal;
in vec3 fragtangent;
in float fragdepthinview;
in uint materialindex;

layout(location = 0) out lowp vec4 albedostorage;
layout(location = 1) out mediump vec3 normalstorage;
layout(location = 2) out lowp vec4 emmisivestorage;

uniform Material material
uniform bool isalbedo;
uniform sampler2D albedosampler;
uniform bool ismetalness;
uniform sampler2D mentalnesssampler;
uniform bool isemissive;
uniform sampler2D emissivesampler;
uniform bool isnormal;
uniform sampler2D normalsampler;
uniform bool istrans;
uniform sampler2D transsampler;

uniform float farplane;

mediump vec2 encodenormal(vec3 n);
vec3 calculatenormal(vec3 n, vec3 tan, vec3 tn);

void main()
{
	//Calculate and Store Normal
	vec3 normal = normalize(fragnormal);
	if (isnormal)
	{
		normal = calculatenormal(normal, normalize(fragtangent), texture(normalsampler, texcoord).xyz);
	}
	normalstorage.xy = encodenormal(fragnormal);
	normalstorage.z = material.shininess;

	//Calculate and Store Material
	if (isalbedo)
	{
		albedostorage.rgb = texture(albedosampler, fragtexcoord).xyz;
	}
	else
	{
		albedostorage.rgb = material.albedocolor;
	}

	if (ismentalness)
	{
		albedostorage.a = texture(mentalnesssampler, fragcoord).r;
	}
	else
	{
		albedostorage.a = material.mentalness;
	}

	if (isemissive)
	{
		emissivestorage.rgb = texture(emissivesampler, fragcoord).rgb;
	}
	else
	{
		emissivestorage.rgb = material.emissivecolor;
	}
	
	if (istrans)
	{
		emissivestorage.a = texture(transsampler, fragcoord).a;
	}
	else
	{
		emissivestorage.a = material.trans;
	}

	//linear depth
	gl_FragDepth = fragdepthinview / farplane;
}

mediump vec2 encodenormal(vec3 n)
{
	return n.xy;
}
vec3 calculatenormal(vec3 n, vec3 tan, vec3 tn)
{
	vec3 fragtangentn = normalize(tan - dot(tan, n) * n);
	vec3 fragbitangentn = cross(n, fragtangentn);
	mat3 TBN = mat3(fragtangentn, fragbitangentn, n);
	return normalize(TBN * normalize(tn * 2.0 - 1.0));
}
