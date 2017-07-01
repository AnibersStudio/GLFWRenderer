#version 430 core
layout (location = 0) out vec4 Color;

uniform sampler2D image;
uniform sampler2D bloom;
uniform sampler2D exposure;
uniform bool istonemap;
uniform bool isbloom;
uniform bool iseyeadapt;
uniform float bloomratio;
uniform float bloomthreshold;
uniform float exposurefreshrate;
uniform float maxexposure = 4.0f;
uniform float minexposure = 0.25f;
uniform float gamma;

layout (std430, binding = 0) buffer prevcolorbuffer
{
	vec3 prevavgcolor;
};

float perceivedluminance(vec3 color);
vec3 ACESToneMapping(vec3 color);

void main()
{
	vec2 offset = 1.0 / textureSize(image, 0); 
	vec2 texcoord = vec2(gl_FragCoord.xy) * offset + 0.5f * offset;
	vec3 color = texture(image, texcoord).rgb;
	//lerp image to bloom
	if (isbloom)
	{
		vec3 bloomcolor = texture(bloom, texcoord).rgb;
		if (perceivedluminance(color) >= bloomthreshold)
		{
			color = mix(color, bloomcolor, bloomratio);
		}
	}
	//Eye Adaption
	if (iseyeadapt)
	{
		vec3 avgcolor = texture(exposure, vec2(0.5f, 0.5f)).rgb;
		float exposurefreshratio = exp2(-exposurefreshrate);
		prevavgcolor = mix(prevavgcolor, avgcolor, exposurefreshratio);
		float avgluminance = 2 * perceivedluminance(prevavgcolor);
		avgluminance = min(maxexposure, max(minexposure, avgluminance));
		color /= avgluminance;
	}
	else
	{
		prevavgcolor = vec3(0.5f);
	}
	//ToneMapping
	if (istonemap)
	{
		color = ACESToneMapping(color);
	}
	//Gamma Correction
	color = pow(color, vec3(1.0f / gamma));
	
	Color = vec4(color, 1.0f);
}

float perceivedluminance(vec3 color)
{
	return dot(color, vec3(0.29, 0.57, 0.14));
}

vec3 ACESToneMapping(vec3 color)
{
    const float A = 2.51f;
    const float B = 0.03f;
    const float C = 2.43f;
    const float D = 0.59f;
    const float E = 0.14f;
    return (color * (A * color + B)) / (color * (C * color + D) + E);
}