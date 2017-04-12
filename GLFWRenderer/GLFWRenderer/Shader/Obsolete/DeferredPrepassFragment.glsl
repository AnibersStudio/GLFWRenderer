#version 430 core
#define MAXMATERIAL 128

struct TexturedMaterial
{
	vec3 albedocolor;
	float metalness;
	vec3 emissivecolor;
	float shininess;
	float trans;

	bool isalbedo;
	bool ismentalness;
	bool isemissive;
	bool isnormal;
	bool istrans;
	sampler2D albedosampler;
	sampler2D mentalnesssampler;
	sampler2D emissivesampler;
	sampler2D normalsampler;
	sampler2D transsampler;
};

in vec2 fragtexcoord;
in vec3 fragnormal;
in vec3 fragtangent;
in float fragdepthinview;
in uint materialindex;

layout(location = 0) out lowp vec4 albedostorage;
layout(location = 1) out mediump vec3 normalstorage;
layout(location = 2) out lowp vec4 emmisivestorage;

layout (std140) uniform materialbuffer
{
	TexturedMaterial material[MAXMATERIAL];
};

uniform float farplane;

mediump vec2 encodenormal(vec3 n);
vec3 calculatenormal(vec3 n, vec3 tan, vec3 tn);

void main()
{
	//Calculate and store Trans
	float trans;
	if (material[materialindex].istrans)
	{
		trans = texture(material[materialindex].transsampler, fragcoord).a;
	}
	else
	{
		trans = material[materialindex].trans;
	}
	if (trans < 0.00001)
	{
		discard;
	}
	emissivestorage.a = trans;

	//Calculate and Store Normal
	vec3 normal = normalize(fragnormal);
	if (isnormal)
	{
		normal = calculatenormal(normal, normalize(fragtangent), texture(normalsampler, texcoord).xyz);
	}
	normalstorage.xy = encodenormal(fragnormal);
	normalstorage.z = material[materialindex].shininess;

	//Calculate and Store Material
	if (isalbedo)
	{
		albedostorage.rgb = texture(material[materialindex].albedosampler, fragtexcoord).xyz;
	}
	else
	{
		albedostorage.rgb = material[materialindex].albedocolor;
	}

	if (ismentalness)
	{
		albedostorage.a = texture(material[materialindex].mentalnesssampler, fragcoord).r;
	}
	else
	{
		albedostorage.a = material[materialindex].mentalness;
	}

	if (isemissive)
	{
		emissivestorage.rgb = texture(material[materialindex].emissivesampler, fragcoord).rgb;
	}
	else
	{
		emissivestorage.rgb = material[materialindex].emissivecolor;
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
