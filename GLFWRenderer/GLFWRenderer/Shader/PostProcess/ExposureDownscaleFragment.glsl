#version 430 core

layout (location = 0) out vec3 Color;

uniform vec2 offsetmask[4] = {vec2(0.0f, 0.0f), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f)};
uniform sampler2D image;

void main()
{
	vec2 offset = 1.0 / textureSize(image, 0); 
	vec2 texcoord = (gl_FragCoord.xy - vec2(0.5f)) * vec2(2.0f, 2.0f) * offset + 0.5f * offset;
	vec3 color = vec3(0.0f);
	
	for (int i = 0; i != 4; i++)
	{
		color += texture(image, texcoord + offset * offsetmask[i]).rgb;
	}
	color /= 4.0f;
	
	Color = color;
}