#version 430 core
#pragma optionNV (unroll all)

layout (location = 0) out vec3 Color;

//uniform float kernel[7] = {0.2256, 0.1934, 0.12, 0.054, 0.016, 0.003, 0.000244};
uniform float kernel[9] = {0.1964, 0.1746, 0.1222, 0.067, 0.028, 0.0085, 0.0018, 0.00024, 0.000015};

uniform vec2 offsetmask[2] = {vec2(1.0f, 0.0f), vec2(0.0f, 1.0f)};
uniform sampler2D image;
uniform uint ishorizonal;

void main()
{
	vec2 offset = 1.0 / textureSize(image, 0); 
	vec2 texcoord = gl_FragCoord.xy * offset + 0.5f * offset;
	vec3 color = texture(image, texcoord).rgb * kernel[0];

	for (int i = 1; i != 9; i++)
	{
		color += texture(image, texcoord + offset * offsetmask[ishorizonal] * i).rgb * kernel[i];
		color += texture(image, texcoord - offset * offsetmask[ishorizonal] * i).rgb * kernel[i];
	}
	
	Color = color;
}